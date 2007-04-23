//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: wsMainFrame.h 6216 2007-04-19 11:28:08Z dpage $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsMainFrame.h - debugger
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//	class wsMainFrame
//
//  wsMainFrame manages the user interface for the workstation. This class
//  manages the toolbar, menu, status bar, and top-level windows.  
//
//	This class also defines event handlers for a number of high-level events
//	(such as window sizing and layout, and creation of new windows).
//
////////////////////////////////////////////////////////////////////////////////

#ifndef WSMAINFRAMEH
#define WSMAINFRAMEH

#include <wx/aui/aui.h>

#include "frm/frmMain.h"
#include "debugger/wsConnProp.h"
#include "debugger/wsRichWindow.h"
#include "debugger/wsTabWindow.h"

#define WSMAINFRAME_PERPSECTIVE_VER wxT("$Rev: 6216 $")

#ifdef __WXMAC__
#define WSMAINFRAME_DEFAULT_PERSPECTIVE wxT("layout2|name=toolBar;caption=Toolbar;state=2108144;dir=1;layer=10;row=0;pos=1;prop=100000;bestw=154;besth=23;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=sourcePane;caption=Source pane;state=1020;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=350;besth=200;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=stackPane;caption=Stack pane;state=2099196;dir=2;layer=0;row=0;pos=0;prop=100000;bestw=250;besth=200;minw=100;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=outputPane;caption=Output pane;state=2099196;dir=3;layer=0;row=0;pos=0;prop=100000;bestw=550;besth=300;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|dock_size(1,10,0)=25|dock_size(5,0,0)=237|dock_size(2,0,0)=237|dock_size(3,0,0)=156|")
#else
#ifdef __WXGTK__
#define WSMAINFRAME_DEFAULT_PERSPECTIVE wxT("layout2|name=toolBar;caption=Toolbar;state=2108144;dir=1;layer=10;row=0;pos=1;prop=100000;bestw=154;besth=23;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=sourcePane;caption=Source pane;state=1020;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=350;besth=200;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=stackPane;caption=Stack pane;state=2099196;dir=2;layer=0;row=0;pos=0;prop=100000;bestw=250;besth=200;minw=100;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=outputPane;caption=Output pane;state=2099196;dir=3;layer=0;row=0;pos=0;prop=100000;bestw=550;besth=300;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|dock_size(1,10,0)=25|dock_size(5,0,0)=237|dock_size(2,0,0)=237|dock_size(3,0,0)=156|")
#else
#define WSMAINFRAME_DEFAULT_PERSPECTIVE wxT("layout2|name=toolBar;caption=Toolbar;state=2108144;dir=1;layer=10;row=0;pos=1;prop=100000;bestw=154;besth=23;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=sourcePane;caption=Source pane;state=1020;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=350;besth=200;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=stackPane;caption=Stack pane;state=2099196;dir=2;layer=0;row=0;pos=0;prop=100000;bestw=250;besth=200;minw=100;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=outputPane;caption=Output pane;state=2099196;dir=3;layer=0;row=0;pos=0;prop=100000;bestw=550;besth=300;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|dock_size(1,10,0)=25|dock_size(5,0,0)=237|dock_size(2,0,0)=237|dock_size(3,0,0)=156|")
#endif
#endif

enum
{
    MENU_ID_EXECUTE = 101,        // Execute command entered by user

    MENU_ID_TOGGLE_BREAK,        // Set/Unset breakpoint
    MENU_ID_CLEAR_ALL_BREAK,    // Clear all breakpoints
    MENU_ID_CONTINUE,        // Continue
    MENU_ID_STEP_OVER,        // Step over    
    MENU_ID_STEP_INTO,        // Step into
    MENU_ID_STOP,            // Stop debugging

    MENU_ID_SPAWN_DEBUGGER,        // Spawn a separate debugger process
    MENU_ID_NOTICE_RECEIVED,    // NOTICE received from server
    WINDOW_ID_STACK,        // Tree-control window
    WINDOW_ID_CONSOLE,        // Console window
    WINDOW_ID_TABS,            // Tab window
    WINDOW_ID_BREAKPOINTS,        // Breakpoints window
    WINDOW_ID_RESULT_GRID,        // Results window
    WINDOW_ID_COMMAND,        // Command window
    SOCKET_ID_DEBUG,        // Debugger Socket ID

    MENU_ID_VIEW_TOOLBAR,       // View menu options
    MENU_ID_VIEW_STACKPANE,
    MENU_ID_VIEW_OUTPUTPANE,
    MENU_ID_VIEW_DEFAULTVIEW,

    RESULT_ID_RESULT_SET_READY,    // Generic - result set received from server

    RESULT_ID_ATTACH_TO_PORT,    // Debugger - attach to port completed
    RESULT_ID_BREAKPOINT,        // Debugger - breakpoint reached
    RESULT_ID_GET_VARS,        // Debugger - variable list complete
    RESULT_ID_GET_STACK,        // Debugger - stack trace complete
    RESULT_ID_GET_BREAKPOINTS,    // Debugger - breakpoint list complete
    RESULT_ID_GET_SOURCE,           // Debugger - source code listing complete
    RESULT_ID_NEW_BREAKPOINT,    // Debugger - set breakpoint complete
    RESULT_ID_NEW_BREAKPOINT_WAIT,    // Debugger - set breakpoint complete, wait for target progress
    RESULT_ID_DEL_BREAKPOINT,    // Debugger - drop breakpoint complete
    RESULT_ID_DEPOSIT_VALUE,    // Debugger - deposit value complete
    RESULT_ID_ABORT_TARGET,         // Debugger - abort target (cancel function)
    RESULT_ID_ADD_BREAKPOINT,    // Debugger - target info received, now set a breakpoint
    RESULT_ID_LISTENER_CREATED,    // Debugger - global listener created
    RESULT_ID_TARGET_READY,        // Debugger - target session attached
    RESULT_ID_LAST_BREAKPOINT,    // Debugger - last breakpoint created

    RESULT_ID_DIRECT_TARGET_COMPLETE,    // DirectDebug - target function complete

    ID_DEBUG_INITIALIZER,        // Debugger - debug package initializer? checkbox
};

class wsResultGrid;
class wsVarWindow;
class wsCodeWindow;
class wsDirectDbg;
class wxSizeReportCtrl;

class wsMainFrame : public pgFrame
{    
    DECLARE_CLASS( wsMainFrame )

  public:
    wsMainFrame(frmMain *parent, const wxString &title);
    virtual ~wsMainFrame();

    wsDirectDbg   *addDirectDbg( const wsConnProp & connProp );        // Create a new direct-debugging window
    wsCodeWindow  *addDebug( const wsConnProp & props );                   // Create a new debugger window
    wxStatusBar   *getStatusBar() { return( m_statusBar ); }            // Returns pointer to the status bar

    wxMenuBar     *m_menuBar;    // Menu bar
    wxToolBar     *m_toolBar;    // Frames' toolbar
    wxMenu        *m_viewMenu; // View menu (can be modified by wxCodeWindow)
    wxMenu        *m_debugMenu; // Debug menu (can be modified by wxCodeWindow)

    wxAuiManager manager;

  private:
    wsCodeWindow   *m_standaloneDebugger;    // Standalone debugger window
    wsDirectDbg    *m_standaloneDirectDbg;    // Standalone direct debugger
    wxStatusBar    *m_statusBar;    // Frame's status bar

    wxMenuBar    *setupMenuBar( void );
    wxToolBar    *setupToolBar( void );
    wxStatusBar  *setupStatusBar( void );

    frmMain *m_parent;

    DECLARE_EVENT_TABLE()

    void OnExecute( wxCommandEvent & event );
    void OnDebugCommand( wxCommandEvent & event );
    void OnSelectFrame( wxCommandEvent & event );
    void OnMarginClick( wxStyledTextEvent & event );    // Set/clear breakpoint on margin click
    void OnClose( wxCloseEvent & event );
    void OnExit( wxCommandEvent & event );
    void OnSize( wxSizeEvent & event );
    void OnHelp(wxCommandEvent& event);
    void OnContents(wxCommandEvent& event);

    void OnToggleToolBar(wxCommandEvent& event);
    void OnToggleStackPane(wxCommandEvent& event);
    void OnToggleOutputPane(wxCommandEvent& event);
    void OnAuiUpdate(wxAuiManagerEvent& event);
    void OnDefaultView(wxCommandEvent& event);

};

#endif // WSMAINFRAMEH
