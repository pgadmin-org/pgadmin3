//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// basePgSet.h - PostgreSQL ResultSet class
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGSETBASE_H
#define PGSETBASE_H

// wxWindows headers
#include <wx/wx.h>
#include <wx/datetime.h>

// PostgreSQL headers
#include <libpq-fe.h>

#include "base/base.h"

typedef enum
{
    PGTYPCLASS_NUMERIC,
    PGTYPCLASS_BOOL,
    PGTYPCLASS_STRING,
    PGTYPCLASS_DATE,
    PGTYPCLASS_OTHER
} pgTypClass;


class pgConnBase;

// Class declarations
class pgSetBase
{
public:
    pgSetBase(PGresult *newRes, pgConnBase *newConn, wxMBConv &cnv, bool needColQt);
    ~pgSetBase();
    long NumRows() const { return nRows; }
    long NumCols() const { return nCols; }

    void MoveNext() { if (pos <= nRows) pos++; }
    void MovePrevious() { if (pos > 0) pos--; }
    void MoveFirst() { if (nRows) pos=1; else pos=0; }
    void MoveLast() { pos=nRows; }
    void Locate(long l) { pos=l; }
    long CurrentPos() const { return pos; }
    bool Bof() const { return (!nRows || pos < 1); }
    bool Eof() const { return (!nRows || pos > nRows); }
    wxString ColName(const int col) const;
    OID ColTypeOid(const int col) const;
    long ColTypeMod(const int col) const;
    wxString ColType(const int col) const;
    wxString ColFullType(const int col) const;
    pgTypClass ColTypClass(const int col) const;

    OID GetInsertedOid() const { return PQoidValue(res); }
    long GetInsertedCount() const;
    int ColSize(const int col) const { return PQfsize(res, col); }
    bool IsNull(const int col) const { return (PQgetisnull(res, pos-1, col) != 0); }
    int ColScale(const int col) const;
    int ColNumber(const wxString &colName) const;


    wxString GetVal(const int col) const;
    wxString GetVal(const wxString& col) const;
    long GetLong(const int col) const;
    long GetLong(const wxString &col) const;
    bool GetBool(const int col) const;
    bool GetBool(const wxString &col) const;
    double GetDouble(const int col) const;
    double GetDouble(const wxString &col) const;
    wxDateTime GetDateTime(const int col) const;
    wxDateTime GetDateTime(const wxString &col) const;
    wxDateTime GetDate(const int col) const;
    wxDateTime GetDate(const wxString &col) const;
    wxULongLong GetLongLong(const int col) const;
    wxULongLong GetLongLong(const wxString &col) const;
    OID GetOid(const int col) const;
    OID GetOid(const wxString &col) const;

    char *GetCharPtr(const int col) const;
    char *GetCharPtr(const wxString &col) const;

    wxMBConv &GetConversion() const { return conv; }


protected:
    pgConnBase *conn;
    PGresult *res;
    long pos, nRows, nCols;
    wxString ExecuteScalar(const wxString& sql) const;
    wxMBConv &conv;
    bool needColQuoting;
    wxArrayString colTypes, colFullTypes;
};



class pgSetIterator
{
public:
    pgSetIterator(pgSetBase *s);
    pgSetIterator(pgConnBase *conn, const wxString &sql);
    ~pgSetIterator();

    bool RowsLeft();
    bool MovePrev();
	bool IsValid() { return set != 0; }
    pgSetBase *Set() { return set; }

    wxString GetVal(const int col) const { return set->GetVal(col); }
    wxString GetVal(const wxString& col) const { return set->GetVal(col); }
    long GetLong(const int col) const { return set->GetLong(col); }
	long GetLong(const wxString &col) const { return set->GetLong(col); }
    bool GetBool(const int col) const { return set->GetBool(col); }
    bool GetBool(const wxString &col) const { return set->GetBool(col); }
    double GetDouble(const int col) const { return set->GetDouble(col); }
    double GetDouble(const wxString &col) const { return set->GetDouble(col); }
    wxDateTime GetDateTime(const int col) const { return set->GetDateTime(col); }
    wxDateTime GetDateTime(const wxString &col) const  { return set->GetDateTime(col); }
    wxDateTime GetDate(const int col) const { return set->GetDate(col); }
    wxDateTime GetDate(const wxString &col) const  { return set->GetDate(col); }
    wxULongLong GetLongLong(const int col) const { return set->GetLongLong(col); }
    wxULongLong GetLongLong(const wxString &col) const { return set->GetLongLong(col); }
    OID GetOid(const int col) const { return set->GetOid(col); }
    OID GetOid(const wxString &col) const { return set->GetOid(col); }

protected:
    pgSetBase *set;
    bool first;
};



class pgQueryThreadBase : public wxThread
{
public:
    pgQueryThreadBase(pgConnBase *_conn, const wxString &qry, int resultToRetrieve=-1);
    ~pgQueryThreadBase();

    virtual void *Entry();
    bool DataValid() const { return dataSet != NULL; }
    pgSetBase *DataSet() { return dataSet; }
    int ReturnCode() const { return rc; }
    long RowsInserted() const { return rowsInserted; }
    OID InsertedOid() const { return insertedOid; }
    wxString GetMessagesAndClear();
    bool IsRunning() const;
    void appendMessage(const wxString &str);

protected:
    int rc;
    int resultToRetrieve;
    long rowsInserted;
    OID insertedOid;

    wxString query;
    pgConnBase *conn;
    PGresult *result;
    wxString messages;
    pgSetBase *dataSet;
    wxCriticalSection criticalSection;

    int execute();
};

#endif

