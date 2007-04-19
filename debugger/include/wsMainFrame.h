//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
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

#include "debugger.h"

#include <wx/frame.h>		
#include <wx/docview.h>		
#include <wx/laywin.h>
#include <wx/menu.h>
#include <wx/splitter.h>
#include <wx/toolbar.h>

#include "wsRichWindow.h"
#include "wsTabWindow.h"

#define WSMAINFRAME_PERPSECTIVE_VER wxT("$Rev$")

#ifdef __WXMAC__
#define WSMAINFRAME_DEFAULT_PERSPECTIVE wxT("layout2|name=toolBar;caption=Toolbar;state=2108144;dir=1;layer=10;row=0;pos=1;prop=100000;bestw=154;besth=23;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=sourcePane;caption=Source pane;state=1020;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=350;besth=200;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=stackPane;caption=Stack pane;state=2099196;dir=2;layer=0;row=0;pos=0;prop=100000;bestw=250;besth=200;minw=100;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=outputPane;caption=Output pane;state=2099196;dir=3;layer=0;row=0;pos=0;prop=100000;bestw=550;besth=300;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|dock_size(1,10,0)=25|dock_size(5,0,0)=237|dock_size(2,0,0)=237|dock_size(3,0,0)=156|")
#else
#ifdef __WXGTK__
#define WSMAINFRAME_DEFAULT_PERSPECTIVE wxT("layout2|name=toolBar;caption=Toolbar;state=2108144;dir=1;layer=10;row=0;pos=1;prop=100000;bestw=154;besth=23;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=sourcePane;caption=Source pane;state=1020;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=350;besth=200;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=stackPane;caption=Stack pane;state=2099196;dir=2;layer=0;row=0;pos=0;prop=100000;bestw=250;besth=200;minw=100;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=outputPane;caption=Output pane;state=2099196;dir=3;layer=0;row=0;pos=0;prop=100000;bestw=550;besth=300;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|dock_size(1,10,0)=25|dock_size(5,0,0)=237|dock_size(2,0,0)=237|dock_size(3,0,0)=156|")
#else
#define WSMAINFRAME_DEFAULT_PERSPECTIVE wxT("layout2|name=toolBar;caption=Toolbar;state=2108144;dir=1;layer=10;row=0;pos=1;prop=100000;bestw=154;besth=23;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=sourcePane;caption=Source pane;state=1020;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=350;besth=200;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=stackPane;caption=Stack pane;state=2099196;dir=2;layer=0;row=0;pos=0;prop=100000;bestw=250;besth=200;minw=100;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=outputPane;caption=Output pane;state=2099196;dir=3;layer=0;row=0;pos=0;prop=100000;bestw=550;besth=300;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|dock_size(1,10,0)=25|dock_size(5,0,0)=237|dock_size(2,0,0)=237|dock_size(3,0,0)=156|")
#endif
#endif

class wsResultGrid;
class wsVarWindow;
class wsCodeWindow;
class wsDirectDbg;
class wxSizeReportCtrl;

class wsMainFrame : public wxFrame  
{    
    DECLARE_CLASS( wsMainFrame )

  public:
	wsMainFrame( wxWindow *parent, const wxString &title, const wxPoint & pos, const wxSize & size );
	virtual ~wsMainFrame();

	void	OnMarginClick( wxStyledTextEvent & event );	// Set/clear breakpoint on margin click
	wsDirectDbg   *addDirectDbg( const wsConnProp & connProp );		// Create a new direct-debugging window
	wsCodeWindow  *addDebug( const wsConnProp & props );			   	// Create a new debugger window
	wxStatusBar   *getStatusBar() { return( m_statusBar ); }			// Returns pointer to the status bar

	wxDocChildFrame *makeFuncFrame( wxDocument * doc, wxView * view );	// Create a new function-editor frame

	wxMenuBar	*m_menuBar;	// Menu bar
	wxToolBar	*m_toolBar;	// Frames' toolbar
	wxMenu		*m_viewMenu; // View menu (can be modified by wxCodeWindow)
	wxMenu		*m_debugMenu; // Debug menu (can be modified by wxCodeWindow)

    wxAuiManager manager;

    wxString VerFromRev(const wxString &rev);

  private:
	wsCodeWindow	*m_standaloneDebugger;	// Standalone debugger window
	wsDirectDbg	*m_standaloneDirectDbg;	// Standalone direct debugger
	wxStatusBar	*m_statusBar;	// Frame's status bar

	wxMenuBar	*setupMenuBar( void );
	wxToolBar	*setupToolBar( void );
	wxStatusBar	*setupStatusBar( void );

    DECLARE_EVENT_TABLE()

	void OnExecute( wxCommandEvent & event );
	void OnDebugCommand( wxCommandEvent & event );
	void OnClose( wxCloseEvent & event );
    void OnExit( wxCommandEvent & event );
	void OnSize( wxSizeEvent & event );

	void OnAbout(wxCommandEvent& evt);

    void OnToggleToolBar(wxCommandEvent& event);
    void OnToggleStackPane(wxCommandEvent& event);
    void OnToggleOutputPane(wxCommandEvent& event);
    void OnAuiUpdate(wxAuiManagerEvent& event);
    void OnDefaultView(wxCommandEvent& event);

//  	+---------------------------------------+
//  	|_______________________________________| <-- Toolbar  
//  	|                                |      |
//  	|                                |      |
//  +->	|                                |      |
//  |	|                                |      | <-- wxWindow
// Tree	|                                |      |
//  |	|                                |      |
//  |	|--------------------------------|      | 
//  |	|_________  _____________________|      | <-- SashLayout with Notebook on top
//  |	|  |__|__|__|                    |      |             ^
//  |	|________________________________|______|             +-- m_tabWindow
//  |	|                                       | <-- StatusBar
//  |	+---------------------------------------+
//  +-- m_treeWindow
};

extern wsMainFrame	*glMainFrame;	// The only instance of this class

#endif // WSMAINFRAMEH
