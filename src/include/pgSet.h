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
#include "misc.h"

// Class declarations
class pgSet
{
public:
    pgSet(PGresult *newRes, PGconn *newConn);
    ~pgSet();
    long NumRows() const { return nRows; }
    long NumCols() const { return PQnfields(res); }
    void MoveNext();
    void MovePrevious();
    void MoveFirst();
    void MoveLast();
    long CurrentPos() const { return pos; }
    bool Eof() const { return eof; }
    bool Bof() const { return bof; }
	
	// Keith 2003.03.04 --
	// pglib column names are zero based, so I replaced col+1 with col
    wxString ColName(int col) const { return wxString(PQfname(res, col)); }

    wxString ColType(int col) const;
    int ColSize(int col) const { return PQfsize(res, col + 1); }
    int ColScale(int col) const;
    wxString GetVal(int col) const { return wxString(PQgetvalue(res, pos -1, col)); }
    wxString GetVal(const wxString& col) const;
    long GetLong(int col) const { return StrToLong(GetVal(col)); }
    long GetLong(const wxString &col) { return StrToLong(GetVal(col)); }
    bool GetBool(int col) const { return StrToBool(GetVal(col)); }
    bool GetBool(const wxString &col) const { return StrToBool(GetVal(col)); }
    double GetDouble(int col) const { return StrToDouble(GetVal(col)); }
    double GetDouble(const wxString &col) const { return StrToDouble(GetVal(col)); }
    // we may replace double oid some time by an own data type
    double GetOid(int col) const { return StrToDouble(GetVal(col)); }
    double GetOid(const wxString &col) const { return StrToDouble(GetVal(col)); }


private:
    PGconn *conn;
    PGresult *res;
    long pos, nRows;
    bool eof, bof;
    wxString ExecuteScalar(const wxString& sql) const;
};

#endif

