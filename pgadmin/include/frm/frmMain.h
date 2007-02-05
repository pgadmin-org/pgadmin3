//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
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

// wxAUI
#include <wx/aui/aui.h>

#include "dlg/dlgClasses.h"
#include "utils/factory.h"

#define FRMMAIN_PERPSECTIVE_VER wxT("$Rev$")

#ifdef __WXMAC__
#define FRMMAIN_DEFAULT_PERSPECTIVE wxT("layout2|name=objectBrowser;caption=Object browser;state=16779260;dir=4;layer=1;row=0;pos=0;prop=100000;bestw=200;besth=450;minw=100;minh=200;maxw=-1;maxh=-1;floatx=236;floaty=222;floatw=-1;floath=-1|name=listViews;caption=Info pane;state=1020;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=400;besth=200;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=sqlPane;caption=SQL pane;state=16779260;dir=3;layer=0;row=0;pos=0;prop=100000;bestw=400;besth=200;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=toolBar;caption=Tool bar;state=16788208;dir=1;layer=10;row=0;pos=0;prop=100000;bestw=465;besth=39;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|dock_size(5,0,0)=18|dock_size(3,0,0)=228|dock_size(1,10,0)=41|dock_size(4,1,0)=233|")
#else
#ifdef __WXGTK__
#define FRMMAIN_DEFAULT_PERSPECTIVE wxT("layout2|name=objectBrowser;caption=Object browser;state=16779260;dir=4;layer=1;row=0;pos=0;prop=100000;bestw=200;besth=450;minw=100;minh=200;maxw=-1;maxh=-1;floatx=236;floaty=222;floatw=-1;floath=-1|name=listViews;caption=Info pane;state=1020;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=400;besth=200;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=sqlPane;caption=SQL pane;state=16779260;dir=3;layer=0;row=0;pos=0;prop=100000;bestw=400;besth=200;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=toolBar;caption=Tool bar;state=16788208;dir=1;layer=10;row=0;pos=0;prop=100000;bestw=510;besth=39;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|dock_size(5,0,0)=18|dock_size(3,0,0)=228|dock_size(1,10,0)=41|dock_size(4,1,0)=233|")
#else
#define FRMMAIN_DEFAULT_PERSPECTIVE wxT("layout2|name=objectBrowser;caption=Object browser;state=16779260;dir=4;layer=1;row=0;pos=0;prop=100000;bestw=200;besth=450;minw=100;minh=200;maxw=-1;maxh=-1;floatx=236;floaty=222;floatw=-1;floath=-1|name=listViews;caption=Info pane;state=1020;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=400;besth=200;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=sqlPane;caption=SQL pane;state=16779260;dir=3;layer=0;row=0;pos=0;prop=100000;bestw=400;besth=200;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=toolBar;caption=Tool bar;state=16788208;dir=1;layer=10;row=0;pos=0;prop=100000;bestw=453;besth=39;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|dock_size(5,0,0)=18|dock_size(3,0,0)=228|dock_size(1,10,0)=41|dock_size(4,1,0)=233|")

#endif
#endif
class pgServer;
class pgServerCollection;
class ctlSQLBox;
class ctlTree;
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
    ctlListView *GetDependencies();
    ctlListView *GetDependenciesCtl();
    ctlListView *GetReferencedBy();
    ctlListView *GetReferencedByCtl();
	void SelectStatisticsTab() { listViews->SetSelection(1); };
    void StoreServers();
    int ReconnectServer(pgServer *server, bool restore = true);
    void ReportConnError(pgServer *server);
    pgServerCollection *GetServerCollection() { return serversObj; }
	pgServer *ConnectToServer(const wxString& servername, bool restore = false);

private:
    wxAuiManager manager;
    ctlTree *browser;
    ctlListView *properties;
    ctlListView *statistics;
    ctlListView *dependents, *dependencies;
    wxNotebook *listViews;
    ctlSQLBox *sqlPane;
    wxMenu *newMenu, *reportMenu, *toolsMenu, *viewMenu, *treeContextMenu, *newContextMenu, *slonyMenu, 
		*scriptingMenu, *viewDataMenu;
    pgServerCollection *serversObj;

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

    void OnEraseBackground(wxEraseEvent& event);
    void OnSize(wxSizeEvent& event);
    
    void CreateMenus();
    void OnContents(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void ViewData(bool filter = false);
    void OnSaveDefinition(wxCommandEvent& event);
    void OnShowSystemObjects(wxCommandEvent& event);
    void OnToggleSqlPane(wxCommandEvent& event);
    void OnToggleObjectBrowser(wxCommandEvent& event);
    void OnToggleToolBar(wxCommandEvent& event);
    void OnDefaultView(wxCommandEvent& event);
    void OnAuiUpdate(wxAuiManagerEvent& event);
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
