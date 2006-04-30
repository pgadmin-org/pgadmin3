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

#define USE_LISTVIEW 1

// wxWindows headers
#include <wx/thread.h>

#include "pgSet.h"
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
    bool CanExport() { return NumRows() >0 && colNames.GetCount() > 0; }

	wxString OnGetItemText(long item, long col) const;
    bool IsColText(int col);
	bool hasRowNumber() { return !rowcountSuppressed; }

    int RunStatus();
    wxString GetMessagesAndClear();
    wxString GetErrorMessage();

	void DisplayData(bool single=false);


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

#endif
