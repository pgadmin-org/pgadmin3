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

// Class declarations
class frmMain : public wxFrame
{
public:
    frmMain(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~frmMain();
    void OnTipOfTheDay();
    wxStatusBar *stBar;
    
private:
    wxTreeCtrl* tvBrowser;
    wxListCtrl* lvProperties;
    wxListCtrl* lvStatistics;
    wxNotebook* nbListViews;
    ctlSQLBox* txtSQLPane;
    wxMenuBar *mnuBar;
    wxToolBar *tlBar;
    wxTreeItemId itmServers;
    void OnAbout(wxCommandEvent& event);
    void OnAddServer();
    void OnExit(wxCommandEvent& event);
    void OnUpgradeWizard(wxCommandEvent& event);
    void OnOptions(wxCommandEvent& event);
    void OnPassword(wxCommandEvent& event);
    void OnSelChanged();
    void OnSelActivated();
    void OnDrop();
    void OnRefresh();
    void SetButtons(bool bRefresh, bool bCreate, bool bDrop, bool bProperties, bool bSQL, bool bViewData, bool bVacuum);

    // Treeview  handlers
    void tvServer(pgServer *objServer);

    // Statistics Handlers
    void svServer(pgServer *objServer);

    void StoreServers();
    void RetrieveServers();
    void ReconnectServer(pgServer *objServer);
    DECLARE_EVENT_TABLE()
};

// Menu options
enum
{
    MNU_ADDSERVER = 101,
    MNU_PASSWORD = 102,
    MNU_SAVEDEFINITION = 103,
    MNU_SAVESCHEMA = 104,
    MNU_EXIT = 105,
    MNU_UPGRADEWIZARD = 106,
    MNU_OPTIONS = 107,
    MNU_SYSTEMOBJECTS = 108,
    MNU_CONTENTS = 109,
    MNU_TIPOFTHEDAY = 110,
    MNU_ABOUT = 111
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
