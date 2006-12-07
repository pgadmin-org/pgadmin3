//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: pgSetBase.cpp 5510 2006-10-21 23:08:05Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgQueryThread.cpp - PostgreSQL threaded query class
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// PostgreSQL headers
#include <libpq-fe.h>

// App headers
#include "schema/pgSet.h"
#include "schema/pgConn.h"
#include "pgQueryThread.h"
#include "utils/sysLogger.h"

static void pgNoticeProcessor(void *arg, const char *message)
{
    wxString str(message, wxConvUTF8);
    
    wxLogNotice(wxT("%s"), str.c_str());
    ((pgQueryThread*)arg)->appendMessage(str);
}

pgQueryThread::pgQueryThread(pgConn *_conn, const wxString &qry, int _resultToRetrieve) 
: wxThread(wxTHREAD_JOINABLE)
{
    query = qry;
    conn=_conn;
    dataSet=0;
    result=0;
    resultToRetrieve=_resultToRetrieve;
    rc=-1;
    insertedOid = (OID)-1;

    wxLogSql(wxT("Thread Query %s"), qry.c_str());

    conn->RegisterNoticeProcessor(pgNoticeProcessor, this);
    if (conn->conn)
        PQsetnonblocking(conn->conn, 1);
}

pgQueryThread::~pgQueryThread()
{
    conn->RegisterNoticeProcessor(0, 0);
    if (dataSet)
        delete dataSet;
}


wxString pgQueryThread::GetMessagesAndClear()
{
    wxString msg;

    {
        wxCriticalSectionLocker cs(criticalSection);
        msg=messages;
        messages.Empty();
    }

    return msg;
}


void pgQueryThread::appendMessage(const wxString &str)
{
    wxCriticalSectionLocker cs(criticalSection);
    messages.Append(str);
}


int pgQueryThread::execute()
{
    rowsInserted = -1L;

    if (!conn->conn)
        return(0);

    if (!PQsendQuery(conn->conn, query.mb_str(*conn->conv)))
    {
        conn->IsAlive();
        return(0);
    }
    int resultsRetrieved=0;
    PGresult *lastResult=0;
    while (true)
    {
        if (TestDestroy())
        {
            if (rc != -3)
            {
                if (!PQrequestCancel(conn->conn)) // could not abort; abort failed.
                    return(-1);

                rc = -3;
            }
        }
        if (!PQconsumeInput(conn->conn))
            return(0);
        if (PQisBusy(conn->conn))
        {
            Yield();
            wxMilliSleep(10);
            continue;
        }

        // If resultToRetrieve is given, the nth result will be returned, 
        // otherwise the last result set will be returned.
        // all others are discarded
        PGresult *res=PQgetResult(conn->conn);

        if (!res)
            break;

        resultsRetrieved++;
        if (resultsRetrieved == resultToRetrieve)
        {
            result=res;
            insertedOid=PQoidValue(res);
            if (insertedOid && insertedOid != (OID)-1)
                appendMessage(wxString::Format(_("Query inserted one row with OID %d.\n"), insertedOid));
			else
                appendMessage(wxString::Format(_("Query result with %d rows will be returned.\n"), PQntuples(result)));
            continue;
        }
        if (lastResult)
        {
            if (PQntuples(lastResult))
                appendMessage(wxString::Format(_("Query result with %d rows discarded.\n"), PQntuples(lastResult)));
            PQclear(lastResult);
        }
        lastResult=res;
    }

    if (!result)
        result = lastResult;

    conn->SetLastResultError(result);

    appendMessage(wxT("\n"));
    rc=PQresultStatus(result);
    insertedOid=PQoidValue(result);
    if (insertedOid == (OID)-1)
        insertedOid=0;

    if (rc == PGRES_TUPLES_OK)
    {
        dataSet = new pgSet(result, conn, *conn->conv, conn->needColQuoting);
        dataSet->MoveFirst();
        dataSet->GetVal(0);
    }
    else if (rc == PGRES_COMMAND_OK)
    {
        char *s=PQcmdTuples(result);
        if (*s)
            rowsInserted = atol(s);
    }

    return(1);
}

void *pgQueryThread::Entry()
{
    rc=-2;
    execute();

    return(NULL);
}
