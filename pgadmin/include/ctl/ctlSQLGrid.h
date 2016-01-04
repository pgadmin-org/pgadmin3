//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
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
	ctlSQLGrid(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size);
	ctlSQLGrid();

	wxString GetExportLine(int row);
	wxString GetExportLine(int row, wxArrayInt cols);
	wxString GetExportLine(int row, int col1, int col2);
	virtual bool IsColText(int col)
	{
		return false;
	}
	int Copy();

	virtual bool CheckRowPresent(int row)
	{
		return true;
	}
	wxSize GetBestSize(int row, int col);
	void OnLabelDoubleClick(wxGridEvent &event);
	void OnLabelClick(wxGridEvent &event);

	void AutoSizeColumn(int col, bool setAsMin = false, bool doLimit = true);
	void AutoSizeColumns(bool setAsMin);

	WX_DECLARE_STRING_HASH_MAP( int, ColKeySizeHashMap );

	DECLARE_DYNAMIC_CLASS(ctlSQLGrid)
	DECLARE_EVENT_TABLE()

private:
	void OnCopy(wxCommandEvent &event);
	void OnMouseWheel(wxMouseEvent &event);
	void OnGridColSize(wxGridSizeEvent &event);
	wxString GetColumnName(int colNum);
	wxString GetColKeyValue(int col);
	void AppendColumnHeader(wxString &str, int start, int end);
	void AppendColumnHeader(wxString &str, wxArrayInt columns);

	// Stores sizes of colums explicitly resized by user
	ColKeySizeHashMap colSizes;
	// Max size for each column
	wxArrayInt colMaxSizes;
};

#endif
