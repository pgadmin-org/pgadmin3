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

#include "wsConsole.h"
#include "wsTabWindow.h"

class wsResultGrid;
class wsVarWindow;
class wsCodeWindow;
class wsDirectDbg;
class wxSizeReportCtrl;

class wsMainFrame : public wxDocParentFrame  
{
	enum
	{
        ID_FirstPerspective = wxID_HIGHEST+1
	};
    
    DECLARE_CLASS( wsMainFrame )

  public:
	wsMainFrame( wxDocManager * docManager, const wxString & title, const wxPoint & pos, const wxSize & size );
	virtual ~wsMainFrame();

	bool	addConnect( const wxString & host, const wxString & database, const wxString & port, const wxString & user, const wxString & password );
	wsDirectDbg   *addDirectDbg( const wsConnProp & connProp );		// Create a new direct-debugging window
	wsCodeWindow  *addDebug( const wsConnProp & props );			   	// Create a new debugger window
	wxStatusBar   *getStatusBar() { return( m_statusBar ); }			// Returns pointer to the status bar

	wxDocChildFrame *makeFuncFrame( wxDocument * doc, wxView * view );	// Create a new function-editor frame

	wxMenuBar	*m_menuBar;	// Menu bar
	wxToolBar	*m_toolBar;	// Frames' toolbar
	wxFrameManager  manager;
	wxArrayString   m_perspectives;

	void PerspectivesDef();

  private:
	wsConsole	*m_console;	// Console window (eventually a list)
	wsCodeWindow	*m_standaloneDebugger;	// Standalone debugger window
	wsDirectDbg	*m_standaloneDirectDbg;	// Standalone direct debugger

  private:

	wxStatusBar	*m_statusBar;	// Frame's status bar
	wxMenu		*m_perspectives_menu;

	wxMenuBar	*setupMenuBar( void );
	wxToolBar	*setupToolBar( void );
	wxStatusBar	*setupStatusBar( void );
	void		writeSettings();
	void		readSettings();

    DECLARE_EVENT_TABLE()

	void OnExecute( wxCommandEvent & event );
	void OnDebugCommand( wxCommandEvent & event );
	void OnClose( wxCloseEvent & event );
	void OnSize( wxSizeEvent & event );
	void OnChar( wxKeyEvent & event );
	void OnEditCommand( wxCommandEvent & event );	// Route cut,copy,paste... tool to active child

	void OnAbout(wxCommandEvent& evt);
	void OnRestorePerspective(wxCommandEvent& evt);
//    void OnToggleToolBar(wxCommandEvent& event);	// wxAUI

//  	+---------------------------------------+
//  	|_______________________________________| <-- Toolbar  
//  	|                                |      |
//  	|                                |      |
//  +->	|                                |      |
//  |	|                                |      | <-- wxSashLayoutWindow
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
