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
#include "images/viewdata.xpm"

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
	EVT_BUTTON               (XRCID("btnAsc"),   dlgEditGridOptions::OnAsc)
	EVT_BUTTON               (XRCID("btnDesc"),  dlgEditGridOptions::OnDesc)
	EVT_COMBOBOX             (XRCID("cboColumns"),  dlgEditGridOptions::OnCboColumnsChange) 
	EVT_LIST_ITEM_SELECTED   (XRCID("lstSortCols"), dlgEditGridOptions::OnLstSortColsChange) 
	EVT_LIST_ITEM_DESELECTED (XRCID("lstSortCols"), dlgEditGridOptions::OnLstSortColsChange) 
END_EVENT_TABLE()

dlgEditGridOptions::dlgEditGridOptions(frmEditGrid *win, ctlSQLGrid *grid)
{
    wxLogInfo(wxT("Creating an edit grid options dialogue"));
    editGrid=grid;
	parent=win;
    wxWindowBase::SetFont(settings->GetSystemFont());
    wxXmlResource::Get()->LoadDialog(this, parent, wxT("dlgEditGridOptions")); 

    // Icon
    SetIcon(wxIcon(viewdata_xpm));
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
	lstSortCols->InsertColumn(0, __("Column name"), wxLIST_FORMAT_LEFT, 175);
	lstSortCols->InsertColumn(1, __("Sort order"), wxLIST_FORMAT_LEFT, 100);

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
            if (sortCols[pos] == '"') inQuote = !inQuote;
			if (!inQuote && sortCols[pos] == ' ')
			    inColumn = false;
			else
				if (sortCols[pos] != '"') col += sortCols[pos];
		} else {
			if (sortCols[pos] == ',') {
			    inColumn = true;
			    lstSortCols->InsertItem(itm, col);
				if (dir[0] == 'A') {
	                lstSortCols->SetItem(itm, 1, wxT("Ascending"));
	                lstSortCols->SetItemData(itm, 0); 
				} else {
	                lstSortCols->SetItem(itm, 1, wxT("Descending"));
	                lstSortCols->SetItemData(itm, 1); 
				}
				col = wxT("");
				dir = wxT("");
				++pos;
				++itm;
            } else {
				dir += sortCols[pos];
			}
        }
	}

	// Insert the last column
	if (col.Length() > 0) {
		lstSortCols->InsertItem(itm, col);
		if (dir[0] == 'A') {
		    lstSortCols->SetItem(itm, 1, wxT("Ascending"));
			lstSortCols->SetItemData(itm, 0); 
		} else {
		    lstSortCols->SetItem(itm, 1, wxT("Descending"));
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
	lstSortCols->SetItem(itm, 1, wxT("Ascending"));
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
	lstSortCols->SetItem(itm, 1, wxT("Descending"));
	lstSortCols->SetItemData(itm, 1);
	cboColumns->Delete(cboColumns->GetSelection());

	// Setup the buttons
	wxCommandEvent nullEvent;
	OnCboColumnsChange(nullEvent);
    OnLstSortColsChange(nullEvent);
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
    Destroy();
}
