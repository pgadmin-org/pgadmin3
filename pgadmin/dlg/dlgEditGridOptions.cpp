//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgEditGridOptions.cpp - Edit Grid Box Options
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/settings.h>

#include <wx/generic/gridctrl.h>

// App headers
#include "pgAdmin3.h"
#include "utils/pgDefs.h"
#include "frm/frmMain.h"

#include "frm/frmEditGrid.h"
#include "dlg/dlgEditGridOptions.h"
#include "schema/pgTable.h"
#include "schema/pgView.h"

// Icons
#include "images/sortfilter.pngc"

#define nbOptions                   CTRL_NOTEBOOK("nbOptions")
#define btnAsc                      CTRL_BUTTON("btnAsc")
#define btnDesc                     CTRL_BUTTON("btnDesc")
#define btnRemove                   CTRL_BUTTON("wxID_REMOVE")
#define btnValidate                 CTRL_BUTTON("btnValidate")
#define cboColumns                  CTRL_COMBOBOX("cboColumns")
#define lstSortCols                 CTRL_LISTVIEW("lstSortCols")
#define pnlSort                     CTRL_PANEL("pnlSort")
#define pnlFilter                   CTRL_PANEL("pnlFilter")
#define filter                      CTRL_SQLBOX("sqlFilter")

BEGIN_EVENT_TABLE(dlgEditGridOptions, pgDialog)
	EVT_CLOSE(                                      dlgEditGridOptions::OnClose)
	EVT_BUTTON               (wxID_OK,              dlgEditGridOptions::OnOK)
	EVT_BUTTON               (wxID_CANCEL,          dlgEditGridOptions::OnCancel)
	EVT_BUTTON               (wxID_REMOVE,          dlgEditGridOptions::OnRemove)
	EVT_BUTTON               (XRCID("btnAsc"),      dlgEditGridOptions::OnAsc)
	EVT_BUTTON               (XRCID("btnDesc"),     dlgEditGridOptions::OnDesc)
	EVT_BUTTON               (XRCID("btnValidate"), dlgEditGridOptions::OnValidate)
	EVT_COMBOBOX             (XRCID("cboColumns"),  dlgEditGridOptions::OnCboColumnsChange)
	EVT_LIST_ITEM_SELECTED   (XRCID("lstSortCols"), dlgEditGridOptions::OnLstSortColsChange)
	EVT_LIST_ITEM_DESELECTED (XRCID("lstSortCols"), dlgEditGridOptions::OnLstSortColsChange)
	EVT_STC_MODIFIED		 (XRCID("sqlFilter"),   dlgEditGridOptions::OnFilterChange)
#ifdef __WXMAC__
	EVT_SIZE(                                       dlgEditGridOptions::OnChangeSize)
#endif
END_EVENT_TABLE()

dlgEditGridOptions::dlgEditGridOptions(frmEditGrid *win, pgConn *conn, const wxString &rel, ctlSQLEditGrid *grid)
{
	editGrid = grid;
	connection = conn;
	relation = rel;
	parent = win;
	SetFont(settings->GetSystemFont());
	LoadResource(win, wxT("dlgEditGridOptions"));
	conv = conn->GetConv();

	// Icon
	SetIcon(*sortfilter_png_ico);
	RestorePosition();

	int cols = grid->GetNumberCols();
	long x;

	for (x = 0; x < cols; x++)
		cboColumns->Append(grid->GetColLabelValue(x).BeforeFirst('\n'));

	// Setup the buttons
	wxCommandEvent nullEvent;
	OnCboColumnsChange(nullEvent);
	wxListEvent nullLstEvent;
	OnLstSortColsChange(nullLstEvent);

	// Setup the list box
	int leftSize = 140, rightSize;
	leftSize = ConvertDialogToPixels(wxPoint(leftSize, 0)).x;
	rightSize = lstSortCols->GetClientSize().GetWidth() - leftSize;
	// This check is to work around a bug in wxGTK that doesn't set
	// appropriately the GetClientSize().
	// Without this workaround, we have an invisible second column.
	if (rightSize < leftSize)
		rightSize = leftSize + 1;
	lstSortCols->InsertColumn(0, _("Column name"), wxLIST_FORMAT_LEFT, leftSize);
	lstSortCols->InsertColumn(1, _("Sort order"), wxLIST_FORMAT_LEFT, rightSize);

	// Setup the filter SQL box. This is an XRC 'unknown' control so must
	// be manually created and attache to the XRC global resource.
	filter->SetText(parent->GetFilter());

	// Get the current sort columns, and populate the listbox.
	// The current columns will be parsed char by char to allow us
	// to cope with quoted column names with commas in them (let's hope
	// no one ever does that, but sod's law etc....)
	bool inColumn = true, inQuote = false;
	wxString sortCols = parent->GetSortCols();
	wxString col, dir;
	size_t pos, len = sortCols.Length();
	int itm = 0;

	for (pos = 0; pos < len; pos++)
	{
		if (inColumn)
		{
			if (sortCols.GetChar(pos) == '"') inQuote = !inQuote;
			if (!inQuote && (sortCols.GetChar(pos) == ' ' || sortCols.GetChar(pos) == ','))
				inColumn = false;
			else if (sortCols.GetChar(pos) != '"') col += sortCols.GetChar(pos);
		}
		else
		{
			if (sortCols.GetChar(pos - 1) == ',')
			{
				inColumn = true;
				lstSortCols->InsertItem(itm, col);
				if (dir.GetChar(0) == 'D')
				{
					lstSortCols->SetItem(itm, 1, _("Descending"));
					lstSortCols->SetItemData(itm, 0);
				}
				else
				{
					lstSortCols->SetItem(itm, 1, _("Ascending"));
					lstSortCols->SetItemData(itm, 1);
				}
				col = wxT("");
				dir = wxT("");
				++itm;
			}
			else
			{
				dir += sortCols.GetChar(pos);
			}
		}
	}

	// Insert the last column
	if (col.Length() > 0)
	{
		lstSortCols->InsertItem(itm, col);
		if (dir.GetChar(0) == 'D')
		{
			lstSortCols->SetItem(itm, 1, _("Descending"));
			lstSortCols->SetItemData(itm, 0);
		}
		else
		{
			lstSortCols->SetItem(itm, 1, _("Ascending"));
			lstSortCols->SetItemData(itm, 1);
		}
	}

	// Finally (phew!) remove all columns we're already sorting on from the list.
	long count = lstSortCols->GetItemCount();

	for (x = 0; x < count; x++)
	{
		int idx = cboColumns->FindString(lstSortCols->GetItemText(x));
		if (idx >= 0)
			cboColumns->Delete(idx);
	}

	// Display the appropriate tab. If the EditGrid is not shown, we must be
	// doing a View Filtered Data.
	if (!parent->IsShown())
		nbOptions->DeletePage(0);

	btnValidate->Disable();
	filter->SetFocus();
}

dlgEditGridOptions::~dlgEditGridOptions()
{
	SavePosition();
}

// Enable/disable the validation button
void dlgEditGridOptions::OnFilterChange(wxStyledTextEvent &ev)
{
	btnValidate->Enable(!filter->GetText().Trim().IsEmpty());
}

void dlgEditGridOptions::OnRemove(wxCommandEvent &ev)
{
	long itm = -1;
	itm = lstSortCols->GetNextItem(itm, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	cboColumns->Append(lstSortCols->GetItemText(itm));
	lstSortCols->DeleteItem(itm);
	if (lstSortCols->GetItemCount() > 0)
	{
		if (lstSortCols->GetItemCount() < itm + 1)
			lstSortCols->SetItemState(lstSortCols->GetItemCount() - 1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
		else
			lstSortCols->SetItemState(itm, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
	}
	wxListEvent nullLstEvent;
	OnLstSortColsChange(nullLstEvent);
}


void dlgEditGridOptions::OnAsc(wxCommandEvent &ev)
{
	long itm = lstSortCols->GetItemCount();
	lstSortCols->InsertItem(itm, cboColumns->GetValue());
	lstSortCols->SetItem(itm, 1, _("Ascending"));
	lstSortCols->SetItemData(itm, 1);
	cboColumns->Delete(cboColumns->GetCurrentSelection());

	// Setup the buttons
	OnCboColumnsChange(ev);
	wxListEvent nullLstEvent;
	OnLstSortColsChange(nullLstEvent);
}

void dlgEditGridOptions::OnDesc(wxCommandEvent &ev)
{
	long itm = lstSortCols->GetItemCount();
	lstSortCols->InsertItem(itm, cboColumns->GetValue());
	lstSortCols->SetItem(itm, 1, _("Descending"));
	lstSortCols->SetItemData(itm, 0);
	cboColumns->Delete(cboColumns->GetCurrentSelection());

	// Setup the buttons
	OnCboColumnsChange(ev);
	wxListEvent nullLstEvent;
	OnLstSortColsChange(nullLstEvent);
}

#ifdef __WXMAC__
void dlgEditGridOptions::OnChangeSize(wxSizeEvent &ev)
{
	if (lstSortCols)
		lstSortCols->SetSize(wxDefaultCoord, wxDefaultCoord,
		                     ev.GetSize().GetWidth(), ev.GetSize().GetHeight() - 350);
	if (GetAutoLayout())
	{
		Layout();
	}
}
#endif

void dlgEditGridOptions::OnValidate(wxCommandEvent &ev)
{
	if (Validate())
		wxMessageBox(_("Filter string syntax validates OK!"), _("Syntax Validation"), wxICON_INFORMATION | wxOK);
}

void dlgEditGridOptions::OnCboColumnsChange(wxCommandEvent &ev)
{
	// Set the command buttons appropriately
	if (cboColumns->GetCurrentSelection() == wxNOT_FOUND)
	{
		btnAsc->Enable(false);
		btnDesc->Enable(false);
	}
	else
	{
		btnAsc->Enable(true);
		btnDesc->Enable(true);
	}
}

void dlgEditGridOptions::OnLstSortColsChange(wxListEvent &ev)
{
	// Set the command buttons appropriately
	if (lstSortCols->GetSelectedItemCount() == 0)
		btnRemove->Enable(false);
	else
		btnRemove->Enable(true);
}

void dlgEditGridOptions::OnCancel(wxCommandEvent &ev)
{
	EndModal(false);
}


void dlgEditGridOptions::OnClose(wxCloseEvent &ev)
{
	EndModal(false);
}

void dlgEditGridOptions::OnOK(wxCommandEvent &ev)
{
#ifdef __WXGTK__
	if (!btnOK->IsEnabled())
		return;
#endif
	// Check the filter syntax
	if (!Validate()) return;

	if (nbOptions->GetPageCount() > 1)
	{
		wxString sortCols;
		long x, count = lstSortCols->GetItemCount();

		for (x = 0; x < count; x++)
		{
			sortCols += qtIdent(lstSortCols->GetItemText(x));
			if (lstSortCols->GetItemData(x) == 0)
				sortCols += wxT(" DESC");
			else
				sortCols += wxT(" ASC");
			sortCols += wxT(", ");
		}

		if (sortCols.Length() > 2)
		{
			sortCols.RemoveLast();
			sortCols.RemoveLast();
		}

		parent->SetSortCols(sortCols);
	}

	parent->SetFilter(filter->GetText().Trim());
	EndModal(true);
}

bool dlgEditGridOptions::Validate()
{
	winMain->StartMsg(_("Validating filter string"));
	filter->MarkerDeleteAll(0);
	if (!filter->GetText().Trim().Length())
	{
		winMain->EndMsg();
		return true;
	}

	wxString sql = wxT("EXPLAIN SELECT * FROM ") + relation + wxT(" WHERE ");
	int queryOffset = sql.Length();
	sql += filter->GetText();

	PGresult *qryRes;
	qryRes = PQexec(connection->connection(), sql.mb_str(*conv));
	int res = PQresultStatus(qryRes);

	// Check for errors
	if (res == PGRES_TUPLES_OK ||
	        res == PGRES_COMMAND_OK)
	{
		// No errors, all OK!
		winMain->EndMsg();
		return true;
	}

	// Figure out where the error is
	wxString errMsg = connection->GetLastError();

	wxString atChar = wxT(" at character ");
	int chp = errMsg.Find(atChar);

	if (chp > 0)
	{
		int selStart = filter->GetSelectionStart(), selEnd = filter->GetSelectionEnd();
		if (selStart == selEnd)
			selStart = 0;

		long errPos = 0;
		errMsg.Mid(chp + atChar.Length()).ToLong(&errPos);
		errPos -= queryOffset;  // do not count EXPLAIN or similar
		wxLogError(wxT("%s"), _("ERROR: Syntax error at character %d!"), errPos);

		int line = 0, maxLine = filter->GetLineCount();
		while (line < maxLine && filter->GetLineEndPosition(line) < errPos + selStart + 1)
			line++;
		if (line < maxLine)
		{
			filter->MarkerAdd(line, 0);
			filter->EnsureVisible(line);
		}
	}
	else
		wxLogError(wxT("%s"), errMsg.BeforeFirst('\n').c_str());

	// Cleanup
	PQclear(qryRes);
	winMain->EndMsg();
	return false;
}
