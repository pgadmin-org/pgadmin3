//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
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



ctlSQLResult::ctlSQLResult(wxWindow *parent, pgConn *_conn, wxWindowID id, const wxPoint &pos, const wxSize &size)
	: ctlSQLGrid(parent, id, pos, size)
{
	conn = _conn;
	thread = NULL;

	SetTable(new sqlResultTable(), true);

	EnableEditing(false);
	SetSizer(new wxBoxSizer(wxVERTICAL));

	Connect(wxID_ANY, wxEVT_GRID_RANGE_SELECT, wxGridRangeSelectEventHandler(ctlSQLResult::OnGridSelect));
}



ctlSQLResult::~ctlSQLResult()
{
	Abort();

	if (thread)
	{
		if (thread->IsRunning())
		{
			thread->CancelExecution();
			thread->Wait();
		}

		delete thread;
		thread = NULL;
	}
}


void ctlSQLResult::SetConnection(pgConn *_conn)
{
	conn = _conn;
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

bool ctlSQLResult::ToFile(frmExport *frm)
{
	if (NumRows() > 0)
	{
		return frm->Export(thread->DataSet());
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


int ctlSQLResult::Execute(const wxString &query, int resultToRetrieve, wxWindow *caller, long eventId, void *data)
{
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

	thread = new pgQueryThread(conn, query, resultToRetrieve, caller, eventId, data);

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

		if (thread->IsRunning())
		{
			thread->CancelExecution();
			thread->Wait();
		}

		delete thread;
		thread = NULL;
	}
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
	 * Resize and repopulate by informing it to delete all the rows and
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
		colNames.Add(thread->DataSet()->ColName(0));
		colTypes.Add(wxT(""));
		colTypClasses.Add(0L);

		AutoSizeColumn(0, false, false);
	}
	else
	{
		long col, nCols = thread->DataSet()->NumCols();

		AutoSizeColumns(false);

		for (col = 0 ; col < nCols ; col++)
		{
			colNames.Add(thread->DataSet()->ColName(col));
			colTypes.Add(thread->DataSet()->ColFullType(col));
			colTypClasses.Add(thread->DataSet()->ColTypClass(col));

			if (thread->DataSet()->ColTypClass(col) == PGTYPCLASS_NUMERIC)
			{
				/*
				 * For numeric columns, set alignment to right.
				 */
				wxGridCellAttr *attr = new wxGridCellAttr();
				attr->SetAlignment(wxALIGN_RIGHT, wxALIGN_TOP);
				SetColAttr(col, attr);
			}
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
	if (thread)
		return thread->GetResultError();

	pgError dummy;
	memset(&dummy, 0, sizeof(dummy));
	return dummy;
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
	return (OID) - 1;
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
				return NumToStr(item + 1L);
		}
		if (item >= 0)
		{
			thread->DataSet()->Locate(item + 1);
			return thread->DataSet()->GetVal(col);
		}
		else
			return thread->DataSet()->ColName(col);
	}
	return wxEmptyString;
}

void ctlSQLResult::OnGridSelect(wxGridRangeSelectEvent &event)
{
	SetFocus();
}

wxString sqlResultTable::GetValue(int row, int col)
{
	if (thread && thread->DataValid())
	{
		if (col >= 0)
		{
			thread->DataSet()->Locate(row + 1);
			if (settings->GetIndicateNull() && thread->DataSet()->IsNull(col))
				return wxT("<NULL>");
			else
			{

				wxString decimalMark = wxT(".");
				wxString s = thread->DataSet()->GetVal(col);

				if(thread->DataSet()->ColTypClass(col) == PGTYPCLASS_NUMERIC &&
				        settings->GetDecimalMark().Length() > 0)
				{
					decimalMark = settings->GetDecimalMark();
					s.Replace(wxT("."), decimalMark);

				}
				if (thread->DataSet()->ColTypClass(col) == PGTYPCLASS_NUMERIC &&
				        settings->GetThousandsSeparator().Length() > 0)
				{
					/* Add thousands separator */
					size_t pos = s.find(decimalMark);
					if (pos == wxString::npos)
						pos = s.length();
					while (pos > 3)
					{
						pos -= 3;
						if (pos > 1 || !s.StartsWith(wxT("-")))
							s.insert(pos, settings->GetThousandsSeparator());
					}
					return s;
				}
				else
				{
					wxString data = thread->DataSet()->GetVal(col);

					if (data.Length() > (size_t)settings->GetMaxColSize())
						return thread->DataSet()->GetVal(col).Left(settings->GetMaxColSize()) + wxT(" (...)");
					else
						return thread->DataSet()->GetVal(col);
				}
			}
		}
		else
			return thread->DataSet()->ColName(col);
	}
	return wxEmptyString;
}

sqlResultTable::sqlResultTable()
{
	thread = NULL;
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

