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
#include "pgSchema.h"

class wxSplitterWindow;


WX_DECLARE_LIST(wxFrame, frameList);


// Class declarations
class frmMain : public wxFrame
{
public:
    frmMain(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~frmMain();
    void OnTipOfTheDay();
    wxStatusBar *statusBar;

    void SetButtons(bool refresh, bool create, bool drop, bool properties, bool sql, bool viewData, bool vacuum);
    void SetDatabase(pgDatabase *newDatabase) { m_database = newDatabase; }

    void RemoveFrame(wxFrame *frame);

private:
    frameList frames;
	pgDatabase *m_database;
    wxTreeCtrl *browser;
    wxListCtrl *properties;
    wxListCtrl *statistics;
    wxNotebook *listViews;
    ctlSQLBox *sqlPane;
    wxMenuBar *menuBar;
    wxMenu *fileMenu, *toolsMenu, *viewMenu, *helpMenu, *treeContextMenu;
    wxToolBar *toolBar;
    wxTreeItemId servers;
	wxImageList *browserImages, *statisticsImages, *propertiesImages;
    wxSplitterWindow *horizontal, *vertical;

    void OnKeyDown(wxKeyEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnAddServer(wxCommandEvent &ev);
    void OnExit(wxCommandEvent& event);
    void OnUpgradeWizard(wxCommandEvent& event);
    void OnOptions(wxCommandEvent& event);
    void OnPassword(wxCommandEvent& event);
    void OnSaveDefinition(wxCommandEvent& event);
    void OnShowSystemObjects(wxCommandEvent& event);
	void OnSql(wxCommandEvent& event);
    
    void OnPropSelChanged(wxListEvent& event);
    
    void OnTreeSelChanged(wxTreeEvent &event);
    void OnConnect(wxCommandEvent &ev);
    void OnSelActivated(wxTreeEvent& event);
	void OnSelRightClick(wxTreeEvent& event);
    void OnCollapse(wxTreeEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnDrop(wxCommandEvent &ev);
    void OnRefresh(wxCommandEvent &ev);
	void OnDisconnect(wxCommandEvent &ev);
	void OnProperties(wxCommandEvent &ev);
	void OnQueryBuilder(wxCommandEvent &ev);

    void StoreServers();
    void RetrieveServers();
    int ReconnectServer(pgServer *server);
    wxTreeItemId RestoreEnvironment(pgServer *server);
    wxTreeItemId denyCollapseItem;

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
	 MNU_PROPERTIES = 115,
	 MNU_QUERYBUILDER = 116
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
