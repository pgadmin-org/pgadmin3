//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: wsMainFrame.h 5827 2007-01-04 16:35:14 hiroshi $
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

#include <wx/frame.h>		
#include <wx/mdi.h>		
#include <wx/laywin.h>
#include <wx/debuggerMenu.h>
#include <wx/splitter.h>
#include <wx/toolbar.h>
#include <wx/docmdi.h>

#include "wsConsole.h"
#include "wsTabWindow.h"

class wsResultGrid;
class wsVarWindow;
class wsCodeWindow;
class wsDirectDbg;

class wsMainFrame : public wxDocMDIParentFrame
{
    DECLARE_CLASS( wsMainFrame )

  public:
    wsMainFrame( wxDocManager * docManager, const wxString & title, const wxPoint & pos, const wxSize & size );
	virtual ~wsMainFrame();

    bool            addConnect( const wxString & host, const wxString & database, const wxString & port, const wxString & user, const wxString & password );
	wsDirectDbg   * addDirectDbg( const wsConnProp & connProp );		// Create a new direct-debugging window
    wsCodeWindow  * addDebug( const wsConnProp & props );			   	// Create a new debugger window
	wxStatusBar   * getStatusBar() { return( m_statusBar ); }			// Returns pointer to the status bar

	wxDocMDIChildFrame * makeFuncFrame( wxDocument * doc, wxView * view );	// Create a new function-editor frame

	wxToolBar		   * getToolBar() { return( m_toolBar ); }
	wxMenuBar		   * getMenuBar() { return( m_menuBar ); }

  private:
    wsConsole			*	m_console;				// Console window (eventually a list)
	wsCodeWindow		* 	m_standaloneDebugger;	// Standalone debugger window
	wsDirectDbg         *   m_standaloneDirectDbg;	// Standalone direct debugger

  private:

    wxMenuBar			*	m_menuBar;			// Menu bar
    wxToolBar			*	m_toolBar;			// Frames' toolbar
    wxStatusBar			*	m_statusBar;		// Frame's status bar

    wxMenuBar	*	setupMenuBar( void );
    wxToolBar	* 	setupToolBar( void );
    wxStatusBar	*	setupStatusBar( void );
	void			writeSettings();
	void			readSettings();

    DECLARE_EVENT_TABLE()

    void OnExecute( wxCommandEvent & event );
    void OnDebugCommand( wxCommandEvent & event );
    void OnClose( wxCloseEvent & event );
    void OnChar( wxKeyEvent & event );
    void OnSize( wxSizeEvent & event );
	void OnEditCommand( wxCommandEvent & event );	// Route cut,copy,paste... tool to active child

//  	+---------------------------------------+
//  	|_______________________________________| <-- Toolbar  
//  	|      |                                |
//  	|      |                                |
//  +->	|      |                                |
//  |	|      |                                | <-- MDIChildFrame
// Tree	|      |                                |
//  |	|      |                                |
//  |	|      |--------------------------------| 
//  |	|      |_________  _____________________| <-- SashLayout with Notebook on top
//  |	|      |  |__|__|__|                    |             ^
//  |	|______|________________________________|             +-- m_tabWindow
//  |	|                                       | <-- StatusBar
//  |	+---------------------------------------+
//  +-- m_treeWindow
};

extern wsMainFrame * glMainFrame;		// The only instance of this class

#endif // WSMAINFRAMEH
