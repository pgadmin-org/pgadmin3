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
    wxLogDebug(wxT("Creating pgSet object"));
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
    wxLogDebug(wxT("Destroying pgSet object"));
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
    // TODO
    return wxString("");
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
    int iCol = PQfnumber(objRes, szCol.c_str()) + 1;
    return GetVal(iCol);
}
