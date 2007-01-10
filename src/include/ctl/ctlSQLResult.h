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

//////////////////////////////////////////////////////////////////////////
// Set this define to use a wxListView control instead of a wxGrid
// This will disable some features such as advanced cell selection and
// copying, but the wxListView is a better designed control
//////////////////////////////////////////////////////////////////////////
#define USE_LISTVIEW 0
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/thread.h>

#include "pgSet.h"
#include "base/pgConnBase.h"
#include "pgConn.h"
#if USE_LISTVIEW
#include <wx/listctrl.h>
#else
#include "ctlSQLGrid.h"
#endif



#define CTLSQL_RUNNING 100  // must be greater than ExecStatusType PGRES_xxx values

#if USE_LISTVIEW
class ctlSQLResult : public wxListView
#else
class ctlSQLResult : public ctlSQLGrid
#endif
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
	bool ToFile();
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

#if USE_LISTVIEW
	void SelectAll();
	wxString GetExportLine(int row);
#else
    void SetMaxRows(int rows);
    void ResultsFinished();
    void OnGridSelect(wxGridRangeSelectEvent& event);
#endif

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

#if !USE_LISTVIEW
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

#endif
