//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgManageMacros.cpp - Manage macros
//
//////////////////////////////////////////////////////////////////////////

// App headers
#include "pgAdmin3.h"

#include "dlg/dlgManageMacros.h"
#include "db/pgConn.h"
#include "schema/pgServer.h"
#include "utils/sysLogger.h"
#include "ctl/ctlSQLBox.h"
#include "utils/macros.h"

#include <wx/imaglist.h>

//pointer to controls
#define lstKeys		CTRL_LISTVIEW("lstKeys")
#define txtName		CTRL_TEXT("txtName")
#define txtSqlBox	CTRL_SQLBOX("txtSqlBox")
#define btnClear	CTRL_BUTTON("btnClear")
#define btnSave		CTRL_BUTTON("btnSave")

BEGIN_EVENT_TABLE(dlgManageMacros, DialogWithHelp)
	EVT_LIST_ITEM_SELECTED (XRCID("lstKeys"),	dlgManageMacros::OnKeySelect)
	EVT_BUTTON (wxID_OK,						dlgManageMacros::OnOK)
	EVT_BUTTON (wxID_CANCEL,					dlgManageMacros::OnCancel)
	EVT_BUTTON (XRCID("btnClear"),				dlgManageMacros::OnClear)
	EVT_BUTTON (XRCID("btnSave"),				dlgManageMacros::OnSave)
	EVT_TEXT   (XRCID("txtName"),				dlgManageMacros::OnNameChange)
	EVT_STC_CHANGE (XRCID("txtSqlBox"),			dlgManageMacros::OnQueryChange)
END_EVENT_TABLE()

dlgManageMacros::dlgManageMacros(wxWindow *parent, frmMain *form, queryMacroList *macros) :
	DialogWithHelp(form)
{
	SetFont(settings->GetSystemFont());
	LoadResource(parent, wxT("dlgManageMacros"));
	RestorePosition();

	this->macros = macros;

	// Setup list of keys
	lstKeys->CreateColumns(NULL, _("Key"), _("Name"), 40);

	lstKeys->Hide();
	size_t i;
	int num = 0;
	for (i = 1; i < 13; i++)
	{
		wxString key;
		key.Printf(wxT("Alt-F%d"), (int)i);
		AddKeyToList(num++, key);
	}
	for (i = 1; i < 11; i++)
	{
		wxString key;
		key.Printf(wxT("Ctrl-%d"), (int)i % 10); // in order of keys 1,2,...,8,9,0
		AddKeyToList(num++, key);
	}
	lstKeys->Show();

	// Initialy no key is selected, so disable editor keys
	btnClear->Disable();
	btnSave->Disable();

	// Clear Markers
	anythingChanged = false;
	thisMacroChanged = false;

	txtSqlBox->SetModEventMask(wxSTC_MOD_INSERTTEXT | wxSTC_MOD_DELETETEXT);
}

void dlgManageMacros::AddKeyToList(int position, const wxString &key)
{
	long tmp = lstKeys->InsertItem(position, key);
	queryMacroItem *item = macros->FindMacro(key);
	if (item != NULL)
		lstKeys->SetItem(tmp, 1, item->GetName());
}

int dlgManageMacros::ManageMacros()
{
	int r = ShowModal();
	if (r == wxID_OK)
	{
		return 1;
	}
	else
	{
		if (anythingChanged)
			return -1;
		else
			return 0;
	}
}

dlgManageMacros::~dlgManageMacros()
{
	SavePosition();
}

void dlgManageMacros::OnOK(wxCommandEvent &ev)
{
	if (thisMacroChanged)
		SetMacro(true);
	EndModal(wxID_OK);
}

void dlgManageMacros::OnCancel(wxCommandEvent &ev)
{
	EndModal(wxID_CANCEL);
}

void dlgManageMacros::DeleteMacro(int listItem)
{
	wxString key;
	key = lstKeys->GetItemText(listItem);

	if (macros->DelMacro(key))
	{
		anythingChanged = true;
		lstKeys->SetItem(listItem, 1, wxT(""));
		txtName->ChangeValue(wxT(""));
		txtSqlBox->SetText(wxT(""));
		thisMacroChanged = false;
		btnSave->Disable();
		btnClear->Disable();
	}
}

void dlgManageMacros::OnClear(wxCommandEvent &ev)
{
	int item;
	item = lstKeys->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

	if (item == -1)
		return;

	DeleteMacro(item);
}

void dlgManageMacros::OnSave(wxCommandEvent &ev)
{
	if (!thisMacroChanged)
		return;
	SetMacro(false);
}

void dlgManageMacros::SetMacro(bool silent)
{
	int item;
	wxString key, Name, query;

	item = lstKeys->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (item == -1)
		return;

	key = lstKeys->GetItemText(item);
	Name = txtName->GetValue().Trim();
	query = txtSqlBox->GetText().Trim();

	if (Name.IsEmpty() && query.IsEmpty())
	{
		DeleteMacro(item);
	}
	else if (Name.IsEmpty() || query.IsEmpty())
	{
		if (!silent)
			wxMessageBox(_("You must specify a query and a name for the macro"), _("Save macro"), wxICON_EXCLAMATION | wxOK);
		return;
	}
	else
	{
		macros->AddOrUpdateMacro(key, Name, query);
		anythingChanged = true;
		thisMacroChanged = false;
		lstKeys->SetItem(item, 1, Name);
		btnClear->Enable();
		btnSave->Disable();
	}
}

void dlgManageMacros::OnKeySelect(wxListEvent &ev)
{
	wxString key;
	key = ev.GetText();

	queryMacroItem *item = macros->FindMacro(key);
	if (item != NULL)
	{
		txtName->ChangeValue(item->GetName());
		txtSqlBox->SetText(item->GetQuery());
		btnClear->Enable();
		btnSave->Disable();
	}
	else
	{
		txtName->ChangeValue(wxT(""));
		txtSqlBox->SetText(wxT(""));
		btnClear->Disable();
		btnSave->Disable();
	}
	thisMacroChanged = false;
}

void dlgManageMacros::OnNameChange(wxCommandEvent &ev)
{
	thisMacroChanged = true;
	btnSave->Enable();
}

void dlgManageMacros::OnQueryChange(wxStyledTextEvent &ev)
{
	thisMacroChanged = true;
	btnSave->Enable();
}

wxString dlgManageMacros::GetHelpPage() const
{
	return wxT("macros");
}
