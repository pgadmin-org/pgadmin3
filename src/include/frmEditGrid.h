//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmQuery.h - The SQL Query form
//
//////////////////////////////////////////////////////////////////////////

#ifndef __FRMEDITGRID_H
#define __FRMEDITGRID_H

#include <wx/grid.h>

#define CTL_EDITGRID 357



class ctlSQLGrid : public wxGrid
{
public:
    ctlSQLGrid(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size);
};




class frmEditGrid : public wxFrame
{
public:
    frmEditGrid(frmMain *form, const wxString& _title, pgConn *conn, const wxPoint& pos, const wxSize& size, pgSchemaObject *obj);
    ~frmEditGrid();

    void Go();

protected:

    void OnAbort(wxCommandEvent& event);
    void Abort();
    ctlSQLGrid *sqlGrid;

    bool hasOids;
    double rowsEstimated;
    wxString relkind;

    wxString tableName;
    frmMain *mainForm;
    pgConn *connection;
    pgQueryThread *thread;
    int *columnTypes;

    DECLARE_EVENT_TABLE();
};
#endif