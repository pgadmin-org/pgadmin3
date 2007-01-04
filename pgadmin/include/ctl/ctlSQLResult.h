//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// ctlSQLResult.h - SQL Query result window
//
//////////////////////////////////////////////////////////////////////////

#ifndef CTLSQLRESULT_H
#define CTLSQLRESULT_H

// wxWindows headers
#include <wx/thread.h>

#include "db/pgSet.h"
#include "db/pgConn.h"
#include "ctlSQLGrid.h"

#define CTLSQL_RUNNING 100  // must be greater than ExecStatusType PGRES_xxx values

class ctlSQLResult : public ctlSQLGrid
{
public:
    ctlSQLResult(wxWindow *parent, pgConn *conn, wxWindowID id, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize);
    ~ctlSQLResult();


    int Execute(const wxString &query, int resultToDisplay=0); // > 0: resultset to display, <=0: last result
    void SetConnection(pgConn *conn);
    long NumRows() const;
    long InsertedCount() const;
    OID  InsertedOid() const;

    int Abort();

    bool Export();
    bool CanExport() { return NumRows() >0 && colNames.GetCount() > 0; }

	wxString OnGetItemText(long item, long col) const;
    bool IsColText(int col);
	bool hasRowNumber() { return !rowcountSuppressed; }

    int RunStatus();
    wxString GetMessagesAndClear();
    wxString GetErrorMessage();
    pgError GetResultError();

	void DisplayData(bool single=false);

    bool GetRowCountSuppressed() { return rowcountSuppressed; };

    void SetMaxRows(int rows);
    void ResultsFinished();
    void OnGridSelect(wxGridRangeSelectEvent& event);

    wxArrayString colNames;
    wxArrayString colTypes;
    wxArrayLong colTypClasses;

    wxArrayInt  colSizes;
    wxArrayString colHeaders;

private:
    pgQueryThread *thread;
    pgConn *conn;
	bool rowcountSuppressed;
};

class sqlResultTable : public wxGridTableBase
{
public:
    sqlResultTable();
    wxString GetValue(int row, int col);
    int GetNumberRows();
    int GetNumberCols();
    bool IsEmptyCell(int row, int col) { return false; }
    wxString GetColLabelValue(int col);
    void SetValue(int row, int col, const wxString& value) { return; }
    void SetThread(pgQueryThread *t) { thread = t; }
    bool DeleteRows(size_t pos = 0, size_t numRows = 1) { return true; }
    bool DeleteCols(size_t pos = 0, size_t numCols = 1) { return true; }

private:
    pgQueryThread *thread;
};

#endif
