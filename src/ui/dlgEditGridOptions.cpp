//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgEditGridOptions.cpp - Edit Grid Box Options
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>

#ifdef __WX_FULLSOURCE
#include "wx/generic/gridsel.h"
#else
#include "wxgridsel.h"
#endif

#include <wx/generic/gridctrl.h>

// App headers
#include "pgAdmin3.h"
#include "pgDefs.h"
#include "frmMain.h"
#include "menu.h"

#include "frmEditGrid.h"
#include "dlgEditGridOptions.h"
#include "pgTable.h"
#include "pgView.h"

// Icons
#include "images/sortfilter.xpm"

#define nbOptions                   CTRL("nbOptions", wxNotebook)
#define btnAsc						CTRL("btnAsc", wxButton)
#define btnDesc						CTRL("btnDesc", wxButton)
#define btnRemove		            CTRL("btnRemove", wxButton)
#define btnOK					    CTRL("btnOK", wxRadioBox)
#define btnCancel					CTRL("btnCancel", wxTextCtrl)
#define cboColumns                  CTRL("cboColumns", wxComboBox)
#define lstSortCols                 CTRL("lstSortCols", wxListCtrl)

BEGIN_EVENT_TABLE(dlgEditGridOptions, wxDialog)
    EVT_BUTTON               (XRCID("btnOK"),       dlgEditGridOptions::OnOK)
    EVT_BUTTON               (XRCID("btnCancel"),   dlgEditGridOptions::OnCancel)
	EVT_BUTTON               (XRCID("btnRemove"),   dlgEditGridOptions::OnRemove)
	EVT_BUTTON               (XRCID("btnAsc"),      dlgEditGridOptions::OnAsc)
	EVT_BUTTON               (XRCID("btnDesc"),     dlgEditGridOptions::OnDesc)
	EVT_BUTTON               (XRCID("btnValidate"), dlgEditGridOptions::OnValidate)
	EVT_COMBOBOX             (XRCID("cboColumns"),  dlgEditGridOptions::OnCboColumnsChange) 
	EVT_LIST_ITEM_SELECTED   (XRCID("lstSortCols"), dlgEditGridOptions::OnLstSortColsChange) 
	EVT_LIST_ITEM_DESELECTED (XRCID("lstSortCols"), dlgEditGridOptions::OnLstSortColsChange) 
END_EVENT_TABLE()

dlgEditGridOptions::dlgEditGridOptions(frmEditGrid *win, pgConn *conn, const wxString &rel, ctlSQLGrid *grid)
{
    wxLogInfo(wxT("Creating an edit grid options dialogue"));
    editGrid=grid;
	connection=conn;
	relation=rel;
	parent=win;
    wxWindowBase::SetFont(settings->GetSystemFont());
    wxXmlResource::Get()->LoadDialog(this, parent, wxT("dlgEditGridOptions")); 
	conv = &wxConvLibc;

    // Icon
    SetIcon(wxIcon(sortfilter_xpm));
    CenterOnParent();

	int cols = grid->GetNumberCols();
	long x;

	for (x = 0; x < cols; x++)
		cboColumns->Append(grid->GetColLabelValue(x).BeforeFirst('\n'));

	// Setup the buttons
	wxCommandEvent nullEvent;
	OnCboColumnsChange(nullEvent);
    OnLstSortColsChange(nullEvent);

	// Setup the list box
    int leftSize = 140, rightSize;
    leftSize = ConvertDialogToPixels(wxPoint(leftSize, 0)).x;
    rightSize = lstSortCols->GetClientSize().GetWidth()-leftSize;
	lstSortCols->InsertColumn(0, _("Column name"), wxLIST_FORMAT_LEFT, leftSize);
	lstSortCols->InsertColumn(1, _("Sort order"), wxLIST_FORMAT_LEFT, rightSize);

	// Setup the filter SQL box. This is an XRC 'unknown' control so must
	// be manually created and attache to the XRC global resource.
	filter = new ctlSQLBox(this);
	wxXmlResource::Get()->AttachUnknownControl(wxT("sqlFilter"), filter);
	filter->SetText(parent->GetFilter());

	// Get the current sort columns, and populate the listbox.
	// The current columns will be parsed char by char to allow us
	// to cope with quoted column names with commas in them (let's hope 
	// noone ever does that, but sod's law etc....)
	bool inColumn = true, inQuote = false;
	wxString sortCols = parent->GetSortCols();
	wxString col, dir;
	size_t pos, len = sortCols.Length();
	int itm = 0;

	for (pos = 0; pos < len; pos++) {
		if (inColumn) {
            if (sortCols.GetChar(pos) == '"') inQuote = !inQuote;
			if (!inQuote && sortCols.GetChar(pos) == ' ')
			    inColumn = false;
			else
				if (sortCols.GetChar(pos) != '"') col += sortCols.GetChar(pos);
		} else {
			if (sortCols.GetChar(pos) == ',') {
			    inColumn = true;
			    lstSortCols->InsertItem(itm, col);
				if (dir.GetChar(0) == 'A') {
	                lstSortCols->SetItem(itm, 1, _("Ascending"));
	                lstSortCols->SetItemData(itm, 0); 
				} else {
	                lstSortCols->SetItem(itm, 1, _("Descending"));
	                lstSortCols->SetItemData(itm, 1); 
				}
				col = wxT("");
				dir = wxT("");
				++pos;
				++itm;
            } else {
				dir += sortCols.GetChar(pos);
			}
        }
	}

	// Insert the last column
	if (col.Length() > 0) {
		lstSortCols->InsertItem(itm, col);
		if (dir.GetChar(0) == 'A') {
		    lstSortCols->SetItem(itm, 1, _("Ascending"));
			lstSortCols->SetItemData(itm, 0); 
		} else {
		    lstSortCols->SetItem(itm, 1, _("Descending"));
			lstSortCols->SetItemData(itm, 1); 
		}
	}

	// Finally (phew!) remove all columns we're already sorting on from the list.
	long count = lstSortCols->GetItemCount();

    for (x = 0; x < count; x++)
        cboColumns->Delete(cboColumns->FindString(lstSortCols->GetItemText(x)));
}

void dlgEditGridOptions::OnRemove(wxCommandEvent &ev)
{
    long itm = -1;
    itm = lstSortCols->GetNextItem(itm, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    cboColumns->Append(lstSortCols->GetItemText(itm));
	lstSortCols->DeleteItem(itm);
}

void dlgEditGridOptions::OnAsc(wxCommandEvent &ev)
{
	long itm = lstSortCols->GetItemCount();
	lstSortCols->InsertItem(itm, cboColumns->GetValue());
	lstSortCols->SetItem(itm, 1, _("Ascending"));
	lstSortCols->SetItemData(itm, 0);
	cboColumns->Delete(cboColumns->GetSelection());

	// Setup the buttons
	wxCommandEvent nullEvent;
	OnCboColumnsChange(nullEvent);
    OnLstSortColsChange(nullEvent);
}

void dlgEditGridOptions::OnDesc(wxCommandEvent &ev)
{
	long itm = lstSortCols->GetItemCount();
	lstSortCols->InsertItem(itm, cboColumns->GetValue());
	lstSortCols->SetItem(itm, 1, _("Descending"));
	lstSortCols->SetItemData(itm, 1);
	cboColumns->Delete(cboColumns->GetSelection());

	// Setup the buttons
	wxCommandEvent nullEvent;
	OnCboColumnsChange(nullEvent);
    OnLstSortColsChange(nullEvent);
}

void dlgEditGridOptions::OnValidate(wxCommandEvent &ev)
{
    if (Validate()) 
		wxMessageBox(_("Filter string syntax validates OK!"), _("Syntax Validation"), wxICON_INFORMATION);
}

void dlgEditGridOptions::OnCboColumnsChange(wxCommandEvent &ev)
{
	// Set the command buttons appropriately
	if (cboColumns->GetSelection() == wxNOT_FOUND) {
		btnAsc->Enable(false);
		btnDesc->Enable(false);
	} else {
		btnAsc->Enable(true);
		btnDesc->Enable(true);
	}
}

void dlgEditGridOptions::OnLstSortColsChange(wxCommandEvent &ev)
{
	// Set the command buttons appropriately
	if (lstSortCols->GetSelectedItemCount() == 0)
		btnRemove->Enable(false);
	else
		btnRemove->Enable(true);
}

void dlgEditGridOptions::OnCancel(wxCommandEvent &ev)
{
    Destroy();
}

void dlgEditGridOptions::OnOK(wxCommandEvent &ev)
{
	// Check the filter syntax
    if (!Validate()) return;

	wxString sortCols;
	long x, count = lstSortCols->GetItemCount();

    for (x = 0; x < count; x++) {
		sortCols += qtIdent(lstSortCols->GetItemText(x));
		if (lstSortCols->GetItemData(x) == 0)
		    sortCols += wxT(" ASC");
		else
			sortCols += wxT(" DESC");
		sortCols += wxT(", ");
	}

	if (sortCols.Length() > 2) {
		sortCols.RemoveLast();
		sortCols.RemoveLast();
	}

	parent->SetSortCols(sortCols);
	parent->SetFilter(filter->GetText().Trim());
    Destroy();
}

bool dlgEditGridOptions::Validate()
{
	StartMsg(_("Validating filter string,,,"));
	filter->MarkerDeleteAll(0);
	if (!filter->GetText().Trim().Length()) {
		EndMsg();
		return false;
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
		EndMsg();
		return true;
    }

	// Figure out where the error is
    wxString errMsg = wxString(PQerrorMessage(connection->connection()), *conv).c_str();

    wxString atChar=wxT(" at character ");
    int chp=errMsg.Find(atChar);

    if (chp > 0)
    {
        int selStart=filter->GetSelectionStart(), selEnd=filter->GetSelectionEnd();
        if (selStart == selEnd)
            selStart=0;

        long errPos=0;
        errMsg.Mid(chp+atChar.Length()).ToLong(&errPos);
        errPos -= queryOffset;  // do not count EXPLAIN or similar
		wxLogError(wxString::Format(_("ERROR: Syntax error at character %d!"), errPos));

        int line=0, maxLine = filter->GetLineCount();
        while (line < maxLine && filter->GetLineEndPosition(line) < errPos + selStart+1)
            line++;
        if (line < maxLine)
        {
            filter->MarkerAdd(line, 0);
            filter->EnsureVisible(line);
        }
    } else {
		wxLogError(errMsg);
	}

	// Cleanup
    PQclear(qryRes);
	EndMsg();
	return false;
}
