//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: frmMain.h 4972 2006-01-27 13:11:17Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmMain.h - The main form
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMMAIN_H
#define FRMMAIN_H


// wxWindows headers
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>

#include "dlgClasses.h"
#include "base/factory.h"

class pgServer;
class pgServerCollection;
class ctlSQLBox;
class ctlTree;
class wxSplitterWindow;
class dlgProperty;
class serverCollection;

class propertyFactory;

// Class declarations
class frmMain : public pgFrame
{
public:
    frmMain(const wxString& title);
    ~frmMain();
    void OnAction(wxCommandEvent &ev);
    void OnReport(wxCommandEvent &ev);
    wxString GetHelpPage() const;

    void StartMsg(const wxString& msg);
    void EndMsg(bool done=true);
    void SetStatusText(const wxString &msg);
    void SetCurrentObject(pgObject *data) { currentObject = data; }
    bool CheckAlive();

    void execSelChange(wxTreeItemId item, bool currentNode);
    void Refresh(pgObject *data);
    void ExecDrop(bool cascaded);
    void ShowObjStatistics(pgObject *data, int sel);

    wxImageList *GetImageList() { return imageList; }
    ctlTree *GetBrowser() { return browser; }
    ctlSQLBox *GetSqlPane() { return sqlPane; }
    ctlListView *GetProperties() { return properties; }
    ctlListView *GetStatistics();
    ctlListView *GetStatisticsCtl();
    ctlListView *GetDependsOn();
    ctlListView *GetDependsOnCtl();
    ctlListView *GetReferencedBy();
    ctlListView *GetReferencedByCtl();
    void StoreServers();
    int ReconnectServer(pgServer *server, bool restore = true);
    void ReportConnError(pgServer *server);
    pgServerCollection *GetServerCollection() { return serversObj; }
	pgServer *ConnectToServer(const wxString& servername, bool restore = false);

private:
    ctlTree *browser;
    ctlListView *properties;
    ctlListView *statistics;
    ctlListView *referencedBy, *dependsOn;
    wxNotebook *listViews;
    ctlSQLBox *sqlPane;
    wxMenu *newMenu, *reportMenu, *toolsMenu, *viewMenu, *treeContextMenu, *newContextMenu, *slonyMenu, 
		*scriptingMenu, *viewDataMenu;
    pgServerCollection *serversObj;
    wxSplitterWindow *horizontal, *vertical;

    propertyFactory *propFactory;
    actionFactory *newMenuFactory;
    actionFactory *reportMenuFactory;
	actionFactory *scriptingMenuFactory;
	actionFactory *viewdataMenuFactory;

    wxStopWatch stopwatch;
    wxString timermsg;
    long msgLevel;

    wxTreeItemId denyCollapseItem;
    pgObject *currentObject;

	void CreateMenus();
    void OnContents(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
	void ViewData(bool filter = false);
    void OnSaveDefinition(wxCommandEvent& event);
    void OnShowSystemObjects(wxCommandEvent& event);
    void OnContextMenu(wxCommandEvent& event);

    void OnPageChange(wxNotebookEvent& event);
    void OnPropSelChanged(wxListEvent& event);
    void OnPropSelActivated(wxListEvent& event);
    void OnPropRightClick(wxListEvent& event);
    void OnTreeSelChanged(wxTreeEvent &event);
    void OnTreeKeyDown(wxTreeEvent& event);
    void OnSelActivated(wxTreeEvent& event);
    void OnSelRightClick(wxTreeEvent& event);
    void OnCollapse(wxTreeEvent& event);
    void OnExpand(wxTreeEvent& event);
    void OnClose(wxCloseEvent& event);

    void OnNew(wxCommandEvent& event);
    void OnDelete(wxCommandEvent &ev);
	void OnCopy(wxCommandEvent &ev) { sqlPane->Copy(); };

    void OnCheckAlive(wxCommandEvent& event);
    void OnOnlineUpdate(wxCommandEvent& event);
    void OnOnlineUpdateNewData(wxCommandEvent& event);

    void OnPositionStc(wxStyledTextEvent& event);

    bool dropSingleObject(pgObject *data, bool updateFinal, bool cascaded);
    void doPopup(wxWindow *win, wxPoint point, pgObject *object);
    void setDisplay(pgObject *data, ctlListView *props=0, ctlSQLBox *sqlbox=0);
    void RetrieveServers();
    bool reportError(const wxString &error, const wxString &msgToIdentify, const wxString &hint);
    wxTreeItemId RestoreEnvironment(pgServer *server);

    DECLARE_EVENT_TABLE()
};

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



#endif
