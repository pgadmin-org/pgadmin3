//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsMainFrame.cpp - debugger
//
//////////////////////////////////////////////////////////////////////////

#include "wsMainFrame.h"
#include "wsCodeWindow.h"
#include "wsQueryWindow.h"
#include "wsPgconn.h"
#include "wsDirectdbg.h"
#include "debugger.h"

#include "images/pgAdmin3.xpm"

#include "debuggerMenu.h"

#include "images/file_open.xpm"
#include "images/file_save.xpm"
#include "images/execute.xpm"
#include "images/orange.xpm"
#include "images/bug.xpm"
#include "images/img.xpm"
#include "images/clip_cut.xpm"
#include "images/clip_copy.xpm"
#include "images/clip_paste.xpm"
#include "images/undo.xpm"
#include "images/redo.xpm"
#include "images/setBreak.xpm"
#include "images/clrBreak.xpm"
#include "images/setWatch.xpm"
#include "images/clrWatch.xpm"
#include "images/continue.xpm"
#include "images/stepOver.xpm"
#include "images/stepInto.xpm"
#include "images/setPC.xpm"
#include "images/stop.xpm"
#include "images/clearall.xpm"
#include "images/restart.xpm"

#include <wx/log.h>

// Copyright text(pgadmin)
#include "../../pgadmin/include/copyright.h"
#include "../../pgadmin/include/version.h"
#include "../../pgadmin/include/svnversion.h"

#define VERSION_WITH_DATE_AND_SVN       wxT("Version ") VERSION_STR wxT(" (") __TDATE__ wxT(", rev: ") wxT(VERSION_SVN) wxT(")")

IMPLEMENT_CLASS( wsMainFrame, wxDocParentFrame  )

BEGIN_EVENT_TABLE( wsMainFrame, wxDocParentFrame  )
    EVT_MENU( MENU_ID_EXECUTE, wsMainFrame::OnExecute )
    EVT_MENU(wxID_ABOUT, wsMainFrame ::OnAbout)
    EVT_MENU_RANGE(wsMainFrame ::ID_FirstPerspective, 
				   wsMainFrame ::ID_FirstPerspective+1000,
                   wsMainFrame ::OnRestorePerspective)

    EVT_MENU_RANGE( MENU_ID_SET_BREAK, MENU_ID_SET_PC, wsMainFrame::OnDebugCommand )

    EVT_CLOSE( wsMainFrame::OnClose )
    EVT_SIZE( wsMainFrame::OnSize )
    EVT_CHAR( wsMainFrame::OnChar )

    EVT_TOOL( wxID_CUT,   wsMainFrame::OnEditCommand)
    EVT_TOOL( wxID_COPY,  wsMainFrame::OnEditCommand)
    EVT_TOOL( wxID_PASTE, wsMainFrame::OnEditCommand)
    EVT_TOOL( wxID_UNDO,  wsMainFrame::OnEditCommand)
    EVT_TOOL( wxID_REDO,  wsMainFrame::OnEditCommand)

END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////////////////
// wsMainFrame constructor
//
//  wsMainFrame manages the user interface for the workstation. This class
//  manages the toolbar, menu, status bar, and top-level windows.  
//
//	This class also defines event handlers for a number of high-level events
//	(such as window sizing and layout, and creation of new windows).


wsMainFrame::wsMainFrame( wxDocManager* docManager, const wxString & title, const wxPoint & pos, const wxSize & size )
	: wxDocParentFrame( docManager, NULL, wxID_ANY, title, pos, size ),
	  m_console( NULL ),
	  m_standaloneDebugger( NULL ),
	  m_menuBar( NULL ),
	  m_toolBar( NULL ),
	  m_statusBar( NULL )
{

	manager.SetManagedWindow(this);
	manager.SetFlags(wxAUI_MGR_DEFAULT | wxAUI_MGR_TRANSPARENT_DRAG);

	// Define the icon for this window
	SetIcons( wxIconBundle( wxIcon( pgAdmin3_xpm )));

	// Create (and configure) the menu bar, toolbar, and status bar
	m_menuBar   = setupMenuBar();
	m_toolBar   = setupToolBar();
	m_statusBar = setupStatusBar();

	readSettings();

	// NOTE: We don't create the console window (that's the big window that the 
	// user types into) until we successfully connect to a database - see addConnect().
	manager.AddPane(m_toolBar, wxAuiPaneInfo().
                  Name(wxT("Toolbar")).Caption(wxT("Toolbar")).
                  ToolbarPane().Top().Row(1).Position(1).
                  LeftDockable(false).RightDockable(false));


	manager.Update();

}

wsMainFrame::~wsMainFrame()
{
	writeSettings();
}
////////////////////////////////////////////////////////////////////////////////
// addDebug()
//
// 	This function creates a new debugger window...

wsCodeWindow * wsMainFrame::addDebug( const wsConnProp & connProps )
{
	if( m_console == NULL )
	{
		m_standaloneDebugger = new wsCodeWindow( this , -1, connProps );
		m_standaloneDebugger->Show( true );
#if 0
		m_standaloneDebugger->Maximize( true );	
#endif

		return( m_standaloneDebugger );
	}
	else
	{
		return( new wsCodeWindow( this, -1, connProps ));
	}
}

wsDirectDbg * wsMainFrame::addDirectDbg( const wsConnProp & connProp )
{

	if( m_console == NULL )
	{
		m_standaloneDirectDbg = new wsDirectDbg( this, -1, connProp );
		m_standaloneDirectDbg->Show( true );
#if 0
		m_standaloneDirectDbg->Maximize( true );		
#endif

		return( m_standaloneDirectDbg );
	}
	else
	{
		return( new wsDirectDbg( this, -1, connProp ));
	}
}

////////////////////////////////////////////////////////////////////////////////
// addConnect()
//
//	This function tries to create to the given PostgreSQL server - if the
//  attempt succeeds, we create a new console window (a window where the 
//  user can type in and execute commands)

bool wsMainFrame::addConnect( const wxString & host, const wxString & database, const wxString & port, const wxString & user, const wxString & password )
{
	// Try to connect to the given server
	//
	// FIXME: the following call to wsPgConn() hangs the GUI thread - we should 
	//	      really use an asynchronous connect function

	wsPgConn * conn = new wsPgConn( host, database, user, password, port );

	// If the connection attempt succeeded, create a new console window and give it the input focus

	if( conn->isConnected())
	{
		m_console = new wsConsole( this, conn->getName(), wxDefaultPosition, wxDefaultSize, conn );
		m_console->Show( false );
#if 0
		m_console->SetFocus();
#endif
		return( true );
	} 
	else
	{
		delete( conn );
		return( false );
	}
}

////////////////////////////////////////////////////////////////////////////////
// OnDebugCommand()
//
// 	This event handler is invoked when the user clicks one of the debugger 
// 	tools (on the debugger toolbar) - we simply forward the event to the 
// 	console window (who then forwards it to the debugger window).

void wsMainFrame::OnDebugCommand( wxCommandEvent & event )
{
	if( m_standaloneDebugger )
		m_standaloneDebugger->OnCommand( event );
	else
		m_console->OnDebugCommand( event );
}

void wsMainFrame ::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxString Versions( VERSION_WITH_DATE_AND_SVN );
	Versions += wxT("\n");
	Versions += COPYRIGHT;
	Versions += wxT("\n");
	Versions += LICENSE;
	wxMessageBox(Versions, _("Debugger"), wxOK, this);
}

void wsMainFrame ::OnRestorePerspective(wxCommandEvent& event)
{
	if( !m_perspectives.IsEmpty() )
		manager.LoadPerspective(m_perspectives.Item(event.GetId() - ID_FirstPerspective));
	else
		event.Skip();
}

void wsMainFrame ::PerspectivesDef()
{
	wxString perspective_all = manager.SavePerspective();
	wxString perspective_default = manager.SavePerspective();

	m_perspectives.Add(perspective_default);
	m_perspectives.Add(perspective_all);
}

////////////////////////////////////////////////////////////////////////////////
// OnSize()
//
//	This event handler is called when a resize event occurs (that is, when 
//  wxWidgets needs to size the application window)

void wsMainFrame::OnSize( wxSizeEvent & event )
{
	event.Skip();
}

////////////////////////////////////////////////////////////////////////////////
// setupToolBar()
//
//	This function creates the standard toolbar

wxToolBar * wsMainFrame::setupToolBar( void )
{
	wxToolBar *t = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                            wxTB_FLAT | wxTB_NODIVIDER);

	t->SetToolBitmapSize( wxSize( 24, 24 ));

#if INCLUDE_FUNCTION_EDITOR
	t->AddTool( wxID_OPEN,       _( "Open" ),    wxBitmap( file_open_xpm ),  _( "Open File" ), wxITEM_NORMAL );
	t->AddTool( wxID_SAVE,       _( "Save" ),    wxBitmap( file_save_xpm ),  _( "Save" ),      wxITEM_NORMAL );
    t->AddTool( MENU_ID_EXECUTE, _( "Execute" ), wxBitmap( execute_xpm ),    _( "Execute" ),   wxITEM_NORMAL );

	t->AddSeparator();

	t->AddTool( wxID_CUT,   	 _( "Cut" ),     wxBitmap( clip_cut_xpm ),   _( "Cut" ),   	   wxITEM_NORMAL );
	t->AddTool( wxID_COPY,  	 _( "Copy" ),    wxBitmap( clip_copy_xpm ),  _( "Copy" ),  	   wxITEM_NORMAL );
	t->AddTool( wxID_PASTE, 	 _( "Paste" ),   wxBitmap( clip_paste_xpm ), _( "Paste" ), 	   wxITEM_NORMAL );
	t->AddTool( wxID_UNDO, 		 _( "Undo" ),    wxBitmap( undo_xpm ),       _( "Undo" ), 	   wxITEM_NORMAL );
	t->AddTool( wxID_REDO, 		 _( "Redo" ),    wxBitmap( redo_xpm ),       _( "Redo" ), 	   wxITEM_NORMAL );

	t->AddSeparator();
#endif

	t->AddTool( MENU_ID_STEP_INTO,		 _( "Step Into" ),    		 wxBitmap( step_into_xpm ),   _( "Step Into (F2)" ),        	  wxITEM_NORMAL );
	t->AddTool( MENU_ID_STEP_OVER,		 _( "Step Over" ),    		 wxBitmap( step_over_xpm ),   _( "Step Over (F3)" ),        	  wxITEM_NORMAL );
	t->AddTool( MENU_ID_CONTINUE,		 _( "Continue" ),    		 wxBitmap( continue_xpm ),    _( "Continue (F4)" ),         	  wxITEM_NORMAL );
	t->AddTool( MENU_ID_SET_BREAK,		 _( "Set Breakpoint" ),   	 wxBitmap( set_break_xpm ),   _( "Set Breakpoint (F5)" ),   	  wxITEM_NORMAL );
	t->AddTool( MENU_ID_CLEAR_BREAK, 	 _( "Clear Breakpoint" ), 	 wxBitmap( clear_break_xpm ), _( "Clear Breakpoint (F6)" ), 	  wxITEM_NORMAL );
	t->AddTool( MENU_ID_CLEAR_ALL_BREAK, _( "Clear All Breakpoints" ), wxBitmap( clearall_xpm ),  _( "Clear ALL Breakpoints (F7)" ),  wxITEM_NORMAL );
	t->AddTool( MENU_ID_STOP,      		 _( "Stop Debugging" ),    	 wxBitmap(stop_xpm),          _( "Stop Debugging (F8)" ),         wxITEM_NORMAL );

#if INCLUDE_RESTART
	t->AddTool( MENU_ID_RESTART,      	 _( "Restart Debugging" ),   wxBitmap( restart_xpm ),     _( "Restart Debugging (F9)" ), 	  wxITEM_NORMAL );
#endif

#if INCLUDE_WATCHPOINTS
	t->AddTool( MENU_ID_SET_WATCH,		 _( "Set Watchpoint" ),   	 wxBitmap( set_watch_xpm ),   _( "Set Watchpoint" ),   			wxITEM_NORMAL );
	t->AddTool( MENU_ID_CLEAR_WATCH, 	 _( "Clear Watchpoint" ), 	 wxBitmap( clear_watch_xpm ), _( "Clear Watchpoint" ), 			wxITEM_NORMAL );
	t->AddTool( MENU_ID_SET_PC,      	 _( "Jump To" ),    		 wxBitmap( set_pc_xpm ),      _( "Jump To" ),          			wxITEM_NORMAL );
#endif

	t->Realize();
    
	t->EnableTool( MENU_ID_STEP_INTO,   	false );
	t->EnableTool( MENU_ID_STEP_OVER,   	false );
	t->EnableTool( MENU_ID_CONTINUE,    	false );
	t->EnableTool( MENU_ID_SET_BREAK,   	false );
	t->EnableTool( MENU_ID_CLEAR_BREAK, 	false );
	t->EnableTool( MENU_ID_CLEAR_ALL_BREAK, false );
	t->EnableTool( MENU_ID_STOP,        	false );
	t->EnableTool( MENU_ID_RESTART,     	false );
#if INCLUDE_WATCHPOINTS
	t->EnableTool( MENU_ID_SET_WATCH,   	false );
	t->EnableTool( MENU_ID_CLEAR_WATCH, 	false );
	t->EnableTool( MENU_ID_SET_PC,      	false );
#endif
	
	m_toolBar = t;

	return( m_toolBar );
}

////////////////////////////////////////////////////////////////////////////////
// setupStatusBar()
//
//	This function initializes the status bar (we don't have one at the moment
//  so this function simply returns)

wxStatusBar * wsMainFrame::setupStatusBar( void )
{
	wxStatusBar * bar = CreateStatusBar( 3, wxST_SIZEGRIP );
	int			  widths[] = { 0, -1, -1 };

	bar->SetStatusWidths( 3, widths );
	bar->SetStatusText( _( "Initializing..." ), 1 );

    return( bar );
}

////////////////////////////////////////////////////////////////////////////////
// setupMenuBar()
//
//	This function creates the standard menu bar

wxMenuBar * wsMainFrame::setupMenuBar( void )
{
	m_menuBar = new wxMenuBar;

	wxMenu * fileMenu = new wxMenu;

#if 0
	fileMenu->Append( wxID_NEW,  _( "&New" ));
	fileMenu->Append( wxID_OPEN, _( "&Open" ));
#endif
	fileMenu->Append( wxID_EXIT, _( "E&xit" ));
	m_menuBar->Append( fileMenu, _( "&File" ));

	m_perspectives_menu = new wxMenu;
	m_perspectives_menu->Append(ID_FirstPerspective, _("&Default view"));
	m_menuBar->Append(m_perspectives_menu, _("&View"));

	wxMenu* help_menu = new wxMenu;
	help_menu->Append(wxID_ABOUT, _("&About..."));
	m_menuBar->Append(help_menu, _("&Help"));


	SetMenuBar( m_menuBar );

	return( m_menuBar );
}

////////////////////////////////////////////////////////////////////////////////
// OnClose()
//
//	wxWidgets invokes this event handler when the user closes the main window

void wsMainFrame::OnClose( wxCloseEvent & event )
{
	manager.UnInit();
	Destroy();
}

////////////////////////////////////////////////////////////////////////////////
// OnExecute()
//
// 	This event handler is called when the user clicks the Execute tool on the
//	toolbar.  We just forward the event to the console window.

void wsMainFrame::OnExecute( wxCommandEvent & event )
{
	m_console->doExecute( );
}

////////////////////////////////////////////////////////////////////////////////
// OnChar()
//
// This event handler intercepts Ctrl+Enter and Ctrl+Space and maps them into
// the Execute command (which sends the selected text to the PostgreSQL server)

void wsMainFrame::OnChar( wxKeyEvent & event )
{
	switch( event.GetKeyCode())
	{
		case WXK_RETURN:
		case WXK_SPACE:
		{
			if( event.ControlDown())
			{
				m_console->doExecute( );
				return;
			}
		}
	}

	event.Skip();
}

////////////////////////////////////////////////////////////////////////////////
// makeFuncFrame()
//
// This function creates a new DocChild frame to hold a view (presumably a 
// wsFuncView). We also adjust the File menu here to reflect the fact that 
// we now have a document to work with.

wxDocChildFrame * wsMainFrame::makeFuncFrame( wxDocument * doc, wxView * view )
{
	wxDocChildFrame * child = new wxDocChildFrame( doc, view, this, wxID_ANY, doc->GetTitle());

	wxMenu *fileMenu = new wxMenu;

#if INCLUDE_FUNCTION_EDITOR
	fileMenu->Append( wxID_NEW,    _T( "&New..." ));
	fileMenu->Append( wxID_OPEN,   _T( "&Open..." ));
#endif
	fileMenu->Append( wxID_CLOSE,  _T( "&Close" ));
	fileMenu->Append( wxID_SAVE,   _T( "&Save" ));
	fileMenu->Append( wxID_SAVEAS, _T( "Save &As..." ));

	wxMenuBar *menuBar = new wxMenuBar;

	menuBar->Append( fileMenu, _T( "&File" ));

	child->SetMenuBar( menuBar );

	return( child );

}

////////////////////////////////////////////////////////////////////////////////
//  OnEditCommand()
//
//	This event handler forwards toolbar/menu bar commands to the currently-
//  active child (that is, the DocChild frame that holds the focus).

void wsMainFrame::OnEditCommand( wxCommandEvent & event )
{
#if 0
	GetActiveChild()->ProcessEvent( event );
#endif
	glMainFrame->ProcessEvent( event );
}

////////////////////////////////////////////////////////////////////////////////
// writeSettings()
//
//	This function saves the application configuration information. It saves
//  application frame, subframes and procedure information in config file.
//

void wsMainFrame::writeSettings()
{
	int 		count =	1;
	wxString 	key;
	int		width;
	int		height;

	GetClientSize( &width, &height );

	glApp->getSettings().Write( wxT( "Resolution/width" ), width );
	glApp->getSettings().Write( wxT( "Resolution/height" ), height );

	glApp->getSettings().Flush();

}


////////////////////////////////////////////////////////////////////////////////
// readSettings()
//
//  This function reads information about procedure and frame resolution
//  from previously saved configuration file.
//

void wsMainFrame::readSettings()
{
	int	width;
	int	height;

	glApp->getSettings().Read( wxT( "Resolution/width" ),  &width,  700 );
	glApp->getSettings().Read( wxT( "Resolution/height" ), &height, 500 );

	SetClientSize( width, height );
}
