//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: dbgPgThread.cpp 6202 2007-04-18 11:22:49Z dpage $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dbgPgThread.cpp - debugger 
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/app.h>
#include <wx/tokenzr.h>
#include <wx/listimpl.cpp>

// App headers
#include "debugger/dbgPgThread.h"
#include "debugger/dbgDbResult.h"
#include "debugger/frmDebugger.h"
#include "debugger/dbgPgConn.h"

#include <libpq-fe.h>

WX_DEFINE_LIST( ThreadCommandList );

////////////////////////////////////////////////////////////////////////////////
// dbgPgThread constructor
//
//    A dbgPgThread object encapsulates a separate execution thread that interacts
//  with a PostgreSQL server.  We use a separate thread to keep the main thread
//  (and thus the user interface) responsive while we're waiting for the server.

dbgPgThread::dbgPgThread( dbgPgConn & owner )
    : wxThread(wxTHREAD_JOINABLE),
    m_owner( owner ),
    m_queueCounter(),
    m_queueMutex(),
    m_commandQueue(),
    m_currentCommand( NULL ),
    run( 0 ),
    die( false )
{
    conv = &wxConvLibc;
}

////////////////////////////////////////////////////////////////////////////////
// startCommand()
//
//     This function is called by the GUI thread when the user decides to 
//     execute a command.  To keep the GUI thread responsive, we use a 
//     separate thread to interact with the PostgreSQL server. This function
//  wakes up the worker thread.

void dbgPgThread::startCommand( const wxString &command, wxEvtHandler * caller, wxEventType eventType, dbgPgParams *params )
{
    // Save the command text (and the event handler that we should 
    // notify on completion) in the command queue and then wake up the
    // worker thread.  The worker thread sleeps until we increment 
    // m_queueCounter

    m_queueMutex.Lock();
    m_commandQueue.Append( new dbgPgThreadCommand( command, caller, eventType, params ));

    wxLogDebug( _( "Queueing: %s" ), command.c_str());

    m_queueMutex.Unlock();

    m_queueCounter.Post();
}

////////////////////////////////////////////////////////////////////////////////
// Die()
//
//    Instruct the thread to kill itself.

void dbgPgThread::Die()
{
    wxLogDebug( _( "Telling the query thread to die..." ));
    die = true;

    m_queueCounter.Post();
}

////////////////////////////////////////////////////////////////////////////////
// Entry()
//
//    This is the entry point (and main loop) for the worker thread.  It simply
//  waits for a command from the GUI thread (see the startCommand() function)
//  and then sends the command to the PostgreSQL server.  This function also
//  waits for a result set from the server - when the result set arrives, we
//  send an event to the GUI thread.

void * dbgPgThread::Entry( void )
{

    wxLogDebug( _( "worker thread waiting for some work to do..." ));

    // This thread should hang at the call to m_condition.Wait()
    // When m_condition is signaled, we wake up, send a command
    // to the PostgreSQL server, and wait for a result.

    while( m_queueCounter.Wait() == wxSEMA_NO_ERROR && !die && !TestDestroy() )
    {
        m_owner.setNoticeHandler( noticeHandler, this );

        m_currentCommand = getNextCommand();
        wxString command = m_currentCommand->getCommand();

        wxLogDebug( _( "Executing: %s" ), command.c_str());

        // This call to PQexec() will hang until we've received
        // a complete result set from the server.
        PGresult *result;

#if defined (__WXMSW__) || (EDB_LIBPQ)
        // If we have a set of params, and we have the required functions...
        dbgPgParams *params = m_currentCommand->getParams();

#ifdef EDB_LIBPQ
        if (params)
#else
        if (PQiGetOutResult && PQiPrepareOut && PQiSendQueryPreparedOut && params)
#endif
        {
            wxLogInfo(_("Using an EnterpriseDB callable statement"));
            wxString stmt = wxString::Format(wxT("DebugStmt-%d-%d"), this->GetId(), ++run);
            PGresult *res = PQiPrepareOut(m_owner.getConnection(), 
                                            stmt.mb_str(wxConvUTF8), 
                                            command.mb_str(wxConvUTF8),
                                            params->nParams,
                                            params->paramTypes,
                                            params->paramModes);

            if( PQresultStatus(res) != PGRES_COMMAND_OK)
            {
                wxLogError(wxT( "Could not prepare the callable statement: %s, error: %s" ), stmt.c_str(), wxString(PQresultErrorMessage(res), *conv).c_str());
                PQclear(res);
                return this;
            }

            int ret = PQiSendQueryPreparedOut(m_owner.getConnection(), 
                                                stmt.mb_str(wxConvUTF8),
                                                params->nParams,
                                                params->paramValues,
                                                NULL, // Can be null - all params are text
                                                NULL, // Can be null - all params are text
                                                1);
            if (ret != 1)
            {
                wxLogError(wxT( "Couldn't execute the callable statement: %s" ), stmt.c_str());
                PQclear(res);
                return this;
            }

            // We need to call PQgetResult before we can call PQgetOutResult
        	// Note that this is all async code as far as libpq is concerned to
        	// ensure we can always bail out when required, without leaving threads
        	// hanging around.
        	PGresult *dummy;
        	while(true)
        	{
        		if (die || TestDestroy())
        		{
        		    PQrequestCancel(m_owner.getConnection());
        		    return this;
        	    }
        			
        	    PQconsumeInput(m_owner.getConnection());
        		
        		if (PQisBusy(m_owner.getConnection()))
        		{
        		    Yield();
        			wxMilliSleep(10);
        			continue;
        		}
        		
        	    dummy = PQgetResult(m_owner.getConnection());
        		
        		// There should be 2 results - the first is the dummy, the second
        		// contains our out params.
        		if (dummy)
        		    break;
        	}

            if((PQresultStatus(dummy) == PGRES_NONFATAL_ERROR) || (PQresultStatus(dummy) == PGRES_FATAL_ERROR))
                result = dummy;
            else
        	{
        	    PQclear(dummy);
                result = PQiGetOutResult(m_owner.getConnection());
        	}
        }
        else
        {
#endif        	
            // This is the normal case for a pl/pgsql function, or if we don't
            // have access to PQgetOutResult.
        	// Note that this is all async code as far as libpq is concerned to
        	// ensure we can always bail out when required, without leaving threads
        	// hanging around.
            int ret = PQsendQuery(m_owner.getConnection(), command.mb_str(wxConvUTF8));
        	
            if (ret != 1)
            {
                wxLogError(wxT( "Couldn't execute the query (%s): %s" ), command.c_str(), wxString(PQerrorMessage(m_owner.getConnection()), *conv).c_str());
                return this;
            }
        	
        	PGresult *part;
        	while(true)
        	{
        		if (die || TestDestroy())
        		{
        		    PQrequestCancel(m_owner.getConnection());
        		    return this;
        	    }
        			
        	    PQconsumeInput(m_owner.getConnection());
        		
        		if (PQisBusy(m_owner.getConnection()))
        		{
        		    Yield();
        			wxMilliSleep(10);
        			continue;
        		}
        		
        		// In theory we should only get one result here, but we'll loop
        		// anyway until we get the last one.
        	    part = PQgetResult(m_owner.getConnection());
        		
        		if (!part)
        		    break;
        			
                result = part;
        	}
        	
#if defined (__WXMSW__) || (EDB_LIBPQ)
        }
#endif

        if(!result)
        {
            wxLogDebug(wxT( "NULL PGresult - user abort?" ));
            return this;
        }

        wxLogDebug(_( "Complete: %s" ), wxString(PQresStatus(PQresultStatus(result)), *conv).c_str());

        // Notify the GUI thread that a result set is ready for display

        if( m_currentCommand->getEventType() == wxEVT_NULL )
        {
            wxCommandEvent resultEvent( wxEVT_COMMAND_MENU_SELECTED, RESULT_ID_DIRECT_TARGET_COMPLETE );
            resultEvent.SetClientData( result );
            m_currentCommand->getCaller()->AddPendingEvent( resultEvent );
        }
        else
        {
            wxCommandEvent resultEvent( wxEVT_COMMAND_MENU_SELECTED, m_currentCommand->getEventType());
            resultEvent.SetClientData( result );
            m_currentCommand->getCaller()->AddPendingEvent( resultEvent );
        }
    }

    return this;
}

////////////////////////////////////////////////////////////////////////////////
// noticeHandler()
//
//     This function is invoked when a NOTICE is received from the PostgreSQL
//    server.  We watch for a specially-formatted NOTICE that the PL debugger 
//    raises when it reaches a breakpoint.

void dbgPgThread::noticeHandler( void * arg, const char * message )
{
    // Remove the last char from the message as it'll be a \n
    wxString msg = wxString(message, wxConvUTF8);
    if (msg.EndsWith(wxT("\n")))
        msg.RemoveLast();

    wxLogDebug(msg);

    dbgPgThread   * thread = (dbgPgThread *)arg;
    wxEvtHandler * caller = thread->m_currentCommand->getCaller();

    if( strstr( message, "PLDBGBREAK" ))
    {
        wxStringTokenizer    tokens( wxString( message, wxConvUTF8 ), wxT( ":\n" ));

        wxString NOTICE     = tokens.GetNextToken();        // NOTICE:
        wxString PLDBGBREAK = tokens.GetNextToken();        // PLDBGBREAK:
        wxString PORT       = tokens.GetNextToken();        // port

        PGconn * conn = thread->m_owner.getConnection();

        // Create a dbgConnProp object that contains the same information in a 
        // more convenient format

        dbgConnProp * debugProps = new dbgConnProp;

        debugProps->m_host         = wxString( PQhost( conn ), wxConvUTF8 );
        debugProps->m_database     = wxString( PQdb( conn ),   wxConvUTF8 );
        debugProps->m_userName     = wxString( PQuser( conn ), wxConvUTF8 );
        debugProps->m_debugPort = PORT;
        debugProps->m_port         = wxString( PQport( conn ), wxConvUTF8 );
        debugProps->m_password  = wxString( PQpass( conn ), wxConvUTF8 );

        wxCommandEvent buttonEvent( wxEVT_COMMAND_BUTTON_CLICKED, MENU_ID_SPAWN_DEBUGGER );
        buttonEvent.SetClientData((wxClientData *)debugProps );
        caller->AddPendingEvent( buttonEvent );
    }
    else if( strstr( message, "INFO" ))
    {
        if( strstr( message, "CONTEXT:" ) == NULL )
        {
            wxCommandEvent buttonEvent( wxEVT_COMMAND_BUTTON_CLICKED, MENU_ID_NOTICE_RECEIVED );

            wxString strippedMessage( wxString( message, wxConvUTF8 ));

            strippedMessage.Replace( wxT( "INFO:  " ), wxT( "" ), false );

            buttonEvent.SetString( strippedMessage );
            caller->AddPendingEvent( buttonEvent );
        }
    }
    else
    {
            wxCommandEvent buttonEvent( wxEVT_COMMAND_BUTTON_CLICKED, MENU_ID_NOTICE_RECEIVED );

            buttonEvent.SetString( wxString( message, wxConvUTF8 ) );
            caller->AddPendingEvent( buttonEvent );
    }
}

dbgPgThreadCommand * dbgPgThread::getNextCommand()
{
    dbgPgThreadCommand * result;

    m_queueMutex.Lock();

    ::wxLogDebug( _( "%d commands in queue" ), m_commandQueue.GetCount());

    ThreadCommandList::Node * node = m_commandQueue.GetFirst();
        
    result = node->GetData();

    m_commandQueue.DeleteNode( node );

    m_queueMutex.Unlock();

    return( result );
}
