//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// ctlSQLResult.h - SQL Query result window
//
//////////////////////////////////////////////////////////////////////////

#ifndef CTLSQLRESULT_H
#define CTLSQLRESULT_H

// wxWindows headers
#include <wx/listctrl.h>
#include <wx/thread.h>

#include <libpq-fe.h>
#include "pgConn.h"
#include "pgSet.h"



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
    int rc;
    PGresult *result;
    wxString messages;
    pgSet *dataSet;

private:
    int execute();
};


#define CTLSQL_RUNNING 100  // must be greater than ExecStatusType PGRES_xxx values

class ctlSQLResult : public wxListView 
{
public:
    ctlSQLResult(wxWindow *parent, pgConn *conn, wxWindowID id, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize);
    ~ctlSQLResult();


    int Execute(const wxString &query);
    long NumRows();

    int Retrieve(long chunk=-1);
    int RetrieveOne();
    int Abort();

    int RunStatus();
    wxString GetMessages();
    wxString GetErrorMessage();

private:
    queryThread *thread;
    pgConn *conn;
    long rowsRetrieved;
};





#endif