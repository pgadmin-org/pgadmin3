//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmMain.cpp - The main form
//
//////////////////////////////////////////////////////////////////////////

// wxWindows Headers
#include <wx/wx.h>

// App headers
#include "../pgAdmin3.h"
#include "frmMain.h"
#include "frmAbout.h"

// Event table
BEGIN_EVENT_TABLE(frmMain, wxFrame)
EVT_MENU(mnuExit,  frmMain::OnExit)
EVT_MENU(mnuAbout, frmMain::OnAbout)
END_EVENT_TABLE()

frmMain::frmMain(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
  
  // Icon
  SetIcon(wxIcon("images/pgAdmin3.xpm", wxBITMAP_TYPE_XPM));
  
  // Build menus
  mnuBar = new wxMenuBar();
  
  // File Menu
  wxMenu *mnuFile = new wxMenu;
  mnuFile->Append(mnuConnect, "&Connect...", "Connect to a PostgreSQL server");
  mnuFile->Append(mnuPassword, "C&hange password...", "Change your password");
  mnuFile->AppendSeparator();
  mnuFile->Append(mnuSaveDefinition, "&Save definition...", "Save the SQL definition of the selected object");
  mnuFile->Append(mnuSaveSchema, "S&ave DB schema...", "Save the schema of the current database");
  mnuFile->AppendSeparator();
  mnuFile->Append(mnuExit, "E&xit", "Quit this program");
  mnuBar->Append(mnuFile, "&File");
  
  // Tools Menu
  wxMenu *mnuTools = new wxMenu;
  mnuTools->Append(mnuUpgradeWizard, "&Upgrade Wizard...", "Run the upgrade wizard");
  mnuTools->AppendSeparator();
  mnuTools->Append(mnuOptions, "&Options...", "Show options dialog");
  mnuBar->Append(mnuTools, "&Tools");
  
  // View Menu
  wxMenu *mnuView = new wxMenu;
  mnuView->Append(mnuSystemObjects, "&System objects", "Show or hide system objects");
  mnuBar->Append(mnuView, "&View");
  
  // Help Menu
  wxMenu *mnuHelp = new wxMenu;
  mnuHelp->Append(mnuContents, "&Help...", "Open the helpfile");
  mnuHelp->Append(mnuTipOfTheDay, "&Tip of the day...", "Show a tip of the day");
  mnuHelp->AppendSeparator();
  mnuHelp->Append(mnuAbout, "&About...", "Show about dialog");
  mnuBar->Append(mnuHelp, "&Help");
  
  // Add the Menubar
  SetMenuBar(mnuBar);
  
  // Status bar
  CreateStatusBar(6);
  SetStatusText("Ready.", 0);
  SetStatusText("0 Secs", 1);
  SetStatusText("Object: Not connected.", 2);
  SetStatusText("Schema: Not connected.", 3);
  SetStatusText("Database: Not connected.", 4);
  SetStatusText("Server: Not connected.", 5);
  
  // Toolbar bar
  CreateToolBar(6);
  
  // Return objects
  stBar = GetStatusBar();
  tlBar = GetToolBar();
}

frmMain::~frmMain()
{
  
}

// Event handlers
void frmMain::OnExit(wxCommandEvent& WXUNUSED(event))
{
  // TRUE is to force the frame to close
  Close(TRUE);
}

void frmMain::OnAbout(wxCommandEvent& WXUNUSED(event))
{
  frmAbout *winAbout = new frmAbout();
  winAbout->Show(TRUE);
}
