//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmQuery.h - The SQL Query form
//
//////////////////////////////////////////////////////////////////////////

#ifndef __FRM_QUERY_H
#define __FRM_QUERY_H

#include "dlg/dlgClasses.h"
#include "utils/factory.h"
#include "utils/favourites.h"
#include "utils/macros.h"

// wxAUI
#include <wx/aui/aui.h>
#include <wx/timer.h>

#define FRMQUERY_PERPSECTIVE_VER wxT("$Rev$")

#ifdef __WXMAC__
#define FRMQUERY_DEFAULT_PERSPECTIVE wxT("layout2|name=toolBar;caption=Tool bar;state=16788208;dir=1;layer=10;row=0;pos=0;prop=100000;bestw=385;besth=23;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=databaseBar;caption=Database bar;state=16788208;dir=1;layer=10;row=0;pos=396;prop=100000;bestw=250;besth=21;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=sqlQuery;caption=SQL query;state=17404;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=350;besth=200;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=outputPane;caption=Output pane;state=16779260;dir=3;layer=0;row=0;pos=0;prop=100000;bestw=550;besth=300;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=scratchPad;caption=Scratch pad;state=16779260;dir=2;layer=0;row=0;pos=0;prop=100000;bestw=250;besth=200;minw=100;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|dock_size(1,10,0)=25|dock_size(5,0,0)=200|dock_size(3,0,0)=290|dock_size(2,0,0)=255|")
#else
#ifdef __WXGTK__
#define FRMQUERY_DEFAULT_PERSPECTIVE wxT("layout2|name=toolBar;caption=Tool bar;state=16788208;dir=1;layer=10;row=0;pos=0;prop=100000;bestw=485;besth=30;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=databaseBar;caption=Database bar;state=16788208;dir=1;layer=10;row=0;pos=396;prop=100000;bestw=250;besth=30;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=sqlQuery;caption=SQL query;state=17404;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=350;besth=200;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=outputPane;caption=Output pane;state=16779260;dir=3;layer=0;row=0;pos=0;prop=100000;bestw=550;besth=300;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=scratchPad;caption=Scratch pad;state=16779260;dir=2;layer=0;row=0;pos=0;prop=100000;bestw=250;besth=200;minw=100;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|dock_size(1,10,0)=25|dock_size(5,0,0)=200|dock_size(3,0,0)=290|dock_size(2,0,0)=255|")
#else
#define FRMQUERY_DEFAULT_PERSPECTIVE wxT("layout2|name=toolBar;caption=Tool bar;state=16788208;dir=1;layer=10;row=0;pos=0;prop=100000;bestw=385;besth=23;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=databaseBar;caption=Database bar;state=16788208;dir=1;layer=10;row=0;pos=396;prop=100000;bestw=250;besth=21;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=sqlQuery;caption=SQL query;state=17404;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=350;besth=200;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=outputPane;caption=Output pane;state=16779260;dir=3;layer=0;row=0;pos=0;prop=100000;bestw=550;besth=300;minw=200;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=scratchPad;caption=Scratch pad;state=16779260;dir=2;layer=0;row=0;pos=0;prop=100000;bestw=250;besth=200;minw=100;minh=100;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|dock_size(1,10,0)=25|dock_size(5,0,0)=200|dock_size(3,0,0)=290|dock_size(2,0,0)=255|")
#endif
#endif

class ExplainCanvas;
class ctlSQLResult;

class QueryExecInfo
{
public:
    int queryOffset;
    bool toFile;
    bool singleResult;
    bool explain;
    bool verbose;
};

class frmQuery : public pgFrame
{
public:
    frmQuery(frmMain *form, const wxString& _title, pgConn *conn, const wxString& qry, const wxString& file = wxEmptyString);
    ~frmQuery();
    void Go();

private:
    frmMain *mainForm;
    wxAuiManager manager;
    ctlSQLBox *sqlQuery;
    wxNotebook *outputPane;
    ctlSQLResult *sqlResult;
    ExplainCanvas *explainCanvas;
    wxTextCtrl *msgResult, *msgHistory;
    ctlComboBoxFix *cbConnection;
    wxTextCtrl *scratchPad;

    // Query timing/status update
    wxTimer timer;
    wxLongLong elapsedQuery, startTimeQuery;

    // Our connection
    pgConn *conn;

	// These status flags are required to work round some wierdness on wxGTK,
	// particularly on Solaris.
    bool closing, loading;
    
    void OnEraseBackground(wxEraseEvent& event);
    void OnSize(wxSizeEvent& event);

    void OnChangeStc(wxStyledTextEvent& event);
    void OnPositionStc(wxStyledTextEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnSetFocus(wxFocusEvent& event);
    void OnContents(wxCommandEvent& event);
    void OnHelp(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnExecute(wxCommandEvent& event);
    void OnExecFile(wxCommandEvent& event);
    void OnExplain(wxCommandEvent& event);
    void OnNew(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnSaveAs(wxCommandEvent& event);
    void OnExport(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnCut(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);
    void OnClear(wxCommandEvent& event);
    void OnSearchReplace(wxCommandEvent& event);
    void OnUndo(wxCommandEvent& event);
    void OnRedo(wxCommandEvent& event);
    void OnSaveHistory(wxCommandEvent& event);
    void OnChangeConnection(wxCommandEvent &ev);
    void OnClearHistory(wxCommandEvent& event);
    void OnActivate(wxActivateEvent& event);
    void OnFocus(wxFocusEvent& event);
    void OnSelectAll(wxCommandEvent& event);
    void OnAddFavourite(wxCommandEvent& event);
    void OnManageFavourites(wxCommandEvent& event);
    void OnSelectFavourite(wxCommandEvent& event);
    void OnQuickReport(wxCommandEvent& event);
    void OnAutoIndent(wxCommandEvent& event);
    void OnWordWrap(wxCommandEvent& event);
    void OnShowIndentGuides(wxCommandEvent& event);
    void OnShowWhitespace(wxCommandEvent& event);
    void OnShowLineEnds(wxCommandEvent& event);

    void OnToggleScratchPad(wxCommandEvent& event);
    void OnToggleDatabaseBar(wxCommandEvent& event);
    void OnToggleToolBar(wxCommandEvent& event);
    void OnToggleOutputPane(wxCommandEvent& event);
    void OnAuiUpdate(wxAuiManagerEvent& event);
    void OnDefaultView(wxCommandEvent& event);

	void OnTimer(wxTimerEvent & event);

    bool CheckChanged(bool canVeto);
    void OpenLastFile();
    void updateMenu(wxObject *obj=0);
    void execQuery(const wxString &query, int resultToRetrieve=0, bool singleResult=false, const int queryOffset=0, bool toFile=false, bool explain=false, bool verbose=false);
    void OnQueryComplete(wxCommandEvent &ev);
    void completeQuery(bool done, bool explain, bool verbose);
    void setTools(const bool running);
    void showMessage(const wxString& msg, const wxString &msgShort=wxT(""));
    void setExtendedTitle();
    void UpdateFavouritesList();
    void SetLineEndingStyle();
    int GetLineEndingStyle();
    void OnSetEOLMode(wxCommandEvent& event);
	void OnMacroInvoke(wxCommandEvent& event);
	void OnMacroManage(wxCommandEvent& event);
	void UpdateMacrosList();
    wxWindow *currentControl();
    wxMenu *queryMenu;
    wxMenu *favouritesMenu;
	wxMenu *macrosMenu;
    wxMenu *lineEndMenu;
    wxString title;
    wxString lastFilename, lastDir;

    queryFavouriteFolder *favourites;
	queryMacroList *macros;

    bool aborted;
    bool lastFileFormat;

    DECLARE_EVENT_TABLE()
};



// Position of status line fields
enum
{
    STATUSPOS_MSGS = 1,
    STATUSPOS_FORMAT,
    STATUSPOS_POS,
    STATUSPOS_ROWS,
    STATUSPOS_SECS
};


enum
{
    CTL_SQLQUERY=331,
    CTL_SQLRESULT,
    CTL_MSGRESULT,
    CTL_MSGHISTORY
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
    queryToolFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
};

    
class queryToolSqlFactory : public queryToolBaseFactory
{
public:
    queryToolSqlFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};


class queryToolSelectFactory : public queryToolDataFactory

{
public:
    queryToolSelectFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
};

class queryToolInsertFactory : public queryToolDataFactory
{
public:
    queryToolInsertFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};

class queryToolUpdateFactory : public queryToolDataFactory
{
public:
    queryToolUpdateFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};

#endif // __FRM_QUERY_H
