//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
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
class frmMain : public pgFrame
{
public:
    frmMain(const wxString& title);
    ~frmMain();
    void OnTipOfTheDay(wxCommandEvent& event);

    void StartMsg(const wxString& msg);
    void EndMsg();

    void SetButtons(bool refresh, bool create, bool drop, bool properties, bool sql, bool viewData, bool maintenance);
    void SetDatabase(pgDatabase *newDatabase) { m_database = newDatabase; }

    void Refresh(pgObject *data);
    void RemoveFrame(wxWindow *frame);

    wxImageList *GetImageList() { return images; }
    wxTreeCtrl *GetBrowser() { return browser; }
    ctlSQLBox *GetSqlPane() { return sqlPane; }
    ctlListView *GetProperties() { return properties; }
    ctlListView *GetStatistics();
    ctlListView *GetDependsOn();
    ctlListView *GetReferencedBy();

private:
    windowList frames;
	pgDatabase *m_database;
    wxTreeCtrl *browser;
    ctlListView *properties;
    ctlListView *statistics;
    ctlListView *referencedBy, *dependsOn;
    wxNotebook *listViews;
    ctlSQLBox *sqlPane;
    wxMenuBar *menuBar;
    wxMenu *fileMenu, *editMenu, *newMenu, *toolsMenu, *viewMenu, *helpMenu, *treeContextMenu, *newContextMenu;
    wxToolBar *toolBar;
    wxTreeItemId servers;
	wxImageList *images;
    wxSplitterWindow *horizontal, *vertical;

    wxStatusBar *statusBar;
    wxStopWatch stopwatch;
    wxString timermsg;
    long msgLevel;

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
    void OnViewFilteredData(wxCommandEvent& event);
	void ViewData(bool filter = false);
    void OnOptions(wxCommandEvent& event);
    void OnPassword(wxCommandEvent& event);
    void OnSaveDefinition(wxCommandEvent& event);
    void OnShowSystemObjects(wxCommandEvent& event);
    void OnSql(wxCommandEvent& event);
    void OnMaintenance(wxCommandEvent& event);
    void OnIndexcheck(wxCommandEvent& event);
    void OnGrantWizard(wxCommandEvent& event);
    void OnStatus(wxCommandEvent& event);
    void OnCount(wxCommandEvent& event);
    void OnContextMenu(wxCommandEvent& event);

    
    void OnPageChange(wxNotebookEvent& event);
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

    void OnCheckAlive(wxCommandEvent& event);

    void execSelChange(wxTreeItemId item, bool currentNode);
    bool checkAlive();
    void setDisplay(pgObject *data, ctlListView *props=0, ctlSQLBox *sqlbox=0);
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
    CTL_NOTEBOOK,
    CTL_PROPVIEW,
    CTL_STATVIEW,
    CTL_DEPVIEW,
    CTL_REFVIEW,
    CTL_SQLPANE
};


enum
{
    NBP_PROPERTIES=0,
    NBP_STATISTICS,
    NBP_DEPENDSON,
    NBP_REFERENCEDBY
};

#endif
