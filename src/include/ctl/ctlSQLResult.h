//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: ctlSQLResult.h 4881 2006-01-09 09:11:33Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// ctlSQLResult.h - SQL Query result window
//
//////////////////////////////////////////////////////////////////////////

#ifndef CTLSQLRESULT_H
#define CTLSQLRESULT_H

// wxWindows headers
#include <wx/grid.h>
#include <wx/thread.h>

#include "pgSet.h"
#include "pgConn.h"




#define CTLSQL_RUNNING 100  // must be greater than ExecStatusType PGRES_xxx values

class ctlSQLResult : public wxGrid
{
public:
    ctlSQLResult(wxWindow *parent, pgConn *conn, wxWindowID id, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize);
    ~ctlSQLResult();


    int Execute(const wxString &query, int resultToDisplay=0); // > 0: resultset to display, <=0: last result
    void SetConnection(pgConn *conn);
    long NumRows() const;
    long InsertedCount() const;
    OID  InsertedOid() const;

    int Retrieve(long chunk=-1);
    int RetrieveOne();
    int Abort();

    bool Export();
    bool CanExport() { return rowsRetrieved>0 && colNames.GetCount() > 0; }

    wxString GetItemText(int row, int col=-1);
    wxString GetExportLine(int row);
    wxString GetExportLine(int row, wxArrayInt cols);
    wxString GetExportLine(int row, int col1, int col2);
    void Copy();

    int RunStatus();
    wxString GetMessagesAndClear();
    wxString GetErrorMessage();

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
    long rowsRetrieved;
    int maxRows;
};

class sqlResultTable : public wxGridTableBase
{
public:
    sqlResultTable();
    ~sqlResultTable();
    int GetNumberRows();
    int GetNumberCols();
    bool AppendCols(size_t numCols); // NOTE: Replaces cols rather than appends
    bool AppendRows(size_t numRows);
    void SetValue(int row, int col, const wxString& s);
    wxString GetValue(int row, int col);
    bool IsEmptyCell(int row, int col);
    bool DeleteRows(size_t pos = 0, size_t numRows = 1);

private:
    int nRows;
    int nCols;
    wxArrayString values;
};

#endif
