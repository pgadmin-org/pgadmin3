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

pgSet::pgSet(PGresult *newRes, PGconn *newConn)
{
    wxLogInfo(wxT("Creating pgSet object"));
    conn = newConn;
    res = newRes;

    // Make sure we have tuples
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        wxString msg;
        msg.Printf(wxT("%s"), PQerrorMessage(conn));
        wxLogError(msg);
        eof = TRUE;
        bof = TRUE;
        pos = 0;
    } else {
        eof = FALSE;
        bof = FALSE;
        pos = 1;
    }
}

pgSet::~pgSet()
{
    wxLogInfo(wxT("Destroying pgSet object"));
    PQclear(res);
}

long pgSet::NumCols()
{
    return PQnfields(res);
}

long pgSet::NumRows()
{
    return PQntuples(res);
}

void pgSet::MoveNext()
{
    // If pos = 0 then there aren't any tuples
    if (pos) {
        long lRows = PQntuples(res);
        if (pos >= lRows) { // Attempt to move past the last row
            pos = lRows;
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
        pos = PQntuples(res);
        eof = FALSE;
        bof = FALSE;
    }
}


long pgSet::CurrentPos()
{
    return pos;
}

bool pgSet::Eof()
{
    return eof;
}

bool pgSet::Bof()
{
    return bof;
}

wxString pgSet::ColName(int col) const
{
    return wxString(PQfname(res, col + 1));
}

wxString pgSet::ColType(int col) const
{
    wxString szSQL, szResult;
    szSQL.Printf("SELECT typname FROM pg_type WHERE oid = %d", PQftype(res, col -1));
    szResult = ExecuteScalar(szSQL);
    return szResult;
}

int pgSet::ColSize(int col)
{
    return PQfsize(res, col + 1);
}

int pgSet::ColScale(int col)
{
    // TODO
    return 0;
}

wxString pgSet::GetVal(int col) const
{
    return wxString(PQgetvalue(res, pos -1, col));
}

wxString pgSet::GetVal(const wxString& colname) const
{
    int col = PQfnumber(res, colname.c_str());
    return GetVal(col);
}

wxString pgSet::ExecuteScalar(const wxString& sql) const
{
    // Execute the query and get the status.
    PGresult *qryRes;
    wxString msg;
    msg.Printf(wxT("Set sub-query: %s"), sql.c_str());
    wxLogInfo(sql);
    qryRes = PQexec(conn, sql.c_str());
    if (PQresultStatus(qryRes) != PGRES_TUPLES_OK) {
        return wxString("");
    }

    // Retrieve the query result and return it.
    wxString result;
    result.Printf("%s", PQgetvalue(qryRes, 0, 0));
    msg.Printf(wxT("Query result: %s"), result.c_str());
    wxLogInfo(msg);

    // Cleanup & exit
    PQclear(qryRes);
    return result;
}