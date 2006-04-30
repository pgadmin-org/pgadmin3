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
#if USE_LISTVIEW
: wxListView(parent, id, pos, size, wxLC_VIRTUAL | wxLC_REPORT | wxSUNKEN_BORDER)
#else
: ctlSQLGrid(parent, id, pos, size)
#endif
{
    conn=_conn;
    thread=0;

#if !USE_LISTVIEW
    CreateGrid(0, 0);
    EnableEditing(false);
    SetSizer(new wxBoxSizer(wxVERTICAL));

    Connect(wxID_ANY, wxEVT_GRID_RANGE_SELECT, wxGridRangeSelectEventHandler(ctlSQLResult::OnGridSelect));
#endif
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
    if (NumRows() > 0)
    {
        frmExport dlg(this);
        if (dlg.ShowModal() == wxID_OK)
            return dlg.Export(thread->DataSet());
    }
    return false;
}


#if USE_LISTVIEW
void ctlSQLResult::SelectAll()
{
	for (int i = 0; i < GetItemCount(); i++)
		Select(i);
}


wxString ctlSQLResult::GetExportLine(int row)
{
   if (GetColumnCount() <= 1)
        return OnGetItemText(row, 1);

    wxString str;
    int col;
    for (col=1 ; col < GetColumnCount() ; col++)
    {
        if (col > 1)
            str.Append(settings->GetCopyColSeparator());

        wxString text=OnGetItemText(row, col);

		bool needQuote  = false;
		if (settings->GetCopyQuoting() == 1)
		{
			/* Quote strings only */
			switch (colTypClasses.Item(col))
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
#endif


bool ctlSQLResult::IsColText(int col)
{
	switch (colTypClasses.Item(col))
	{
	case PGTYPCLASS_NUMERIC:
	case PGTYPCLASS_BOOL:
		return false;
	}

    return true;
}


int ctlSQLResult::Execute(const wxString &query, int resultToRetrieve)
{
    Abort();

    colSizes.Empty();
    colHeaders.Empty();
    int i;

#if USE_LISTVIEW
    wxListItem item;
    item.SetMask(wxLIST_MASK_TEXT|wxLIST_MASK_WIDTH);

    for (i=0 ; i < GetColumnCount() ; i++)
    {
        GetColumn(i, item);
        colHeaders.Add(item.GetText());
        colSizes.Add(item.GetWidth());
    }

    ClearAll();

#else
    for (i=0 ; i < GetNumberCols() ; i++)
    {
        colHeaders.Add(GetColLabelValue(i));
        colSizes.Add(GetColSize(i));
    }

    int num;
    num = GetNumberRows();
    if (num)
        DeleteRows(0, num);
    num = GetNumberCols();
    if (num)
        DeleteCols(0, num);
    maxRows = 0;
#endif


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



void ctlSQLResult::DisplayData(bool single)
{
    if (!thread || !thread->DataValid())
        return;

    if (thread->ReturnCode() != PGRES_TUPLES_OK)
		return;

	rowcountSuppressed = single;
    Freeze();
	SetItemCount(NumRows());

	if (single)
    {
        int w, h;
        if (colSizes.GetCount() == 1)
            w = colSizes.Item(0);
        else
            GetSize(&w, &h);

        colNames.Add(thread->DataSet()->ColName(0));
        colTypes.Add(wxT(""));
        colTypClasses.Add(0L);


        InsertColumn(0, thread->DataSet()->ColName(0), wxLIST_FORMAT_LEFT, w);
    }
	else
    {
        wxString colName, colType;
        colTypes.Add(wxT(""));
        colTypClasses.Add(0L);

		wxString rowname=_("Row");
		size_t rowcolsize=NumToStr(NumRows()).Length();
		if (rowname.Length() > rowcolsize)
			rowcolsize = rowname.Length();

        InsertColumn(0, rowname, wxLIST_FORMAT_RIGHT, rowcolsize*8);
        colNames.Add(wxT("Row"));

        size_t hdrIndex=0;
		long col, nCols=thread->DataSet()->NumCols();

        for (col=0 ; col < nCols ; col++)
        {
            colName = thread->DataSet()->ColName(col);
            colType = thread->DataSet()->ColType(col);
            colNames.Add(colName);
            colTypes.Add(colType);
            colTypClasses.Add(thread->DataSet()->ColTypClass(col));

            wxString colHeader=colName +wxT(" (")+ colType +wxT(")");

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

            InsertColumn(col+1, colHeader, wxLIST_FORMAT_LEFT, w);
        }
    }
    Thaw();
}


#if !USE_LISTVIEW

int ctlSQLResult::RetrieveOne()
{
    if (!thread || !thread->DataValid())
        return -1;

    if (thread->ReturnCode() != PGRES_TUPLES_OK)
        return -1;

    if (!rowsRetrieved)
    {
        wxString colName, colType;
        colTypes.Add(wxT(""));
        colTypClasses.Add(0L);

        Freeze();
			
		colName = thread->DataSet()->ColName(0);
		colType = thread->DataSet()->ColType(0);
		colNames.Add(colName);
		colTypes.Add(colType);
		colTypClasses.Add(thread->DataSet()->ColTypClass(0));

	    wxString colHeader = colName + wxT("\n") + colType;
			
        GetTable()->AppendCols(1);

        SetColLabelValue(0, colHeader);

        while (!thread->DataSet()->Eof())
        {
            GetTable()->AppendRows(1);
            GetTable()->SetValue(rowsRetrieved, 0, thread->DataSet()->GetVal(0));
            rowsRetrieved++;
            thread->DataSet()->MoveNext();
        }
		
		Thaw();
		
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
        
        GetTable()->AppendCols(nCols);
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
    if (GetNumberCols() == 0)
	{
        wxString t;
        return t;
    }

    if (col < 0) {
        if (GetNumberCols() > 1)
            return GetExportLine(row);
        else
            return GetCellValue(row, 0);
    }
    else
        return GetCellValue(row, col);
}

#endif


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


wxString ctlSQLResult::OnGetItemText(long item, long col) const
{
    if (thread && thread->DataValid())
	{
		if (!rowcountSuppressed)
		{
			if (col)
				col--;
			else
				return NumToStr(item+1L);
		}
		if (item >= 0)
		{
			thread->DataSet()->Locate(item+1);
			return thread->DataSet()->GetVal(col);
		}
		else
			return thread->DataSet()->ColName(col);
	}
	return wxEmptyString;
}


#if !USE_LISTVIEW


void ctlSQLResult::ResultsFinished()
{
    int rows = GetTable()->GetNumberRows();

    if (rowsRetrieved < rows)
        GetTable()->DeleteRows(rowsRetrieved, rows - rowsRetrieved);
}

void ctlSQLResult::OnGridSelect(wxGridRangeSelectEvent& event)
{
    SetFocus();
}
#endif

