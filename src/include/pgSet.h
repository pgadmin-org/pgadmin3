//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgSet.h - PostgreSQL ResultSet class
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGSET_H
#define PGSET_H

// wxWindows headers
#include <wx/wx.h>

// PostgreSQL headers
#include <libpq-fe.h>

// App headers
#include "pgAdmin3.h"

// Class declarations
class pgSet
{
public:
    pgSet(PGresult *newRes, PGconn *newConn);
    ~pgSet();
    long NumRows() const { return PQntuples(res); }
    long NumCols() const { return PQnfields(res); }
    void MoveNext();
    void MovePrevious();
    void MoveFirst();
    void MoveLast();
    long CurrentPos() const { return pos; }
    bool Eof() const { return eof; }
    bool Bof() const { return bof; }
    wxString ColName(int col) const { return wxString(PQfname(res, col + 1)); }
    wxString ColType(int col) const;
    int ColSize(int col) const { return PQfsize(res, col + 1); }
    int ColScale(int col) const;
    wxString GetVal(int col) const { return wxString(PQgetvalue(res, pos -1, col)); }
    wxString GetVal(const wxString& col) const;

private:
    PGconn *conn;
    PGresult *res;
    long pos;
    bool eof, bof;
    wxString ExecuteScalar(const wxString& sql) const;
};

#endif

