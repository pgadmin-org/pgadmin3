//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
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
            wxLogError(wxT("%s"), wxString(PQerrorMessage(conn), wxConvUTF8).c_str());

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
    szSQL.Printf(wxT("SELECT typname FROM pg_type WHERE oid = %d"), ColTypeOid(col));
    szResult = ExecuteScalar(szSQL);
    return szResult;
}

int pgSet::ColScale(int col) const
{
    // TODO
    return 0;
}
wxString pgSet::ColName(int col) const
{
    return wxString(PQfname(res, col), wxConvUTF8);
}


int pgSet::ColNumber(const wxString &colname) const
{
    int col = PQfnumber(res, colname.ToAscii());
    if (col < 0)
        wxLogError(__("Column not found in pgSet: ") + colname);
    return col;
}


wxString pgSet::GetVal(int col) const
{
    return wxString(PQgetvalue(res, pos -1, col), wxConvUTF8);
}


wxString pgSet::GetVal(const wxString& colname) const
{
    return GetVal(ColNumber(colname));
}


long pgSet::GetLong(int col) const
{
    char *c=PQgetvalue(res, pos-1, col);
    if (c)
        return atol(c);
    else
        return 0;
}


long pgSet::GetLong(const wxString &col)
{
    return GetLong(ColNumber(col));
}


bool pgSet::GetBool(int col) const
{
    char *c=PQgetvalue(res, pos-1, col);
    if (c)
    {
        if (*c == 't' || *c == '1')
            return true;
    }
    return false;
}


bool pgSet::GetBool(const wxString &col) const
{
    return GetBool(ColNumber(col));
}


double pgSet::GetDouble(int col) const
{
    char *c=PQgetvalue(res, pos-1, col);
    if (c)
        return strtod(c, 0);
    else
        return 0;
}


double pgSet::GetDouble(const wxString &col) const
{
    return GetDouble(ColNumber(col));
}


wxULongLong pgSet::GetLongLong(int col) const
{
    char *c=PQgetvalue(res, pos-1, col);
    if (c)
        return atolonglong(c);
    else
        return 0;
}

wxULongLong pgSet::GetLongLong(const wxString &col) const
{
    return GetLongLong(ColNumber(col));
}


OID pgSet::GetOid(int col) const
{
    return (OID)GetLong(col);
}


OID pgSet::GetOid(const wxString &col) const
{
    return GetOid(ColNumber(col));
}


wxString pgSet::ExecuteScalar(const wxString& sql) const
{
    // Execute the query and get the status.
    PGresult *qryRes;

    wxLogSql(wxT("Set sub-query: %s"), sql.c_str());

    qryRes = PQexec(conn, sql.mb_str(wxConvUTF8));
    if (PQresultStatus(qryRes) != PGRES_TUPLES_OK) {
        return wxEmptyString;
    }

    // Retrieve the query result and return it.
    wxString result=wxString(PQgetvalue(qryRes, 0, 0), wxConvUTF8);
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

    wxLogSql(wxT("Thread Query %s"), query.c_str());

    if (!PQsendQuery(conn, query.mb_str(wxConvUTF8)))
        return(0);

    while (true)
    {
        if (TestDestroy())
        {
            if (rc != -3)
            {
                if (!PQrequestCancel(conn)) // could not abort; abort failed.
                    return(-1);

                rc = -3;
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

        startTime = wxGetLocalTimeMillis();
        if (!res)
            break;
        if (result)
        {
            messages.Printf(_("Query result with %d rows discarded.\n"), PQntuples(result));
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


bool pgQueryThread::IsRunning() const
{
    if (rc <  0)
        return wxThread::IsRunning();
    return false;
}


void *pgQueryThread::Entry()
{
    rc=-2;
    wxLogInfo(wxT("Running query %s"), query.c_str());

    execute();

    return(NULL);
}
