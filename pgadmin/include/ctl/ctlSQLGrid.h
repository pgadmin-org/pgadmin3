//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// ctlSQLGrid.h - SQL Data Display Grid
//
//////////////////////////////////////////////////////////////////////////

#ifndef CTLSQLGRID_H
#define CTLSQLGRID_H

// wxWindows headers
#include <wx/grid.h>


class ctlSQLGrid : public wxGrid
{
public:
    ctlSQLGrid(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size);
    ctlSQLGrid();

    wxString GetExportLine(int row);
    wxString GetExportLine(int row, wxArrayInt cols);
    wxString GetExportLine(int row, int col1, int col2);
    virtual bool IsColText(int col) { return false; }
    int Copy();

    virtual bool CheckRowPresent(int row) { return true; }
    wxSize GetBestSize(int row, int col);
    void OnLabelDoubleClick(wxGridEvent& event);

    DECLARE_DYNAMIC_CLASS(ctlSQLGrid)
    DECLARE_EVENT_TABLE()

private:
    void OnCopy(wxCommandEvent& event);
};

#endif
