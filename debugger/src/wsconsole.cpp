//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: wsconsole.cpp 5827 2007-01-04 16:35:14 hiroshi $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsconsole.cpp - debugger 
//
//////////////////////////////////////////////////////////////////////////

#include "wsconsole.h"
#include "wscodewindow.h"
#include "wsquerywindow.h"
#include "wspgconn.h"
#include "wsmainframe.h"
#include "menu.h"

#include "images/pgAdmin3.xpm"

#include <wx/log.h>

IMPLEMENT_CLASS( wsConsole, wxMDIChildFrame )

BEGIN_EVENT_TABLE( wsConsole, wxMDIChildFrame )
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

wsConsole::wsConsole( wxMDIParentFrame * parent, const wxString & title, const wxPoint & pos, const wxSize & size, wsPgConn * conn )
    : wxMDIChildFrame( parent, -1, title, pos, size ),
      m_codeWindow( NULL ),
      m_queryWindow( NULL ),
      m_conn( conn )
{

	// Define the icon for this window

    SetIcons( wxIconBundle( wxIcon( pgAdmin3_xpm )));

	// Create a query window - we'll create a debug window later if required

    m_queryWindow = new wsQueryWindow( this, -1, m_conn );
    m_queryWindow->Show( true );
	m_queryWindow->SetFocus();
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
	wxToolBar * t = glMainFrame->getToolBar();

	t->EnableTool( MENU_ID_EXECUTE, event.GetActive());
}
