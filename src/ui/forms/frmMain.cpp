//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmMain.cpp - The main form
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/treectrl.h>
#include <wx/listctrl.h>
#include <wx/textctrl.h>

// fl framework headers
#include <wx/fl/controlbar.h>
#include <wx/fl/barhintspl.h>    // bevel for bars with "X"s and grooves
#include <wx/fl/rowdragpl.h>     // NC-look with draggable rows
#include <wx/fl/cbcustom.h>      // customization plugin
#include <wx/fl/hintanimpl.h>
#include <wx/fl/gcupdatesmgr.h>  // smooth d&d
#include <wx/fl/antiflickpl.h>   // double-buffered repaint of decorations
#include <wx/fl/dyntbar.h>       // auto-layout toolbar
#include <wx/fl/dyntbarhnd.h>    // control-bar dimension handler for it
#include <wx/fl/panedrawpl.h>
#include <wx/fl/bardragpl.h>
#include <wx/fl/cbcustom.h>


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
#ifdef __WXMSW__
  SetIcon(wxIcon("images/pgAdmin3.xpm"));
#else
  SetIcon(wxIcon("images/pgAdmin3.xpm", wxBITMAP_TYPE_XPM));
#endif
  
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
  static const int iWidths[6] = {-1, 50, 100, 100, 100, 100};
  SetStatusWidths(6, iWidths);
  SetStatusText("Ready.", 0);
  SetStatusText("0 Secs", 1);
  SetStatusText("Object: None", 2);
  SetStatusText("Schema: None", 3);
  SetStatusText("Database: None", 4);
  SetStatusText("Server: None", 5);
  
  // Toolbar bar
  // CreateToolBar(6);
  
  // Return objects
  stBar = GetStatusBar();
  // tlBar = GetToolBar();

  int cbWidth  = 90;
  int cbHeight = 60;

  wxPanel* pArea = new wxPanel();  
  pArea->Create( this, -1 );
  wxFrameLayout* pLayout = new wxFrameLayout(this, pArea, TRUE);
  pLayout->SetUpdatesManager(new cbGCUpdatesMgr());  
  pLayout->PushDefaultPlugins();
  pLayout->AddPlugin(CLASSINFO(cbSimpleCustomizationPlugin));
  pLayout->AddPlugin(CLASSINFO(cbBarDragPlugin));
  pLayout->AddPlugin(CLASSINFO(cbPaneDrawPlugin));
  pLayout->AddPlugin(CLASSINFO(cbBarHintsPlugin));
  pLayout->AddPlugin(CLASSINFO(cbHintAnimationPlugin));
  pLayout->AddPlugin(CLASSINFO(cbRowDragPlugin ));
  pLayout->AddPlugin(CLASSINFO(cbAntiflickerPlugin));
  pLayout->AddPlugin(CLASSINFO(cbSimpleCustomizationPlugin));
  
  cbDimInfo dimTreeView(200,200, 300,300, 200,450, FALSE, 3, 3);
  cbDimInfo dimListView(200,500, 300,300, 450,200, FALSE, 3, 3);
  cbDimInfo dimSQLView(200,500, 300,300, 350,250, FALSE, 3, 3);

  pLayout->AddBar(CreateTreeCtrl(), dimTreeView, FL_ALIGN_LEFT, 0, 0,   "TreeView");
  pLayout->AddBar(CreateListCtrl(), dimListView, FL_ALIGN_RIGHT, 0, 0,   "ListView");
  pLayout->AddBar(CreateTextCtrl(), dimSQLView, FL_ALIGN_RIGHT, 0, 1,   "SQL");

  pLayout->RecalcLayout();
}

wxTreeCtrl* frmMain::CreateTreeCtrl()
{
  wxTreeCtrl* pTree = new wxTreeCtrl(this, -1, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxSUNKEN_BORDER);

  // Add some items
  wxTreeItemId pTreeItem = pTree->AddRoot("Root node");
	pTree->AppendItem(pTreeItem, "Child Node #1");
	pTree->AppendItem(pTreeItem, "Child Node #2");
	pTree->Expand(pTreeItem);
	pTreeItem = pTree->AppendItem(pTreeItem, "Child Node #3");
	pTree->AppendItem(pTreeItem, "Child Node #4");
	pTree->AppendItem(pTreeItem, "Child Node #5");

  return pTree;
}

wxListCtrl* frmMain::CreateListCtrl()
{
  wxListCtrl* pList = new wxListCtrl(this, -1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxSUNKEN_BORDER);

  // Add some items
	pList->InsertColumn(1, "This is Column #1", wxLIST_FORMAT_LEFT, 200);
	pList->InsertColumn(2, "This is Column #2", wxLIST_FORMAT_LEFT, 200);
	pList->InsertItem(1, "This is Listview item #1");
	pList->InsertItem(1, "This is Listview item #2");
	pList->InsertItem(1, "This is Listview item #3");

  return pList;
}

wxTextCtrl* frmMain::CreateTextCtrl()
{
  wxTextCtrl* pText = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxSUNKEN_BORDER);

  // Add some text
  pText->SetValue("SQL Pane");

  return pText;
}

// Event handlers
void frmMain::OnExit(wxCommandEvent& WXUNUSED(event))
{
  // TRUE is to force the frame to close
  Close(TRUE);
}

void frmMain::OnAbout(wxCommandEvent& WXUNUSED(event))
{
  frmAbout *winAbout = new frmAbout(this);
  winAbout->Show(TRUE);
}
