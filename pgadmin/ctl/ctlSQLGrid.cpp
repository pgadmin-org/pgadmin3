//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ctlSQLGrid.cpp - SQL Query result window
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/clipbrd.h>

#include "db/pgConn.h"
#include "ctl/ctlSQLGrid.h"
#include "utils/sysSettings.h"
#include "frm/frmExport.h"


#define EXTRAEXTENT_HEIGHT 6
#define EXTRAEXTENT_WIDTH  6

BEGIN_EVENT_TABLE(ctlSQLGrid, wxGrid)
	EVT_MOUSEWHEEL(ctlSQLGrid::OnMouseWheel)
	EVT_GRID_COL_SIZE(ctlSQLGrid::OnGridColSize)
	EVT_GRID_LABEL_LEFT_CLICK(ctlSQLGrid::OnLabelClick)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(ctlSQLGrid, wxGrid)

ctlSQLGrid::ctlSQLGrid()
{
}

ctlSQLGrid::ctlSQLGrid(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size)
	: wxGrid(parent, id, pos, size, wxWANTS_CHARS | wxVSCROLL | wxHSCROLL)
{
	// Set cells font
	wxFont fntCells(settings->GetSQLFont());
	SetDefaultCellFont(fntCells);
	// Set labels font
	wxFont fntLabel(settings->GetSystemFont());
	fntLabel.SetWeight(wxBOLD);
	SetLabelFont(fntLabel);
	SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);
	SetRowLabelSize(50);
	SetDefaultRowSize(fntCells.GetPointSize() * 2);
	SetColLabelSize(fntLabel.GetPointSize() * 4);
	SetDefaultCellOverflow(false);

	Connect(wxID_ANY, wxEVT_GRID_LABEL_LEFT_DCLICK, wxGridEventHandler(ctlSQLGrid::OnLabelDoubleClick));
}

void ctlSQLGrid::OnGridColSize(wxGridSizeEvent &event)
{
	// Save key="index:label", value=size
	int col = event.GetRowOrCol();
	colSizes[GetColKeyValue(col)] = GetColSize(col);
}

void ctlSQLGrid::OnCopy(wxCommandEvent &ev)
{
	Copy();
}

void ctlSQLGrid::OnMouseWheel(wxMouseEvent &event)
{
	if (event.ControlDown() || event.CmdDown())
	{
		wxFont fontlabel = GetLabelFont();
		wxFont fontcells = GetDefaultCellFont();
		if (event.GetWheelRotation() > 0)
		{
			fontlabel.SetPointSize(fontlabel.GetPointSize() - 1);
			fontcells.SetPointSize(fontcells.GetPointSize() - 1);
		}
		else
		{
			fontlabel.SetPointSize(fontlabel.GetPointSize() + 1);
			fontcells.SetPointSize(fontcells.GetPointSize() + 1);
		}
		SetLabelFont(fontlabel);
		SetDefaultCellFont(fontcells);
		SetColLabelSize(fontlabel.GetPointSize() * 4);
		SetDefaultRowSize(fontcells.GetPointSize() * 2);
		for (int index = 0; index < GetNumberCols(); index++)
			SetColSize(index, -1);
		ForceRefresh();
	}
	else
		event.Skip();
}

wxString ctlSQLGrid::GetExportLine(int row)
{
	return GetExportLine(row, 0, GetNumberCols() - 1);
}


wxString ctlSQLGrid::GetExportLine(int row, int col1, int col2)
{
	wxArrayInt cols;
	wxString str;
	int i;

	if (col2 < col1)
		return str;

	cols.Alloc(col2 - col1 + 1);
	for (i = col1; i <= col2; i++)
	{
		cols.Add(i);
	}

	return GetExportLine(row, cols);
}

wxString ctlSQLGrid::GetExportLine(int row, wxArrayInt cols)
{
	wxString str;
	unsigned int col;

	if (GetNumberCols() == 0)
		return str;

	for (col = 0 ; col < cols.Count() ; col++)
	{
		if (col > 0)
			str.Append(settings->GetCopyColSeparator());

		wxString text = GetCellValue(row, cols[col]);

		bool needQuote  = false;
		if (settings->GetCopyQuoting() == 1)
		{
			needQuote = IsColText(cols[col]);
		}
		else if (settings->GetCopyQuoting() == 2)
			/* Quote everything */
			needQuote = true;

		if (needQuote)
			str.Append(settings->GetCopyQuoteChar());
		str.Append(text);
		if (needQuote)
			str.Append(settings->GetCopyQuoteChar());
	}
	return str;
}

wxString ctlSQLGrid::GetColumnName(int colNum)
{
	wxString columnName = GetColLabelValue(colNum);
	columnName = columnName.Left(columnName.find(wxT("\n")));
	return columnName;
}

void ctlSQLGrid::AppendColumnHeader(wxString &str, int start, int end)
{
	size_t i, arrsize;
	arrsize = (end - start + 1);
	wxArrayInt columns;

	for(i = 0; i < arrsize; i++)
	{
		columns.Add(start + i);
	}

	AppendColumnHeader(str, columns);
}

void ctlSQLGrid::AppendColumnHeader(wxString &str, wxArrayInt columns)
{
	if(settings->GetColumnNames())
	{
		bool CopyQuoting = (settings->GetCopyQuoting() == 1 || settings->GetCopyQuoting() == 2);
		size_t i;

		for(i = 0; i < columns.Count() ; i++)
		{
			long columnPos = columns.Item(i);
			if(i > 0)
				str.Append(settings->GetCopyColSeparator());

			if(CopyQuoting)
				str.Append(settings->GetCopyQuoteChar());
			str.Append(GetColumnName(columnPos));
			if(CopyQuoting)
				str.Append(settings->GetCopyQuoteChar());

		}
		str.Append(END_OF_LINE);
	}
}

int ctlSQLGrid::Copy()
{
	wxString str;
	int copied = 0;
	size_t i;



	if (GetSelectedRows().GetCount())
	{
		AppendColumnHeader(str, 0, (GetNumberCols() - 1));

		wxArrayInt rows = GetSelectedRows();

		for (i = 0 ; i < rows.GetCount() ; i++)
		{
			str.Append(GetExportLine(rows.Item(i)));

			if (rows.GetCount() > 1)
				str.Append(END_OF_LINE);
		}

		copied = rows.GetCount();
	}
	else if (GetSelectedCols().GetCount())
	{
		wxArrayInt cols = GetSelectedCols();
		size_t numRows = GetNumberRows();

		AppendColumnHeader(str, cols);

		for (i = 0 ; i < numRows ; i++)
		{
			str.Append(GetExportLine(i, cols));

			if (numRows > 1)
				str.Append(END_OF_LINE);
		}

		copied = numRows;
	}
	else if (GetSelectionBlockTopLeft().GetCount() > 0 &&
	         GetSelectionBlockBottomRight().GetCount() > 0)
	{
		unsigned int x1, x2, y1, y2;

		x1 = GetSelectionBlockTopLeft()[0].GetCol();
		x2 = GetSelectionBlockBottomRight()[0].GetCol();
		y1 = GetSelectionBlockTopLeft()[0].GetRow();
		y2 = GetSelectionBlockBottomRight()[0].GetRow();

		AppendColumnHeader(str, x1, x2);

		for (i = y1; i <= y2; i++)
		{
			str.Append(GetExportLine(i, x1, x2));

			if (y2 > y1)
				str.Append(END_OF_LINE);
		}

		copied = y2 - y1 + 1;
	}
	else
	{
		int row, col;

		row = GetGridCursorRow();
		col = GetGridCursorCol();

		AppendColumnHeader(str, col, col);

		str.Append(GetExportLine(row, col, col));
		copied = 1;
	}

	if (copied && wxTheClipboard->Open())
	{
		wxTheClipboard->SetData(new wxTextDataObject(str));
		wxTheClipboard->Close();
	}
	else
	{
		copied = 0;
	}

	return copied;
}

void ctlSQLGrid::OnLabelDoubleClick(wxGridEvent &event)
{
	int maxHeight, maxWidth;
	GetClientSize(&maxWidth, &maxHeight);
	int row = event.GetRow();
	int col = event.GetCol();

	int extent, extentWant = 0;

	if (row >= 0)
	{
		for (col = 0 ; col < GetNumberCols() ; col++)
		{
			extent = GetBestSize(row, col).GetHeight();
			if (extent > extentWant)
				extentWant = extent;
		}

		extentWant += EXTRAEXTENT_HEIGHT;
		extentWant = wxMax(extentWant, GetRowMinimalAcceptableHeight());
		extentWant = wxMin(extentWant, maxHeight * 3 / 4);
		int currentHeight = GetRowHeight(row);

		if (currentHeight >= maxHeight * 3 / 4 || currentHeight == extentWant)
			extentWant = GetRowMinimalAcceptableHeight();
		else if (currentHeight < maxHeight / 4)
			extentWant = wxMin(maxHeight / 4, extentWant);
		else if (currentHeight < maxHeight / 2)
			extentWant = wxMin(maxHeight / 2, extentWant);
		else if (currentHeight < maxHeight * 3 / 4)
			extentWant = wxMin(maxHeight * 3 / 4, extentWant);

		if (extentWant != currentHeight)
		{
			BeginBatch();
			if(IsCellEditControlShown())
			{
				HideCellEditControl();
				SaveEditControlValue();
			}

			SetRowHeight(row, extentWant);
			EndBatch();
		}
	}
	else if (col >= 0)
	{
		// Holding Ctrl or Meta switches back to automatic column's sizing
		if (event.ControlDown() || event.CmdDown())
		{
			colSizes.erase(GetColKeyValue(col));
			BeginBatch();
			if(IsCellEditControlShown())
			{
				HideCellEditControl();
				SaveEditControlValue();
			}
			AutoSizeColumn(col, false);
			EndBatch();
		}
		else // toggle between some predefined sizes
		{

			if (col < (int)colMaxSizes.GetCount() && colMaxSizes[col] >= 0)
				extentWant = colMaxSizes[col];
			else
			{
				for (row = 0 ; row < GetNumberRows() ; row++)
				{
					if (CheckRowPresent(row))
					{
						extent = GetBestSize(row, col).GetWidth();
						if (extent > extentWant)
							extentWant = extent;
					}
				}
			}

			extentWant += EXTRAEXTENT_WIDTH;
			extentWant = wxMax(extentWant, GetColMinimalAcceptableWidth());
			extentWant = wxMin(extentWant, maxWidth * 3 / 4);
			int currentWidth = GetColumnWidth(col);

			if (currentWidth >= maxWidth * 3 / 4 || currentWidth == extentWant)
				extentWant = GetColMinimalAcceptableWidth();
			else if (currentWidth < maxWidth / 4)
				extentWant = wxMin(maxWidth / 4, extentWant);
			else if (currentWidth < maxWidth / 2)
				extentWant = wxMin(maxWidth / 2, extentWant);
			else if (currentWidth < maxWidth * 3 / 4)
				extentWant = wxMin(maxWidth * 3 / 4, extentWant);

			if (extentWant != currentWidth)
			{
				BeginBatch();
				if(IsCellEditControlShown())
				{
					HideCellEditControl();
					SaveEditControlValue();
				}
				SetColumnWidth(col, extentWant);
				EndBatch();
				colSizes[GetColKeyValue(col)] = extentWant;
			}
		}
	}
}

void ctlSQLGrid::OnLabelClick(wxGridEvent &event)
{
	int row = event.GetRow();
	int col = event.GetCol();

	// add support for (de)selecting multiple rows and cols with Control pressed
	if ( row >= 0 && (event.ControlDown() || event.CmdDown()) )
	{
		if (GetSelectedRows().Index(row) == wxNOT_FOUND)
			SelectRow(row, true);
		else
			DeselectRow(row);
	}
	else if ( col >= 0 && (event.ControlDown() || event.CmdDown()) )
	{
		if (GetSelectedCols().Index(col) == wxNOT_FOUND)
			SelectCol(col, true);
		else
			DeselectCol(col);
	}
	else
		event.Skip();
}

void ctlSQLGrid::AutoSizeColumn(int col, bool setAsMin, bool doLimit)
{
	ColKeySizeHashMap::iterator it = colSizes.find(GetColKeyValue(col));
	if (it != colSizes.end()) // Restore user-specified size
		SetColSize(col, it->second);
	else
		wxGrid::AutoSizeColumn(col, setAsMin);

	if (doLimit)
	{
		int newSize, oldSize;
		int maxSize, totalSize = 0, availSize;

		oldSize = GetColSize(col);
		availSize = GetClientSize().GetWidth() - GetRowLabelSize();
		maxSize = availSize / 2;
		for (int i = 0 ; i < GetNumberCols() ; i++)
			totalSize += GetColSize(i);

		if (oldSize > maxSize && totalSize > availSize)
		{
			totalSize -= oldSize;
			/* Shrink wide column to maxSize.
			 * If the rest of the columns are short, make sure to use all the remaining space,
			 *   but no more than oldSize (which is enough according to AutoSizeColumns())
			 */
			newSize = wxMin(oldSize, wxMax(maxSize, availSize - totalSize));
			SetColSize(col, newSize);
		}
	}
}

void ctlSQLGrid::AutoSizeColumns(bool setAsMin)
{
	wxCoord newSize, oldSize;
	wxCoord maxSize, totalSize = 0, availSize;
	int col, nCols = GetNumberCols();
	int row, nRows = GetNumberRows();
	colMaxSizes.Empty();

	/* We need to check each cell's width to choose best. wxGrid::AutoSizeColumns()
	 * is good, but looping through long result sets gives a noticeable slowdown.
	 * Thus we'll check every first 500 cells for each column.
	 */

	// First pass: auto-size columns
	for (col = 0 ; col < nCols; col++)
	{
		ColKeySizeHashMap::iterator it = colSizes.find(GetColKeyValue(col));
		if (it != colSizes.end()) // Restore user-specified size
		{
			newSize = it->second;
			colMaxSizes.Add(-1);
		}
		else
		{
			wxClientDC dc(GetGridWindow());
			newSize = 0;
			// get cells's width
			for (row = 0 ; row < wxMin(nRows, 500) ; row++)
			{
				wxSize size = GetBestSize(row, col);
				if ( size.x > newSize )
					newSize = size.x;
			}
			// get column's label width
			wxCoord w, h;
			dc.SetFont( GetLabelFont() );
			dc.GetMultiLineTextExtent( GetColLabelValue(col), &w, &h );
			if ( GetColLabelTextOrientation() == wxVERTICAL )
				w = h;

			if ( w > newSize )
				newSize = w;

			if (!newSize)
				newSize = GetRowLabelSize();
			else
				// leave some space around text
				newSize += 6;

			colMaxSizes.Add(newSize);
		}
		SetColSize(col, newSize);
		totalSize += newSize;
	}

	availSize = GetClientSize().GetWidth() - GetRowLabelSize();

	// Second pass: shrink wide columns if exceeded available width
	if (totalSize > availSize)
	{
		// A wide column shouldn't take up more than 50% of the visible space
		maxSize = availSize / 2;
		for (col = 0 ; col < nCols ; col++)
		{
			oldSize = GetColSize(col);
			// Is too wide and no user-specified size
			if (oldSize > maxSize && !(col < (int)colMaxSizes.GetCount() && colMaxSizes[col] == -1))
			{
				totalSize -= oldSize;
				/* Shrink wide column to maxSize.
				 * If the rest of the columns are short, make sure to use all the remaining space,
				 *   but no more than oldSize (which is enough according to first pass)
				 */
				newSize = wxMin(oldSize, wxMax(maxSize, availSize - totalSize));
				SetColSize(col, newSize);
				totalSize += newSize;
			}
		}
	}
}

wxString ctlSQLGrid::GetColKeyValue(int col)
{
	wxString colKey = wxString::Format(wxT("%d:"), col) + GetColLabelValue(col);
	return colKey;
}

wxSize ctlSQLGrid::GetBestSize(int row, int col)
{
	wxSize size;

	wxGridCellAttr *attr = GetCellAttr(row, col);
	wxGridCellRenderer *renderer = attr->GetRenderer(this, row, col);
	if ( renderer )
	{
		wxClientDC dc(GetGridWindow());
		size = renderer->GetBestSize(*this, *attr, dc, row, col);
		renderer->DecRef();
	}

	attr->DecRef();

	return size;
}
