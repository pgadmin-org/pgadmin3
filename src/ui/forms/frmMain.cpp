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
#include <wx/splitter.h>
#include <wx/treectrl.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include <wx/splitter.h>
#include <wx/toolbar.h>
#include <wx/tbarsmpl.h>
#include <wx/imaglist.h>
#include <wx/stc/stc.h>

// App headers
#include "../../pgAdmin3.h"
#include "frmMain.h"
#include "frmAbout.h"
#include "../controls/ctlSQLBox.h"

// Icons
#include "../../images/aggregate.xpm"
#include "../../images/arguments.xpm"
#include "../../images/baddatabase.xpm"
#include "../../images/check.xpm"
#include "../../images/closeddatabase.xpm"
#include "../../images/column.xpm"
#include "../../images/connect.xpm"
#include "../../images/create.xpm"
#include "../../images/database.xpm"
#include "../../images/domain.xpm"
#include "../../images/drop.xpm"
#include "../../images/function.xpm"
#include "../../images/group.xpm"
#include "../../images/hiproperty.xpm"
#include "../../images/index.xpm"
#include "../../images/indexcolumn.xpm"
#include "../../images/language.xpm"
#include "../../images/namespace.xpm"
#include "../../images/operator.xpm"
#include "../../images/pgAdmin3.xpm"
#include "../../images/properties.xpm"
#include "../../images/property.xpm"
#include "../../images/public.xpm"
#include "../../images/record.xpm"
#include "../../images/refresh.xpm"
#include "../../images/relationship.xpm"
#include "../../images/rule.xpm"
#include "../../images/sequence.xpm"
#include "../../images/server.xpm"
#include "../../images/sql.xpm"
#include "../../images/statistics.xpm"
#include "../../images/stop.xpm"
#include "../../images/table.xpm"
#include "../../images/trigger.xpm"
#include "../../images/type.xpm"
#include "../../images/user.xpm"
#include "../../images/vacuum.xpm"
#include "../../images/view.xpm"
#include "../../images/viewdata.xpm"

// Event table
BEGIN_EVENT_TABLE(frmMain, wxFrame)
EVT_MENU(mnuExit,  frmMain::OnExit)
EVT_MENU(mnuAbout, frmMain::OnAbout)
END_EVENT_TABLE()

frmMain::frmMain(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{

    // Icon
    SetIcon(wxIcon(pgAdmin3_xpm));

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

    CreateToolBar();

    // Return objects
    stBar = GetStatusBar();
    tlBar = GetToolBar();

    // Set up toolbar
    wxBitmap tlBarBitmaps[10];
    tlBar->SetToolBitmapSize(wxSize(32, 32));
    tlBarBitmaps[0] = wxBitmap(connect_xpm);
    tlBarBitmaps[1] = wxBitmap(refresh_xpm);
    tlBarBitmaps[2] = wxBitmap(create_xpm);
    tlBarBitmaps[3] = wxBitmap(drop_xpm);
    tlBarBitmaps[4] = wxBitmap(properties_xpm);
    tlBarBitmaps[5] = wxBitmap(sql_xpm);
    tlBarBitmaps[6] = wxBitmap(viewdata_xpm);
    tlBarBitmaps[7] = wxBitmap(vacuum_xpm);
    tlBarBitmaps[8] = wxBitmap(record_xpm);
    tlBarBitmaps[9] = wxBitmap(stop_xpm);

    tlBar->AddTool(100, _T("Connect"), tlBarBitmaps[0], _T("Connect to a server"), wxITEM_NORMAL);
    tlBar->AddTool(101, _T("Refresh"), tlBarBitmaps[1], _T("Refrsh the data below the selected object"), wxITEM_NORMAL);
    tlBar->AddSeparator();
    tlBar->AddTool(102, _T("Create"), tlBarBitmaps[2], _T("Create a new object of the same type as the selected object"), wxITEM_NORMAL);
    tlBar->AddTool(103, _T("Drop"), tlBarBitmaps[3], _T("Drop the currently selected object"), wxITEM_NORMAL);
    tlBar->AddTool(104, _T("Properties"), tlBarBitmaps[4], _T("Display/edit the properties of the selected object"), wxITEM_NORMAL);
    tlBar->AddSeparator();
    tlBar->AddTool(105, _T("SQL"), tlBarBitmaps[5], _T("Execute arbitrary SQL queries"), wxITEM_NORMAL);
    tlBar->AddTool(106, _T("View Data"), tlBarBitmaps[6], _T("View the data in the selected object"), wxITEM_NORMAL);
    tlBar->AddTool(107, _T("Vacuum"), tlBarBitmaps[7], _T("Vacuum the current database or table"), wxITEM_NORMAL);
    tlBar->AddSeparator();
    tlBar->AddTool(108, _T("Record"), tlBarBitmaps[8], _T("Record a query log"), wxITEM_NORMAL);
    tlBar->AddTool(109, _T("Stop"), tlBarBitmaps[9], _T("Stop recording the query log"), wxITEM_NORMAL);

    tlBar->Realize();
    
    // Setup the vertical splitter & treeview
    wxSplitterWindow* splVertical = new wxSplitterWindow(this, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_LIVE_UPDATE | wxCLIP_CHILDREN);
    wxSplitterWindow* splHorizontal = new wxSplitterWindow(splVertical, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_LIVE_UPDATE | wxCLIP_CHILDREN);
    tvBrowser = new wxTreeCtrl(splVertical, -1, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxSIMPLE_BORDER);
    splVertical->SplitVertically(tvBrowser, splHorizontal, 200);
    splVertical->SetMinimumPaneSize(50);

    // Setup the horizontal splitter for the listview & sql pane
    wxNotebook* nbListViews = new wxNotebook(splHorizontal, -1, wxDefaultPosition, wxDefaultSize, wxNB_BOTTOM);
    lvProperties = new wxListCtrl(nbListViews, -1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxSIMPLE_BORDER);
    lvStatistics = new wxListCtrl(nbListViews, -1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxSIMPLE_BORDER);
    nbListViews->AddPage(lvProperties, "Properties");
    nbListViews->AddPage(lvStatistics, "Statistics");
    txtSQLPane = new ctlSQLBox(splHorizontal, -1, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxSIMPLE_BORDER | wxTE_READONLY | wxTE_RICH2);
    txtSQLPane->SetBackgroundColour(*wxLIGHT_GREY);
    splHorizontal->SplitHorizontally(nbListViews, txtSQLPane, 300);
    splHorizontal->SetMinimumPaneSize(50);

    //Setup a Browser imagemap
    wxImageList *ilBrowser = new wxImageList(16, 16);
    //Associate the Browser imagemap to the Browser
    tvBrowser->SetImageList(ilBrowser);
    //Stuff the Image List
    ilBrowser->Add(wxIcon(server_xpm));
    ilBrowser->Add(wxIcon(database_xpm));
    ilBrowser->Add(wxIcon(language_xpm));
    ilBrowser->Add(wxIcon(namespace_xpm));
    ilBrowser->Add(wxIcon(aggregate_xpm));
    ilBrowser->Add(wxIcon(function_xpm));
    ilBrowser->Add(wxIcon(operator_xpm));
    ilBrowser->Add(wxIcon(sequence_xpm));
    ilBrowser->Add(wxIcon(table_xpm));
    ilBrowser->Add(wxIcon(type_xpm));
    ilBrowser->Add(wxIcon(view_xpm));
    ilBrowser->Add(wxIcon(user_xpm));
    ilBrowser->Add(wxIcon(group_xpm));
    ilBrowser->Add(wxIcon(baddatabase_xpm));
    ilBrowser->Add(wxIcon(closeddatabase_xpm));

    // Add some treeview items
    wxTreeItemId itmDummy = tvBrowser->AddRoot("Root node",0);
    tvBrowser->AppendItem(itmDummy, "Child Node #1");
    tvBrowser->AppendItem(itmDummy, "Child Node #2");
    tvBrowser->Expand(itmDummy);
    itmDummy = tvBrowser->AppendItem(itmDummy, "Child Node #3");
    tvBrowser->AppendItem(itmDummy, "Child Node #4");
    tvBrowser->AppendItem(itmDummy, "Child Node #5");

    //Setup a listview imagemap
    wxImageList *ilProperties = new wxImageList(16, 16);
    //Associate the listview imagemap to the listview
    lvProperties->SetImageList(ilProperties, wxIMAGE_LIST_SMALL);
    //Stuff the BrowserImage Listu:
    ilProperties->Add(wxIcon(property_xpm));

    // Add some listview items
    lvProperties->InsertColumn(0, "Property", wxLIST_FORMAT_LEFT, 100);
    lvProperties->InsertColumn(1, "Value", wxLIST_FORMAT_LEFT, 400);

    // This is the bit that puts it all on one line over 2 colums
    lvProperties->InsertItem(0, "Property #1", 0);
    lvProperties->SetItem(0, 1, "Property #1a");

    lvProperties->InsertItem(1, "Property #2", 0);
    lvProperties->SetItem(1, 1, "Property #2a");

    //Setup a listview imagemap
    wxImageList *ilStatistics = new wxImageList(16, 16);
    //Associate the listview imagemap to the listview
    lvStatistics->SetImageList(ilStatistics, wxIMAGE_LIST_SMALL);
    //Stuff the BrowserImage Listu:
    ilStatistics->Add(wxIcon(statistics_xpm));

    // Add some listview items
    lvStatistics->InsertColumn(0, "Statistic", wxLIST_FORMAT_LEFT, 100);
    lvStatistics->InsertColumn(1, "Value", wxLIST_FORMAT_LEFT, 400);

    // This is the bit that puts it all on one line over 2 colums
    lvStatistics->InsertItem(0, "Statistic #1", 0);
    lvStatistics->SetItem(0, 1, "Statistic #1a");

    lvStatistics->InsertItem(1, "Statistic #2", 0);
    lvStatistics->SetItem(1, 1, "Statistic #2a");


    // Setup the SQL Pane
    txtSQLPane->InsertText(0, "-- Select all records from pg_class\nSELECT\n  *\nFROM\n  pg_class\nWHERE\n relname LIKE 'pg_%'\nORDER BY\n  rename;");

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
