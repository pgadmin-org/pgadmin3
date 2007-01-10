//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// ctlSQLResult.cpp - SQL Query result window
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/clipbrd.h>

#include "db/pgConn.h"
#include "db/pgQueryThread.h"
#include "ctl/ctlSQLResult.h"
#include "utils/sysSettings.h"
#include "frm/frmExport.h"



ctlSQLResult::ctlSQLResult(wxWindow *parent, pgConn *_conn, wxWindowID id, const wxPoint& pos, const wxSize& size)
: ctlSQLGrid(parent, id, pos, size)
{
    conn=_conn;
    thread=0;

    SetTable(new sqlResultTable(), true);

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
    if (NumRows() > 0)
    {
        frmExport dlg(this);
        if (dlg.ShowModal() == wxID_OK)
            return dlg.Export(NULL);
    }
    return false;
}

bool ctlSQLResult::ToFile()
{
    if (NumRows() > 0)
    {
        frmExport dlg(this);
        if (dlg.ShowModal() == wxID_OK)
            return dlg.Export(thread->DataSet());
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
    colSizes.Empty();
    colHeaders.Empty();

    wxGridTableMessage *msg;
    sqlResultTable *table = (sqlResultTable *)GetTable();
    msg = new wxGridTableMessage(table, wxGRIDTABLE_NOTIFY_ROWS_DELETED, 0, GetNumberRows());
    ProcessTableMessage(*msg);
    delete msg;
    msg = new wxGridTableMessage(table, wxGRIDTABLE_NOTIFY_COLS_DELETED, 0, GetNumberCols());
    ProcessTableMessage(*msg);
    delete msg;

    Abort();

    colNames.Empty();
    colTypes.Empty();
    colTypClasses.Empty();

    thread = new pgQueryThread(conn, query, resultToRetrieve);

    if (thread->Create() != wxTHREAD_NO_ERROR)
    {
        Abort();
        return -1;
    }

    ((sqlResultTable *)GetTable())->SetThread(thread);

    thread->Run();
    return RunStatus();
}


int ctlSQLResult::Abort()
{
    if (thread)
    {
        ((sqlResultTable *)GetTable())->SetThread(0);

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

    /*
     * Resize and repopulate by informing itto delete all the rows and
     * columns, then append the correct number of them. Probably is a
     * better way to do this.
     */
    wxGridTableMessage *msg;
    sqlResultTable *table = (sqlResultTable *)GetTable();
    msg = new wxGridTableMessage(table, wxGRIDTABLE_NOTIFY_ROWS_DELETED, 0, GetNumberRows());
    ProcessTableMessage(*msg);
    delete msg;
    msg = new wxGridTableMessage(table, wxGRIDTABLE_NOTIFY_COLS_DELETED, 0, GetNumberCols());
    ProcessTableMessage(*msg);
    delete msg;
    msg = new wxGridTableMessage(table, wxGRIDTABLE_NOTIFY_ROWS_APPENDED, NumRows());
    ProcessTableMessage(*msg);
    delete msg;
    msg = new wxGridTableMessage(table, wxGRIDTABLE_NOTIFY_COLS_APPENDED, thread->DataSet()->NumCols());
    ProcessTableMessage(*msg);
    delete msg;

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

        SetColSize(0, w);
    }
	else
    {
        wxString colName, colType;

        size_t hdrIndex=0;
		long col, nCols=thread->DataSet()->NumCols();

        for (col=0 ; col < nCols ; col++)
        {
            colName = thread->DataSet()->ColName(col);
            colType = thread->DataSet()->ColFullType(col);
            colNames.Add(colName);
            colTypes.Add(colType);
            colTypClasses.Add(thread->DataSet()->ColTypClass(col));

            wxString colHeader = colName + wxT("\n") + colType;

            int w;
            if (hdrIndex < colHeaders.GetCount() && colHeaders.Item(hdrIndex) == colHeader) {
                w = colSizes.Item(hdrIndex++);
            }
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

            SetColSize(col, w);
        }
    }
    Thaw();
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

pgError ctlSQLResult::GetResultError()
{
    return conn->GetLastResultError();
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

void ctlSQLResult::OnGridSelect(wxGridRangeSelectEvent& event)
{
    SetFocus();
}

wxString sqlResultTable::GetValue(int row, int col)
{
    if (thread && thread->DataValid())
	{
		if (col >= 0)
		{
			thread->DataSet()->Locate(row+1);
            if (settings->GetIndicateNull() && thread->DataSet()->IsNull(col))
                return wxT("<NULL>");
            else
			    return thread->DataSet()->GetVal(col);
		}
		else
			return thread->DataSet()->ColName(col);
	}
	return wxEmptyString;
}

sqlResultTable::sqlResultTable()
{
    thread = 0;
}

int sqlResultTable::GetNumberRows()
{
    if (thread && thread->DataValid())
        return thread->DataSet()->NumRows();
    return 0;
}


wxString sqlResultTable::GetColLabelValue(int col)
{
    if (thread && thread->DataValid())
        return thread->DataSet()->ColName(col) + wxT("\n") +
            thread->DataSet()->ColFullType(col);
    return wxEmptyString;
}

int sqlResultTable::GetNumberCols()
{
    if (thread && thread->DataValid())
        return thread->DataSet()->NumCols();
    return 0;
}

