//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: frmQuery.h 4991 2006-02-08 15:32:21Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmQuery.h - The SQL Query form
//
//////////////////////////////////////////////////////////////////////////

#ifndef __FRM_QUERY_H
#define __FRM_QUERY_H

#include "dlgClasses.h"
#include "base/factory.h"
#include "favourites.h"

// wxAUI
#include "manager.h"

#define FRMQUERY_DEFAULT_PERSPECTIVE wxT("layout1|name=toolBar;caption=Tool bar;state=16788208;dir=1;layer=10;row=0;pos=0;prop=100000;bestw=362;besth=23;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=databaseBar;caption=Database bar;state=16788208;dir=1;layer=10;row=0;pos=373;prop=100000;bestw=241;besth=23;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=246;floaty=191;floatw=256;floath=47|name=sqlQuery;caption=SQL query;state=18428;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=198;besth=81;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=resultsPane;caption=Results pane;state=16779260;dir=3;layer=0;row=0;pos=0;prop=138218;bestw=400;besth=200;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=75;floaty=372;floatw=615;floath=298|name=messagePane;caption=Message pane;state=16779260;dir=3;layer=0;row=0;pos=1;prop=61782;bestw=400;besth=100;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=559;floaty=375;floatw=404;floath=230|name=scratchPad;caption=Scratch pad;state=16779263;dir=3;layer=0;row=0;pos=2;prop=100000;bestw=279;besth=179;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=779;floaty=525;floatw=300;floath=205|dock_size(5,0,0)=200|dock_size(3,0,0)=199|dock_size(1,10,0)=25|")

class ExplainCanvas;
class ctlSQLResult;

class frmQuery : public pgFrame
{
public:
    frmQuery(frmMain *form, const wxString& _title, pgConn *conn, const wxString& qry);
    ~frmQuery();
    void Go();

private:
    frmMain *mainForm;
    wxFrameManager manager;
    ctlSQLBox *sqlQuery;
    wxNotebook *resultsPane, *messagePane;
    ctlSQLResult *sqlResult;
    ExplainCanvas *explainCanvas;
    wxTextCtrl *msgResult, *msgHistory;
    ctlComboBoxFix *cbConnection;
    wxTextCtrl *scratchPad;

    pgConn *conn;
    wxLongLong elapsedQuery, elapsedRetrieve;
    
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
	void OnExplainText(wxCommandEvent& event);
	void OnNew(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnSaveAs(wxCommandEvent& event);
    void OnExport(wxCommandEvent& event);
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
	void OnWordWrap(wxCommandEvent& event);
	void OnShowWhitespace(wxCommandEvent& event);
	void OnShowLineEnds(wxCommandEvent& event);

    void OnToggleScratchPad(wxCommandEvent& event);
    void OnToggleDatabaseBar(wxCommandEvent& event);
    void OnToggleToolBar(wxCommandEvent& event);
    void OnToggleResultsPane(wxCommandEvent& event);
    void OnToggleMessagePane(wxCommandEvent& event);
    void OnAuiUpdate(wxFrameManagerEvent& event);
    void OnDefaultView(wxCommandEvent& event);

    bool CheckChanged(bool canVeto);
    void OpenLastFile();
    void updateMenu(wxObject *obj=0);
    bool execQuery(const wxString &query, int resultToRetrieve=0, bool singleResult=false, const int queryOffset=0, bool toFile=false);
    void setTools(const bool running);
    void showMessage(const wxString& msg, const wxString &msgShort=wxT(""));
    void setExtendedTitle();
	void UpdateFavouritesList();
    wxWindow *currentControl();
    wxMenu *queryMenu;
	wxMenu *favouritesMenu;
    wxString title;
    wxString lastFilename, lastDir;

	queryFavouriteFolder *favourites;

    bool aborted;
    bool lastFileFormat;

    DECLARE_EVENT_TABLE()
};



// Position of status line fields
enum
{
    STATUSPOS_MSGS = 1,
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
