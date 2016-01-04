//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmQuery.h - The SQL Query form
//
//////////////////////////////////////////////////////////////////////////

#ifndef __FRM_QUERY_H
#define __FRM_QUERY_H

#include "ctl/ctlAuiNotebook.h"
#include "db/pgQueryResultEvent.h"
#include "dlg/dlgClasses.h"
#include "gqb/gqbViewController.h"
#include "gqb/gqbModel.h"
#include "frm/frmExport.h"
#include "utils/factory.h"
#include "utils/favourites.h"
#include "utils/macros.h"

#include <wx/sstream.h>
#include <wx/txtstrm.h>
#include <wx/bmpcbox.h>

// wxAUI
#include <wx/aui/aui.h>
#include <wx/textctrl.h>
#include <wx/dcbuffer.h>
#include <wx/timer.h>

//
// This number MUST be incremented if changing any of the default perspectives
//
#define FRMQUERY_PERSPECTIVE_VER wxT("8320")
#ifdef __WXMAC__
#define FRMQUERY_DEFAULT_PERSPECTIVE wxT("layout2|name=toolBar;caption=Tool bar;state=16788208;dir=1;layer=10;row=0;pos=0;prop=100000;bestw=465;besth=23;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=databaseBar;caption=Database bar;state=16788208;dir=1;layer=10;row=0;pos=396;prop=100000;bestw=300;besth=21;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=sqlQuery;caption=SQL query;state=17404;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=350;besth=200;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=outputPane;caption=Output pane;state=16779260;dir=3;layer=0;row=0;pos=0;prop=100000;bestw=550;besth=300;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=scratchPad;caption=Scratch pad;state=16779260;dir=2;layer=0;row=0;pos=0;prop=100000;bestw=250;besth=200;minw=100;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|dock_size(1,10,0)=25|dock_size(5,0,0)=200|dock_size(3,0,0)=290|dock_size(2,0,0)=255|")
#else
#ifdef __WXGTK__
#define FRMQUERY_DEFAULT_PERSPECTIVE wxT("layout2|name=toolBar;caption=Tool bar;state=16788208;dir=1;layer=10;row=0;pos=0;prop=100000;bestw=590;besth=30;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=databaseBar;caption=Database bar;state=16788208;dir=1;layer=10;row=0;pos=396;prop=100000;bestw=300;besth=30;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=sqlQuery;caption=SQL query;state=17404;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=350;besth=200;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=outputPane;caption=Output pane;state=16779260;dir=3;layer=0;row=0;pos=0;prop=100000;bestw=550;besth=300;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=scratchPad;caption=Scratch pad;state=16779260;dir=2;layer=0;row=0;pos=0;prop=100000;bestw=250;besth=200;minw=100;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|dock_size(1,10,0)=25|dock_size(5,0,0)=200|dock_size(3,0,0)=290|dock_size(2,0,0)=255|")
#else
#define FRMQUERY_DEFAULT_PERSPECTIVE wxT("layout2|name=toolBar;caption=Tool bar;state=16788208;dir=1;layer=10;row=0;pos=0;prop=100000;bestw=465;besth=23;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=databaseBar;caption=Database bar;state=16788208;dir=1;layer=10;row=0;pos=396;prop=100000;bestw=300;besth=21;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=sqlQuery;caption=SQL query;state=17404;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=350;besth=200;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=outputPane;caption=Output pane;state=16779260;dir=3;layer=0;row=0;pos=0;prop=100000;bestw=550;besth=300;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=scratchPad;caption=Scratch pad;state=16779260;dir=2;layer=0;row=0;pos=0;prop=100000;bestw=250;besth=200;minw=100;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|dock_size(1,10,0)=25|dock_size(5,0,0)=200|dock_size(3,0,0)=290|dock_size(2,0,0)=255|")
#endif
#endif

class ExplainCanvas;
class ctlSQLResult;
class pgsApplication;
class pgScriptTimer;

class QueryExecInfo
{
public:
	QueryExecInfo()
	{
		toFileExportForm = NULL;
	}
	~QueryExecInfo()
	{
		if (toFileExportForm)
			delete toFileExportForm;
	}

	int queryOffset;
	frmExport *toFileExportForm;
	bool singleResult;
	bool explain;
	bool verbose;
};

enum
{
	ORIGIN_MANUAL,
	ORIGIN_FILE,
	ORIGIN_INITIAL,
	ORIGIN_HISTORY,
	ORIGIN_GQB
};

class frmQuery : public pgFrame
{
public:
	frmQuery(frmMain *form, const wxString &_title, pgConn *conn, const wxString &qry, const wxString &file = wxEmptyString);
	~frmQuery();
	void Go();

	void writeScriptOutput();
	void setExtendedTitle();
	void SetLineEndingStyle();

	void SetQueryText(wxString str)
	{
		sqlQuery->SetText(str);
	}
	void ColouriseQuery(int start, int stop)
	{
		sqlQuery->Colourise(start, stop);
	}
	void SetChanged(bool p_changed)
	{
		changed = p_changed;
	}
	void SetOrigin(int p_origin)
	{
		origin = p_origin;
	}
	void SetLastPath(wxString p_lastpath)
	{
		lastPath = p_lastpath;
	}
	bool CheckChanged(bool canVeto);

	void UpdateFavouritesList();
	void UpdateMacrosList();

	void UpdateAllRecentFiles();
	void UpdateAllFavouritesList();
	void UpdateAllMacrosList();

private:
	frmMain *mainForm;
	wxAuiManager manager;
	ctlSQLBox *sqlQuery;
	ctlAuiNotebook *outputPane;
	ctlSQLResult *sqlResult;
	ExplainCanvas *explainCanvas;
	wxTextCtrl *msgResult, *msgHistory;
	wxBitmapComboBox *cbConnection;
	wxTextCtrl *scratchPad;
	wxComboBox *sqlQueries;
	wxButton *btnDeleteCurrent;
	wxButton *btnDeleteAll;
	wxArrayString histoQueries;

	// Query timing/status update
	wxTimer timer;
	wxLongLong elapsedQuery, startTimeQuery;

	// pgScript interface
	pgsApplication *pgScript;
	wxString pgsOutputString;
	wxStringOutputStream pgsStringOutput;
	wxTextOutputStream pgsOutput;
	pgScriptTimer *pgsTimer;

	//GQB related
	void OnChangeNotebook(wxAuiNotebookEvent &event);
	void OnAdjustSizesTimer(wxTimerEvent &event);
	void OnResizeHorizontally(wxSplitterEvent &event);
	void adjustGQBSizes();
	bool updateFromGqb(bool executing);
	ctlAuiNotebook *sqlNotebook;
	gqbModel *model;
	gqbController *controller;
	bool firstTime;
	bool gqbUpdateRunning;
	wxTimer *adjustSizesTimer;

	// Our connection
	pgConn *conn;

	// These status flags are required to work round some wierdness on wxGTK,
	// particularly on Solaris.
	bool closing, loading;

	void OnEraseBackground(wxEraseEvent &event);
	void OnSize(wxSizeEvent &event);

	void OnChangeStc(wxStyledTextEvent &event);
	void OnPositionStc(wxStyledTextEvent &event);
	void OnClose(wxCloseEvent &event);
	void OnSetFocus(wxFocusEvent &event);
	void OnContents(wxCommandEvent &event);
	void OnHelp(wxCommandEvent &event);
	void OnCancel(wxCommandEvent &event);
	void OnExecute(wxCommandEvent &event);
	void OnExecScript(wxCommandEvent &event);
	void OnExecFile(wxCommandEvent &event);
	void OnExplain(wxCommandEvent &event);
	void OnCommit(wxCommandEvent &event);
	void OnRollback(wxCommandEvent &event);
	void OnBuffers(wxCommandEvent &event);
	void OnTiming(wxCommandEvent &event);
	void OnNew(wxCommandEvent &event);
	void OnOpen(wxCommandEvent &event);
	void OnSave(wxCommandEvent &event);
	void OnSaveAs(wxCommandEvent &event);
	void SaveExplainAsImage(wxCommandEvent &event);
	void OnExport(wxCommandEvent &event);
	void OnExit(wxCommandEvent &event);
	void OnCut(wxCommandEvent &event);
	void OnCopy(wxCommandEvent &event);
	void OnPaste(wxCommandEvent &event);
	void OnClear(wxCommandEvent &event);
	void OnSearchReplace(wxCommandEvent &event);
	void OnUndo(wxCommandEvent &event);
	void OnRedo(wxCommandEvent &event);
	void OnSaveHistory(wxCommandEvent &event);
	void OnAutoRollback(wxCommandEvent &event);
	void OnAutoCommit(wxCommandEvent &event);
	void OnChangeConnection(wxCommandEvent &ev);
	void OnClearHistory(wxCommandEvent &event);
	void OnActivate(wxActivateEvent &event);
	void OnFocus(wxFocusEvent &event);
	void OnSelectAll(wxCommandEvent &event);
	void OnAddFavourite(wxCommandEvent &event);
	void OnInjectFavourite(wxCommandEvent &event);
	void OnManageFavourites(wxCommandEvent &event);
	void OnSelectFavourite(wxCommandEvent &event);
	void OnQuickReport(wxCommandEvent &event);
	void OnAutoIndent(wxCommandEvent &event);
	void OnWordWrap(wxCommandEvent &event);
	void OnShowIndentGuides(wxCommandEvent &event);
	void OnShowWhitespace(wxCommandEvent &event);
	void OnShowLineEnds(wxCommandEvent &event);
	void OnShowLineNumber(wxCommandEvent &event);

	void OnToggleScratchPad(wxCommandEvent &event);
	void OnToggleDatabaseBar(wxCommandEvent &event);
	void OnToggleToolBar(wxCommandEvent &event);
	void OnToggleOutputPane(wxCommandEvent &event);
	void OnAuiUpdate(wxAuiManagerEvent &event);
	void OnDefaultView(wxCommandEvent &event);
	void OnBlockIndent(wxCommandEvent &event);
	void OnBlockOutDent(wxCommandEvent &event);
	void OnChangeToUpperCase(wxCommandEvent &event);
	void OnChangeToLowerCase(wxCommandEvent &event);
	void OnCommentText(wxCommandEvent &event);
	void OnUncommentText(wxCommandEvent &event);
	void OnExternalFormat(wxCommandEvent &event);

	void OnDeleteCurrent(wxCommandEvent &event);
	void OnDeleteAll(wxCommandEvent &event);

	void OnTimer(wxTimerEvent &event);

	void OpenLastFile();
	void updateMenu(bool allowUpdateModelSize = true);
	void execQuery(const wxString &query, int resultToRetrieve = 0, bool singleResult = false, const int queryOffset = 0, bool toFile = false, bool explain = false, bool verbose = false);
	void OnQueryComplete(pgQueryResultEvent &ev);
	void completeQuery(bool done, bool explain, bool verbose);
	bool isBeginNotRequired(wxString query);
	void OnScriptComplete(wxCommandEvent &ev);
	void setTools(const bool running);
	void showMessage(const wxString &msg, const wxString &msgShort = wxT(""));
	int GetLineEndingStyle();
	void OnSetEOLMode(wxCommandEvent &event);
	void SetEOLModeDisplay(int mode);
	void OnMacroInvoke(wxCommandEvent &event);
	void OnMacroManage(wxCommandEvent &event);

	void LoadQueries();
	void SaveQueries();
	void OnChangeQuery(wxCommandEvent &event);

	wxBitmap CreateBitmap(const wxColour &colour);
	wxColour GetServerColour(pgConn *connection);

	bool relatesToWindow(wxWindow *which, wxWindow *related);

	wxWindow *currentControl();
	wxMenu *queryMenu;
	wxMenu *favouritesMenu;
	wxMenu *macrosMenu;
	wxMenu *lineEndMenu;
	wxMenu *formatMenu;
	wxMenu *saveasImageMenu;

	wxString title;
	wxString lastFilename, lastDir;

	queryFavouriteFolder *favourites;
	queryMacroList *macros;

	bool aborted;
	bool lastFileFormat;
	bool m_loadingfile;

	// Complements dirty flag, showing last origin of query's modification (see enum ORIGIN_..)
	int origin;

	// A simple mutex-like flag to prevent concurrent script execution.
	// Required because the pgScript parser isn't currently thread-safe :-(
	static bool    ms_pgScriptRunning;

	DECLARE_EVENT_TABLE()
};

// Position of status line fields
enum
{
	STATUSPOS_MSGS = 1,
	STATUSPOS_FORMAT,
	STATUSPOS_POS,
	STATUSPOS_SEL,
	STATUSPOS_ROWS,
	STATUSPOS_SECS
};

enum
{
	CTL_SQLQUERY = 331,
	CTL_SQLRESULT,
	CTL_MSGRESULT,
	CTL_MSGHISTORY,
	CTL_NTBKCENTER,
	CTL_COLSGRID,
	CTL_TIMERSIZES,
	CTL_TIMERFRM,
	CTL_NTBKGQB,
	CTL_SQLQUERYCBOX,
	CTL_DELETECURRENTBTN,
	CTL_DELETEALLBTN,
	CTL_SCRATCHPAD
};

///////////////////////////////////////////////////////

class queryToolBaseFactory : public actionFactory
{
protected:
	queryToolBaseFactory(menuFactoryList *list) : actionFactory(list) {}
	wxWindow *StartDialogSql(frmMain *form, pgObject *obj, const wxString &sql);
public:
	bool CheckEnable(pgObject *obj);
};

class queryToolDataFactory : public queryToolBaseFactory
{
protected:
	queryToolDataFactory(menuFactoryList *list) : queryToolBaseFactory(list) {}
public:
	bool CheckEnable(pgObject *obj);
};

class queryToolFactory : public queryToolBaseFactory
{
public:
	queryToolFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
};

class queryToolSqlFactory : public queryToolBaseFactory
{
public:
	queryToolSqlFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};

class queryToolSelectFactory : public queryToolDataFactory
{
public:
	queryToolSelectFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};

class queryToolExecFactory : public queryToolDataFactory
{
public:
	queryToolExecFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};

class queryToolDeleteFactory : public queryToolDataFactory
{
public:
	queryToolDeleteFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};

class queryToolInsertFactory : public queryToolDataFactory
{
public:
	queryToolInsertFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};

class queryToolUpdateFactory : public queryToolDataFactory
{
public:
	queryToolUpdateFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};

///////////////////////////////////////////////////////
class pgScriptTimer : public wxTimer
{
private:
	frmQuery *m_parent;

public:
	pgScriptTimer(frmQuery *parent);
	void Notify();
};

#endif // __FRM_QUERY_H
