//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// ctlSQLResult.h - SQL Query result window
//
//////////////////////////////////////////////////////////////////////////

#ifndef CTLSQLRESULT_H
#define CTLSQLRESULT_H

// wxWindows headers
#include <wx/listctrl.h>
#include <wx/thread.h>

#include "pgSet.h"
#include "pgConn.h"




#define CTLSQL_RUNNING 100  // must be greater than ExecStatusType PGRES_xxx values

class ctlSQLResult : public wxListView 
{
public:
    ctlSQLResult(wxWindow *parent, pgConn *conn, wxWindowID id, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize);
    ~ctlSQLResult();


    int Execute(const wxString &query, int resultToDisplay=0); // > 0: resultset to display, <=0: last result
    long NumRows() const;
    long InsertedCount() const;

    int Retrieve(long chunk=-1);
    int RetrieveOne();
    int Abort();

    bool Export();
    bool CanExport() { return rowsRetrieved>0 && colNames.GetCount() > 0; }

    int RunStatus();
    wxString GetMessagesAndClear();
    wxString GetErrorMessage();

    wxArrayString colNames;
    wxArrayString colTypes;
    wxArrayLong colTypClasses;

private:
    pgQueryThread *thread;
    pgConn *conn;
    long rowsRetrieved;
};

#endif
