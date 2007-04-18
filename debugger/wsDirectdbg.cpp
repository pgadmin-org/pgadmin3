//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsdirectdbg.cpp - debugger 
//
//////////////////////////////////////////////////////////////////////////

#include <wx/grid.h>		// For wxGrid
#include <wx/log.h>		// For wxLogDebug
#include <wx/msgdlg.h>		// For wxMessageBox
#include <wx/stattext.h>	// For wxStaticText
#include <wx/sizer.h>		// For wxBoxSizer
#include <wx/checkbox.h>	// For wxCheckBox
#include <wx/debug.h>		// For wxASSERT_MSG

#include "wsDirectdbg.h"
#include "debugger.h"
#include "wsMainFrame.h"
#include "wsTargetInfo.h"
#include "wsPgconn.h"
#include "wsResultGrid.h"
#include "wsResultset.h"
#include "wsConst.h"
#include "wsDbresult.h"
#include "wsCodeWindow.h"
#include "debuggerMenu.h"

#include <stdexcept>

IMPLEMENT_CLASS( wsDirectDbg, wxDialog )

BEGIN_EVENT_TABLE( wsDirectDbg, wxDialog )
    EVT_BUTTON( wxID_OK,       			 wsDirectDbg::OnOk )
    EVT_BUTTON( wxID_CANCEL,   			 wsDirectDbg::OnCancel )    
    EVT_BUTTON( MENU_ID_SPAWN_DEBUGGER,  wsDirectDbg::OnDebug )
    EVT_BUTTON( MENU_ID_NOTICE_RECEIVED, wsDirectDbg::OnNoticeReceived )

    EVT_MENU( RESULT_ID_DIRECT_TARGET_COMPLETE, wsDirectDbg::OnTargetComplete )
    EVT_MENU( RESULT_ID_RESULT_SET_READY,       wsDirectDbg::OnResultReady )

    EVT_CLOSE( wsDirectDbg::OnClose )

END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////////////////
// wsDirectDbg constructor
//
//	This class implements 'direct-debugging'. In direct-debugging, the user 
//  provides a function signature, procedure signature, or OID on the command
//  line (this identifies the debug target).  We query the server for the 
//  names, types, and in/out modes for each target parameter and then prompt
//	the user to enter a value for each of the IN (and IN/OUT) parameters.
//
//  When the user fills in the parameter values and clicks OK, we set a 
//  breakpoint at the target and then execute a SELECT statement or an 
//  EXEC statement that invokes the target (with the parameter values 
//  provided by the user).

wsDirectDbg::wsDirectDbg( wxDocParentFrame * parent, wxWindowID id, const wsConnProp & connProp )
	: wxDialog( parent, id,  connProp.m_host + wxT( "/" ) + connProp.m_database, 
	wxDefaultPosition, wxDefaultSize, wxRESIZE_BORDER | wxCAPTION  ),
	m_target(),
	m_connProp( connProp ),
	m_targetInfo( NULL ),
	m_grid( NULL ),
	m_conn( NULL ),
	m_codeWindow( NULL )
{
    wxWindowBase::SetFont(glApp->GetSystemFont());

	setupParamWindow();
}

////////////////////////////////////////////////////////////////////////////////
// setupParamWindow()
//
//	This function lays out the parameter prompt window.  It contains a grid that
//  displays the name and type of each IN (and IN/OUT) parameter and a place to
//  enter a value for each of those parameters.  It also contains an OK button
//	and a CANCEL button

void wsDirectDbg::setupParamWindow( )
{
	wxStaticText * txtMessage  = new wxStaticText( this, ID_TXTMESSAGE, _( "Please enter argument values here. Press Enter after each value." ), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	wxBoxSizer   * topSizer    = new wxBoxSizer( wxVERTICAL ); 
	wxBoxSizer   * buttonSizer = new wxBoxSizer( wxHORIZONTAL );   

	// Create the grid control and add three columns:
	//	(Parameter) Name, Type, and Value

	m_grid = new wxGrid( this, ID_GRDFUNCARGS, wxDefaultPosition, wxSize( 380, 120));
	
	m_grid->CreateGrid( 0, 3 );
	m_grid->SetColLabelValue( COL_NAME,  _( "Name" ));
	m_grid->SetColLabelValue( COL_TYPE,  _( "Type" ));
	m_grid->SetColLabelValue( COL_VALUE, _( "Value" ));
	m_grid->SetRowLabelSize( 40 );
	m_grid->SetColSize( 0, 110 );
	m_grid->SetColSize( 1, 105 );
	m_grid->SetColSize( 2, 114 );

	m_debugInitializer = new wxCheckBox( this, ID_DEBUG_INITIALIZER, _( "Debug package initializer?" ));
	m_debugInitializer->SetValue(false);
	m_debugInitializer->Hide();

	// Add a prompt to the top of the window and then add 
	// the grid control

	topSizer->Add( txtMessage, 0, wxGROW, 0 );      
	topSizer->Add( m_grid, 1, wxEXPAND | wxALL, 7 );      
	// Now add an OK button and a Cancel button

	buttonSizer->Add( m_debugInitializer, 0, wxALL, 7 );
	buttonSizer->Add( new wxButton( this, wxID_OK, _( "OK" )), 0, wxALL, 7 );      
	buttonSizer->Add( new wxButton( this, wxID_CANCEL, _( "Cancel" )), 0, wxALL, 7 );      

	topSizer->Add( buttonSizer, 0, wxALIGN_CENTER ); 

	// And size everything to fit within the window
	SetSizer( topSizer ); 			// use the sizer for layout 
	topSizer->Fit( this );          // fit the dialog to the contents 
	topSizer->SetSizeHints( this ); // set hints to honor min size 	

}

////////////////////////////////////////////////////////////////////////////////
// startDebugging()
//
//	This function initializes *this with information about the target function
//  (or procedure), constructs a grid that prompts the user for parameter values,
//  and then displays the prompt dialog to the user.  Call this function after
//  you construct a wsDirectDbg (obviously) when you're ready to display the
//  prompt dialog to the user.

void wsDirectDbg::startDebugging( void )
{
	// First, figure out what kind of target we are going to debug.
	// The caller filled in our m_breakpoint list with the name and
	// type of each target that he's interested in.
	//
	// FIXME: For now, we only allow one initial breakpoint for direct
	//		  debugging - you can create other breakpoints once you see
	//		  the source code.

	wsBreakpointList::Node * node = m_breakpoints.GetFirst(); 

	wxASSERT_MSG( node != NULL, _( "Expected to find at least one target on the command line" ));

	wsBreakpoint * breakpoint = node->GetData();

	m_target = breakpoint->getTarget();

	char	targetType;

	switch( breakpoint->getTargetType())
	{
		case wsBreakpoint::TRIGGER: 	targetType = 't'; break;
		case wsBreakpoint::FUNCTION: 	targetType = 'f'; break;
		case wsBreakpoint::PROCEDURE: 	targetType = 'p'; break;
		case wsBreakpoint::OID: 		targetType = 'o'; break;
		default:
		{
			wxASSERT_MSG( false, _( "Unexpected target type" ));
			break;
		}
	}

	loadTargetInfo( m_target, m_connProp, targetType );
	populateParamGrid();
}

////////////////////////////////////////////////////////////////////////////////
// loadTargetInfo()
//
//	This function establishes a connection to the server and creates a new
//  wsTargetInfo object that loads information about the debug target (that is,
//  the function or procedure of interest).  Call this function with two
//  arguments: target should contain the signature of a function or procedure
//  or the OID of a function or procedure and connProp should contain the 
//  information required to connect to the server (like the hostname, port number,
//  and user name).

void wsDirectDbg::loadTargetInfo( const wxString & target, const wsConnProp & connProp, char targetType )
{
	// Connect to the server using the connection properties contained in connProp

	m_conn = new wsPgConn( connProp );

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
		// the wsTargetInfo() constructor queries the server for all 
		// required information)

		try
		{
			m_targetInfo = new wsTargetInfo( target, m_conn, targetType );
		}
		catch( std::runtime_error & e )
		{
			m_conn->close();

			throw( e );
		}

		this->SetTitle( m_connProp.m_host + wxT( "/" ) + m_connProp.m_database + wxT( "/" ) + m_targetInfo->getName());
	}
}

////////////////////////////////////////////////////////////////////////////////
// populateParamGrid()
//
//	This function reads parameter descriptions from m_targetInfo and adds a new
//  row to the grid control for each IN (or IN/OUT) parameter.  Each row displays
//	the parameter name, the data type, and an entry box where the user can type
//  in a value for that parameter

void wsDirectDbg::populateParamGrid( )
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
			m_grid->AppendRows( 1 );
			m_grid->SetCellValue( i, COL_NAME,  arg.getName());
			m_grid->SetCellValue( i, COL_TYPE,  arg.getType());
			m_grid->SetCellValue( i, COL_VALUE, arg.getValue());
		
			m_grid->SetReadOnly( i, COL_NAME,  true );
			m_grid->SetReadOnly( i, COL_TYPE,  true );
			m_grid->SetReadOnly( i, COL_VALUE, false );
		
			i++;
		}
	}

	// Move the cursor to the first value (so that the user
	// can just start typing)

	m_grid->SetGridCursor( 0, COL_VALUE );
	m_grid->SetFocus();

	// If the target is defined within package, offer the user
	// a chance to debug the initializer (there may or may not
	// be an initializer, we don't really know at this point)

	if( m_targetInfo->getPkgOid() == 0 )
		m_debugInitializer->Hide();
	else
		m_debugInitializer->Show();

	// If the target function has no parameters (and it's not defined within
	// a package), there's no good reason to wait for the user to hit the Ok
	// button before we invoke the target...

	if(( m_targetInfo->getArgCount() == 0 ) && ( m_targetInfo->getPkgOid() == 0 ))
	{
		wxStaticText * txtMessage = (wxStaticText *)FindWindow( ID_TXTMESSAGE );

		if( txtMessage )
			txtMessage->SetLabel( wxString::Format( _( "This %s requires no argument values\nClick 'OK' to invoke %s" ), m_targetInfo->getIsFunction() ? _("function") : _("procedure"), m_targetInfo->getFQName().c_str()));

		m_grid->AppendRows( 1 );
		m_grid->SetReadOnly( i, COL_NAME,  true );
		m_grid->SetReadOnly( i, COL_TYPE,  true );
		m_grid->SetReadOnly( i, COL_VALUE, true );

		m_grid->SetCellValue( 0, COL_NAME, _T( "   No arguments required   " ));

		wxFont	font = m_grid->GetCellFont( 0, COL_NAME );
		
		font.SetStyle( wxFONTSTYLE_ITALIC );

		m_grid->SetCellFont( 0, COL_NAME, font );
		m_grid->AutoSizeColumns();
		activateDebugger();

	}
}

////////////////////////////////////////////////////////////////////////////////
// OnOk()
//
//	This event handler is called when the user clicks the OK button - we call the
//  activateDebugger() function to set the required breakpoint and invoke the 
//  target (after nabbing any parameter values from the prompt dialog)

void wsDirectDbg::OnOk( wxCommandEvent & event )
{
	if( activateDebugger())
	{
#if 0
		glMainFrame->SetLastSize( );
		this->Show( false );
		glMainFrame->getToolBar( )->Show( true );
#endif
	}
}

////////////////////////////////////////////////////////////////////////////////
// loadSettings()
//
//	Loads default values from our .ini file. We save the OID of the most 
//	recent direct-debugging target when close a session. If we're direct-
//	debugging the same target this time around, we load the argument values
//	from the .ini file.

void wsDirectDbg::loadSettings()
{
	wxConfig  & settings = glApp->getSettings();
	long		lastOID;
	
	settings.Read( wxT( "Debugger/Proc/OID" ), &lastOID, -1 );

	if( lastOID == m_targetInfo->getOid())
	{
		int	count = 0;

		for( int i = 0; i < m_targetInfo->getArgCount(); ++i )
		{
			wsArgInfo & arg = (*m_targetInfo)[i];

			if( arg.getMode() != wxT( "o" ))
			{
				settings.Read( wxString::Format( wxT( "Debugger/Proc/argValue%d" ), ++count ), &(arg.getValue()), wxT( "" ));
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// saveSettings()
//
//	Save default values to our .ini file. We save the OID of the most 
//	recent direct-debugging target when close a session. We also save the 
//  value of each argument - if you debug the same target again next time,
//	loadSettings() will initialize the parameter-values window with the 
//	same parameter values that you entered in this session.

void wsDirectDbg::saveSettings()
{
	wxConfig & settings = glApp->getSettings();

	settings.Write( wxT( "Debugger/Proc/OID" ), m_targetInfo->getOid());

	int	count = 0;

	for( int i = 0; i < m_targetInfo->getArgCount(); ++i )
	{
		wsArgInfo & arg = ( *m_targetInfo)[i];

		if( arg.getMode() != wxT( "o" ))
		{
			settings.Write( wxString::Format( wxT( "Debugger/Proc/argName%d" ), ++count ), arg.getName());
			settings.Write( wxString::Format( wxT( "Debugger/Proc/argType%d" ),   count ), arg.getType());
			settings.Write( wxString::Format( wxT( "Debugger/Proc/argValue%d" ),  count ), arg.getValue());
		}
	}

	settings.Flush();
}

////////////////////////////////////////////////////////////////////////////////
// OnCancel()
//
//	This event handler is called when the user clicks the Cancel button - we
//	close the connection to the server and then close ourself.

void wsDirectDbg::OnCancel( wxCommandEvent & event )
{
// This piece of code is disabled as it is repeated in wsDirectDbg::OnClose().
// We simply raise close event to notify wsDirectDbg::OnClose().

#if 0
	if( m_conn )
		m_conn->close();

#endif

	// This will raise close event which is handled by 
	// wsDirectDbg::OnClose().
	Close();
}

////////////////////////////////////////////////////////////////////////////////
// OnClose()
//
//	wxWidgets invokes this event handler when the user closes the parameter
//	window. We close the connection with server and raise close event for 
//	MainFrame.

void wsDirectDbg::OnClose( wxCloseEvent & event )
{
	if( m_conn )
		m_conn->close();
			
	// This will inform the MainWindow to close.
	glMainFrame->Close();
}

////////////////////////////////////////////////////////////////////////////////
// activateDebugger()
//
//	This function is called after the user has filled in any parameter values
//  and clicked the Ok button.  activateDebugger() extracts the paramter values
//  from the grid control and copies those values into our wsTargeInfo object
//  (m_targetInfo).  Next, we set a breakpoint at the target, and, finally, 
//	we invoke the target function/procedure

bool wsDirectDbg::activateDebugger( )
{
	// Copy the parameter values from the grid into m_targetInfo
	int i = 0;

	for( int count = 0; count < m_targetInfo->getArgCount(); ++count )
	{
		wsArgInfo & arg = (*m_targetInfo)[count];

		// Populate the ArgInfo object's IN or INOUT variables only, OUT
		// variables will be assigned NULL later on.

		if( arg.getMode() != wxT( "o" ))
		{
			if( m_grid->GetCellValue( i, COL_VALUE ) == wxT( "" ))
				arg.setValue( wxT( "NULL" ));
			else
				arg.setValue( m_grid->GetCellValue( i, COL_VALUE ));
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
		if( m_debugInitializer->GetValue())
			setBreakpoint( m_targetInfo->getPkgOid(), m_targetInfo->getPkgOid());

		setBreakpoint( m_targetInfo->getPkgOid(), m_targetInfo->getOid());
	}
	catch( const std::runtime_error & error )
	{
		wxMessageBox( wxString( error.what(), wxConvUTF8 ), _( "Can't Create Breakpoint" ), wxOK | wxICON_ERROR );
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
		wxMessageBox( wxString( error.what(), wxConvUTF8 ), _( "Can't Invoke Target" ), wxOK | wxICON_ERROR );
		return( false );
	}

	return( true );
}

////////////////////////////////////////////////////////////////////////////////
// setBreakpoint()
//
//	This function creates a breakpoint at the target.  For now, we always create
//  a breakpoint by calling edb_procoid_debug() or plpgsql_procoid_debug() with 
//  the OID of the target.  Later, we'll change this function to use the 
//  new CREATE BREAKPOINT command.

void wsDirectDbg::setBreakpoint( long pkgOid, long funcOid )
{
	wsResultSet * result;

	if( m_targetInfo->getLanguage() == wxT( "edbspl" ))
		result = new wsResultSet( m_conn->waitForCommand( wxString::Format( wxT( "select edb_oid_debug( %ld, %ld );" ), pkgOid, funcOid )));
	else
		result = new wsResultSet( m_conn->waitForCommand( wxString::Format( wxT( "select plpgsql_oid_debug( %ld, %ld );" ),  pkgOid, funcOid )));

	if( result->getCommandStatus() != PGRES_TUPLES_OK )
		throw( std::runtime_error( result->getRawErrorMessage()));
}

////////////////////////////////////////////////////////////////////////////////
// invokeTarget()
//
//	This function invokes the debugger target (that is, the function or procedure
//  that the user wants to debug).  If the target is a function, we generate a 
//  SELECT statement; if the target is a procedure, we generate an EXEC statement.
//  In either case, we build the argument list from the argument values found
//  in m_targetInfo

void wsDirectDbg::invokeTarget()
{
	wxString	 query( m_targetInfo->getIsFunction() ? wxT( "SELECT " ) : wxT( "EXEC " ));
	wxString delimiter = wxT( "( " );

	// Stuff the verb (SELECT or EXEC), schema, and target name into the query

	query.Append( m_targetInfo->getFQName());

	// Now append the argument list

	for( int i = 0; i < m_targetInfo->getArgCount(); ++i )
	{
		wsArgInfo & arg = (*m_targetInfo)[i];

		query.Append( delimiter );

		if( arg.getMode() == wxT( "o" ))
			query.Append( wxT( "NULL::" ) + arg.getType());
		else
			query.Append( arg.quoteValue() + wxT( "::" ) + arg.getType());

		delimiter = wxT( ", " );
	}

	// And terminate the argument list
	if( m_targetInfo->getArgCount() == 0 )
	{
		if( m_targetInfo->getIsFunction())
			query.Append( wxT( "()" ));
	}
	else
	{
		query.Append( wxT( " )" ));
	}

	// And send the completed command to the server - we'll get 
	// a wsDBRESULT event when the command completes (and that 
	// event will get routed to wsDirectDbg::OnResultReady())

	m_conn->startCommand( query, GetEventHandler(), RESULT_ID_DIRECT_TARGET_COMPLETE );
	
	// Since parameter window has done its job, we need to hide
	// it and let code window come in front.
	if (m_codeWindow) 
	{
		m_codeWindow->Show( true );
		m_codeWindow->SetFocus();
		m_codeWindow->enableTools();
		
		m_codeWindow->resumeLocalDebugging();
	}
	
	this->Show( false );
}

////////////////////////////////////////////////////////////////////////////////
// OnResultReady()
//
//	This event handler is called when the target function/procedure completes
//  and a result set (or error) has been returned by the server.  The event
//  object contains a pointer to the result set.
//
//  For now, we display an error message (if an error occurred) or write the
//  command status to the status bar (if the target completed without error).
//
//  We should really display the complete result set somewhere too.

void wsDirectDbg::OnResultReady( wxCommandEvent & event )
{
	::wxLogDebug( _( "OnResultReady() called\n" ));

	// Extract the result set handle from the event and log the status info

	PGresult *result = (PGresult *)event.GetClientData();

	::wxLogDebug( wxT( "%s\n" ), PQresStatus( PQresultStatus( result )));

	// If the query failed, write the error message to the status line, otherwise, copy the result set into the grid

	if(( PQresultStatus( result ) == PGRES_NONFATAL_ERROR ) || ( PQresultStatus( result ) == PGRES_FATAL_ERROR ))
	{
		wxString message( PQresultErrorMessage( result ), wxConvUTF8 ) ;

		message.Replace( wxT( "\r" ), wxT( "" ));
		message.Replace( wxT( "\n" ), wxT( " " ));

		glApp->getStatusBar()->SetStatusText( message, 1 );
		::wxLogDebug( wxT( "%s\n" ), PQerrorMessage( m_conn->getConnection()));
	}
	else
	{
		wxString message( PQcmdStatus( result ), wxConvUTF8 );

		message.Replace( wxT( "\r" ), wxT( "" ));
		message.Replace( wxT( "\n" ), wxT( " " ));

		glApp->getStatusBar()->SetStatusText( message, 1 );

		// If this result set has any columns, add a result grid to the code window so
		// we can show the results to the user

		if( m_codeWindow && PQnfields( result ))
			m_codeWindow->OnResultSet( result );
	}

	this->Show( true );
	this->SetFocus();
//    this->Activate();
    
	// Debugging has finished so we need to hide the code window.
	if (m_codeWindow)
	{
		m_codeWindow->Show( false );
	}

}

////////////////////////////////////////////////////////////////////////////////
// OnTargetComplete()
//
//	This event handler is called when the target function/procedure completes
//  and a result set (or error) has been returned by the server.  The event
//  object contains a pointer to the result set.
//
//  For now, we display an error message (if an error occurred) or write the
//  command status to the status bar (if the target completed without error).
//
//  We should really display the complete result set somewhere too.

void wsDirectDbg::OnTargetComplete( wxCommandEvent & event )
{
	// Extract the result set handle from the event and log the status info

	PGresult   * rawResult = (PGresult *)event.GetClientData();
	wsResultSet  result( rawResult );

	::wxLogDebug( _( "OnTargetComplete() called\n" ));
	::wxLogDebug( wxT( "%s\n" ), PQresStatus( PQresultStatus( rawResult )));

	// If the query failed, write the error message to the status line, otherwise, copy the result set into the grid

	if(( PQresultStatus( rawResult ) == PGRES_NONFATAL_ERROR ) || ( PQresultStatus( rawResult ) == PGRES_FATAL_ERROR ))
	{
		wxString	message( PQresultErrorMessage( rawResult ), wxConvUTF8 );

		message.Replace( wxT( "\r" ), wxT( "" ));
		message.Replace( wxT( "\n" ), wxT( " " ));

		glApp->getStatusBar()->SetStatusText( message, 1 );
		::wxLogDebug( wxT( "%s\n" ), PQerrorMessage( m_conn->getConnection()));
	}
	else
	{
		wxString message( PQcmdStatus( rawResult ), wxConvUTF8 );

		message.Replace( wxT( "\r" ), wxT( "" ));
		message.Replace( wxT( "\n" ), wxT( " " ));

		glApp->getStatusBar()->SetStatusText( message, 1 );

		// If this result set has any columns, add a result grid to the code window so
		// we can show the results to the user

		if( m_codeWindow && PQnfields( rawResult ))
		{
			m_codeWindow->OnResultSet( rawResult );

			m_codeWindow->disableTools( );
		}
	}

	this->Show( true );
	this->SetFocus();
//    this->Activate();

}

////////////////////////////////////////////////////////////////////////////////
// OnNoticeReceived()
//
//	This event handler is called when a notice is received from the server (in
//  response to our invoking the target).  For now, we just forward this event
//  to the debugger window (m_codeWindow) and the notification message is added
//  to the debugger's message window.  
//
//  When/if we get around to adding a result set window to this class, we should
//  also add a message window too and display notice messages here instead of in
//  the debugger window.

void wsDirectDbg::OnNoticeReceived( wxCommandEvent & event )
{   
	if( m_codeWindow )
		m_codeWindow->OnNoticeReceived( event );
}

////////////////////////////////////////////////////////////////////////////////
// OnDebug()
//
//	This event handler is called when a PLDBGBREAK notice is received from the
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
//  a wsDBResult event and handle the result set inside of OnResultReady()

void wsDirectDbg::OnDebug( wxCommandEvent & event )
{
	// This event contains a string of the form:
	//	 /path/debugger -k --database=db --host=host --port=port --user=user &"
	// We can use that string to launch a separate debugger client.
	//
	// The event also contains a pointer to a map that contains keyword=value
	// pairs for the debugger connection properties.  To get to that map, we
	// call event.GetClientData().  Once we have the map, we can look for the
	// debugger connection properties such as "database", "host", "port", ...

	wsConnProp * debugProps = (wsConnProp *)event.GetClientData();

	m_codeWindow = glMainFrame->addDebug( *debugProps );

	m_codeWindow->startLocalDebugging();

	this->Show( false );
}

////////////////////////////////////////////////////////////////////////////////
// getBreakpointList()
//
//	This function returns a non-const reference to our breakpoint list.  The
//  caller typically populates this list before calling startDebugging() - we
//  set a breakpoint for each member of the list

wsBreakpointList & wsDirectDbg::getBreakpointList()
{ 
	return( m_breakpoints ); 
}
