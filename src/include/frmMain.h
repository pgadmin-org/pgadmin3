//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
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
class dlgProperty;

WX_DECLARE_LIST(wxWindow, windowList);


// Class declarations
class frmMain : public wxFrame
{
public:
    frmMain(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~frmMain();
    void OnTipOfTheDay(wxCommandEvent& event);
    wxStatusBar *statusBar;

    void SetButtons(bool refresh, bool create, bool drop, bool properties, bool sql, bool viewData, bool maintenance);
    void SetDatabase(pgDatabase *newDatabase) { m_database = newDatabase; }

    void Refresh(pgObject *data);
    void RemoveFrame(wxWindow *frame);

    wxImageList *GetImageList() { return images; }
    wxTreeCtrl *GetBrowser() { return browser; }
    ctlSQLBox *GetSqlPane() { return sqlPane; }

private:
    windowList frames;
	pgDatabase *m_database;
    wxTreeCtrl *browser;
    wxListCtrl *properties;
    wxListCtrl *statistics;
    wxNotebook *listViews;
    ctlSQLBox *sqlPane;
    wxMenuBar *menuBar;
    wxMenu *fileMenu, *editMenu, *newMenu, *toolsMenu, *viewMenu, *helpMenu, *treeContextMenu, *newContextMenu;
    wxToolBar *toolBar;
    wxTreeItemId servers;
	wxImageList *images;
    wxSplitterWindow *horizontal, *vertical;

    void OnKeyDown(wxKeyEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnContents(wxCommandEvent& event);
    void OnHelp(wxCommandEvent& event);
    void OnPgsqlHelp(wxCommandEvent& event);
    void OnFaq(wxCommandEvent& event);
    void OnBugreport(wxCommandEvent& event);
    void OnAddServer(wxCommandEvent &ev);
    void OnExit(wxCommandEvent& event);
    void OnViewData(wxCommandEvent& event);
    void OnOptions(wxCommandEvent& event);
    void OnPassword(wxCommandEvent& event);
    void OnSaveDefinition(wxCommandEvent& event);
    void OnShowSystemObjects(wxCommandEvent& event);
    void OnSql(wxCommandEvent& event);
    void OnMaintenance(wxCommandEvent& event);
    void OnIndexcheck(wxCommandEvent& event);
    void OnStatus(wxCommandEvent& event);
    void OnContextMenu(wxCommandEvent& event);

    
    void OnPropSelChanged(wxListEvent& event);
    void OnTreeSelChanged(wxTreeEvent &event);
    void OnTreeKeyDown(wxTreeEvent& event);
    void OnConnect(wxCommandEvent &ev);
    void OnSelActivated(wxTreeEvent& event);
    void OnSelRightClick(wxTreeEvent& event);
    void OnCollapse(wxTreeEvent& event);
    void OnExpand(wxTreeEvent& event);
    void OnClose(wxCloseEvent& event);

    void OnNew(wxCommandEvent& event);
    void OnCreate(wxCommandEvent &ev);
    void OnDrop(wxCommandEvent &ev);
    void OnDelete(wxCommandEvent &ev);
    void OnProperties(wxCommandEvent &ev);
    void OnReload(wxCommandEvent &ev);
    void OnRefresh(wxCommandEvent &ev);
    void OnDisconnect(wxCommandEvent &ev);
    void OnQueryBuilder(wxCommandEvent &ev);

    void execSelChange(wxTreeItemId item, bool currentNode);
    void setDisplay(pgObject *data, wxListCtrl *props=0, wxListCtrl *stats=0, ctlSQLBox *sqlbox=0);
    void StoreServers();
    void RetrieveServers();
    int ReconnectServer(pgServer *server);
    wxTreeItemId RestoreEnvironment(pgServer *server);
    wxTreeItemId denyCollapseItem;
    pgObject *GetSelectedObject();

    DECLARE_EVENT_TABLE()
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
