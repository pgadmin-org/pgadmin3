//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: wsapp.h 5827 2007-01-04 16:35:14 hiroshi $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsapp.h - debugger 
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// class wsApp
//
//	This class is the wxWidgets application class.  It handles the command line
//  and program entry and exit.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef WSAPPH
#define WSAPPH

#include <wx/wx.h>
#include <wx/cmdline.h>
#include <wx/docview.h>
#include <wx/config.h>

#include "wsconnprop.h"

class wsMainFrame;		 	// Forward (incomplete) declaration

class wsApp : public wxApp
{
  public:
    virtual	bool	OnInit();			// Initialize the Workstation application
    virtual	int		OnExit();			// De-initialize the Workstation application

	wxDocManager *  getDocMgr() { return( m_docManager ); }		// Returns document manager
	wsConnProp   &  getConnProp() { return( m_connProp ); }		// Returns command-line connection properties
	wxConfig     &  getSettings() { return( *m_settings ); }    // Returns settings/configuration object
	wxStatusBar  *  getStatusBar();								// Returns application-wide status bar

  private:   

    wxCmdLineParser	*	m_cmdLine;		// Command line given by the user
    wsMainFrame		*	m_mainFrame;	// Main frame object
	wxDocManager    *   m_docManager;	// Document/View manager
    wxConfig        *   m_settings;     // Setting/configuration manager
	wsConnProp			m_connProp;     // Connection properties (from command-line)

	void handleCmdLine( void );					// Process command-line arguments
	void initializeLocale( wxChar * argv0 );	// Initialize locale and load language catalog
};

extern wsApp * glApp;

#endif // WSAPPH
