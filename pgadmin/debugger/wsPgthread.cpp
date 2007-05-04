//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: wsPgthread.cpp 6202 2007-04-18 11:22:49Z dpage $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wspgthread.cpp - debugger 
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/app.h>
#include <wx/tokenzr.h>
#include <wx/listimpl.cpp>

// App headers
#include "debugger/wsPgthread.h"
#include "debugger/wsDbresult.h"
#include "debugger/wsMainFrame.h"
#include "debugger/wsPgconn.h"

#include <libpq-fe.h>

WX_DEFINE_LIST( ThreadCommandList );

////////////////////////////////////////////////////////////////////////////////
// wsPgThread constructor
//
//	A wsPgThread object encapsulates a separate execution thread that interacts
//  with a PostgreSQL server.  We use a separate thread to keep the main thread
//  (and thus the user interface) responsive while we're waiting for the server.

wsPgThread::wsPgThread( wsPgConn & owner )
	: wxThread( ),
	m_owner( owner ),
	m_queueCounter(),
	m_queueMutex(),
	m_commandQueue(),
	m_currentCommand( NULL )
{
    conv = &wxConvLibc;
}

////////////////////////////////////////////////////////////////////////////////
// startCommand()
//
// 	This function is called by the GUI thread when the user decides to 
// 	execute a command.  To keep the GUI thread responsive, we use a 
// 	separate thread to interact with the PostgreSQL server. This function
//  wakes up the worker thread.

void wsPgThread::startCommand( const wxString &command, wxEvtHandler * caller, wxEventType eventType )
{
	// Save the command text (and the event handler that we should 
	// notify on completion) in the command queue and then wake up the
	// worker thread.  The worker thread sleeps until we increment 
	// m_queueCounter

	m_queueMutex.Lock();
	m_commandQueue.Append( new wsPgThreadCommand( command, caller, eventType ));

	::wxLogDebug( _( "Queueing: %s" ), command.c_str());

	m_queueMutex.Unlock();

	m_queueCounter.Post();
}

////////////////////////////////////////////////////////////////////////////////
// Entry()
//
//	This is the entry point (and main loop) for the worker thread.  It simply
//  waits for a command from the GUI thread (see the startCommand() function)
//  and then sends the command to the PostgreSQL server.  This function also
//  waits for a result set from the server - when the result set arrives, we
//  send an event to the GUI thread.

void * wsPgThread::Entry( void )
{

	::wxLogDebug( _( "worker thread waiting for some work to do..." ));

	// This thread should hang at the call to m_condition.Wait()
	// When m_condition is signaled, we wake up, send a command
	// to the PostgreSQL server, and wait for a result.

	while( m_queueCounter.Wait() == wxSEMA_NO_ERROR )
	{
		m_owner.setNoticeHandler( noticeHandler, this );

		m_currentCommand = getNextCommand();

		::wxLogDebug( _( "Executing: %s" ), m_currentCommand->getCommand().c_str());

		// This call to PQexec() will hang until we've received
		// a complete result set from the server.

		PGresult * result = PQexec( m_owner.getConnection(), m_currentCommand->getCommand().mb_str( wxConvUTF8 ));

        if(!result)
        {
            ::wxLogDebug(wxT( "NULL PGresult - user abort?" ));
            this->Exit();
        }

		::wxLogDebug(_( "Complete: %s" ), wxString(PQresStatus(PQresultStatus(result)), *conv));

		if( PQresultStatus(result) == PGRES_FATAL_ERROR)
			::wxLogDebug(wxT( "        : %s" ), wxString(PQresultErrorMessage(result), *conv));

		// Notify the GUI thread that a result set is ready for display

		if( m_currentCommand->getEventType() == wxEVT_NULL )
		{
			wxCommandEvent resultEvent( wxEVT_COMMAND_MENU_SELECTED, RESULT_ID_RESULT_SET_READY );

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
// 	This function is invoked when a NOTICE is received from the PostgreSQL
//	server.  We watch for a specially-formatted NOTICE that the PL debugger 
//	raises when it reaches a breakpoint.

void wsPgThread::noticeHandler( void * arg, const char * message )
{
	::wxLogDebug( wxT( "%s\n" ), message );

	wsPgThread   * thread = (wsPgThread *)arg;
	wxEvtHandler * caller = thread->m_currentCommand->getCaller();

	if( strstr( message, "PLDBGBREAK" ))
	{
		wxStringTokenizer	tokens( wxString( message, wxConvUTF8 ), wxT( ":\n" ));

		wxString NOTICE     = tokens.GetNextToken();		// NOTICE:
		wxString PLDBGBREAK = tokens.GetNextToken();		// PLDBGBREAK:
		wxString PORT       = tokens.GetNextToken();		// port

		PGconn * conn = thread->m_owner.getConnection();

		// Create a wsConnProp object that contains the same information in a 
		// more convenient format

		wsConnProp * debugProps = new wsConnProp;

		debugProps->m_host     	= wxString( PQhost( conn ), wxConvUTF8 );
		debugProps->m_database 	= wxString( PQdb( conn ),   wxConvUTF8 );
		debugProps->m_userName 	= wxString( PQuser( conn ), wxConvUTF8 );
		debugProps->m_debugPort = PORT;
		debugProps->m_port     	= wxString( PQport( conn ), wxConvUTF8 );
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
}

wsPgThreadCommand * wsPgThread::getNextCommand()
{
	wsPgThreadCommand * result;

	m_queueMutex.Lock();

	::wxLogDebug( _( "%d commands in queue" ), m_commandQueue.GetCount());

	ThreadCommandList::Node * node = m_commandQueue.GetFirst();
		
	result = node->GetData();

	m_commandQueue.DeleteNode( node );

	m_queueMutex.Unlock();

	return( result );
}
