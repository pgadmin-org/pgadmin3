//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// ctlSQLResult.cpp - SQL Query result window
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>


#include "ctlSQLResult.h"


ctlSQLResult::ctlSQLResult(wxWindow *parent, pgConn *_conn, wxWindowID id, const wxPoint& pos, const wxSize& size)
: wxListView(parent, id, pos, size, wxLC_REPORT | wxSIMPLE_BORDER)
{
    conn=_conn;
    thread=0;
}



ctlSQLResult::~ctlSQLResult()
{
    Abort();
}



int ctlSQLResult::Execute(const wxString &query)
{
    Abort();
    ClearAll();
    rowsRetrieved=0;

    thread = new queryThread(conn->connection(), query);

    if (thread->Create() != wxTHREAD_NO_ERROR)
    {
        Abort();
        return -1;
    }

    thread->Run();
    return RunStatus();
}


int ctlSQLResult::Abort()
{
    if (thread)
    {
        thread->Delete();
        delete thread;
    }
    thread=0;
    return 0;
}


int ctlSQLResult::RetrieveOne()
{
    if (!thread || !thread->dataSet)
        return -1;

    if (thread->rc != PGRES_TUPLES_OK)
        return -1;

    if (!rowsRetrieved)
    {
        int w, h;
        GetSize(&w, &h);
        InsertColumn(0, thread->dataSet->ColName(0), wxLIST_FORMAT_LEFT, w);

        while (!thread->dataSet->Eof())
        {
            InsertItem(rowsRetrieved, thread->dataSet->GetVal(0));
            rowsRetrieved++;
            thread->dataSet->MoveNext();
        }
        return rowsRetrieved;
    }
    return 0;
}


int ctlSQLResult::Retrieve(long chunk)
{
    if (!thread || !thread->dataSet)
        return 0;

    if (chunk<0)
        chunk=thread->dataSet->NumRows();

    long col, nCols=thread->dataSet->NumCols();
    if (!rowsRetrieved)
    {
        wxString colName, colType;

        InsertColumn(0, wxT("Row"), wxLIST_FORMAT_RIGHT, 30);

        for (col=0 ; col < nCols ; col++)
        {
            colName = thread->dataSet->ColName(col);
            colType = thread->dataSet->ColType(col);
            InsertColumn(col+1, colName +wxT(" (")+ colType +wxT(")"), wxLIST_FORMAT_LEFT, -1);
        }
    }

    long count=0;
    while (chunk-- && !thread->dataSet->Eof())
    {
        InsertItem(rowsRetrieved, NumToStr(rowsRetrieved+1L));

        for (col=0 ; col < nCols ; col++)
        {
            wxString value = thread->dataSet->GetVal(col);
            SetItem(rowsRetrieved, col+1, value);
        }
        
        thread->dataSet->MoveNext();
        rowsRetrieved++;
        count++;
    }
    return (count);
}


wxString ctlSQLResult::GetMessages()
{
    if (thread)
        return thread->messages;
    return wxString();
}


wxString ctlSQLResult::GetErrorMessage()
{
    wxString errmsg=PQerrorMessage(conn->connection());
    return errmsg;
}



long ctlSQLResult::NumRows()
{
    if (thread && thread->dataSet)
        return thread->dataSet->NumRows();
    return 0;
}


int ctlSQLResult::RunStatus()
{
    if (!thread)
        return -1;
    
    if (thread->IsRunning())
        return CTLSQL_RUNNING;

    return thread->rc;
}



queryThread::queryThread(PGconn *_conn, const wxString &qry) : wxThread(wxTHREAD_JOINABLE)
{
    running = 1;

    query = qry;
    conn=_conn;
    dataSet=0;
    result=0;
    PQsetnonblocking(conn, 1);
}


queryThread::~queryThread()
{
    if (dataSet)
        delete dataSet;
}


int queryThread::execute()
{
    wxLongLong startTime=wxGetLocalTimeMillis();

    if (!PQsendQuery(conn, query.c_str()))
        return(0);

    while (running > 0)
    {
        if (TestDestroy())
        {
            if (!PQrequestCancel(conn)) // could not abort; abort failed.
            {
                running=-1;
                return(-1);
            }
        }
        if (!PQconsumeInput(conn))
            return(0);
        if (PQisBusy(conn))
        {
            Yield();
            wxUsleep(10);
            continue;
        }

        // only the last result set will be returned
        // all others are discarded
        PGresult *res=PQgetResult(conn);
        wxLongLong elapsed=wxGetLocalTimeMillis() - startTime;
        startTime = wxGetLocalTimeMillis();
        if (!res)
            break;
        if (result)
        {
            messages += wxT(
                "Query result with ") + NumToStr((long)PQntuples(result)) + wxT(" rows discarded.\n");
            PQclear(result);
        }
        result=res;
    }
    messages += wxT("\n");
    running=0;
    rc=PQresultStatus(result);

    if (rc == PGRES_TUPLES_OK)
    {
        dataSet = new pgSet(result, conn);
        dataSet->MoveFirst();
        dataSet->GetVal(0);
    }
    return(1);
}


void *queryThread::Entry()
{
    wxLogInfo(wxT("Running query %s"), query.c_str());

    execute();

    return(NULL);
}
