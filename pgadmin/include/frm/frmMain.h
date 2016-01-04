//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
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

#include "frm/frmQuery.h"
#include "dlg/dlgClasses.h"
#include "utils/factory.h"

//
// This number MUST be incremented if changing any of the default perspectives
//
#define FRMMAIN_PERSPECTIVE_VER wxT("8418")

#ifdef __WXMAC__
#define FRMMAIN_DEFAULT_PERSPECTIVE wxT("layout2|name=objectBrowser;caption=Object browser;state=16779260;dir=4;layer=1;row=0;pos=0;prop=100000;bestw=200;besth=450;minw=100;minh=200;maxw=-1;maxh=-1;floatx=236;floaty=222;floatw=-1;floath=-1|name=listViews;caption=Info pane;state=1020;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=400;besth=200;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=sqlPane;caption=SQL pane;state=16779260;dir=3;layer=0;row=0;pos=0;prop=100000;bestw=400;besth=200;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=toolBar;caption=Tool bar;state=16788208;dir=1;layer=10;row=0;pos=0;prop=100000;bestw=520;besth=39;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|dock_size(5,0,0)=18|dock_size(3,0,0)=228|dock_size(1,10,0)=41|dock_size(4,1,0)=233|")
#else
#ifdef __WXGTK__
#define FRMMAIN_DEFAULT_PERSPECTIVE wxT("layout2|name=objectBrowser;caption=Object browser;state=16779260;dir=4;layer=1;row=0;pos=0;prop=100000;bestw=200;besth=450;minw=100;minh=200;maxw=-1;maxh=-1;floatx=236;floaty=222;floatw=-1;floath=-1|name=listViews;caption=Info pane;state=1020;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=400;besth=200;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=sqlPane;caption=SQL pane;state=16779260;dir=3;layer=0;row=0;pos=0;prop=100000;bestw=400;besth=200;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=toolBar;caption=Tool bar;state=16788208;dir=1;layer=10;row=0;pos=0;prop=100000;bestw=586;besth=44;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|dock_size(5,0,0)=18|dock_size(3,0,0)=228|dock_size(1,10,0)=41|dock_size(4,1,0)=233|")
#else
#define FRMMAIN_DEFAULT_PERSPECTIVE wxT("layout2|name=objectBrowser;caption=Object browser;state=16779260;dir=4;layer=1;row=0;pos=0;prop=100000;bestw=200;besth=450;minw=100;minh=200;maxw=-1;maxh=-1;floatx=236;floaty=222;floatw=-1;floath=-1|name=listViews;caption=Info pane;state=1020;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=400;besth=200;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=sqlPane;caption=SQL pane;state=16779260;dir=3;layer=0;row=0;pos=0;prop=100000;bestw=400;besth=200;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=toolBar;caption=Tool bar;state=16788208;dir=1;layer=10;row=0;pos=0;prop=100000;bestw=506;besth=39;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|dock_size(5,0,0)=18|dock_size(3,0,0)=228|dock_size(1,10,0)=41|dock_size(4,1,0)=233|")

#endif
#endif
class pgServer;
class pgServerCollection;
class ctlSQLBox;
class ctlTree;
class dlgProperty;
class serverCollection;

class propertyFactory;
class pluginUtilityFactory;
class ctlMenuButton;

// A plugin utility
typedef struct PluginUtility
{
	wxString title;
	wxString command;
	wxString description;
	wxString keyfile;
	wxString platform;
	wxArrayString server_types;
	bool database;
	wxArrayString applies_to;
	bool set_password;
	wxArrayString set_env;
} PluginUtility;


enum
{
	NBP_PROPERTIES = 0,
	NBP_STATISTICS,
	NBP_DEPENDENCIES,
	NBP_DEPENDENTS
};

enum
{
	REFRESH_OBJECT_NONE = 0,
	REFRESH_OBJECT_ONLY,
	REFRESH_OBJECT_AND_CHILDREN
};

#if defined(HAVE_OPENSSL_CRYPTO) || defined(HAVE_GCRYPT)
BEGIN_DECLARE_EVENT_TYPES()
extern const wxEventType SSH_TUNNEL_ERROR_EVENT;
END_DECLARE_EVENT_TYPES()
#endif

// Class declarations
class frmMain : public pgFrame
{
public:
	frmMain(const wxString &title);
	~frmMain();

	void OnAction(wxCommandEvent &ev);
	void OnReport(wxCommandEvent &ev);
	wxString GetHelpPage() const;

	void StartMsg(const wxString &msg);
	void EndMsg(bool done = true);
	void SetStatusText(const wxString &msg);
	void SetCurrentObject(pgObject *data)
	{
		currentObject = data;
	}
	bool CheckAlive();

	void execSelChange(wxTreeItemId item, bool currentNode);
	void Refresh(pgObject *data);
	void ExecDrop(bool cascaded);
	void ShowObjStatistics(pgObject *data, wxWindow *ctrl = NULL);

	wxImageList *GetImageList()
	{
		return imageList;
	}
	ctlTree *GetBrowser()
	{
		return browser;
	}
	ctlSQLBox *GetSqlPane()
	{
		return sqlPane;
	}
	ctlListView *GetProperties()
	{
		return properties;
	}
	ctlListView *GetStatistics();
	ctlListView *GetDependencies();
	ctlListView *GetReferencedBy();
	void SelectStatisticsTab()
	{
		listViews->SetSelection(1);
	};
	void StoreServers();
	int ReconnectServer(pgServer *server, bool restore = true);
	void ReportConnError(pgServer *server);
	pgServerCollection *GetServerCollection()
	{
		return serversObj;
	}
	pgServer *ConnectToServer(const wxString &servername, bool restore = false);

	void SetLastPluginUtility(pluginUtilityFactory *pluginFactory)
	{
		lastPluginUtility = pluginFactory;
	}
	pluginUtilityFactory *GetLastPluginUtility()
	{
		return lastPluginUtility;
	}
	wxMenu *GetPluginsMenu()
	{
		return pluginsMenu;
	}

	wxString GetCurrentNodePath();
	bool SetCurrentNode(wxTreeItemId node, const wxString &path);

	void UpdateAllRecentFiles();
	void UpdateAllFavouritesList();
	void UpdateAllMacrosList();

	void SetItemBackgroundColour(wxTreeItemId item, wxColour colour);
	wxString GetNodePath(wxTreeItemId node);
	void ObjectBrowserRefreshing(bool refresh)
	{
		m_refreshing = refresh;
	}

#if defined(HAVE_OPENSSL_CRYPTO) || defined(HAVE_GCRYPT)
	void OnSSHTunnelEvent(wxCommandEvent &event);
#endif

private:
	wxAuiManager manager;
	ctlTree *browser;
	ctlListView *properties;
	ctlListView *statistics;
	ctlListView *dependents, *dependencies;
	ctlAuiNotebook *listViews;
	ctlSQLBox *sqlPane;
	wxMenu *newMenu, *debuggingMenu, *reportMenu, *toolsMenu, *pluginsMenu, *viewMenu,
	       *treeContextMenu, *newContextMenu, *slonyMenu, *scriptingMenu, *viewDataMenu;
	pgServerCollection *serversObj;

	pluginUtilityFactory *lastPluginUtility;
	int pluginUtilityCount;

	propertyFactory *propFactory;
	actionFactory *newMenuFactory;
	actionFactory *debuggingMenuFactory;
	actionFactory *reportMenuFactory;
	actionFactory *scriptingMenuFactory;
	actionFactory *viewdataMenuFactory;

	wxStopWatch stopwatch;
	wxString timermsg;
	long msgLevel;

	bool m_refreshing;

	wxTreeItemId denyCollapseItem;
	pgObject *currentObject;
	wxControl *currentControl;

	void OnChildFocus(wxChildFocusEvent &event);
	void OnEraseBackground(wxEraseEvent &event);
	void OnSize(wxSizeEvent &event);
	void OnSelectItem(wxListEvent &event);

	void CreateMenus();
	void OnContents(wxCommandEvent &event);
	void OnExit(wxCommandEvent &event);
	void ViewData(bool filter = false);
	void OnSaveDefinition(wxCommandEvent &event);
	void OnToggleSqlPane(wxCommandEvent &event);
	void OnToggleObjectBrowser(wxCommandEvent &event);
	void OnToggleToolBar(wxCommandEvent &event);
	void OnDefaultView(wxCommandEvent &event);
	void OnAuiUpdate(wxAuiManagerEvent &event);
	void OnAuiNotebookPageClose(wxAuiNotebookEvent &event);
	void OnContextMenu(wxCommandEvent &event);

	void OnPageChange(wxAuiNotebookEvent &event);
	void OnPropSelChanged(wxListEvent &event);
	void OnPropSelActivated(wxListEvent &event);
	void OnPropRightClick(wxListEvent &event);
	void OnTreeSelChanged(wxTreeEvent &event);
	void OnTreeKeyDown(wxTreeEvent &event);
	void OnSelActivated(wxTreeEvent &event);
	void OnSelRightClick(wxTreeEvent &event);
	void OnCollapse(wxTreeEvent &event);
	void OnExpand(wxTreeEvent &event);
	void OnClose(wxCloseEvent &event);

	void OnNew(wxCommandEvent &event);
	void OnDelete(wxCommandEvent &ev);
	void OnCopy(wxCommandEvent &ev);

	void OnCheckAlive(wxCommandEvent &event);

	void OnPositionStc(wxStyledTextEvent &event);

	void ResetLists();
	bool dropSingleObject(pgObject *data, bool updateFinal, bool cascaded);
	void doPopup(wxWindow *win, wxPoint point, pgObject *object);
	void setDisplay(pgObject *data, ctlListView *props = 0, ctlSQLBox *sqlbox = 0);
	void RetrieveServers();
	bool reportError(const wxString &error, const wxString &msgToIdentify, const wxString &hint);
	wxTreeItemId RestoreEnvironment(pgServer *server);

	void GetExpandedChildNodes(wxTreeItemId node, wxArrayString &expandedNodes);
	void ExpandChildNodes(wxTreeItemId node, wxArrayString &expandedNodes);


	void PopulatePluginButtonMenu(wxCommandEvent &event);

	// In plugins.cpp
	void LoadPluginUtilities();
	void AddPluginUtility(PluginUtility *util);
	void CreatePluginUtility(PluginUtility *util);
	void ClearPluginUtility(PluginUtility *util);

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

class contentsFactory : public actionFactory
{
public:
	contentsFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
};


class pgsqlHelpFactory : public actionFactory
{
public:
	pgsqlHelpFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar, bool bigTool);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
};


class edbHelpFactory : public actionFactory
{
public:
	edbHelpFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar, bool bigTool);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
};

class greenplumHelpFactory : public actionFactory
{
public:
	greenplumHelpFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar, bool bigTool);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
};

class slonyHelpFactory : public actionFactory
{
public:
	slonyHelpFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar, bool bigTool);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
};


class faqFactory : public actionFactory
{
public:
	faqFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
};


class bugReportFactory : public actionFactory
{
public:
	bugReportFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
};

class pluginUtilityFactory : public actionFactory
{
public:
	pluginUtilityFactory(menuFactoryList *list, wxMenu *menu, PluginUtility *util);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);

private:
	bool HaveDatabase(pgObject *obj);

	wxString title, command, description;
	bool database, set_password;
	wxArrayString applies_to, server_types, set_env;
};

class pluginButtonMenuFactory : public actionFactory
{
public:
	pluginButtonMenuFactory(menuFactoryList *list, wxMenu *popupmenu, ctlMenuToolbar *toolbar, int pluginCount);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);

private:
	ctlMenuButton *pulldownButton;
	bool enableButton;
};

#endif
