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
    long NumRows();
    long NumCols();
    void MoveNext();
    void MovePrevious();
    void MoveFirst();
    void MoveLast();
    long CurrentPos();
    bool Eof();
    bool Bof();
    wxString ColName(int col) const;
    wxString ColType(int col) const;
    int ColSize(int col);
    int ColScale(int col);
    wxString GetVal(int col) const;
    wxString GetVal(const wxString& col) const;

private:
    PGconn *conn;
    PGresult *res;
    long pos;
    bool eof, bof;
    wxString ExecuteScalar(const wxString& sql) const;
};

#endif

