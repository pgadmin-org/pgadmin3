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
#include "../../pgAdmin3.h"

pgSet::pgSet(PGresult *objNewRes, PGconn *objNewConn)
{
    wxLogInfo(wxT("Creating pgSet object"));
    objConn = objNewConn;
    objRes = objNewRes;

    // Make sure we have tuples
    if (PQresultStatus(objRes) != PGCONN_TUPLES_OK) {
        bEof = TRUE;
        bBof = TRUE;
        lPos = 0;
    } else {
        bEof = FALSE;
        bBof = FALSE;
        lPos = 1;
    }
}

pgSet::~pgSet()
{
    wxLogInfo(wxT("Destroying pgSet object"));
    PQclear(objRes);
}

long pgSet::NumCols()
{
    return PQnfields(objRes);
}

long pgSet::NumRows()
{
    return PQntuples(objRes);
}

void pgSet::MoveNext()
{
    // If lPos = 0 then there aren't any tuples
    if (lPos) {
        long lRows = PQntuples(objRes);
        if (lPos >= lRows) { // Attempt to move past the last row
            lPos = lRows;
            bEof = TRUE;
            return;
        } else {
            ++lPos;
            bEof = FALSE;
            return;
        }
    }
}

void pgSet::MovePrevious()
{
    // If lPos = 0 then there aren't any tuples
    if (lPos) {
        if (lPos <= 1) { // Attempt to move past the first row
            lPos = 1;
            bBof = TRUE;
            return;
        } else {
            --lPos;
            bEof = FALSE;
            return;
        }
    }
}

void pgSet::MoveFirst()
{
    if(lPos) {
        lPos = 1;
        bEof = FALSE;
        bBof = FALSE;
    }
}

void pgSet::MoveLast()
{
    if(lPos) {
        lPos = PQntuples(objRes);
        bEof = FALSE;
        bBof = FALSE;
    }
}


long pgSet::CurrentPos()
{
    return lPos;
}

bool pgSet::Eof()
{
    return bEof;
}

bool pgSet::Bof()
{
    return bBof;
}

wxString pgSet::ColName(int iCol) const
{
    return wxString(PQfname(objRes, iCol + 1));
}

wxString pgSet::ColType(int iCol) const
{
    wxString szSQL, szResult;
    szSQL.Printf("SELECT typname FROM pg_type WHERE oid = %d", PQftype(objRes, iCol -1));
    szResult = ExecuteScalar(szSQL);
    return szResult;
}

int pgSet::ColSize(int iCol)
{
    return PQfsize(objRes, iCol + 1);
}

int pgSet::ColScale(int iCol)
{
    // TODO
    return 0;
}

wxString pgSet::GetVal(int iCol) const
{
    return wxString(PQgetvalue(objRes, lPos -1, iCol));
}

wxString pgSet::GetVal(const wxString& szCol) const
{
    int iCol = PQfnumber(objRes, szCol.c_str());
    return GetVal(iCol);
}

wxString pgSet::ExecuteScalar(const wxString& szSQL) const
{
    // Execute the query and get the status.
    PGresult *qryRes;
    wxString szMsg;
    szMsg.Printf(wxT("Set sub-query: %s"), szSQL.c_str());
    wxLogInfo(szMsg);
    qryRes = PQexec(objConn, szSQL.c_str());
    if (PQresultStatus(qryRes) != PGRES_TUPLES_OK) {
        return wxString("");
    }

    // Retrieve the query result and return it.
    wxString szResult;
    szResult.Printf("%s", PQgetvalue(qryRes, 0, 0));
    szMsg.Printf(wxT("Query result: %s"), szResult.c_str());
    wxLogInfo(szMsg);

    // Cleanup & exit
    PQclear(qryRes);
    return szResult;
}