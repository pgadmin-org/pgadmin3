//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsquerywindow.cpp - debugger 
//
//////////////////////////////////////////////////////////////////////////

#include "debugger.h"
#include "wsQueryWindow.h"
#include "wsCodeWindow.h"
#include "wsConsole.h"
#include "debuggerMenu.h"

#include <wx/log.h>
#include <wx/file.h>
#include "wsMainFrame.h"

IMPLEMENT_CLASS( wsQueryWindow, wxSashLayoutWindow );

BEGIN_EVENT_TABLE( wsQueryWindow, wxSashLayoutWindow )
    EVT_CLOSE(                       	  wsQueryWindow::OnClose )
    EVT_MENU( MENU_ID_EXECUTE,       	  wsQueryWindow::OnExecute )
	EVT_MENU( RESULT_ID_RESULT_SET_READY, wsQueryWindow::OnResultReady )
	EVT_BUTTON( MENU_ID_SPAWN_DEBUGGER,   wsQueryWindow::OnSpawnDebugger )
	EVT_SASH_DRAGGED( wxID_ANY,           wsQueryWindow::OnSashDrag )

    EVT_CHAR( wsQueryWindow::OnKeyEvent )

END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////////////////
// wsQueryWindow constructor
//
//	This function prepares a wsQueryWindow object - it records the wsPgConn
//  pointer and initializes the text control with a 'demo' command.

wsQueryWindow::wsQueryWindow( wxWindow * parent, wxWindowID id, wsPgConn * conn )
    : wxSashLayoutWindow( parent , id ), m_conn( conn )
{
	// For demo purposes, we fill the edit control with the text of a few commands and then
	// select that text so that it's easy to delete it if you don't want the demo...

	m_gridHolder    = new wxSashLayoutWindow( glMainFrame /* this */ , WINDOW_ID_RESULT_GRID );
	m_commandHolder = new wxSashLayoutWindow( glMainFrame /* this */ , WINDOW_ID_COMMAND );

	m_gridHolder->SetOrientation( wxLAYOUT_HORIZONTAL );
	m_gridHolder->SetAlignment( wxLAYOUT_BOTTOM );
	m_gridHolder->SetSashVisible( wxSASH_TOP, true );
	m_gridHolder->SetDefaultSize( wxSize( 1000, 60 ));

	m_commandHolder->SetOrientation( wxLAYOUT_HORIZONTAL );
	m_commandHolder->SetAlignment( wxLAYOUT_TOP );
	m_commandHolder->SetSashVisible( wxSASH_BOTTOM, true );
	m_commandHolder->SetDefaultSize( wxSize( 1000, 180 ));

	m_resultGrid    = new wsResultGrid( m_gridHolder, wxID_ANY );
	m_command       = new wsRichWindow( m_commandHolder, wxID_ANY );

	wxString	defaultCmd( wxT("CREATE OR REPLACE FUNCTION my_factorial( INTEGER ) RETURNS INTEGER AS $$\n")
							wxT("DECLARE\n")
							wxT("  arg INTEGER;\n")
							wxT("BEGIN\n")
							wxT("\n")
							wxT("  arg := $1;\n")
							wxT("\n")
							wxT("  IF arg IS NULL OR arg < 0 THEN\n")
							wxT("    RAISE NOTICE 'Invalid Argument';\n")
							wxT("    RETURN NULL;\n")
							wxT("  ELSE \n")
							wxT("    IF arg = 1 THEN\n")
							wxT("      RETURN 1;\n")
							wxT("    ELSE\n")
							wxT("      DECLARE \n")
							wxT("        next_value INTEGER;\n")
							wxT("      BEGIN\n")
							wxT("        next_value := my_factorial(arg - 1) * arg;\n")
							wxT("        RETURN next_value;\n")
							wxT("      END;\n")
							wxT("    END IF;\n")
							wxT("  END IF;\n")
							wxT("END;\n")
							wxT("$$ LANGUAGE 'plpgsql'; \n")
                            wxT("\n")
							wxT("COMMIT;\n")
                            wxT("-- Set a breakpoint at this function\n")
                            wxT("SELECT pl_dbg_break( 'my_factorial( int4 )' );\n")
                            wxT("-- And call the function\n")
						    wxT("SELECT my_factorial( 4 );\n"));

	wxFont	numfont(8, wxSWISS, wxNORMAL, wxNORMAL);
	m_command->StyleSetFont(wxSTC_STYLE_DEFAULT, numfont);
	m_command->SetMarginType(1, wxSTC_MARGIN_NUMBER); 
	m_command->SetMarginWidth( 1, 30 );

    m_command->SetReadOnly( false );

	wxFile history( _T( ".ws_history" ), wxFile::read );

	if( history.IsOpened())
	{
		size_t	len = (size_t)history.Length();
		
		if( len > 0 )
		{
			defaultCmd.Clear();
			wxChar * buf = defaultCmd.GetWriteBuf( len );

			len = history.Read( buf, len );

			defaultCmd.UngetWriteBuf( len );
		}
	}

	m_command->AppendText( defaultCmd );
	m_command->AppendText( wxT( "\n" ));
	m_command->SetSelection( -1, -1 );

	wxLayoutAlgorithm	layout;
	layout.LayoutWindow( glMainFrame /*this*/, m_commandHolder );
	
	// wxAUI
	glMainFrame->manager.AddPane(m_gridHolder, wxAuiPaneInfo().Name(wxT("resultGrid")).Caption(_("resultGrid")).Bottom().MinSize(wxSize(200,100)).BestSize(wxSize(550,300)));
	glMainFrame->manager.AddPane(m_commandHolder, wxAuiPaneInfo().Name(wxT("command")).Caption(_("command")).Center().CaptionVisible(false).CloseButton(false).MinSize(wxSize(200,100)).BestSize(wxSize(350,200)));

	glMainFrame->manager.GetPane(wxT("resultGrid")).Caption(_("resultGrid"));
	glMainFrame->manager.GetPane(wxT("command")).Caption(_("command"));

	glMainFrame->manager.Update();

}

////////////////////////////////////////////////////////////////////////////////
// OnSpawnDebugger()
//
// 	This event handler is called when the wsPgThread intercepts a 
//  PLDBGBREAK NOTICE from the debugger server. 

void wsQueryWindow::OnSpawnDebugger( wxCommandEvent & event )
{
	// This event contains a pointer to a wsConnProp object - that object
	// contains all of the information (host name, port number, ...) that
	// we need  in order to connect to the debugger server

	wsConnProp * debugProps = (wsConnProp *)event.GetClientData();
    wsConsole  * parent = dynamic_cast<wsConsole *>( GetParent());

	// Ask our parent (a wsConsole) to attach to create a new 
	// debugger window and attach to the debugger server...

    if( parent )
		parent->doDebug( *debugProps );
}

////////////////////////////////////////////////////////////////////////////////
// OnResultReady()
//
// 	This event handler is called when the wsPgThread gets a complete result set 
//  (a PGresult handle) from the PostgreSQL server.  The event contains the 
//  result set handle.

void wsQueryWindow::OnResultReady( wxCommandEvent & event )
{
    ::wxLogDebug( _( "OnResultReady() called\n" ));

	// Extract the result set handle from the event and log the status info

    PGresult * result = (PGresult *)event.GetClientData();

    ::wxLogDebug( wxT( "%s\n" ), PQresStatus( PQresultStatus( result )));

	// If the query failed, write the error message to the status line, otherwise, copy the result set into the grid

    if(( PQresultStatus( result ) == PGRES_NONFATAL_ERROR ) || ( PQresultStatus( result ) == PGRES_FATAL_ERROR ))
    {
		wxString	message( wxString( PQresultErrorMessage( result ), wxConvUTF8 ));

		message.Replace( wxT( "\r" ), wxT( "" ));
		message.Replace( wxT( "\n" ), wxT( " " ));

		glApp->getStatusBar()->SetStatusText( message, 1 );
		::wxLogDebug( wxT( "%s\n" ), PQerrorMessage( m_conn->getConnection()));
    }
    else
    {
		glApp->getStatusBar()->SetStatusText( wxString( PQcmdStatus( result ), wxConvUTF8 ), 1 );
		m_resultGrid->fillGrid( result );
    }
}

////////////////////////////////////////////////////////////////////////////////
// OnKeyEvent()
//
//	This event handler is called when the user presses (and releases) a key.
//  We intercept a few key combinations and pass the rest through to the 
//  text control.

void wsQueryWindow::OnKeyEvent( wxKeyEvent & event )
{
	// For now, we intercept Ctrl+Enter and Ctrl+Space
	// and map those combinations into the Execute verb.

    switch( event.GetKeyCode())
    {
		case WXK_RETURN:
		case WXK_SPACE:
		{
			if( event.ControlDown())
			{
				doExecute();
				return;
			}
		}

		case 'C':
		{
			if( event.ControlDown())
			{
				// commentSelection();
				// return;
			}
		}
    }

    event.Skip();
}

////////////////////////////////////////////////////////////////////////////////
// OnClose()
//
//	This event handler is called when the user closes the query window

void wsQueryWindow::OnClose( wxCloseEvent & event )
{
    Destroy();
}

////////////////////////////////////////////////////////////////////////////////
// OnExecute()
//
//	This event handler is called when the user clicks the 'Execute' button
//  on the toolbar - we simply invoke the doExecute() function to carry out
//  the request.

void wsQueryWindow::OnExecute( wxCommandEvent & event )
{
    doExecute();
}

////////////////////////////////////////////////////////////////////////////////
// doExecute()
//
// 	This member function is invoked when the user clicks the Execute button, 
// 	chooses Execute from the menu, or types in one of the Execute keyboard 
// 	shortcuts (Ctrl+Enter or Ctrl+Space).
//
// 	We as the worker thread to send the selected text to the PostgreSQL
//  server

void wsQueryWindow::doExecute( void )
{
	// If the user has selected any text in the edit control, just
	// execute that text.  Otherwise, grab all of the text in the 
	// edit control.

	wxString	command = m_command->GetSelectedText();

    if( command.IsNull())
		command = m_command->GetText();

    if( !command.IsNull())
    {
		::wxLogDebug( command );
    }

    ::wxLogDebug( _( "main thread starting command for handler %p\n" ), GetEventHandler());

	// Notify the worker thread (the one connected to the database)
	// that it has a command to execute.

	glApp->getStatusBar()->SetStatusText( _( "Executing..." ), 1 );
    m_conn->startCommand( command, GetEventHandler());

	wxFile history;

	if( history.Create( wxT( ".ws_history" ), true ))
	{
		history.Write( command );
		history.Close();
	}
}

void wsQueryWindow::OnSashDrag( wxSashEvent & event )
{
    if( event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE )
        return;

    switch( event.GetId())
    {
		case WINDOW_ID_RESULT_GRID:
			m_gridHolder->SetDefaultSize( wxSize( 60, event.GetDragRect().height ));
			break;

		case WINDOW_ID_COMMAND:
			m_commandHolder->SetDefaultSize( wxSize( 60, event.GetDragRect().height ));
			break;
    }

    wxLayoutAlgorithm	layout;
    layout.LayoutWindow( this, m_commandHolder );
	
}
