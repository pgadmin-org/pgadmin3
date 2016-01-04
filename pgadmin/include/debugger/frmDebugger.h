//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmDebugger.h - debugger
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//	class frmDebugger
//
//  frmDebugger manages the user interface for the workstation. This class
//  manages the toolbar, menu, status bar, and top-level windows.
//
//	This class also defines event handlers for a number of high-level events
//	(such as window sizing and layout, and creation of new windows).
//
////////////////////////////////////////////////////////////////////////////////

#ifndef FRMDEBUGGER_H
#define FRMDEBUGGER_H

#include <wx/aui/aui.h>
#include <wx/progdlg.h>

#include "ctl/ctlProgressStatusBar.h"

//
// This number MUST be incremented if changing any of the default perspectives
//
#define FRMDEBUGGER_PERSPECTIVE_VER wxT("8189")

#ifdef __WXMAC__
#define FRMDEBUGGER_DEFAULT_PERSPECTIVE wxT("layout2|name=toolBar;caption=Toolbar;state=2108144;dir=1;layer=10;row=0;pos=1;prop=100000;bestw=154;besth=23;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=sourcePane;caption=Source pane;state=1020;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=350;besth=200;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=stackPane;caption=Stack pane;state=2099196;dir=2;layer=0;row=0;pos=0;prop=100000;bestw=250;besth=200;minw=100;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=outputPane;caption=Output pane;state=2099196;dir=3;layer=0;row=0;pos=0;prop=100000;bestw=550;besth=300;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|dock_size(1,10,0)=25|dock_size(5,0,0)=237|dock_size(2,0,0)=237|dock_size(3,0,0)=156|")
#else
#ifdef __WXGTK__
#define FRMDEBUGGER_DEFAULT_PERSPECTIVE wxT("layout2|name=toolBar;caption=Toolbar;state=2108144;dir=1;layer=10;row=0;pos=1;prop=100000;bestw=205;besth=30;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=sourcePane;caption=Source pane;state=1020;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=350;besth=200;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=stackPane;caption=Stack pane;state=2099196;dir=2;layer=0;row=0;pos=0;prop=100000;bestw=250;besth=200;minw=100;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=outputPane;caption=Output pane;state=2099196;dir=3;layer=0;row=0;pos=0;prop=100000;bestw=550;besth=300;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|dock_size(1,10,0)=25|dock_size(5,0,0)=237|dock_size(2,0,0)=237|dock_size(3,0,0)=156|")
#else
#define FRMDEBUGGER_DEFAULT_PERSPECTIVE wxT("layout2|name=toolBar;caption=Toolbar;state=2108144;dir=1;layer=10;row=0;pos=1;prop=100000;bestw=154;besth=23;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=sourcePane;caption=Source pane;state=1020;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=350;besth=200;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=stackPane;caption=Stack pane;state=2099196;dir=2;layer=0;row=0;pos=0;prop=100000;bestw=250;besth=200;minw=100;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=outputPane;caption=Output pane;state=2099196;dir=3;layer=0;row=0;pos=0;prop=100000;bestw=550;besth=300;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|dock_size(1,10,0)=25|dock_size(5,0,0)=237|dock_size(2,0,0)=237|dock_size(3,0,0)=156|")
#endif
#endif

// Debugger Controller
class dbgController;
class frmMain;
class ctlTabWindow;
class ctlResultGrid;
class ctlVarWindow;
class ctlSQLBox;
class dbgCachedStack;

class frmDebugger : public pgFrame
{
	DECLARE_CLASS(frmDebugger)

public:
	frmDebugger(frmMain *_parent, dbgController *_controller,
	            const wxString &_title);
	virtual ~frmDebugger();

	void SetupDebugger();

	void SetStatusText(const wxString &_status);

	ctlTabWindow *GetTabWindow()
	{
		return m_tabWindow;
	}
	ctlStackWindow *GetStackWindow()
	{
		return m_stackWindow;
	}
	ctlMessageWindow *GetMessageWindow()
	{
		return m_tabWindow->GetMessageWindow();
	}

	ctlVarWindow *GetVarWindow(bool create)
	{
		return m_tabWindow->GetVarWindow(create);
	}
	ctlVarWindow *GetParamWindow(bool create)
	{
		return m_tabWindow->GetParamWindow(create);
	}
	ctlVarWindow *GetPkgVarWindow(bool create)
	{
		return m_tabWindow->GetPkgVarWindow(create);
	}
	ctlResultGrid *GetResultWindow()
	{
		return m_tabWindow->GetResultWindow();
	}

	void           DisplaySource(dbgCachedStack &);

	void EnableToolsAndMenus(bool enable = true);
	void UnhilightCurrentLine();
	void HighlightLine(int _lineNo);
	void ClearBreakpointMarkers();
	void MarkBreakpoint(int lineNo);
	void CloseProgressBar();
	void LaunchWaitingDialog(const wxString &msg = wxEmptyString);

private:

	int  GetLineNo();
	bool IsBreakpoint(int _lineNo);
	void ClearAllBreakpoints();

	wxMenuBar      *SetupMenuBar( void );
	ctlMenuToolbar *SetupToolBar( void );
	ctlProgressStatusBar *SetupStatusBar( void );

	enum
	{
		MARKER_CURRENT    = 0x02,       // Current line marker
		MARKER_CURRENT_BG = 0x04,       // Current line marker - background hilight
		MARKER_BREAKPOINT = 0x01,       // Breakpoint marker
	};

private:
	// Menu bar
	wxMenuBar        *m_menuBar;
	// Frames' toolbar
	ctlMenuToolbar   *m_toolBar;
	// View menu (can be modified by wxCodeWindow)
	wxMenu           *m_viewMenu;
	// Debug menu (can be modified by wxCodeWindow)
	wxMenu           *m_debugMenu;

	wxAuiManager      m_manager;
	// Frame's status bar
	ctlProgressStatusBar *m_statusBar;
	// Main Frame
	frmMain          *m_parent;
	// Debugger Controller
	dbgController    *m_controller;

	// Stack Window
	ctlStackWindow   *m_stackWindow;
	// Tab Window
	ctlTabWindow     *m_tabWindow;
	// Function Code Viewer
	ctlSQLBox        *m_codeViewer;

	// Operation Status
	wxString          m_statusTxt;

	DECLARE_EVENT_TABLE()

	void OnExecute(wxCommandEvent &_ev);
	void OnDebugCommand(wxCommandEvent &_ev);
	void OnSelectFrame(wxCommandEvent &_ev);
	// Toggle break-point on margin click
	void OnMarginClick(wxStyledTextEvent &_ev);
	void OnPositionStc(wxStyledTextEvent &_ev);
	void OnVarChange(wxGridEvent &_ev);
	void OnClose(wxCloseEvent &_ev);
	void OnExit(wxCommandEvent &_ev);
	void OnSize(wxSizeEvent &_ev);
	void OnEraseBackground(wxEraseEvent &_ev);
	void OnHelp(wxCommandEvent &_ev);
	void OnContents(wxCommandEvent &_ev);

	void OnToggleToolBar(wxCommandEvent &_ev);
	void OnToggleStackPane(wxCommandEvent &_ev);
	void OnToggleOutputPane(wxCommandEvent &_ev);
	void OnAuiUpdate(wxAuiManagerEvent &_ev);
	void OnDefaultView(wxCommandEvent &_ev);
};

#endif
