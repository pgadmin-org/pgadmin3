//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
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
    wxSplitterWindow* horizontal;
    ctlSQLBox *sqlQuery;
    wxNotebook *output;
    ctlSQLResult *sqlResult;
    ExplainCanvas *explainCanvas;
    wxTextCtrl *msgResult, *msgHistory;
    ctlComboBoxFix *cbConnection;

    pgConn *conn;
    wxLongLong elapsedQuery, elapsedRetrieve;

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
    void OnOpen(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnSaveAs(wxCommandEvent& event);
    void OnExport(wxCommandEvent& event);
    void OnCut(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);
    void OnClear(wxCommandEvent& event);
    void OnFind(wxCommandEvent& event);
    void OnReplace(wxCommandEvent& event);
    void OnUndo(wxCommandEvent& event);
    void OnRedo(wxCommandEvent& event);
    void OnSaveHistory(wxCommandEvent& event);
    void OnChangeConnection(wxCommandEvent &ev);
    void OnClearHistory(wxCommandEvent& event);
    void OnActivate(wxActivateEvent& event);
    void OnFocus(wxFocusEvent& event);

    bool CheckChanged(bool canVeto);
    void OpenLastFile();
    void updateMenu(wxObject *obj=0);
    bool execQuery(const wxString &query, int resultToRetrieve=0, bool singleResult=false, const int queryOffset=0, bool toFile=false);
    void setTools(const bool running);
    void showMessage(const wxString& msg, const wxString &msgShort=wxT(""));
    void setExtendedTitle();
    wxWindow *currentControl();
    wxMenu *queryMenu;
    wxString title;
    wxString lastFilename, lastDir;

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


class queryToolFactory : public actionFactory
{
public:
    queryToolFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};

#endif // __FRM_QUERY_H
