//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmQuery.h - The SQL Query form
//
//////////////////////////////////////////////////////////////////////////

#ifndef __FRM_QUERY_H
#define __FRM_QUERY_H


#include <libpq-fe.h>
#include "pgSet.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/thread.h>


class frmQuery;

class queryThread : public wxThread
{
private:
    wxString query;
    PGconn *conn;

public:

    queryThread(PGconn *_conn, const wxString &qry);
    ~queryThread();
    virtual void *Entry();

    int running;
    PGresult *result;
    wxString messages;

private:
    int execute();
};

class frmQuery : public wxFrame
{
public:
    frmQuery(frmMain *form, const wxString& _title, pgConn *conn, const wxPoint& pos, const wxSize& size, const wxString& qry);
    ~frmQuery();
    void Go();

private:
    frmMain *mainForm;
    wxSplitterWindow* horizontal;
    ctlSQLBox *sqlQuery;
    wxListView *sqlResult;
    wxTextCtrl *msgResult;
    wxStatusBar *statusBar;
    wxToolBar *toolBar;
    pgConn *conn;
    queryThread *thread;
    long nRows, nCols;
    wxLongLong elapsedQuery, elapsedRetrieve;

    void OnClose(wxCloseEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnExecute(wxCommandEvent& event);
    void OnExplain(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnSaveAs(wxCommandEvent& event);
    void OnChange(wxNotifyEvent& event);

    void execQuery(const wxString &query, const bool singleResult=false, const int queryOffset=0);
    void setTools(const bool running);
    void showMessage(const wxString& msg, const wxString &msgShort=wxT(""));
    void setExtendedTitle();
    wxMenuBar *menuBar;
    wxMenu *fileMenu, *queryMenu;
    wxString title;
    wxString lastFilename, lastDir, lastPath;

    friend class queryThread;
    bool changed;

    DECLARE_EVENT_TABLE()
};


enum
{
   BTN_OPEN = 231,
   BTN_SAVE,
   BTN_EXECUTE,
   BTN_EXPLAIN,
   BTN_CANCEL
};

enum
{
   MNU_OPEN = 131,
   MNU_SAVE,
   MNU_SAVEAS,
   MNU_CANCEL,
   MNU_EXECUTE,
   MNU_EXPLAIN
};


// Position of status line fields
enum
{
   STATUSPOS_MSGS = 1,
   STATUSPOS_ROWS,
   STATUSPOS_SECS
};


enum
{
   CTL_SQLQUERY=331,
   CTL_SQLRESULT,
   CTL_MSGRESULT
};


#endif // __FRM_QUERY_H