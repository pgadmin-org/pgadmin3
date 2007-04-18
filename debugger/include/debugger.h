//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// debugger.h - debugger 
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// class wsApp
//
//	This class is the wxWidgets application class.  It handles the command line
//  and program entry and exit.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef DEBUGGER
#define DEBUGGER

#include <wx/wx.h>
#include <wx/cmdline.h>
#include <wx/docview.h>
#include <wx/config.h>
#include <wx/aui/aui.h>

#include "wsConnProp.h"

class wsMainFrame;		 	// Forward (incomplete) declaration

class wsApp : public wxApp
{
  public:
	virtual	bool	OnInit();	// Initialize the Workstation application
	virtual	int	OnExit();	// De-initialize the Workstation application

	wxDocManager *getDocMgr() { return( m_docManager ); }	// Returns document manager
	wsConnProp   &getConnProp() { return( m_connProp ); }	// Returns command-line connection properties
	wxConfig     &getSettings() { return( *m_settings ); }	// Returns settings/configuration object
	wxStatusBar  *getStatusBar();				// Returns application-wide status bar
	wxFont		 GetSystemFont() { return( systemFont) ; }
	wxFont		 GetSqlFont() { return( sqlFont) ; }

  private:   

	wxCmdLineParser	*m_cmdLine;	// Command line given by the user
	wsMainFrame	*m_mainFrame;	// Main frame object
	wxDocManager    *m_docManager;	// Document/View manager
	wxConfig	*m_settings;	// Setting/configuration manager
	wsConnProp	m_connProp;	// Connection properties (from command-line)

	wxFont systemFont, sqlFont;

	void handleCmdLine( void );	// Process command-line arguments
	void initializeLocale( wxChar * argv0 );	// Initialize locale and load language catalog
};

extern wsApp *glApp;

#endif // DEBUGGER
