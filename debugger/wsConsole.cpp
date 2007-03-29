//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsconsole.cpp - debugger 
//
//////////////////////////////////////////////////////////////////////////

#include "wsConsole.h"
#include "wsCodeWindow.h"
#include "wsQueryWindow.h"
#include "wsPgconn.h"
#include "wsMainFrame.h"
#include "debuggerMenu.h"

#include "images/pgAdmin3.xpm"

#include <wx/log.h>

IMPLEMENT_CLASS( wsConsole, wxTextCtrl )

BEGIN_EVENT_TABLE( wsConsole, wxTextCtrl )
    EVT_ACTIVATE( wsConsole::OnActivate )
END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////////////////
// wsConsole constructor
//
//	This class manages the user interface for a single connection to a server.  
//  Each wsConsole object keeps track of a connection (m_conn), a debugger window
//  (m_codeWindow), and a query window (m_queryWindow).
//
//  A wsConsole object is typically a child of the wsMainFrame object

wsConsole::wsConsole( wxDocParentFrame * parent, const wxString & title, const wxPoint & pos, const wxSize & size, wsPgConn * conn )
//	:wxDocChildFrame(NULL, NULL, parent, -1, _T(""),
//        wxDefaultPosition, wxDefaultSize, wxFRAME_NO_TASKBAR ),
//    : wxDocChildFrame( parent, -1, title, pos, size ),
	: wxTextCtrl( parent , wxID_ANY , title,
                          wxDefaultPosition, wxDefaultSize, wxFRAME_NO_TASKBAR ),
      m_codeWindow( NULL ),
      m_queryWindow( NULL ),
      m_conn( conn )
{

	// Define the icon for this window
#if 0
    SetIcons( wxIconBundle( wxIcon( pgAdmin3_xpm )));
#endif
	// Create a query window - we'll create a debug window later if required

    m_queryWindow = new wsQueryWindow( glMainFrame /* this */  , -1, m_conn );
#if 0
    m_queryWindow->Show( true );
	m_queryWindow->SetFocus();
#endif
	glMainFrame->PerspectivesDef();
}
  
////////////////////////////////////////////////////////////////////////////////
// doExecute()
//
//	This function asks the query window to execute the command that the user
//	entered into the query window

void wsConsole::doExecute( void )
{
    m_queryWindow->doExecute();
}

////////////////////////////////////////////////////////////////////////////////
// doDebug()
//
// 	Creates a debugger window and attach it to the debugger server

void wsConsole::doDebug( wsConnProp & connProps )
{
    m_codeWindow = glMainFrame->addDebug( connProps );
    m_codeWindow->Show( true );

	// And start soliciting debugger events...

    m_codeWindow->startLocalDebugging( );
}

////////////////////////////////////////////////////////////////////////////////
// OnDebugCommand()
//
// 	Forward debugger commands (set breakpoint, step, etc.) to the debugger 
//	window

void wsConsole::OnDebugCommand( wxCommandEvent & event )
{
    if( m_codeWindow )
		m_codeWindow->OnCommand( event );
}

void wsConsole::OnActivate( wxActivateEvent & event )
{
	wxToolBar * t = glMainFrame->m_toolBar;

	t->EnableTool( MENU_ID_EXECUTE, event.GetActive());
}
