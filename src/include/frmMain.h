//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmMain.h - The main form
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMMAIN_H
#define FRMMAIN_H

// wxWindows headers
#include <wx/wx.h>
#include <wx/treectrl.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>

// App headers
#include "pgAdmin3.h"
#include "ctlSQLBox.h"
#include "pgServer.h"
#include "pgCollection.h"
#include "pgDatabase.h"

// Class declarations
class frmMain : public wxFrame
{
public:
    frmMain(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~frmMain();
    void OnTipOfTheDay();
    wxStatusBar *statusBar;
    
private:
    wxTreeCtrl *browser;	
    wxListCtrl *properties;
    wxListCtrl *statistics;
    wxNotebook *listViews;
    ctlSQLBox *sqlPane;
    wxMenuBar *menuBar;
    wxMenu *fileMenu, *toolsMenu, *viewMenu, *helpMenu, *treeContextMenu;
    wxToolBar *toolBar;
    wxTreeItemId servers;
    void OnAbout(wxCommandEvent& event);
    void OnAddServer();
    void OnExit(wxCommandEvent& event);
    void OnUpgradeWizard(wxCommandEvent& event);
    void OnOptions(wxCommandEvent& event);
    void OnPassword(wxCommandEvent& event);
    void OnSaveDefinition(wxCommandEvent& event);
    void OnShowSystemObjects(wxCommandEvent& event);
    void OnSelChanged();
    void OnSelActivated();
	void OnSelRightClick(wxTreeEvent& event);
    void OnDrop();
    void OnRefresh();
	void OnDisconnect();
	void OnProperties();
    void SetButtons(bool refresh, bool create, bool drop, bool properties, bool sql, bool viewData, bool vacuum);

    // Treeview  handlers
    void tvServer(pgServer *server);
    void tvDatabases(pgCollection *collection);
    void tvDatabase(pgDatabase *database);

    // Statistics Handlers
    void svServer(pgServer *server);
    void svDatabases(pgCollection *collection);

    void StoreServers();
    void RetrieveServers();
    void ReconnectServer(pgServer *server);
    DECLARE_EVENT_TABLE()
};

// Menu options
enum
{
    MNU_ADDSERVER = 101,
    MNU_PASSWORD = 102,
    MNU_SAVEDEFINITION = 103,
    MNU_EXIT = 104,
    MNU_UPGRADEWIZARD = 105,
    MNU_OPTIONS = 106,
    MNU_SYSTEMOBJECTS = 107,
    MNU_CONTENTS = 108,
    MNU_TIPOFTHEDAY = 109,
    MNU_ABOUT = 110,
	MNU_REFRESH = 111,
	MNU_CONNECT = 112,
	MNU_DISCONNECT = 113,
	MNU_DROP = 114,
	MNU_PROPERTIES = 115
};

// Toolbar buttons
enum
{
    BTN_ADDSERVER = 201,
    BTN_REFRESH = 202,
    BTN_CREATE = 203,
    BTN_DROP = 204,
    BTN_PROPERTIES = 205,
    BTN_SQL = 206,
    BTN_VIEWDATA = 207,
    BTN_VACUUM = 208,
    BTN_RECORD = 209,
    BTN_STOP = 210
};

// Controls
enum
{
    CTL_BROWSER = 301,
    CTL_PROPVIEW = 302,
    CTL_STATVIEW = 303,
    CTL_SQLPANE = 304
};

#endif
