//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
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

    void MoveNext() { if (pos <= nRows) pos++; }
    void MovePrevious() { if (pos > 0) pos--; }
    void MoveFirst() { if (nRows) pos=1; else pos=0; }
    void MoveLast() { pos=nRows; }
    void Locate(long l) { pos=l; }
    long CurrentPos() const { return pos; }
    bool Bof() const { return (!nRows || pos < 1); }
    bool Eof() const { return (!nRows || pos > nRows); }
	
    wxString ColName(int col) const { return wxString(PQfname(res, col)); }

    Oid ColTypeOid(int col) const;
    wxString ColType(int col) const;
    Oid GetInsertedOid() const { return PQoidValue(res); }
    int ColSize(int col) const { return PQfsize(res, col); }
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
    wxString ExecuteScalar(const wxString& sql) const;
};



class pgQueryThread : public wxThread
{
public:
    pgQueryThread(PGconn *_conn, const wxString &qry);
    ~pgQueryThread();

    virtual void *Entry();
    bool DataValid() const { return dataSet != NULL; }
    pgSet *DataSet() { return dataSet; }
    int ReturnCode() const { return rc; }
    wxString GetMessages() const { return messages; }
    bool IsRunning() const;

private:
    int rc;

    wxString query;
    PGconn *conn;
    PGresult *result;
    wxString messages;
    pgSet *dataSet;

    int execute();
};

#endif

