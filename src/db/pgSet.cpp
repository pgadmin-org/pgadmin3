//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgSet.cpp - PostgreSQL ResultSet class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// PostgreSQL headers
#include <libpq-fe.h>

// Network headers

#if 0
#ifdef __WXMSW__
    #include <winsock.h>
#else
    #include <arpa/inet.h>
    #include <netdb.h>
#endif
#endif

// App headers
#include "pgSet.h"
#include "pgConn.h"
#include "pgAdmin3.h"
#include "sysLogger.h"

pgSet::pgSet(PGresult *newRes, PGconn *newConn)
{
    wxLogInfo(wxT("Creating pgSet object"));
    conn = newConn;
    res = newRes;

    // Make sure we have tuples
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        if (PQresultStatus(res) != PGRES_COMMAND_OK)
            wxLogError(wxT("%s"), PQerrorMessage(conn));

        nRows = 0;
        pos = 0;
    }
    else
    {
        nRows = PQntuples(res);
        MoveFirst();
    }
}


pgSet::~pgSet()
{
    wxLogInfo(wxT("Destroying pgSet object"));
    PQclear(res);
}



Oid pgSet::ColTypeOid(int col) const
{
    return PQftype(res, col);
}


wxString pgSet::ColType(int col) const
{
    wxString szSQL, szResult;
    szSQL.Printf("SELECT typname FROM pg_type WHERE oid = %d", ColTypeOid(col));
    szResult = ExecuteScalar(szSQL);
    return szResult;
}

int pgSet::ColScale(int col) const
{
    // TODO
    return 0;
}

wxString pgSet::GetVal(const wxString& colname) const
{
    int col = PQfnumber(res, colname.c_str());
    if (col < 0)
        wxLogError(wxT("Column not found in pgSet: ") + colname);
    return GetVal(col);
}

wxString pgSet::ExecuteScalar(const wxString& sql) const
{
    // Execute the query and get the status.
    PGresult *qryRes;

    wxLogSql(wxT("Set sub-query: %s"), sql.c_str());

    qryRes = PQexec(conn, sql.c_str());
    if (PQresultStatus(qryRes) != PGRES_TUPLES_OK) {
        return wxString("");
    }

    // Retrieve the query result and return it.
    wxString result=PQgetvalue(qryRes, 0, 0);
    
    wxLogInfo(wxT("Query result: %s"), result.c_str());

    // Cleanup & exit
    PQclear(qryRes);
    return result;
}




pgQueryThread::pgQueryThread(PGconn *_conn, const wxString &qry) : wxThread(wxTHREAD_JOINABLE)
{
    query = qry;
    conn=_conn;
    dataSet=0;
    result=0;
    rc=-1;
    PQsetnonblocking(conn, 1);
}


pgQueryThread::~pgQueryThread()
{
    if (dataSet)
        delete dataSet;
}


int pgQueryThread::execute()
{
    wxLongLong startTime=wxGetLocalTimeMillis();

    if (!PQsendQuery(conn, query.c_str()))
        return(0);

    while (true)
    {
        if (TestDestroy())
        {
            if (!PQrequestCancel(conn)) // could not abort; abort failed.
            {
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
    rc=PQresultStatus(result);

    if (rc == PGRES_TUPLES_OK)
    {
        dataSet = new pgSet(result, conn);
        dataSet->MoveFirst();
        dataSet->GetVal(0);
    }
    return(1);
}


void *pgQueryThread::Entry()
{
    wxLogInfo(wxT("Running query %s"), query.c_str());

    execute();

    return(NULL);
}
