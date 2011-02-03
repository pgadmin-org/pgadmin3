//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmDebugger.cpp - debugger
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

#include <stdexcept>

// App headers
#include "debugger/frmDebugger.h"
#include "debugger/ctlCodeWindow.h"
#include "debugger/dbgPgConn.h"
#include "debugger/dlgDirectDbg.h"
#include "ctl/ctlMenuToolbar.h"

#include "images/debugger.xpm"
#include "images/clearAll.xpm"
#include "images/continue.xpm"
#include "images/setBreak.xpm"
#include "images/stepOver.xpm"
#include "images/stepInto.xpm"
#include "images/stop.xpm"

IMPLEMENT_CLASS( frmDebugger, pgFrame  )

BEGIN_EVENT_TABLE( frmDebugger, pgFrame  )
	EVT_MENU(MNU_EXIT,  frmDebugger ::OnExit)

	EVT_MENU_RANGE(MENU_ID_TOGGLE_BREAK, MENU_ID_STOP, frmDebugger::OnDebugCommand)
	EVT_CLOSE(frmDebugger::OnClose)
	EVT_SIZE(frmDebugger::OnSize)
	EVT_ERASE_BACKGROUND(frmDebugger::OnEraseBackground)

	EVT_STC_MARGINCLICK(wxID_ANY,        frmDebugger::OnMarginClick)
	EVT_STC_UPDATEUI(wxID_ANY,           frmDebugger::OnPositionStc)
	EVT_LISTBOX(wxID_ANY,                frmDebugger::OnSelectFrame)
	EVT_GRID_CELL_CHANGE(                frmDebugger::OnVarChange)

	EVT_MENU(MENU_ID_VIEW_TOOLBAR,       frmDebugger::OnToggleToolBar)
	EVT_MENU(MENU_ID_VIEW_STACKPANE,     frmDebugger::OnToggleStackPane)
	EVT_MENU(MENU_ID_VIEW_OUTPUTPANE,    frmDebugger::OnToggleOutputPane)
	EVT_MENU(MENU_ID_VIEW_DEFAULTVIEW,   frmDebugger::OnDefaultView)

	EVT_MENU(MNU_CONTENTS,               frmDebugger::OnContents)
	EVT_MENU(MNU_HELP,                   frmDebugger::OnHelp)

	EVT_AUI_PANE_CLOSE(frmDebugger::OnAuiUpdate)
END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////////////////
// frmDebugger constructor
//
//  frmDebugger manages the user interface for the workstation. This class
//  manages the toolbar, menu, status bar, and top-level windows.

frmDebugger::frmDebugger(frmMain *parent, const wxString &title)
	: pgFrame(NULL, title),
	  m_standaloneDebugger(NULL),
	  m_standaloneDirectDbg(NULL),
	  m_parent(parent)
{
	dlgName = wxT("frmDebugger");
	RestorePosition(100, 100, 600, 500, 450, 300);

	wxWindowBase::SetFont(settings->GetSystemFont());

	manager.SetManagedWindow(this);
	manager.SetFlags(wxAUI_MGR_DEFAULT | wxAUI_MGR_TRANSPARENT_DRAG);

	// Define the icon for this window
	SetIcon(wxIcon(debugger_xpm));

	// Create (and configure) the menu bar, toolbar, and status bar
	m_menuBar   = setupMenuBar();
	m_toolBar   = setupToolBar();
	m_statusBar = setupStatusBar();

	manager.AddPane(m_toolBar, wxAuiPaneInfo().Name(wxT("toolBar")).Caption(wxT("Toolbar")).ToolbarPane().Top().Row(1).Position(1).LeftDockable(false).RightDockable(false));

	// Now load the layout
	wxString perspective;
	settings->Read(wxT("Debugger/frmDebugger/Perspective-") + wxString(FRMDEBUGGER_PERSPECTIVE_VER), &perspective, FRMDEBUGGER_DEFAULT_PERSPECTIVE);
	manager.LoadPerspective(perspective, true);

	// and reset the captions for the current language
	manager.GetPane(wxT("toolBar")).Caption(_("Toolbar"));

	// Sync the View menu options
	m_viewMenu->Check(MENU_ID_VIEW_TOOLBAR, manager.GetPane(wxT("toolBar")).IsShown());

	manager.Update();

}

frmDebugger::~frmDebugger()
{
	// Only save the settings if the window was completely setup
	// This may not be the case if the params dialog was displayed,
	// and the user hit cancel before the main form opened.
	wxAuiPaneInfo &pane = manager.GetPane(wxT("sourcePane"));
	if (pane.IsOk())
		settings->Write(wxT("Debugger/frmDebugger/Perspective-") + wxString(FRMDEBUGGER_PERSPECTIVE_VER), manager.SavePerspective());

	manager.UnInit();

	if (m_parent)
		m_parent->RemoveFrame(this);
}

////////////////////////////////////////////////////////////////////////////////
// onContents()
// onHelp()
//
//     Help menu options
void frmDebugger::OnContents(wxCommandEvent &event)
{
	DisplayHelp(wxT("debugger"), HELP_PGADMIN);
}

void frmDebugger::OnHelp(wxCommandEvent &event)
{
	DisplayHelp(wxT("plpgsql"), HELP_POSTGRESQL);
}

////////////////////////////////////////////////////////////////////////////////
// addDebug()
//
//     This function creates a new debugger window...

ctlCodeWindow *frmDebugger::addDebug( const dbgConnProp &connProps )
{
	try
	{
		m_standaloneDebugger = new ctlCodeWindow( this , -1, connProps );
		m_standaloneDebugger->Show( false );
		return( m_standaloneDebugger );
	}
	catch( const std::runtime_error &error )
	{
		wxLogError(wxT("%s"), wxString(error.what(), wxConvUTF8).c_str());
		return NULL;
	}
}

dlgDirectDbg *frmDebugger::addDirectDbg( const dbgConnProp &connProp )
{
	try
	{
		m_standaloneDirectDbg = new dlgDirectDbg( this, -1, connProp );
		m_standaloneDirectDbg->setupParamWindow();
		return( m_standaloneDirectDbg );
	}
	catch( const std::runtime_error &error )
	{
		wxLogError(wxT("%s"), wxString(error.what(), wxConvUTF8).c_str());
		return NULL;
	}
}

////////////////////////////////////////////////////////////////////////////////
// OnDebugCommand()
//
//     This event handler is invoked when the user clicks one of the debugger
//     tools (on the debugger toolbar) - we simply forward the event to the
//  debugger window.

void frmDebugger::OnDebugCommand( wxCommandEvent &event )
{
	if (m_standaloneDebugger)
		m_standaloneDebugger->OnCommand( event );
}

////////////////////////////////////////////////////////////////////////////////
// OnSelectFrame()
//
//     This event handler is invoked when the user clicks one of the stack frames
//  - we simply forward the event to the debugger window.

void frmDebugger::OnSelectFrame( wxCommandEvent &event )
{
	if (m_standaloneDebugger)
		m_standaloneDebugger->OnSelectFrame( event );
}

////////////////////////////////////////////////////////////////////////////////
// OnMarginClick()
//
//     This event handler is invoked when the user clicks one of the stack frames
//  - we simply forward the event to the debugger window.

void frmDebugger::OnMarginClick( wxStyledTextEvent &event )
{
	if (m_standaloneDebugger)
		m_standaloneDebugger->OnMarginClick( event );
}

////////////////////////////////////////////////////////////////////////////////
// OnPositionStc()
//
//     This event handler is invoked when the user positions the cursor in the
//       code window
//  - we simply forward the event to the debugger window.

void frmDebugger::OnPositionStc( wxStyledTextEvent &event )
{
	if (m_standaloneDebugger)
		m_standaloneDebugger->OnPositionStc( event );
}

////////////////////////////////////////////////////////////////////////////////
// OnVarChange()
//
//     This event handler is invoked when the user edits a variable value
//  - we simply forward the event to the debugger window.

void frmDebugger::OnVarChange( wxGridEvent &event )
{
	if (m_standaloneDebugger)
		m_standaloneDebugger->OnVarChange( event );
}

////////////////////////////////////////////////////////////////////////////////
// OnSize()
//
//    This event handler is called when a resize event occurs (that is, when
//  wxWidgets needs to size the application window)

void frmDebugger::OnSize( wxSizeEvent &event )
{
	event.Skip();
}

void frmDebugger::OnEraseBackground(wxEraseEvent &event)
{
	event.Skip();
}

////////////////////////////////////////////////////////////////////////////////
// setupToolBar()
//
//    This function creates the standard toolbar

ctlMenuToolbar *frmDebugger::setupToolBar( void )
{
	m_toolBar = new ctlMenuToolbar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);

	m_toolBar->SetToolBitmapSize( wxSize( 16, 16 ));

	m_toolBar->AddTool( MENU_ID_STEP_INTO,    	 _( "Step into" ),    		 wxBitmap(stepInto_xpm), _( "Step into" ));
	m_toolBar->AddTool( MENU_ID_STEP_OVER,    	 _( "Step over" ),    		 wxBitmap(stepOver_xpm), _( "Step over" ));
	m_toolBar->AddTool( MENU_ID_CONTINUE,    	 _( "Continue" ),    		 wxBitmap(continue_xpm), _( "Continue" ));
	m_toolBar->AddSeparator();
	m_toolBar->AddTool( MENU_ID_TOGGLE_BREAK,     _( "Toggle breakpoint" ),   wxBitmap(setBreak_xpm), _( "Toggle breakpoint" ));
	m_toolBar->AddTool( MENU_ID_CLEAR_ALL_BREAK, _( "Clear all breakpoints" ), wxBitmap(clearAll_xpm), _( "Clear all breakpoints" ));
	m_toolBar->AddSeparator();
	m_toolBar->AddTool( MENU_ID_STOP,          	 _( "Stop debugging" ),    	 wxBitmap(stop_xpm), _( "Stop debugging" ));

	m_toolBar->EnableTool(MENU_ID_STEP_INTO,           false);
	m_toolBar->EnableTool(MENU_ID_STEP_OVER,           false);
	m_toolBar->EnableTool(MENU_ID_CONTINUE,            false);
	m_toolBar->EnableTool(MENU_ID_TOGGLE_BREAK,       false);
	m_toolBar->EnableTool(MENU_ID_CLEAR_ALL_BREAK,  false);
	m_toolBar->EnableTool(MENU_ID_STOP,    		    false);

	m_toolBar->Realize();

	return( m_toolBar );
}

////////////////////////////////////////////////////////////////////////////////
// setupStatusBar()
//
//    This function initializes the status bar (we don't have one at the moment
//  so this function simply returns)

wxStatusBar *frmDebugger::setupStatusBar( void )
{
	wxStatusBar *bar = CreateStatusBar( 3, wxST_SIZEGRIP );
	int    		  widths[] = { 0, -1, 130 };

	bar->SetStatusWidths(3, widths);
	bar->SetStatusText(_( "Initializing..."), 1);

	return( bar );
}

////////////////////////////////////////////////////////////////////////////////
// setupMenuBar()
//
//    This function creates the standard menu bar

wxMenuBar *frmDebugger::setupMenuBar(void)
{
	m_menuBar = new wxMenuBar;

	wxMenu *fileMenu = new wxMenu;
	fileMenu->Append(MNU_EXIT, _("E&xit\tAlt-F4"), _("Exit debugger window"));
// Don't append the File menu on Mac, as the Exit option
// will be moved to the application menu, leaving File empty.
#ifndef __WXMAC__
	m_menuBar->Append(fileMenu, _("&File"));
#endif

	m_debugMenu = new wxMenu;
#ifdef __WXGTK__
	/*
	*  F10 is treated as a System menu under GTK. Hence, we will use Ctrl+F10 for
	*  "step over" operation under GTK, instead of F10.
	*
	*  To make the behavior consitent, we will also use Ctrl+ for all the operations
	*  under GTK. (i.e. Step into, Step over, Continue, Toggle breakpoint, Stop
	*  debugging)
	*
	*  Please follow this link for more details:
	*  http://trac.wxwidgets.org/ticket/2404
	*/
	m_debugMenu->Append(MENU_ID_STEP_INTO,          _( "Step into\tCtrl+F11" ));
	m_debugMenu->Append(MENU_ID_STEP_OVER,          _( "Step over\tCtrl+F10" ));
	m_debugMenu->Append(MENU_ID_CONTINUE,           _( "Continue\tCtrl+F5" ));
	m_debugMenu->AppendSeparator();
	m_debugMenu->Append(MENU_ID_TOGGLE_BREAK,       _( "Toggle breakpoint\tCtrl+F9" ));
	m_debugMenu->Append(MENU_ID_CLEAR_ALL_BREAK,    _( "Clear all breakpoints\tCtrl+Shift+F9" ));
	m_debugMenu->AppendSeparator();
	m_debugMenu->Append(MENU_ID_STOP,               _( "Stop debugging\tCtrl+F8" ));
#else
	m_debugMenu->Append(MENU_ID_STEP_INTO,          _( "Step into\tF11" ));
	m_debugMenu->Append(MENU_ID_STEP_OVER,          _( "Step over\tF10" ));
	m_debugMenu->Append(MENU_ID_CONTINUE,           _( "Continue\tF5" ));
	m_debugMenu->AppendSeparator();
	m_debugMenu->Append(MENU_ID_TOGGLE_BREAK,       _( "Toggle breakpoint\tF9" ));
	m_debugMenu->Append(MENU_ID_CLEAR_ALL_BREAK,    _( "Clear all breakpoints\tCtrl+Shift+F9" ));
	m_debugMenu->AppendSeparator();
	m_debugMenu->Append(MENU_ID_STOP,               _( "Stop debugging\tF8" ));
#endif //__WXGTK__
	m_debugMenu->Enable(MENU_ID_STEP_INTO,   	    false);
	m_debugMenu->Enable(MENU_ID_STEP_OVER,   	    false);
	m_debugMenu->Enable(MENU_ID_CONTINUE,    	    false);
	m_debugMenu->Enable(MENU_ID_TOGGLE_BREAK,   	false);
	m_debugMenu->Enable(MENU_ID_CLEAR_ALL_BREAK,    false);
	m_debugMenu->Enable(MENU_ID_STOP,			    false);
	m_menuBar->Append(m_debugMenu, _("&Debug"));

	m_viewMenu = new wxMenu;
	m_viewMenu->Append(MENU_ID_VIEW_OUTPUTPANE, _("&Output pane\tCtrl-Alt-O"), _("Show or hide the output pane."), wxITEM_CHECK);
	m_viewMenu->Append(MENU_ID_VIEW_STACKPANE, _("&Stack pane\tCtrl-Alt-S"),   _("Show or hide the stack pane."), wxITEM_CHECK);
	m_viewMenu->Append(MENU_ID_VIEW_TOOLBAR, _("&Tool bar\tCtrl-Alt-T"),       _("Show or hide the tool bar."), wxITEM_CHECK);
	m_viewMenu->AppendSeparator();
	m_viewMenu->Append(MENU_ID_VIEW_DEFAULTVIEW, _("&Default view\tCtrl-Alt-V"),     _("Restore the default view."));
	m_viewMenu->Enable(MENU_ID_VIEW_OUTPUTPANE,    false);
	m_viewMenu->Enable(MENU_ID_VIEW_STACKPANE,	   false);
	m_menuBar->Append(m_viewMenu, _("&View"));

	wxMenu *helpMenu = new wxMenu();
	helpMenu->Append(MNU_CONTENTS, _("&Help"),                 _("Open the helpfile."));
	helpMenu->Append(MNU_HELP, _("&SQL Help\tF1"),                _("Display help on SQL commands."));
	m_menuBar->Append(helpMenu, _("&Help"));

	SetMenuBar( m_menuBar );

	return( m_menuBar );
}

////////////////////////////////////////////////////////////////////////////////
// OnClose()
//
//    wxWidgets invokes this event handler when the user closes the main window

void frmDebugger::OnClose( wxCloseEvent &event )
{
	if (m_standaloneDebugger)
	{
		m_standaloneDebugger->OnClose( event );
		if (event.GetVeto())
			return;

		delete m_standaloneDebugger;
		m_standaloneDebugger = NULL;
	}
	event.Skip();
}

////////////////////////////////////////////////////////////////////////////////
// OnExit()
//
//    Close the debugger

void frmDebugger::OnExit( wxCommandEvent &event )
{
	Close();
}


////////////////////////////////////////////////////////////////////////////////
//  OnToggleToolBar()
//
//    Turn the tool bar on or off
void frmDebugger::OnToggleToolBar(wxCommandEvent &event)
{
	if (m_viewMenu->IsChecked(MENU_ID_VIEW_TOOLBAR))
		manager.GetPane(wxT("toolBar")).Show(true);
	else
		manager.GetPane(wxT("toolBar")).Show(false);
	manager.Update();
}

////////////////////////////////////////////////////////////////////////////////
//  OnToggleStackPane()
//
//    Turn the tool bar on or off
void frmDebugger::OnToggleStackPane(wxCommandEvent &event)
{
	if (m_viewMenu->IsChecked(MENU_ID_VIEW_STACKPANE))
		manager.GetPane(wxT("stackPane")).Show(true);
	else
		manager.GetPane(wxT("stackPane")).Show(false);
	manager.Update();
}

////////////////////////////////////////////////////////////////////////////////
//  OnToggleOutputPane()
//
//    Turn the tool bar on or off
void frmDebugger::OnToggleOutputPane(wxCommandEvent &event)
{
	if (m_viewMenu->IsChecked(MENU_ID_VIEW_OUTPUTPANE))
		manager.GetPane(wxT("outputPane")).Show(true);
	else
		manager.GetPane(wxT("outputPane")).Show(false);
	manager.Update();
}

////////////////////////////////////////////////////////////////////////////////
//  OnAuiUpdate()
//
//    Update the view menu to reflect AUI changes
void frmDebugger::OnAuiUpdate(wxAuiManagerEvent &event)
{
	if(event.pane->name == wxT("toolBar"))
	{
		m_viewMenu->Check(MENU_ID_VIEW_TOOLBAR, false);
	}
	else if(event.pane->name == wxT("stackPane"))
	{
		m_viewMenu->Check(MENU_ID_VIEW_STACKPANE, false);
	}
	else if(event.pane->name == wxT("outputPane"))
	{
		m_viewMenu->Check(MENU_ID_VIEW_OUTPUTPANE, false);
	}
	event.Skip();
}

////////////////////////////////////////////////////////////////////////////////
//  OnDefaultView()
//
//    Reset the AUI view to the default
void frmDebugger::OnDefaultView(wxCommandEvent &event)
{
	manager.LoadPerspective(FRMDEBUGGER_DEFAULT_PERSPECTIVE, true);

	// Reset the captions for the current language
	manager.GetPane(wxT("toolBar")).Caption(_("Toolbar"));
	manager.GetPane(wxT("stackPane")).Caption(_("Stack pane"));
	manager.GetPane(wxT("outputPane")).Caption(_("Output pane"));

	// tell the manager to "commit" all the changes just made
	manager.Update();

	// Sync the View menu options
	m_viewMenu->Check(MENU_ID_VIEW_TOOLBAR, manager.GetPane(wxT("toolBar")).IsShown());
	m_viewMenu->Check(MENU_ID_VIEW_STACKPANE, manager.GetPane(wxT("stackPane")).IsShown());
	m_viewMenu->Check(MENU_ID_VIEW_OUTPUTPANE, manager.GetPane(wxT("outputPane")).IsShown());
}
