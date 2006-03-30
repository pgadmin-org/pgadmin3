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
: ctlSQLGrid(parent, id, pos, size)
{
    conn=_conn;
    thread=0;
    CreateGrid(0, 0);
    EnableEditing(false);
    SetSizer(new wxBoxSizer(wxVERTICAL));

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
			SetCellOverflow(rowsRetrieved, 0, false);
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
			SetCellOverflow(rowsRetrieved, col, false);
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


void ctlSQLResult::SetMaxRows(int rows)
{
    maxRows = rows;
}

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
