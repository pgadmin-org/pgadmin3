//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
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
#include "ctl/ctlSQLBox.h"
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
    wxString GetHelpPage() const;

    void StartMsg(const wxString& msg);
    void EndMsg(bool done=true);
    void SetStatusText(const wxString &msg);
    void SetCurrentObject(pgObject *data) { currentObject = data; }

    void SetButtons(pgObject *obj=0);

    void execSelChange(wxTreeItemId item, bool currentNode);
    void Refresh(pgObject *data);
    void RemoveFrame(wxWindow *frame);
    void SetDatabase(pgDatabase *newDatabase) { m_database = newDatabase; }
    void ShowStatistics(pgObject *data, int sel);

    wxImageList *GetImageList() { return imageList; }
    wxTreeCtrl *GetBrowser() { return browser; }
    ctlSQLBox *GetSqlPane() { return sqlPane; }
    ctlListView *GetProperties() { return properties; }
    ctlListView *GetStatistics();
    ctlListView *GetDependsOn();
    ctlListView *GetReferencedBy();
    void AddFrame(wxWindow *wnd) { frames.Append(wnd); }
    void StoreServers();
    void ReportConnError(pgServer *server);
    wxTreeItemId GetServersNode() { return servers; }

private:
    windowList frames;
    wxTreeCtrl *browser;
    ctlListView *properties;
    ctlListView *statistics;
    ctlListView *referencedBy, *dependsOn;
    wxNotebook *listViews;
    ctlSQLBox *sqlPane;
    wxMenu *newMenu, *toolsMenu, *viewMenu, *treeContextMenu, *newContextMenu, *slonyMenu;
    wxTreeItemId servers;
    wxSplitterWindow *horizontal, *vertical;

    wxStopWatch stopwatch;
    wxString timermsg;
    long msgLevel;

    wxTreeItemId denyCollapseItem;
    pgObject *currentObject;
    pgDatabase *m_database;

    void OnContents(wxCommandEvent& event);
    void OnPgsqlHelp(wxCommandEvent& event);
    void OnFaq(wxCommandEvent& event);
    void OnHint(wxCommandEvent& event);
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
    void OnStatus(wxCommandEvent& event);
    void OnBackup(wxCommandEvent& event);
    void OnRestore(wxCommandEvent& event);
    void OnCount(wxCommandEvent& event);
    void OnContextMenu(wxCommandEvent& event);

    void OnMainFileConfig(wxCommandEvent& event);
    void OnHbaFileConfig(wxCommandEvent& event);

    void OnMergeSet(wxCommandEvent& event);
    void OnMoveSet(wxCommandEvent& event);
    void OnFailover(wxCommandEvent& event);
    void OnUpgradeNode(wxCommandEvent& event);
    void OnRestartNode(wxCommandEvent& event);

    void OnPageChange(wxNotebookEvent& event);
    void OnPropSelChanged(wxListEvent& event);
    void OnPropSelActivated(wxListEvent& event);
    void OnPropRightClick(wxListEvent& event);
    void OnTreeSelChanged(wxTreeEvent &event);
    void OnTreeKeyDown(wxTreeEvent& event);
    void OnConnect(wxCommandEvent &ev);
    void OnSelActivated(wxTreeEvent& event);
    void OnSelRightClick(wxTreeEvent& event);
    void OnCollapse(wxTreeEvent& event);
    void OnExpand(wxTreeEvent& event);
    void OnClose(wxCloseEvent& event);

    void OnNew(wxCommandEvent& event);
    void OnAction(wxCommandEvent& event);
    void OnCreate(wxCommandEvent &ev);
    void OnDrop(wxCommandEvent &ev);
    void OnDropCascaded(wxCommandEvent &ev);
    void OnDelete(wxCommandEvent &ev);
    void OnProperties(wxCommandEvent &ev);
    void OnStartService(wxCommandEvent &ev);
    void OnStopService(wxCommandEvent &ev);
    void OnRefresh(wxCommandEvent &ev);
    void OnDisconnect(wxCommandEvent &ev);

    void OnCheckAlive(wxCommandEvent& event);
    void OnOnlineUpdate(wxCommandEvent& event);
    void OnOnlineUpdateNewData(wxCommandEvent& event);

    void execDrop(wxCommandEvent &ev, bool cascaded);
    bool dropSingleObject(pgObject *data, bool updateFinal, bool cascaded);
    void doPopup(wxWindow *win, wxPoint point, pgObject *object);
    void appendIfEnabled(int id);
    bool checkAlive();
    void setDisplay(pgObject *data, ctlListView *props=0, ctlSQLBox *sqlbox=0);
    void RetrieveServers();
    int ReconnectServer(pgServer *server);
    bool reportError(const wxString &error, const wxString &msgToIdentify, const wxString &hint);
    wxTreeItemId RestoreEnvironment(pgServer *server);

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
