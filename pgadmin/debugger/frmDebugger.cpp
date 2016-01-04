//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmDebugger.cpp - debugger
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/event.h>

#include <stdexcept>

// App headers
#include "ctl/ctlMenuToolbar.h"
#include "ctl/ctlSQLBox.h"
#include "ctl/ctlAuiNotebook.h"
#include "ctl/ctlProgressStatusBar.h"
#include "debugger/dbgController.h"
#include "debugger/ctlTabWindow.h"
#include "debugger/frmDebugger.h"
#include "frm/frmMain.h"
#include "debugger/dbgConst.h"

#include "images/debugger.pngc"
#include "images/clearAll.pngc"
#include "images/continue.pngc"
#include "images/setBreak.pngc"
#include "images/stepOver.pngc"
#include "images/stepInto.pngc"
#include "images/stop.pngc"

IMPLEMENT_CLASS(frmDebugger, pgFrame)

BEGIN_EVENT_TABLE(frmDebugger, pgFrame)
	EVT_CLOSE(frmDebugger::OnClose)
	EVT_SIZE(frmDebugger::OnSize)
	EVT_ERASE_BACKGROUND(frmDebugger::OnEraseBackground)
	EVT_AUI_PANE_CLOSE(frmDebugger::OnAuiUpdate)

	EVT_MENU_RANGE(MENU_ID_TOGGLE_BREAK, MENU_ID_STOP, frmDebugger::OnDebugCommand)

	EVT_STC_MARGINCLICK(wxID_ANY,        frmDebugger::OnMarginClick)
	EVT_STC_UPDATEUI(wxID_ANY,           frmDebugger::OnPositionStc)
	EVT_LISTBOX(wxID_ANY,                frmDebugger::OnSelectFrame)
	EVT_GRID_CELL_CHANGE(                frmDebugger::OnVarChange)

	EVT_MENU(MNU_EXIT,                   frmDebugger ::OnExit)
	EVT_MENU(MENU_ID_VIEW_TOOLBAR,       frmDebugger::OnToggleToolBar)
	EVT_MENU(MENU_ID_VIEW_STACKPANE,     frmDebugger::OnToggleStackPane)
	EVT_MENU(MENU_ID_VIEW_OUTPUTPANE,    frmDebugger::OnToggleOutputPane)
	EVT_MENU(MENU_ID_VIEW_DEFAULTVIEW,   frmDebugger::OnDefaultView)

	EVT_MENU(MNU_CONTENTS,               frmDebugger::OnContents)
	EVT_MENU(MNU_HELP,                   frmDebugger::OnHelp)
END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////////////////
// frmDebugger constructor
//
//  frmDebugger manages the user interface for the workstation. This class
//  manages the toolbar, menu, status bar, and top-level windows.

frmDebugger::frmDebugger(frmMain *_parent, dbgController *_controller,
                         const wxString &_title) : pgFrame(_parent, _title), m_menuBar(NULL),
	m_toolBar(NULL), m_viewMenu(NULL), m_debugMenu(NULL), m_statusBar(NULL),
	m_parent(_parent), m_controller(_controller), m_stackWindow(NULL),
	m_tabWindow(NULL), m_codeViewer(NULL)
{
	dlgName = wxT("frmDebugger");
	RestorePosition(100, 100, 600, 500, 450, 300);

	SetFont(settings->GetSystemFont());

	m_manager.SetManagedWindow(this);
	m_manager.SetFlags(wxAUI_MGR_DEFAULT | wxAUI_MGR_TRANSPARENT_DRAG);

	// Define the icon for this window
	SetIcon(*debugger_png_ico);

	// Create (and configure) the menu bar, toolbar, and status bar
	m_menuBar   = SetupMenuBar();
	m_toolBar   = SetupToolBar();
	m_statusBar = SetupStatusBar();

	m_manager.AddPane(
	    m_toolBar,
	    wxAuiPaneInfo().Name(wxT("toolBar")).Caption(wxT("Toolbar"))
	    .ToolbarPane().Top().Row(1).Position(1).LeftDockable(false)
	    .RightDockable(false));

	// Now load the layout
	wxString perspective;
	settings->Read(
	    wxT("Debugger/frmDebugger/Perspective-")
	    + wxString(FRMDEBUGGER_PERSPECTIVE_VER),
	    &perspective, FRMDEBUGGER_DEFAULT_PERSPECTIVE);

	m_manager.LoadPerspective(perspective, true);

	// and reset the captions for the current language
	m_manager.GetPane(wxT("toolBar")).Caption(_("Toolbar"));

	// Sync the View menu options
	m_viewMenu->Check(MENU_ID_VIEW_TOOLBAR, m_manager.GetPane(wxT("toolBar")).IsShown());

	SetupDebugger();

	m_manager.Update();
}


frmDebugger::~frmDebugger()
{
	// Only save the settings if the window was completely setup
	// This may not be the case if the params dialog was displayed,
	// and the user hit cancel before the main form opened.
	wxAuiPaneInfo &pane = m_manager.GetPane(wxT("sourcePane"));
	if (pane.IsOk())
		settings->Write(wxT("Debugger/frmDebugger/Perspective-") + wxString(FRMDEBUGGER_PERSPECTIVE_VER), m_manager.SavePerspective());

	m_manager.UnInit();

	if (m_parent)
		m_parent->RemoveFrame(this);

	if (m_controller)
	{
		PopEventHandler();
		delete m_controller;
		m_controller = NULL;
	}
}


////////////////////////////////////////////////////////////////////////////////
// OnContents()
// OnHelp()
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
// SetupDebugger()
//
//     This function creates the debugger views...
//
void frmDebugger::SetupDebugger()
{
	SetFont(settings->GetSystemFont());

	// Initializing Stack Frame Window
	if (m_stackWindow == NULL)
	{
		m_stackWindow = new ctlStackWindow(this, WINDOW_ID_STACK,  wxDefaultPosition,
		                                   wxDefaultSize, 0);

		m_manager.AddPane(m_stackWindow,
		                  wxAuiPaneInfo().Name(wxT("stackPane")).Caption(_("Stack pane")).Right()
		                  .MinSize(wxSize(100, 100)).BestSize(wxSize(250, 200)));
	}
	// Initializing Tab Window
	if (m_tabWindow == NULL)
	{
		m_tabWindow = new ctlTabWindow(this, WINDOW_ID_TABS, wxDefaultPosition,
		                               wxDefaultSize, wxAUI_NB_BOTTOM | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_WINDOWLIST_BUTTON);

		m_manager.AddPane(m_tabWindow,
		                  wxAuiPaneInfo().Name(wxT("outputPane")).Caption(_("Output pane")).Bottom()
		                  .MinSize(wxSize(200, 100)).BestSize(wxSize(550, 300)));
	}
	// Initialing Code viewer Window
	if (m_codeViewer == NULL)
	{
		m_codeViewer = new ctlSQLBox(this, -1);

		// Set up the markers that we use to indicate the current line and
		// the break-point
		m_codeViewer->MarkerDefine(MARKER_CURRENT, wxSTC_MARK_ARROW, *wxGREEN,
		                           *wxGREEN);
		m_codeViewer->MarkerDefine(MARKER_CURRENT_BG, wxSTC_MARK_BACKGROUND, *wxGREEN,
		                           *wxGREEN);
		m_codeViewer->MarkerDefine(MARKER_BREAKPOINT, wxSTC_MARK_CIRCLEPLUS, *wxRED,
		                           *wxRED);

		m_codeViewer->SetMarginWidth(1, 16);
		m_codeViewer->SetMarginType(1, wxSTC_MARGIN_SYMBOL);

		// Make sure that the text control tells us when the user clicks in the
		// left margin
		m_codeViewer->SetMarginSensitive(0, true);
		m_codeViewer->SetMarginSensitive(1, true);
		m_codeViewer->SetMarginSensitive(2, true);

		m_manager.AddPane(m_codeViewer,
		                  wxAuiPaneInfo().Name(wxT("sourcePane")).Caption(_("Source pane")).Center()
		                  .CaptionVisible(false).CloseButton(false).MinSize(wxSize(200, 100))
		                  .BestSize(wxSize(350, 200)));
	}

	// Make sure the user can't edit the source code for this function
	m_codeViewer->SetReadOnly(true);

	// Now (re)load the layout
	wxString perspective;
	settings->Read(
	    wxT("Debugger/frmDebugger/Perspective-") +
	    wxString(FRMDEBUGGER_PERSPECTIVE_VER), &perspective,
	    FRMDEBUGGER_DEFAULT_PERSPECTIVE);
	m_manager.LoadPerspective(perspective, true);

	// And reset the captions
	m_manager.GetPane(wxT("sourcePane")).Caption(_("Source pane"));
	m_manager.GetPane(wxT("stackPane")).Caption(_("Stack pane"));
	m_manager.GetPane(wxT("outputPane")).Caption(_("Output pane"));

	// Sync the View menu options
	m_viewMenu->Check(MENU_ID_VIEW_STACKPANE,
	                  m_manager.GetPane(wxT("stackPane")).IsShown());
	m_viewMenu->Check(MENU_ID_VIEW_OUTPUTPANE,
	                  m_manager.GetPane(wxT("outputPane")).IsShown());

	// Enable the options for these controls
	m_viewMenu->Enable(MENU_ID_VIEW_OUTPUTPANE, true);
	m_viewMenu->Enable(MENU_ID_VIEW_STACKPANE,  true);

	m_manager.Update();

	// force
	EnableToolsAndMenus(false);

	PushEventHandler(m_controller);
}


////////////////////////////////////////////////////////////////////////////////
// EnableToolsAndMenus
//
//     Enable or disable the debugging tools and menus
//
// Parameters:
//     - enable/disable (boolean)
//
void frmDebugger::EnableToolsAndMenus(bool enable)
{
	if (m_toolBar)
	{
		m_toolBar->EnableTool(MENU_ID_STEP_INTO,       enable);
		m_toolBar->EnableTool(MENU_ID_STEP_OVER,       enable);
		m_toolBar->EnableTool(MENU_ID_CONTINUE,
		                      (enable ||
		                       (m_controller && m_controller->CanRestart())));
		m_toolBar->EnableTool(MENU_ID_TOGGLE_BREAK,    enable);
		m_toolBar->EnableTool(MENU_ID_CLEAR_ALL_BREAK, enable);
		m_toolBar->EnableTool(MENU_ID_STOP,            enable);
	}

	if (m_debugMenu)
	{
		m_debugMenu->Enable(MENU_ID_STEP_INTO,       enable);
		m_debugMenu->Enable(MENU_ID_STEP_OVER,       enable);
		m_debugMenu->Enable(MENU_ID_CONTINUE,
		                    (enable ||
		                     (m_controller && m_controller->CanRestart())));
		m_debugMenu->Enable(MENU_ID_TOGGLE_BREAK,    enable);
		m_debugMenu->Enable(MENU_ID_CLEAR_ALL_BREAK, enable);
		m_debugMenu->Enable(MENU_ID_STOP,            enable);
	}
}


////////////////////////////////////////////////////////////////////////////////
// GetLineNo
//
//     This function returns the current line-number, where the execution flow
//     has been stopped
//
// Returns:
//     Current line-number
//
int frmDebugger::GetLineNo()
{
	if (m_codeViewer)
	{
		return (m_codeViewer->LineFromPosition(m_codeViewer->GetCurrentPos()));
	}
	else
	{
		return -1;
	}
}


////////////////////////////////////////////////////////////////////////////////
// IsBreakpoint(int)
//
//     This function helps checking if break-point is set on the particular line
//
// Parameters:
//     - line-number
//
// Returns:
//     - True if a break-point is set on the line-no, otherwise false
//
bool frmDebugger::IsBreakpoint(int _lineNo)
{
	if (m_codeViewer)
		return (m_codeViewer->MarkerGet(_lineNo) &
		        MARKERINDEX_TO_MARKERMASK(MARKER_BREAKPOINT) ? true : false);
	else
		return false;
}


////////////////////////////////////////////////////////////////////////////////
// ClearAllBreakpoints
//
//     This function clears all the break-points
//
void frmDebugger::ClearAllBreakpoints()
{
	int lineNo = -1;
	bool updateBreakpoints = false;

	if (m_codeViewer)
	{
		while((lineNo = m_codeViewer->MarkerNext(lineNo + 1,
		                MARKERINDEX_TO_MARKERMASK(MARKER_BREAKPOINT))) != -1)
		{
			// Clear the break-point at particular location
			m_controller->ClearBreakpoint(lineNo);
			updateBreakpoints = true;
		}

		// Update break-points only if required
		if (updateBreakpoints)
			m_controller->UpdateBreakpoints();
	}
}


void frmDebugger::ClearBreakpointMarkers()
{
	int lineNo = 0;
	if (m_codeViewer)
	{
		while((lineNo = m_codeViewer->MarkerNext(lineNo,
		                MARKERINDEX_TO_MARKERMASK(MARKER_BREAKPOINT))) != -1)
		{
			m_codeViewer->MarkerDelete(lineNo++, MARKER_BREAKPOINT);
		}
	}
}


void frmDebugger::MarkBreakpoint(int _line)
{
	if (m_codeViewer)
	{
		m_codeViewer->MarkerAdd(_line, MARKER_BREAKPOINT);
	}
}


////////////////////////////////////////////////////////////////////////////////
// UnhilightCurrentLine
//
//     This function removes the hilight from the current line
//
void frmDebugger::UnhilightCurrentLine()
{
	if (m_codeViewer)
	{
		int lineNo
		    = m_codeViewer->MarkerNext(0, MARKERINDEX_TO_MARKERMASK(MARKER_CURRENT));

		if(lineNo != -1)
		{
			m_codeViewer->MarkerDelete(lineNo, MARKER_CURRENT);
			m_codeViewer->MarkerDelete(lineNo, MARKER_CURRENT_BG);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
// OnDebugCommand(wxCommandEvent)
//
//     This function handles the user clicks one of the debugger tools and menus
//     and take care of actions requires. These events are toggle break-point,
//     clear all break-points, debugging continue, step-in, step-over and stop
//     debugging.
//
//  Parameters
//     - event object
//
void frmDebugger::OnDebugCommand(wxCommandEvent &_event)
{
	switch(_event.GetId())
	{
		case MENU_ID_TOGGLE_BREAK:
		{
			int lineNo = GetLineNo();

			// This event should have not been called
			if (lineNo == -1)
				return;

			// The user wants to set or clear a breakpoint at the line that
			// contains the insertion point (the caret)
			if (IsBreakpoint(lineNo))
			{
				m_controller->ClearBreakpoint(lineNo);
			}
			else
			{
				m_controller->SetBreakpoint(lineNo);
			}
			m_controller->UpdateBreakpoints();
		}
		break;

		case MENU_ID_CLEAR_ALL_BREAK:
			// The user wants to clear all the breakpoint
			ClearAllBreakpoints();

			break;

		case MENU_ID_CONTINUE:
			// The user wants to continue execution (as opposed to
			// single-stepping through the code).  Unhilite all
			// variables and tell the debugger server to continue.
			if (m_controller)
			{
				if (m_controller->CanRestart())
				{
					m_controller->Start();
				}
				else
				{
					EnableToolsAndMenus(false);
					LaunchWaitingDialog(_("Waiting for target (continue)..."));
					m_controller->Countinue();
					UnhilightCurrentLine();
				}
			}
			break;

		case MENU_ID_STEP_OVER:
			// The user wants to step-over a function invocation (or
			// just single-step). Unhilite all variables and tell the
			// debugger server to step-over
			EnableToolsAndMenus(false);
			LaunchWaitingDialog(_("Waiting for target (step over)..."));
			m_controller->StepOver();
			UnhilightCurrentLine();

			break;

		case MENU_ID_STEP_INTO:
			// The user wants to step-into a function invocation (or
			// just single-step). Unhilite all variables and tell the
			// debugger server to step-into
			EnableToolsAndMenus(false);
			LaunchWaitingDialog(_("Waiting for target (step into)..."));
			m_controller->StepInto();
			UnhilightCurrentLine();

			break;

		case MENU_ID_STOP:
			EnableToolsAndMenus(false);
			LaunchWaitingDialog(_("Waiting for target to stop execution..."));
			m_controller->Stop();
			UnhilightCurrentLine();

			break;

		default:
			break;
	}
}


////////////////////////////////////////////////////////////////////////////////
// OnSelectFrame()
//
//     This function handles the user click on one of the stack-frames, and
//     asks the debugger server to switch to that frame, update the breakpoint
//     markers, and send a list of variables that are in-scope in the selected
//     frame
//
// Parametes:
//     Selection Event Object
//
void frmDebugger::OnSelectFrame(wxCommandEvent &_ev)
{
	if(_ev.GetSelection() != -1)
	{
		if (!m_controller->SelectFrame(_ev.GetSelection()))
		{
			// Reset to previous selection
			dbgModel *model = m_controller->GetModel();
			m_stackWindow->SelectFrame(model->GetDisplayedPackage(),
			                           model->GetDisplayedFunction());
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
// OnMarginClick()
//
//     This function handles the user clicks in the margin to the left of a
//     line of source code. We use the margin to display breakpoint indicators
//     so it makes sense that if you click on an breakpoint indicator, we will
//     clear that breakpoint.  If you click on a spot that does not contain a
//     breakpoint indicator (but it's still in the margin), we create a new
//     breakpoint at that line
//
// Parametes:
//     Selection Event Object
//
void frmDebugger::OnMarginClick(wxStyledTextEvent &event)
{
	int lineNo;

	// Check that the user clicked on the line number or breakpoint margin
	// We don't want to set a breakpoint when the user folds/unfolds code
	if (!(event.GetMargin() == 0 || event.GetMargin() == 1))
		return;

	lineNo = m_codeViewer->LineFromPosition(event.GetPosition());

	if (lineNo <= 0)
		return;

	// If we already have a breakpoint at the clickpoint, disable it, otherwise
	// create a new breakpoint
	if(m_codeViewer->MarkerGet(lineNo) &
	        MARKERINDEX_TO_MARKERMASK(MARKER_BREAKPOINT))
	{
		m_controller->ClearBreakpoint(lineNo);
	}
	else
	{
		m_controller->SetBreakpoint(lineNo);
	}
	m_controller->UpdateBreakpoints();
}


////////////////////////////////////////////////////////////////////////////////
// OnPositionStc()
//
//     This function handles the event called when the user positions the cursor
//     in the code viewer. We will update the current line number etc in the
//     status bar.
//
// Parametes:
//     wxStyledTextEvent Object
//
void frmDebugger::OnPositionStc(wxStyledTextEvent &event)
{
	if (m_codeViewer)
	{
		wxString strPos;

		int pos = m_codeViewer->GetCurrentPos();

		strPos.Printf(_("Ln %d Col %d Ch %d"), m_codeViewer->LineFromPosition(pos) + 1,
		              m_codeViewer->GetColumn(pos) + 1, pos + 1);

		GetStatusBar()->SetStatusText(strPos, 3);
	}
}


////////////////////////////////////////////////////////////////////////////////
// OnVarChange()
//
//     This functions handles the event occurred when the user edits a variable
//     value. Submit the changed value to the debugger server
//
void frmDebugger::OnVarChange(wxGridEvent &_ev)
{
	ctlVarWindow *varWin = NULL;
	wxString      varName, varValue;

	if(_ev.GetId() == ID_PARAMGRID)
	{
		varWin = GetParamWindow(false);
		varName = varWin->GetVarName(_ev.GetRow());
		varValue = varWin->GetVarValue(_ev.GetRow());
	}
	else if (_ev.GetId() == ID_VARGRID)
	{
		varWin = GetVarWindow(false);
		varName = varWin->GetVarName(_ev.GetRow());
		varValue = varWin->GetVarValue(_ev.GetRow());
	}
	else if (_ev.GetId() == ID_PKGVARGRID)
	{
		varWin = GetPkgVarWindow(false);
		varName = wxT("@") + varWin->GetVarName(_ev.GetRow());
		varValue = varWin->GetVarValue(_ev.GetRow());
	}
	else
	{
		// Theorically - the execution flow must not reach at this point.
		return;
	}

	m_controller->DepositValue(varName, varValue);
}

////////////////////////////////////////////////////////////////////////////////
// OnSize()
//
//    This event handler is called when a resize event occurs (that is, when
//  wxWidgets needs to size the application window)
//
void frmDebugger::OnSize(wxSizeEvent &event)
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
//
ctlMenuToolbar *frmDebugger::SetupToolBar(void)
{
	m_toolBar = new ctlMenuToolbar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);

	m_toolBar->SetToolBitmapSize(wxSize(16, 16));

	m_toolBar->AddTool(MENU_ID_STEP_INTO, wxEmptyString, *stepInto_png_bmp,
	                   _("Step into"));
	m_toolBar->AddTool(MENU_ID_STEP_OVER, wxEmptyString, *stepOver_png_bmp,
	                   _("Step over"));
	m_toolBar->AddTool(MENU_ID_CONTINUE, wxEmptyString, *continue_png_bmp,
	                   _("Continue/Start"));
	m_toolBar->AddSeparator();
	m_toolBar->AddTool(MENU_ID_TOGGLE_BREAK, wxEmptyString, *setBreak_png_bmp,
	                   _("Toggle breakpoint"));
	m_toolBar->AddTool(MENU_ID_CLEAR_ALL_BREAK, wxEmptyString, *clearAll_png_bmp,
	                   _("Clear all breakpoints"));
	m_toolBar->AddSeparator();
	m_toolBar->AddTool(MENU_ID_STOP, wxEmptyString, *stop_png_bmp,
	                   _("Stop debugging"));

	m_toolBar->EnableTool(MENU_ID_STEP_INTO,       false);
	m_toolBar->EnableTool(MENU_ID_STEP_OVER,       false);
	m_toolBar->EnableTool(MENU_ID_CONTINUE,        false);
	m_toolBar->EnableTool(MENU_ID_TOGGLE_BREAK,    false);
	m_toolBar->EnableTool(MENU_ID_CLEAR_ALL_BREAK, false);
	m_toolBar->EnableTool(MENU_ID_STOP,            false);

	m_toolBar->Realize();

	return(m_toolBar);
}


////////////////////////////////////////////////////////////////////////////////
// SetupStatusBar()
//
//    This function initializes the status bar (we don't have one at the moment
//  so this function simply returns)
//
ctlProgressStatusBar *frmDebugger::SetupStatusBar(void)
{
	ctlProgressStatusBar *bar = new ctlProgressStatusBar(this, false);
	int          widths[] = { -1, ctlProgressStatusBar::ms_progressbar_width, ctlProgressStatusBar::ms_progressstatus_width, 190};

	bar->SetFieldsCount(4);
	bar->SetStatusWidths(4, widths);
	this->SetStatusBar(bar);

	return(bar);
}


////////////////////////////////////////////////////////////////////////////////
// SetupMenuBar()
//
//    This function creates the standard menu bar
//
wxMenuBar *frmDebugger::SetupMenuBar(void)
{
	m_menuBar = new wxMenuBar;

	// Don't append the File menu on Mac, as the Exit option
	// will be moved to the application menu, leaving File empty.
#ifndef __WXMAC__
	wxMenu *fileMenu = new wxMenu;
	fileMenu->Append(MNU_EXIT, _("E&xit\tAlt-F4"), _("Exit debugger window"));

	m_menuBar->Append(fileMenu, _("&File"));
#endif

	m_debugMenu = new wxMenu;
#ifdef __WXGTK__
	//
	// F10 is treated as a System menu under GTK. Hence, we will use Ctrl+F10 for
	// "step over" operation under GTK, instead of F10.
	//
	// To make the behavior consitent, we will also use Ctrl+ for all the operations
	// under GTK. (i.e. Step into, Step over, Continue, Toggle breakpoint, Stop
	// debugging)
	//
	// Please follow this link for more details:
	// http://trac.wxwidgets.org/ticket/2404
	//
	m_debugMenu->Append(MENU_ID_STEP_INTO,       _("Step into\tCtrl+F11"));
	m_debugMenu->Append(MENU_ID_STEP_OVER,       _("Step over\tCtrl+F10"));
	m_debugMenu->Append(MENU_ID_CONTINUE,        _("Continue/Start\tCtrl+F5"));
	m_debugMenu->AppendSeparator();
	m_debugMenu->Append(MENU_ID_TOGGLE_BREAK,    _("Toggle breakpoint\tCtrl+F9"));
	m_debugMenu->Append(MENU_ID_CLEAR_ALL_BREAK, _("Clear all breakpoints\tCtrl+Shift+F9"));
	m_debugMenu->AppendSeparator();
	m_debugMenu->Append(MENU_ID_STOP,            _("Stop debugging\tCtrl+F8"));
#else
	m_debugMenu->Append(MENU_ID_STEP_INTO,       _("Step into\tF11"));
	m_debugMenu->Append(MENU_ID_STEP_OVER,       _("Step over\tF10"));
	m_debugMenu->Append(MENU_ID_CONTINUE,        _("Continue/Start\tF5"));
	m_debugMenu->AppendSeparator();
	m_debugMenu->Append(MENU_ID_TOGGLE_BREAK,    _("Toggle breakpoint\tF9"));
	m_debugMenu->Append(MENU_ID_CLEAR_ALL_BREAK, _("Clear all breakpoints\tCtrl+Shift+F9"));
	m_debugMenu->AppendSeparator();
	m_debugMenu->Append(MENU_ID_STOP,            _("Stop debugging\tF8"));
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

	SetMenuBar(m_menuBar);

	return(m_menuBar);
}

////////////////////////////////////////////////////////////////////////////////
// OnClose()
//
//    wxWidgets invokes this event handler when the user closes the main window

void frmDebugger::OnClose(wxCloseEvent &_ev)
{
	if (!m_controller->CanRestart() && _ev.CanVeto())
	{
		if (wxMessageBox(
		            _("Are you sure you wish to abort the debugging session?\nThis will abort the function currently being debugged."),
		            _("Close debugger"), wxICON_QUESTION | wxYES_NO) != wxYES)
		{
			_ev.Veto();

			return;
		}
	}

	if (!m_controller->CloseDebugger() && _ev.CanVeto())
	{
		_ev.Veto();

		return;
	}

	_ev.Skip();
}

////////////////////////////////////////////////////////////////////////////////
// OnExit()
//
//    Close the debugger

void frmDebugger::OnExit(wxCommandEvent &event)
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
		m_manager.GetPane(wxT("toolBar")).Show(true);
	else
		m_manager.GetPane(wxT("toolBar")).Show(false);

	m_manager.Update();
}

////////////////////////////////////////////////////////////////////////////////
//  OnToggleStackPane()
//
//    Turn the tool bar on or off
void frmDebugger::OnToggleStackPane(wxCommandEvent &event)
{
	if (m_viewMenu->IsChecked(MENU_ID_VIEW_STACKPANE))
		m_manager.GetPane(wxT("stackPane")).Show(true);
	else
		m_manager.GetPane(wxT("stackPane")).Show(false);

	m_manager.Update();
}

////////////////////////////////////////////////////////////////////////////////
//  OnToggleOutputPane()
//
//    Turn the tool bar on or off
void frmDebugger::OnToggleOutputPane(wxCommandEvent &event)
{
	if (m_viewMenu->IsChecked(MENU_ID_VIEW_OUTPUTPANE))
		m_manager.GetPane(wxT("outputPane")).Show(true);
	else
		m_manager.GetPane(wxT("outputPane")).Show(false);

	m_manager.Update();
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
	m_manager.LoadPerspective(FRMDEBUGGER_DEFAULT_PERSPECTIVE, true);

	// Reset the captions for the current language
	m_manager.GetPane(wxT("toolBar")).Caption(_("Toolbar"));
	m_manager.GetPane(wxT("stackPane")).Caption(_("Stack pane"));
	m_manager.GetPane(wxT("outputPane")).Caption(_("Output pane"));

	// tell the m_manager to "commit" all the changes just made
	m_manager.Update();

	// Sync the View menu options
	m_viewMenu->Check(MENU_ID_VIEW_TOOLBAR, m_manager.GetPane(wxT("toolBar")).IsShown());
	m_viewMenu->Check(MENU_ID_VIEW_STACKPANE, m_manager.GetPane(wxT("stackPane")).IsShown());
	m_viewMenu->Check(MENU_ID_VIEW_OUTPUTPANE, m_manager.GetPane(wxT("outputPane")).IsShown());
}


void frmDebugger::HighlightLine(int _lineNo)
{
	int lineNo = m_codeViewer->MarkerNext(
	                 0, MARKERINDEX_TO_MARKERMASK( MARKER_CURRENT));

	if (lineNo != -1)
	{
		m_codeViewer->MarkerDelete(lineNo, MARKER_CURRENT);
		m_codeViewer->MarkerDelete(lineNo, MARKER_CURRENT_BG);
	}

	// Add the current-line indicator to the current line of code
	m_codeViewer->MarkerAdd(_lineNo, MARKER_CURRENT);
	m_codeViewer->MarkerAdd(_lineNo, MARKER_CURRENT_BG);

	// Scroll the source code listing (if required) to make sure
	// that this line of code is visible
	//
	// (NOTE: we set the anchor and the caret to the same position to avoid
	// creating a selection region)
	int pos = m_codeViewer->PositionFromLine(_lineNo);

	m_codeViewer->SetAnchor(pos);
	m_codeViewer->SetCurrentPos(pos);

	m_codeViewer->EnsureCaretVisible();
}

void frmDebugger::DisplaySource(dbgCachedStack &_cached)
{
	dbgModel *model = m_controller->GetModel();

	m_codeViewer->SetFocus();

	if (model->RequireDisplayUpdate())
	{
		ctlVarWindow *varWin = NULL;
		if ((varWin = GetVarWindow(false)) != NULL)
		{
			varWin->DelVar();
		}
		if ((varWin = GetParamWindow(false)) != NULL)
		{
			varWin->DelVar();
		}
		if ((varWin = GetPkgVarWindow(false)) != NULL)
		{
			varWin->DelVar();
		}

		model->GetDisplayedFunction() = _cached.m_func;
		model->GetDisplayedPackage() = _cached.m_pkg;

		// Now erase any old code and write out the new listing
		m_codeViewer->SetReadOnly(false);

		m_codeViewer->SetText(_cached.m_source);
		m_codeViewer->Colourise(0, _cached.m_source.Length());

		m_codeViewer->SetReadOnly(true);
	}
	HighlightLine(model->GetCurrLineNo());
}


void frmDebugger::SetStatusText(const wxString &_status)
{
	m_statusBar->SetStatusText(_status, 0);
}


void frmDebugger::LaunchWaitingDialog(const wxString &msg)
{
	dbgModel *model = m_controller->GetModel();
	wxString strStatus,
	         strTargetPid = model->GetTargetPid(),
	         strTarget    = model->GetTarget()->GetQualifiedName();

	if (msg.IsEmpty())
	{
		if (strTargetPid != wxT("NULL"))
		{
			strStatus =
			    wxString::Format(
			        _("Waiting for the session (pid:%s) to invoke the specified targets."),
			        strTargetPid.c_str());
		}
		else
		{
			strStatus =
			    wxString::Format(
			        _("Waiting for another session to invoke the target - \"%s\""), strTarget.c_str());
		}
	}
	else
	{
		strStatus = msg;
	}
	m_statusTxt = strStatus;

	SetStatusText(strStatus);
	m_statusBar->ShowProgress();
}


void frmDebugger::CloseProgressBar()
{
	if (!m_statusTxt.IsEmpty())
	{
		SetStatusText(m_statusTxt + wxT(" Done"));
		m_statusTxt = wxT("");
	}
	m_statusBar->StopProgress();
}
