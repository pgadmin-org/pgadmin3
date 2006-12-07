//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// ctlSQLResult.cpp - SQL Query result window
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/clipbrd.h>

#include "schema/pgConn.h"
#include "pgQueryThread.h"
#include "ctl/ctlSQLResult.h"
#include "utils/sysSettings.h"
#include "frm/frmExport.h"



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
    SetTable(new sqlResultTable(), true);

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
    colSizes.Empty();
    colHeaders.Empty();

#if USE_LISTVIEW
    ClearAll();
#else
    wxGridTableMessage *msg;
    sqlResultTable *table = (sqlResultTable *)GetTable();
    msg = new wxGridTableMessage(table, wxGRIDTABLE_NOTIFY_ROWS_DELETED, 0, GetNumberRows());
    ProcessTableMessage(*msg);
    delete msg;
    msg = new wxGridTableMessage(table, wxGRIDTABLE_NOTIFY_COLS_DELETED, 0, GetNumberCols());
    ProcessTableMessage(*msg);
    delete msg;
#endif

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

#if !USE_LISTVIEW
    ((sqlResultTable *)GetTable())->SetThread(thread);
#endif
    thread->Run();
    return RunStatus();
}


int ctlSQLResult::Abort()
{
    if (thread)
    {
#if !USE_LISTVIEW
        ((sqlResultTable *)GetTable())->SetThread(0);
#endif
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

#if USE_LISTVIEW
    SetItemCount(NumRows());
#else
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
#endif

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
#if USE_LISTVIEW
        InsertColumn(0, thread->DataSet()->ColName(0), wxLIST_FORMAT_LEFT, w);
#else
        SetColSize(0, w);
#endif
    }
	else
    {
        wxString colName, colType;
#if USE_LISTVIEW
        colTypes.Add(wxT(""));
        colTypClasses.Add(0L);

		wxString rowname=_("Row");
    	size_t rowcolsize=NumToStr(NumRows()).Length();
	    if (rowname.Length() > rowcolsize)
			rowcolsize = rowname.Length();

        InsertColumn(0, rowname, wxLIST_FORMAT_RIGHT, rowcolsize*8);
        colNames.Add(wxT("Row"));
#endif
        size_t hdrIndex=0;
		long col, nCols=thread->DataSet()->NumCols();

        for (col=0 ; col < nCols ; col++)
        {
            colName = thread->DataSet()->ColName(col);
            colType = thread->DataSet()->ColFullType(col);
            colNames.Add(colName);
            colTypes.Add(colType);
            colTypClasses.Add(thread->DataSet()->ColTypClass(col));

#if USE_LISTVIEW
            wxString colHeader = colName;
#else
            wxString colHeader = colName + wxT("\n") + colType;
#endif
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

#if USE_LISTVIEW
            InsertColumn(col+1, colHeader, wxLIST_FORMAT_LEFT, w);
#else
            SetColSize(col, w);
#endif
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

#if !USE_LISTVIEW
void ctlSQLResult::OnGridSelect(wxGridRangeSelectEvent& event)
{
    SetFocus();
}
#endif

#if !USE_LISTVIEW
wxString sqlResultTable::GetValue(int row, int col)
{
    if (thread && thread->DataValid())
	{
		if (col >= 0)
		{
			thread->DataSet()->Locate(row+1);
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

#endif
