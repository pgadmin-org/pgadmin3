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
#ifdef __WXMSW__
    #include <winsock.h>
#else
    #include <arpa/inet.h>
    #include <netdb.h>
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
        wxLogError(wxT("%s"), PQerrorMessage(conn));

        eof = TRUE;
        bof = TRUE;
        pos = 0;
    }
    else
    {
        nRows = PQntuples(res);
        eof = (nRows <= 0);
        bof = true;
        pos = 1;
    }
}

pgSet::~pgSet()
{
    wxLogInfo(wxT("Destroying pgSet object"));
    PQclear(res);
}

void pgSet::MoveNext()
{
    // If pos = 0 then there aren't any tuples
    if (pos) {
        if (pos >= nRows) { // Attempt to move past the last row
            pos = nRows;
            eof = TRUE;
            return;
        } else {
            ++pos;
            eof = FALSE;
            return;
        }
    }
}

void pgSet::MovePrevious()
{
    // If pos = 0 then there aren't any tuples
    if (pos) {
        if (pos <= 1) { // Attempt to move past the first row
            pos = 1;
            bof = TRUE;
            return;
        } else {
            --pos;
            eof = FALSE;
            return;
        }
    }
}

void pgSet::MoveFirst()
{
    if(pos) {
        pos = 1;
        eof = FALSE;
        bof = FALSE;
    }
}

void pgSet::MoveLast()
{
    if(pos) {
        pos = nRows;
        eof = FALSE;
        bof = FALSE;
    }
}

wxString pgSet::ColType(int col) const
{
    wxString szSQL, szResult;
    szSQL.Printf("SELECT typname FROM pg_type WHERE oid = %d", PQftype(res, col));
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
