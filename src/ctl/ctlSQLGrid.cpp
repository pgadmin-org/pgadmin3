//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: ctlSQLGrid.cpp 4887 2006-01-11 09:19:12Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// ctlSQLGrid.cpp - SQL Query result window
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/clipbrd.h>

#include "pgConn.h"
#include "ctl/ctlSQLGrid.h"
#include "sysSettings.h"
#include "frmExport.h"



ctlSQLGrid::ctlSQLGrid(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size)
: wxGrid(parent, id, pos, size, wxWANTS_CHARS|wxVSCROLL|wxHSCROLL)
{
    wxFont fntLabel(settings->GetSystemFont());
    fntLabel.SetWeight(wxBOLD);
    SetLabelFont(fntLabel);
    SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);
	SetRowLabelSize(50);
	SetDefaultCellOverflow(false);

    Connect(wxID_ANY, wxEVT_GRID_LABEL_LEFT_DCLICK, wxGridEventHandler(ctlSQLGrid::OnLabelDoubleClick));
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

    for (col=0 ; col < cols.Count() ; col++)
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

int ctlSQLGrid::Copy()
{
    wxString str;
    int copied = 0;
    size_t i;

    if (GetSelectedRows().GetCount()) 
	{
        wxArrayInt rows = GetSelectedRows();

        for (i=0 ; i < rows.GetCount() ; i++)
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

        for (i=0 ; i < numRows ; i++)
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

        str.Append(GetExportLine(row, col, col));
        copied = 1;
    }

    if (copied && wxTheClipboard->Open())
    {
        wxTheClipboard->SetData(new wxTextDataObject(str));
        wxTheClipboard->Close();
    }
    else {
        copied = 0;
    }

    return copied;
}

#define EXTRAEXTENT_HEIGHT 6
#define EXTRAEXTENT_WIDTH  6

void ctlSQLGrid::OnLabelDoubleClick(wxGridEvent& event)
{
#if wxCHECK_VERSION(2, 5, 0)
    // at the moment, not implemented for 2.4
    int maxHeight, maxWidth;
    GetClientSize(&maxWidth, &maxHeight);
    int row = event.GetRow();
    int col = event.GetCol();

    int extent, extentWant=0;

    if (row >= 0)
    {
        for (col=0 ; col < GetNumberCols() ; col++)
        {
            extent = GetBestSize(row, col).GetHeight();
            if (extent > extentWant)
                extentWant = extent;
        }

        extentWant += EXTRAEXTENT_HEIGHT;
        extentWant = wxMax(extentWant, GetRowMinimalAcceptableHeight());
        extentWant = wxMin(extentWant, maxHeight*3/4);
        int currentHeight = GetRowHeight(row);
            
        if (currentHeight >= maxHeight*3/4 || currentHeight == extentWant)
            extentWant = GetRowMinimalAcceptableHeight();
        else if (currentHeight < maxHeight/4)
            extentWant = wxMin(maxHeight/4, extentWant);
        else if (currentHeight < maxHeight/2)
            extentWant = wxMin(maxHeight/2, extentWant);
        else if (currentHeight < maxHeight*3/4)
            extentWant = wxMin(maxHeight*3/4, extentWant);

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
        for (row=0 ; row < GetNumberRows() ; row++)
        {
            if (CheckRowPresent(row))
            {
                extent = GetBestSize(row, col).GetWidth();
                if (extent > extentWant)
                    extentWant=extent;
            }
        }

        extentWant += EXTRAEXTENT_WIDTH;
        extentWant = wxMax(extentWant, GetColMinimalAcceptableWidth());
        extentWant = wxMin(extentWant, maxWidth*3/4);
        int currentWidth=GetColumnWidth(col);
            
        if (currentWidth >= maxWidth*3/4 || currentWidth == extentWant)
            extentWant = GetColMinimalAcceptableWidth();
        else if (currentWidth < maxWidth/4)
            extentWant = wxMin(maxWidth/4, extentWant);
        else if (currentWidth < maxWidth/2)
            extentWant = wxMin(maxWidth/2, extentWant);
        else if (currentWidth < maxWidth*3/4)
            extentWant = wxMin(maxWidth*3/4, extentWant);

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
        }
    }
#endif
}

wxSize ctlSQLGrid::GetBestSize(int row, int col)
{
    wxSize size;

    wxGridCellAttr* attr = GetCellAttr(row, col);
    wxGridCellRenderer* renderer = attr->GetRenderer(this, row, col);
    if ( renderer )
    {
        wxClientDC dc(GetGridWindow());
        size = renderer->GetBestSize(*this, *attr, dc, row, col);
        renderer->DecRef();
    }

    attr->DecRef();

    return size;
}
