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
#include "../../pgAdmin3.h"

// Class declarations
class pgSet
{
public:
    pgSet(PGresult *objNewRes, PGconn *objNewConn);
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
    wxString ColName(int iCol) const;
    wxString ColType(int iCol) const;
    int ColSize(int iCol);
    int ColScale(int iCol);
    wxString GetVal(int iCol) const;
    wxString GetVal(const wxString& szCol) const;

private:
    PGconn *objConn;
    PGresult *objRes;
    long lPos;
    bool bEof, bBof;
    wxString ExecuteScalar(const wxString& szSQL) const;
};

#endif

