//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: dlgDirectDbg.cpp 6213 2007-04-19 08:47:08Z dpage $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgDirectDbg.cpp - debugger 
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/grid.h>

// App headers
#include "debugger/dlgDirectDbg.h"
#include "debugger/frmDebugger.h"
#include "debugger/dbgTargetInfo.h"
#include "debugger/dbgPgConn.h"
#include "debugger/ctlResultGrid.h"
#include "debugger/dbgResultset.h"
#include "debugger/dbgConst.h"
#include "debugger/dbgDbResult.h"
#include "debugger/ctlCodeWindow.h"

#include "images/debugger.xpm"

#include <stdexcept>

#ifdef __WXMSW__
// Dynamically loaded EDB functions
PQGETOUTRESULT PQiGetOutResult;
PQPREPAREOUT PQiPrepareOut;
PQSENDQUERYPREPAREDOUT PQiSendQueryPreparedOut;
#endif

#define lblMessage                  CTRL_STATIC("lblMessage")
#define grdParams                   CTRL("grdParams", wxGrid)
#define chkPkgInit                  CTRL_CHECKBOX("chkPkgInit")

IMPLEMENT_CLASS( dlgDirectDbg, pgDialog )

BEGIN_EVENT_TABLE( dlgDirectDbg, pgDialog )
    EVT_BUTTON( wxID_OK,                    dlgDirectDbg::OnOk )
    EVT_BUTTON( wxID_CANCEL,                dlgDirectDbg::OnCancel )    
    EVT_BUTTON( MENU_ID_SPAWN_DEBUGGER,  dlgDirectDbg::OnDebug )
    EVT_BUTTON( MENU_ID_NOTICE_RECEIVED, dlgDirectDbg::OnNoticeReceived )

    EVT_MENU( RESULT_ID_DIRECT_TARGET_COMPLETE, dlgDirectDbg::OnTargetComplete )

    EVT_CLOSE( dlgDirectDbg::OnClose )

END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////////////////
// dlgDirectDbg constructor
//
//    This class implements 'direct-debugging'. In direct-debugging, the user 
//  provides a function signature, procedure signature, or OID on the command
//  line (this identifies the debug target).  We query the server for the 
//  names, types, and in/out modes for each target parameter and then prompt
//    the user to enter a value for each of the IN (and IN/OUT) parameters.
//
//  When the user fills in the parameter values and clicks OK, we set a 
//  breakpoint at the target and then execute a SELECT statement or an 
//  EXEC statement that invokes the target (with the parameter values 
//  provided by the user).

dlgDirectDbg::dlgDirectDbg( frmDebugger *parent, wxWindowID id, const dbgConnProp & connProp )
  : m_connProp(connProp),
    m_targetInfo(NULL),
    m_conn(NULL),
    m_codeWindow(NULL),
    m_parent (parent)
{
    wxWindowBase::SetFont(settings->GetSystemFont());
    LoadResource(m_parent, wxT("dlgDirectDbg"));

    // Icon
    SetIcon(wxIcon(debugger_xpm));
    RestorePosition();

#ifdef __WXMSW__
    // Attempt to dynamically load PGgetOutResult from libpq. this
    // is only present in EDB versions.
    InitLibpq();
#endif
}

////////////////////////////////////////////////////////////////////////////////
// setupParamWindow()
//
//    This function lays out the parameter prompt window.  It contains a grid that
//  displays the name and type of each IN (and IN/OUT) parameter and a place to
//  enter a value for each of those parameters.  It also contains an OK button
//    and a CANCEL button

void dlgDirectDbg::setupParamWindow( )
{
    // Add three columns to the grid control:
    //   (Parameter) Name, Type, and Value
    grdParams->CreateGrid( 0, 3 );
    grdParams->SetColLabelValue( COL_NAME,  _( "Name" ));
    grdParams->SetColLabelValue( COL_TYPE,  _( "Type" ));
    grdParams->SetColLabelValue( COL_VALUE, _( "Value" ));
    grdParams->SetRowLabelSize( 25 );
    grdParams->SetColSize( 0, 75 );
    grdParams->SetColSize( 1, 100 );
    grdParams->SetColSize( 2, grdParams->GetClientSize().x - 210 );
    grdParams->SetColLabelSize( 18 );

    chkPkgInit->SetValue(false);
    chkPkgInit->Disable();
}

////////////////////////////////////////////////////////////////////////////////
// startDebugging()
//
//    This function initializes *this with information about the target function
//  (or procedure), constructs a grid that prompts the user for parameter values,
//  and then displays the prompt dialog to the user.  Call this function after
//  you construct a dlgDirectDbg (obviously) when you're ready to display the
//  prompt dialog to the user.

bool dlgDirectDbg::startDebugging( void )
{
    // First, figure out what kind of target we are going to debug.
    // The caller filled in our m_breakpoint list with the name and
    // type of each target that he's interested in.
    //
    // FIXME: For now, we only allow one initial breakpoint for direct
    //          debugging - you can create other breakpoints once you see
    //          the source code.

    dbgBreakPointList::Node * node = m_breakpoints.GetFirst(); 

    wxASSERT_MSG( node != NULL, wxT( "Expected to find at least one target on the command line" ));

    dbgBreakPoint * breakpoint = node->GetData();

    m_target = breakpoint->getTarget();

    char    targetType=0;

    switch( breakpoint->getTargetType())
    {
        case dbgBreakPoint::TRIGGER:     targetType = 't'; break;
        case dbgBreakPoint::FUNCTION:    targetType = 'f'; break;
        case dbgBreakPoint::PROCEDURE:   targetType = 'p'; break;
        case dbgBreakPoint::OID:         targetType = 'o'; break;
        default:
        {
            wxASSERT_MSG( false, wxT( "Unexpected target type" ));
            break;
        }
    }

    if (!loadTargetInfo( m_target, m_connProp, targetType ))
        return false;

    populateParamGrid();
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// loadTargetInfo()
//
//    This function establishes a connection to the server and creates a new
//  dbgTargetInfo object that loads information about the debug target (that is,
//  the function or procedure of interest).  Call this function with two
//  arguments: target should contain the signature of a function or procedure
//  or the OID of a function or procedure and connProp should contain the 
//  information required to connect to the server (like the hostname, port number,
//  and user name).

bool dlgDirectDbg::loadTargetInfo( const wxString &target, const dbgConnProp & connProp, char targetType )
{
    // Connect to the server using the connection properties contained in connProp

    m_conn = new dbgPgConn(m_parent, connProp);

    if( m_conn && m_conn->isConnected())
    {
        if( getenv( "DEBUGGER_INIT" ))
        {
            PQclear( m_conn->waitForCommand( wxString(getenv( "DEBUGGER_INIT" ), wxConvUTF8 )));
        }

        // Our proxy API may throw (perfectly legitimate) errors at us (for example,
        // if the target process ends while we are waiting for a breakpoint) - apparently
        // those error messages scare the user when they show up in the log, so we'll
        // just suppress logging for this session

        PQclear( m_conn->waitForCommand( wxT( "SET log_min_messages TO fatal" )));

        // Now load information about the target into m_targetInfo (note:
        // the dbgTargetInfo() constructor queries the server for all 
        // required information)

        try
        {
            m_targetInfo = new dbgTargetInfo( target, m_conn, targetType );
        }
        catch( const std::runtime_error & error )
        {
            wxLogError(wxString(error.what(), wxConvUTF8));
            m_conn->Close();
            return false;
        }

        this->SetTitle(m_targetInfo->getName());
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
// populateParamGrid()
//
//    This function reads parameter descriptions from m_targetInfo and adds a new
//  row to the grid control for each IN (or IN/OUT) parameter.  Each row displays
//    the parameter name, the data type, and an entry box where the user can type
//  in a value for that parameter

void dlgDirectDbg::populateParamGrid( )
{
    // First, try to load default values from a previous invocation into 
    // m_targetInfo (assuming that we're debugging the same target this 
    // time around)

    loadSettings();

    int i = 0;

    for( int count = 0; count < m_targetInfo->getArgCount(); ++count )
    {
        wsArgInfo & arg = ((*m_targetInfo)[count] );

        // If this is an IN parameter (or an IN/OUT parameter), add 
        // a new row to the grid

        if( arg.getMode() != wxT( "o" ))
        {
            grdParams->AppendRows( 1 );
            grdParams->SetCellValue( i, COL_NAME,  arg.getName());
            grdParams->SetCellValue( i, COL_TYPE,  arg.getType());
            grdParams->SetCellValue( i, COL_VALUE, arg.getValue());
        
            grdParams->SetReadOnly( i, COL_NAME,  true );
            grdParams->SetReadOnly( i, COL_TYPE,  true );
            grdParams->SetReadOnly( i, COL_VALUE, false );
        
            i++;
        }
    }

    // Move the cursor to the first value (so that the user
    // can just start typing)

    grdParams->SetGridCursor( 0, COL_VALUE );
    grdParams->SetFocus();

    // If the target is defined within package, offer the user
    // a chance to debug the initializer (there may or may not
    // be an initializer, we don't really know at this point)

    if( m_targetInfo->getPkgOid() == 0 )
        chkPkgInit->Disable();
    else
        chkPkgInit->Enable();

    // If the target function has no parameters (and it's not defined within
    // a package), there's no good reason to wait for the user to hit the Ok
    // button before we invoke the target...

    if((m_targetInfo->getArgInCount() + m_targetInfo->getArgInOutCount() == 0 ) && ( m_targetInfo->getPkgOid() == 0))
    {
        grdParams->AppendRows( 1 );
        grdParams->SetReadOnly( i, COL_NAME,  true );
        grdParams->SetReadOnly( i, COL_TYPE,  true );
        grdParams->SetReadOnly( i, COL_VALUE, true );

        grdParams->SetCellValue( 0, COL_NAME, _T( "No arguments required" ));
        wxFont font = grdParams->GetCellFont( 0, COL_NAME );
        font.SetStyle( wxFONTSTYLE_ITALIC );
        grdParams->SetCellFont( 0, COL_NAME, font );

        grdParams->AutoSizeColumns();
        activateDebugger();
    }
    else
        this->ShowModal();
}

////////////////////////////////////////////////////////////////////////////////
// OnOk()
//
//    This event handler is called when the user clicks the OK button - we call the
//  activateDebugger() function to set the required breakpoint and invoke the 
//  target (after nabbing any parameter values from the prompt dialog)

void dlgDirectDbg::OnOk( wxCommandEvent & event )
{
    activateDebugger();
}

////////////////////////////////////////////////////////////////////////////////
// loadSettings()
//
//    Loads default values from our .ini file. We save the OID of the most 
//    recent direct-debugging target when close a session. If we're direct-
//    debugging the same target this time around, we load the argument values
//    from the .ini file.

void dlgDirectDbg::loadSettings()
{
    long        lastOID;
    
    settings->Read( wxT( "Debugger/Proc/OID" ), &lastOID, -1 );

    if( lastOID == m_targetInfo->getOid())
    {
        int    count = 0;

        for( int i = 0; i < m_targetInfo->getArgCount(); ++i )
        {
            wsArgInfo & arg = (*m_targetInfo)[i];

            if( arg.getMode() != wxT( "o" ))
            {
                settings->Read( wxString::Format( wxT( "Debugger/Proc/argValue%d" ), ++count ), &(arg.getValue()), wxT( "" ));
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// saveSettings()
//
//    Save default values to our .ini file. We save the OID of the most 
//    recent direct-debugging target when close a session. We also save the 
//  value of each argument - if you debug the same target again next time,
//    loadSettings() will initialize the parameter-values window with the 
//    same parameter values that you entered in this session.

void dlgDirectDbg::saveSettings()
{
    settings->Write( wxT( "Debugger/Proc/OID" ), m_targetInfo->getOid());

    int    count = 0;

    for( int i = 0; i < m_targetInfo->getArgCount(); ++i )
    {
        wsArgInfo & arg = ( *m_targetInfo)[i];

        if( arg.getMode() != wxT( "o" ))
        {
            settings->Write( wxString::Format( wxT( "Debugger/Proc/argName%d" ), ++count ), arg.getName());
            settings->Write( wxString::Format( wxT( "Debugger/Proc/argType%d" ),   count ), arg.getType());
            settings->Write( wxString::Format( wxT( "Debugger/Proc/argValue%d" ),  count ), (arg.getValue() == wxT("NULL") ? wxEmptyString : arg.getValue().c_str()));
        }
    }

    settings->Flush();
}

////////////////////////////////////////////////////////////////////////////////
// OnCancel()
//
//    This event handler is called when the user clicks the Cancel button - we
//    close the connection to the server and then close ourself.

void dlgDirectDbg::OnCancel( wxCommandEvent & event )
{
    // This will raise close event which is handled by 
    // dlgDirectDbg::OnClose().
    Close();
}

////////////////////////////////////////////////////////////////////////////////
// OnClose()
//
//    wxWidgets invokes this event handler when the user closes the parameter
//    window. We close the connection with server and raise close event for 
//    MainFrame.

void dlgDirectDbg::OnClose( wxCloseEvent & event )
{
    // Destroy the grid - required as it seems to create threads in some cases
    if (grdParams)
    {
        grdParams->Destroy();
        delete grdParams;
    }

    // Close the debugger (proxy) connection
    if (m_conn)
        m_conn->Close();
    m_conn = NULL;

    // This will inform the MainWindow to close.
    // if it's not visible yet.
    if (m_parent->IsShown())
        event.Skip();
    else
        m_parent->Close();
}

////////////////////////////////////////////////////////////////////////////////
// activateDebugger()
//
//    This function is called after the user has filled in any parameter values
//  and clicked the Ok button.  activateDebugger() extracts the paramter values
//  from the grid control and copies those values into our dbgTargetInfo object
//  (m_targetInfo).  Next, we set a breakpoint at the target, and, finally, 
//  we invoke the target function/procedure

bool dlgDirectDbg::activateDebugger( )
{
    // Unset the completed flag (if it exists)
    if (m_codeWindow)
        m_codeWindow->m_targetComplete = false;

    // Copy the parameter values from the grid into m_targetInfo
    int i = 0;

    for( int count = 0; count < m_targetInfo->getArgCount(); ++count )
    {
        wsArgInfo & arg = (*m_targetInfo)[count];

        // Populate the ArgInfo object's IN or INOUT variables only, OUT
        // variables will be assigned NULL later on.

        if(arg.getMode() != wxT("o"))
        {
            arg.setValue( grdParams->GetCellValue(i, COL_VALUE));
            i++;
        }    
    }

    // Write the target OID and argument values to our settings file
    // so that we can default them next time around
    saveSettings();

    // Now set a breakpoint at the target (note: the call to setBreakpoint()
    // will hang until the  server sends us a response)
    
    try
    {
        // Debug the initialiser. We can only do so once, so unset, and disable
        // the option after setting the breakpoint
        if( chkPkgInit->GetValue())
            setBreakpoint( m_targetInfo->getPkgOid(), m_targetInfo->getPkgInitOid());

        chkPkgInit->SetValue(false);
        chkPkgInit->Disable();

        setBreakpoint( m_targetInfo->getPkgOid(), m_targetInfo->getOid());
    }
    catch( const std::runtime_error & error )
    {
        wxMessageBox( wxString( error.what(), wxConvUTF8 ), _( "Cannot create breakpoint" ), wxOK | wxICON_ERROR );
        return( false );
    }

    // And invoke the target (note: the call to invokeTarget() will *NOT*
    // wait for a result set from the server - instead, OnResultReady() will
    // be called when the result set is ready)

    try
    {
        invokeTarget();
    }
    catch( const std::runtime_error & error )
    {
        wxMessageBox( wxString( error.what(), wxConvUTF8 ), _( "Cannot invoke target" ), wxOK | wxICON_ERROR );
        return( false );
    }

    return( true );
}

////////////////////////////////////////////////////////////////////////////////
// setBreakpoint()
//
//    This function creates a breakpoint at the target.  For now, we always create
//  a breakpoint by calling edb_procoid_debug() or plpgsql_procoid_debug() with 
//  the OID of the target.  Later, we'll change this function to use the 
//  new CREATE BREAKPOINT command.

void dlgDirectDbg::setBreakpoint(long pkgOid, long funcOid)
{
    dbgResultset * result;

    if (m_conn->DebuggerApiVersion() <= DEBUGGER_V2_API)
    {
        if( m_targetInfo->getLanguage() == wxT( "edbspl" ))
            result = new dbgResultset(m_conn->waitForCommand(wxString::Format(wxT("select edb_oid_debug( %ld, %ld );"), pkgOid, funcOid)));
        else
            result = new dbgResultset(m_conn->waitForCommand(wxString::Format(wxT("select plpgsql_oid_debug( %ld, %ld );"),  pkgOid, funcOid)));
    }
    else
    {
        if( m_targetInfo->getLanguage() == wxT( "edbspl" ))
            result = new dbgResultset(m_conn->waitForCommand(wxString::Format(wxT("select edb_oid_debug(%ld);"), funcOid)));
        else
            result = new dbgResultset(m_conn->waitForCommand(wxString::Format(wxT("select plpgsql_oid_debug(%ld);"), funcOid)));
    }

    if( result->getCommandStatus() != PGRES_TUPLES_OK )
        throw( std::runtime_error( result->getRawErrorMessage()));
}

////////////////////////////////////////////////////////////////////////////////
// invokeTarget()
//
//    This function invokes the debugger target (that is, the function or procedure
//  that the user wants to debug).  If the target is a function, we generate a 
//  SELECT statement; if the target is a procedure, we generate an EXEC statement.
//  In either case, we build the argument list from the argument values found
//  in m_targetInfo

void dlgDirectDbg::invokeTarget()
{
    // If we have access the the EDB extended libpq functions,
    // and this is a stored procedure, we should execute the
    // procedure using the callable statement interface to allow
    // us to retrieve the OUT/INOUT parameter results.
    // Otherwise, just SELECT/EXEC it as per normal.
#ifdef __WXMSW__
    if (!m_targetInfo->getIsFunction() &&
        PQiGetOutResult && 
        PQiPrepareOut && 
        PQiSendQueryPreparedOut)
        invokeTargetCallable();
    else
#else
#ifdef EDB_LIBPQ
    if (!m_targetInfo->getIsFunction())
        invokeTargetCallable();
    else
#endif
#endif
        invokeTargetStatement();

    // Since parameter window has done its job, we need to hide
    // it and let code window come in front.
    if (m_codeWindow) 
    {
        m_codeWindow->enableTools();
        m_codeWindow->resumeLocalDebugging();
    }
    
    this->Show( false );
}

void dlgDirectDbg::invokeTargetCallable()
{
    dbgPgParams *params = new dbgPgParams();

    wxString query = wxT("CALL ") + m_targetInfo->getFQName() + wxT("(");

    // Setup the param struct.
    params->nParams = m_targetInfo->getArgCount();
    params->paramTypes = new Oid[params->nParams];
    params->paramValues = new char*[params->nParams];
    params->paramModes = new int[params->nParams];

    // Iterate through the parameters, adding them to the param 
    // struct and the statement as we go.
    for( int i = 0; i < params->nParams; ++i )
    {
        wsArgInfo &arg = (*m_targetInfo)[i];

        params->paramTypes[i] = arg.getTypeOid();

        if(arg.getMode() == wxT("o")) // OUT
        {
            params->paramModes[i] = 2;
            params->paramValues[i] = 0;
        }
        else if(arg.getMode() == wxT("b")) // IN OUT
        {
            params->paramModes[i] = 3;

            int len = arg.getValue().Length() + 1;
            char *tmp = new char[len];
            snprintf(tmp, len, "%s", (const char *)arg.getValue().mb_str(wxConvUTF8));
            if (strcmp(tmp, "") == 0)
                params->paramValues[i] = 0;
            else if (strcmp(tmp, "''") == 0)
                params->paramValues[i] = "";
            else if (strcmp(tmp, "\\'\\'") == 0)
                params->paramValues[i] = "''";
            else
                params->paramValues[i] = tmp;
        }
        else // IN
        {
            params->paramModes[i] = 1;

            int len = arg.getValue().Length() + 1;
            char *tmp = new char[len];
            snprintf(tmp, len, "%s", (const char *)arg.getValue().mb_str(wxConvUTF8));
            if (strcmp(tmp, "") == 0)
                params->paramValues[i] = 0;
            else if (strcmp(tmp, "''") == 0)
                params->paramValues[i] = "";
            else if (strcmp(tmp, "\\'\\'") == 0)
                params->paramValues[i] = "''";
            else
                params->paramValues[i] = tmp;
        }

            if (i)
                query += wxT(", ");
            query += wxString::Format(wxT("$%d"), i + 1);

    }

    query += wxT(");");

    // And send the completed command to the server - we'll get 
    // a dbgDbResult event when the command completes (and that 
    // event will get routed to dlgDirectDbg::OnResultReady())
    m_conn->startCommand( query, GetEventHandler(), RESULT_ID_DIRECT_TARGET_COMPLETE, params );
}

void dlgDirectDbg::invokeTargetStatement()
{
    wxString query( m_targetInfo->getIsFunction() ? wxT( "SELECT " ) : wxT( "EXEC " ));

    // If this is a function, and the return type is not record, or 
    // we have at least one OUT/INOUT param, we should select from
    // the function to get a full resultset.
    if (m_targetInfo->getIsFunction() &&
        (m_targetInfo->getReturnType() != wxT("record") || 
         m_targetInfo->getArgInOutCount() > 0 ||
         m_targetInfo->getArgOutCount() > 0))
         query.Append(wxT("* FROM "));

    // Stuff the verb (SELECT or EXEC), schema, and target name into the query
    query.Append(m_targetInfo->getFQName());

    // Now append the argument list
    query.Append(wxT("("));

    for( int i = 0; i < m_targetInfo->getArgCount(); ++i )
    {
        wsArgInfo & arg = (*m_targetInfo)[i];

        if( arg.getMode() == wxT("o"))
        {
            if (!m_targetInfo->getIsFunction())
                query.Append( wxT("NULL::") + arg.getType() + wxT(", "));
        }
        else
            query.Append( arg.quoteValue() + wxT("::") + arg.getType() + wxT(", "));
    }

    if (query.EndsWith(wxT(", ")))
        query = query.Left(query.Length() - 2);
    else if (query.EndsWith(wxT("(")))
        query = query.Left(query.Length() - 1);

    // And terminate the argument list
    if( m_targetInfo->getArgInCount() + m_targetInfo->getArgInOutCount() == 0 )
    {
        if( m_targetInfo->getIsFunction())
            query.Append( wxT( "()" ));
    }
    else
    {
        query.Append( wxT( ")" ));
    }

    // And send the completed command to the server - we'll get 
    // a dbgDbResult event when the command completes (and that 
    // event will get routed to dlgDirectDbg::OnResultReady())
    m_conn->startCommand( query, GetEventHandler(), RESULT_ID_DIRECT_TARGET_COMPLETE );
}


////////////////////////////////////////////////////////////////////////////////
// OnTargetComplete()
//
//    This event handler is called when the target function/procedure completes
//  and a result set (or error) has been returned by the server.  The event
//  object contains a pointer to the result set.
//
//  For now, we display an error message (if an error occurred) or write the
//  command status to the status bar (if the target completed without error).
//
//  We should really display the complete result set somewhere too.

void dlgDirectDbg::OnTargetComplete( wxCommandEvent & event )
{
    // Extract the result set handle from the event and log the status info

    PGresult   * result = (PGresult *)event.GetClientData();

    wxLogDebug( wxT( "OnTargetComplete() called\n" ));
    wxLogDebug( wxT( "%s\n" ), wxString(PQresStatus( PQresultStatus( result )), wxConvUTF8).c_str());

    // If the query failed, write the error message to the status line, otherwise, copy the result set into the grid
    if(( PQresultStatus( result ) == PGRES_NONFATAL_ERROR ) || ( PQresultStatus( result ) == PGRES_FATAL_ERROR ))
    {
        wxString    message( PQresultErrorMessage( result ), wxConvUTF8 );

        message.Replace( wxT( "\n" ), wxT( " " ));

        m_parent->getStatusBar()->SetStatusText( message, 1 );
        char *state = PQresultErrorField(result,PG_DIAG_SQLSTATE);

        // Don't bother telling the user that he aborted - he already knows!
        if (strcmp(state, "57014"))
            wxLogError( wxT( "%s\n" ), wxString(PQerrorMessage(m_conn->getConnection()), wxConvUTF8).c_str());
        else
            wxLogDebug( wxT( "%s\n" ), wxString(PQerrorMessage(m_conn->getConnection()), wxConvUTF8).c_str());
    }
    else
    {
        wxString message( PQcmdStatus( result ), wxConvUTF8 );

        message.Replace( wxT( "\r" ), wxT( "" ));
        message.Replace( wxT( "\n" ), wxT( " " ));

        m_parent->getStatusBar()->SetStatusText( message, 1 );

        // If this result set has any columns, add a result grid to the code window so
        // we can show the results to the user

        if( m_codeWindow && PQnfields( result ))
        {
            m_codeWindow->OnResultSet( result );
        }
    }

    if (m_codeWindow)
    {
        m_codeWindow->m_targetComplete = true;
        m_codeWindow->disableTools( );
    }
    this->Show( true );
    this->SetFocus();
}

////////////////////////////////////////////////////////////////////////////////
// OnNoticeReceived()
//
//    This event handler is called when a notice is received from the server (in
//  response to our invoking the target).  For now, we just forward this event
//  to the debugger window (m_codeWindow) and the notification message is added
//  to the debugger's message window.  
//
//  When/if we get around to adding a result set window to this class, we should
//  also add a message window too and display notice messages here instead of in
//  the debugger window.

void dlgDirectDbg::OnNoticeReceived( wxCommandEvent & event )
{   
    if( m_codeWindow )
        m_codeWindow->OnNoticeReceived( event );
}

////////////////////////////////////////////////////////////////////////////////
// OnDebug()
//
//    This event handler is called when a PLDBGBREAK notice is received from the
//  server.  A quick review:  we've already set a breakpoint at the target and 
//  then we invoked the target (using the parameter values entered by the user).
//  Now we're waiting for a result set from the target.  Since we set a breakpoint
//  inside of the target, the server will send us a specially crafted NOTICE 
//  that tells use which port to attach to in order to contact the debugger 
//  server - that's what 'event' contains.
//
//  When we get the PLDBGBREAK message (inside of 'event'), we create a new
//  debugger window by calling glMainFrame->addDebug() and let that window 
//  take over for a while.  When the target finally completes, we'll get a 
//  a dbgDbResult event and handle the result set inside of OnResultReady()

void dlgDirectDbg::OnDebug( wxCommandEvent & event )
{
    // This event contains a string of the form:
    //     /path/debugger -k --database=db --host=host --port=port --user=user &"
    // We can use that string to launch a separate debugger client.
    //
    // The event also contains a pointer to a map that contains keyword=value
    // pairs for the debugger connection properties.  To get to that map, we
    // call event.GetClientData().  Once we have the map, we can look for the
    // debugger connection properties such as "database", "host", "port", ...

    dbgConnProp * debugProps = (dbgConnProp *)event.GetClientData();

    m_codeWindow = m_parent->addDebug( *debugProps );

    m_codeWindow->startLocalDebugging();

    this->Show( false );
}

////////////////////////////////////////////////////////////////////////////////
// getBreakpointList()
//
//    This function returns a non-const reference to our breakpoint list.  The
//  caller typically populates this list before calling startDebugging() - we
//  set a breakpoint for each member of the list

dbgBreakPointList & dlgDirectDbg::getBreakpointList()
{ 
    return( m_breakpoints ); 
}

////////////////////////////////////////////////////////////////////////////////
// InitLibpq()
//
//    Dynamically load EDB-specific functions from libpq

#ifdef __WXMSW__
void dlgDirectDbg::InitLibpq()
{
    HINSTANCE hinstLib;
 
    // Get a handle to the DLL module.
    hinstLib = LoadLibrary(TEXT("libpq")); 
 
    // If the handle is valid, try to get the function address.
    if (hinstLib != NULL) 
    { 
        PQiGetOutResult = (PQGETOUTRESULT) GetProcAddress(hinstLib, "PQgetOutResult"); 
        PQiPrepareOut = (PQPREPAREOUT) GetProcAddress(hinstLib, "PQprepareOut"); 
        PQiSendQueryPreparedOut = (PQSENDQUERYPREPAREDOUT) GetProcAddress(hinstLib, "PQsendQueryPreparedOut"); 
 
        // If the function address is valid, call the function.
        if (PQiGetOutResult != NULL) 
            wxLogInfo(wxT("Using runtime dynamically linked EDB libpq functions."));
        else
            wxLogInfo(wxT("EDB libpq functions are not available."));
    }
}
#endif
