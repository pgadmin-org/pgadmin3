//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: ctlSQLResult.cpp 4887 2006-01-11 09:19:12Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// ctlSQLResult.cpp - SQL Query result window
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/clipbrd.h>

#include "pgConn.h"
#include "ctl/ctlSQLResult.h"
#include "sysSettings.h"
#include "frmExport.h"



ctlSQLResult::ctlSQLResult(wxWindow *parent, pgConn *_conn, wxWindowID id, const wxPoint& pos, const wxSize& size)
: wxGrid(parent, id, pos, size, wxWANTS_CHARS|wxVSCROLL|wxHSCROLL)
{
    conn=_conn;
    thread=0;
    CreateGrid(0, 0);
    EnableEditing(false);
    SetSizer(new wxBoxSizer(wxVERTICAL));

    wxFont fntLabel(settings->GetSystemFont());
    fntLabel.SetWeight(wxBOLD);
    SetLabelFont(fntLabel);

    Connect(wxID_ANY, wxEVT_GRID_RANGE_SELECT, wxGridRangeSelectEventHandler(ctlSQLResult::OnGridSelect));
}



ctlSQLResult::~ctlSQLResult()
{
    Abort();
}


void ctlSQLResult::SetConnection(pgConn *_conn)
{
    conn=_conn;
}


bool ctlSQLResult::Export()
{
    if (rowsRetrieved>0 || (thread && thread->DataSet()->NumRows() > 0))
    {
        frmExport dlg(this);
        if (dlg.ShowModal() == wxID_OK)
        {
            if (rowsRetrieved> 0)
                return dlg.Export(this);
            else
                return dlg.Export(thread->DataSet());
        }
    }
    return false;
}



wxString ctlSQLResult::GetExportLine(int row)
{
    return GetExportLine(row, 0, GetNumberCols() - 1);
}

wxString ctlSQLResult::GetExportLine(int row, int col1, int col2)
{
    wxArrayInt cols;
    wxString str;
    int i;

    if (col2 < col1)
        return str;

    cols.Alloc(col2 - col1 + 1);
    for (i = col1; i <= col2; i++) {
        cols.Add(i);
    }

    return GetExportLine(row, cols);
}


wxString ctlSQLResult::GetExportLine(int row, wxArrayInt cols)
{
    if (GetNumberCols() <= 1)
        return GetItemText(row);

    wxString str;
    unsigned int col;
    for (col=0 ; col < cols.Count() ; col++)
    {
        if (col > 0)
            str.Append(settings->GetCopyColSeparator());

        wxString text = GetCellValue(row, cols[col]);

		bool needQuote  = false;
		if (settings->GetCopyQuoting() == 1)
		{
			/* Quote strings only */
			switch (colTypClasses.Item(cols[col]))
			{
			case PGTYPCLASS_NUMERIC:
			case PGTYPCLASS_BOOL:
				break;
			default:
				needQuote=true;
				break;
			}
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


int ctlSQLResult::Execute(const wxString &query, int resultToRetrieve)
{
    Abort();

    colSizes.Empty();
    colHeaders.Empty();
    int num;

    int i;
    for (i=0 ; i < GetNumberCols() ; i++)
    {
        colHeaders.Add(GetColLabelValue(i));
        colSizes.Add(GetColSize(i));
    }

    num = GetNumberRows();
    if (num)
        DeleteRows(0, num);
    num = GetNumberCols();
    if (num)
        DeleteCols(0, num);

    rowsRetrieved=0;
    maxRows = 0;
    colNames.Empty();
    colTypes.Empty();
    colTypClasses.Empty();

    thread = new pgQueryThread(conn, query, resultToRetrieve);

    if (thread->Create() != wxTHREAD_NO_ERROR)
    {
        Abort();
        return -1;
    }

    thread->Run();
    return RunStatus();
}


int ctlSQLResult::Abort()
{
    if (thread)
    {
        thread->Delete();
        delete thread;
    }
    thread=0;
    return 0;
}


int ctlSQLResult::RetrieveOne()
{
    if (!thread || !thread->DataValid())
        return -1;

    if (thread->ReturnCode() != PGRES_TUPLES_OK)
        return -1;

    if (!rowsRetrieved)
    {
        colNames.Add(thread->DataSet()->ColName(0));
        colTypes.Add(wxT(""));
        colTypClasses.Add(0L);


        SetColLabelValue(0, thread->DataSet()->ColName(0));
        SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

        while (!thread->DataSet()->Eof())
        {
            SetCellValue(rowsRetrieved, 0, thread->DataSet()->GetVal(0));
            rowsRetrieved++;
            thread->DataSet()->MoveNext();
        }
        return rowsRetrieved;
    }
    return 0;
}


int ctlSQLResult::Retrieve(long chunk)
{
    if (!thread || !thread->DataValid())
        return 0;

    if (chunk<0)
        chunk=thread->DataSet()->NumRows();
    wxLogInfo(wxT("retrieve %ld: did %ld of %ld"), chunk, rowsRetrieved, NumRows());

    long col, nCols=thread->DataSet()->NumCols();
    if (!rowsRetrieved)
    {
        wxString colName, colType;
        colTypes.Add(wxT(""));
        colTypClasses.Add(0L);

        Freeze();
        
        if (!GetTable()) {
            SetTable(new sqlResultTable(), true);
        }
        GetTable()->AppendCols(nCols);
        SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);
        if (maxRows)
            GetTable()->AppendRows(maxRows);
        else
            GetTable()->AppendRows(NumRows());

        size_t hdrIndex=0;

        for (col=0 ; col < nCols ; col++)
        {
            colName = thread->DataSet()->ColName(col);
            colType = thread->DataSet()->ColType(col);
            colNames.Add(colName);
            colTypes.Add(colType);
            colTypClasses.Add(thread->DataSet()->ColTypClass(col));

            wxString colHeader = colName + wxT("\n") + colType;

            int w;
            if (hdrIndex < colHeaders.GetCount() && colHeaders.Item(hdrIndex) == colHeader)
                w = colSizes.Item(hdrIndex++);
            else
            {
                if (hdrIndex+1 < colHeaders.GetCount() && colHeaders.Item(hdrIndex+1) == colHeader)
                {
                    hdrIndex++;
                    w = colSizes.Item(hdrIndex++);
                }
                else
                    w=-1;
            }

            SetColLabelValue(col, colHeader);
            SetColSize(col, w);
        }
        Thaw();
    }

    long count=0;
    long maxColSize=settings->GetMaxColSize();

    while (chunk-- && !thread->DataSet()->Eof())
    {
        for (col=0 ; col < nCols ; col++)
        {
            wxString value = thread->DataSet()->GetVal(col);
            if (maxColSize > 0)
            {
                if ((int)value.Length() > maxColSize)
                    value = value.Left(maxColSize) + wxT(" (..)");
            }
            GetTable()->SetValue(rowsRetrieved, col, value);
        }
        
        thread->DataSet()->MoveNext();
        rowsRetrieved++;
        count++;
    }
    wxLogInfo(wxT("retrieve done %ld: did %ld of %ld"), count, rowsRetrieved, NumRows());

    return (count);
}


wxString ctlSQLResult::GetItemText(int row, int col)
{
    if (col < 0)
        return GetExportLine(row);
    else
        return GetCellValue(row, col);
}


wxString ctlSQLResult::GetMessagesAndClear()
{
    if (thread)
        return thread->GetMessagesAndClear();
    return wxEmptyString;
}


wxString ctlSQLResult::GetErrorMessage()
{
    return conn->GetLastError();
}



long ctlSQLResult::NumRows() const
{
    if (thread && thread->DataValid())
        return thread->DataSet()->NumRows();
    return 0;
}


long ctlSQLResult::InsertedCount() const
{
    if (thread)
        return thread->RowsInserted();
    return -1;
}


OID ctlSQLResult::InsertedOid() const
{
    if (thread)
        return thread->InsertedOid();
    return (OID)-1;
}


int ctlSQLResult::RunStatus()
{
    if (!thread)
        return -1;
    
    if (thread->IsRunning())
        return CTLSQL_RUNNING;

    return thread->ReturnCode();
}


void ctlSQLResult::Copy()
{
    wxString str;
    int copied = 0;
    size_t i;

    if (GetSelectedRows().GetCount()) {
        wxArrayInt rows = GetSelectedRows();

        for (i=0 ; i < rows.GetCount() ; i++)
        {
            str.Append(GetExportLine(rows.Item(i)));
    
            if (rows.GetCount() > 1)
                str.Append(END_OF_LINE);
        }

        copied = rows.GetCount();
    }
    else if (GetSelectedCols().GetCount()) {
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
        GetSelectionBlockBottomRight().GetCount() > 0) {
        unsigned int x1, x2, y1, y2;

        x1 = GetSelectionBlockTopLeft()[0].GetCol();
        x2 = GetSelectionBlockBottomRight()[0].GetCol();
        y1 = GetSelectionBlockTopLeft()[0].GetRow();
        y2 = GetSelectionBlockBottomRight()[0].GetRow();

        for (i = y1; i <= y2; i++) {
            str.Append(GetExportLine(i, x1, x2));

            if (y2 > y1)
                str.Append(END_OF_LINE);
        }

        copied = y2 - y1 + 1;
    }
    else {
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
}

void ctlSQLResult::SetMaxRows(int rows)
{
    maxRows = rows;
}

void ctlSQLResult::ResultsFinished()
{
    int rows = GetTable()->GetNumberRows();

    if (rowsRetrieved < rows) {
        GetTable()->DeleteRows(rowsRetrieved, rows - rowsRetrieved);
    }
}

void ctlSQLResult::OnGridSelect(wxGridRangeSelectEvent& event)
{
    SetFocus();
}

sqlResultTable::sqlResultTable()
{
    nRows = nCols = 0;
    values = 0;
}

sqlResultTable::~sqlResultTable()
{
    values.Clear();
}

int sqlResultTable::GetNumberRows()
{
    return nRows;
}

int sqlResultTable::GetNumberCols()
{
    return nCols;
}

bool sqlResultTable::AppendCols(size_t numCols)
{
    if (values.Count()) {
        values.Clear();
        nRows = 0;
    }

    nCols = numCols;
    return true;
}

bool sqlResultTable::AppendRows(size_t numRows)
{
    if (numRows <= 0)
        return false;

    values.Add(wxT(""), nCols * numRows);
    nRows += numRows;
    return true;
}

void sqlResultTable::SetValue(int row, int col, const wxString& s)
{
    if (row < 0 || row >= nRows)
        return;

    if (col < 0 || col >= nCols)
        return;

    values[row * nCols + col] = s;
}

wxString sqlResultTable::GetValue(int row, int col)
{
    if (row < 0 || row >= nRows)
        return wxT("");

    if (col < 0 || col >= nCols)
        return wxT("");

    return values[row * nCols + col];
}

bool sqlResultTable::IsEmptyCell(int row, int col)
{
    return false;
}

bool sqlResultTable::DeleteRows(size_t pos, size_t numRows)
{
    if (pos >= (size_t)nRows)
        return false;

    values.RemoveAt(pos, numRows);

    if (pos + numRows > (size_t)nRows)
        nRows = pos;
    else
        nRows -= numRows;

    return true;
}
