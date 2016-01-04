//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgManageFavourites.cpp - Manage favourites
//
//////////////////////////////////////////////////////////////////////////



// App headers
#include "pgAdmin3.h"

#include "dlg/dlgManageFavourites.h"
#include "db/pgConn.h"
#include "schema/pgServer.h"
#include "utils/sysLogger.h"

#include "images/folder.pngc"
#include "images/favourite.pngc"

#include "utils/favourites.h"

#include <wx/imaglist.h>

BEGIN_EVENT_TABLE(dlgManageFavourites, pgDialog)
	EVT_TREE_SEL_CHANGED(XRCID("trLocation"),	dlgManageFavourites::OnTreeChange)
	EVT_BUTTON (wxID_OK,               dlgManageFavourites::OnOK)
	EVT_BUTTON (wxID_CANCEL,           dlgManageFavourites::OnCancel)
	EVT_BUTTON (XRCID("btnRename"),	   dlgManageFavourites::OnRename)
	EVT_BUTTON (XRCID("btnDelete"),    dlgManageFavourites::OnDelete)
	EVT_BUTTON (XRCID("btnNewFolder"), dlgManageFavourites::OnNewFolder)
END_EVENT_TABLE()


#define btnRename		CTRL_BUTTON("btnRename")
#define btnDelete		CTRL_BUTTON("btnDelete")
#define btnNewFolder	CTRL_BUTTON("btnNewFolder")
// #define txtTitle		CTRL_TEXT("txtTitle")
#define trLocation		CTRL_TREE("trLocation")


dlgManageFavourites::dlgManageFavourites(wxWindow *parent, queryFavouriteFolder *favourites) :
	pgDialog()
{
	SetFont(settings->GetSystemFont());
	LoadResource(parent, wxT("dlgManageFavourites"));
	RestorePosition();

	anythingChanged = false;

	this->favourites = favourites;

	wxImageList *imgList = new wxImageList(16, 16, true, 2);
	imgList->Add(*favourite_png_ico);
	imgList->Add(*folder_png_ico);

	trLocation->AssignImageList(imgList);

	// Setup the default values
	trLocation->AddRoot(_("Favourites"), 1);
	trLocation->SetItemImage(trLocation->GetRootItem(), 1, wxTreeItemIcon_Normal);
	trLocation->SelectItem(trLocation->GetRootItem());
	favourites->AppendAllToTree(trLocation, trLocation->GetRootItem(), false);
	trLocation->Expand(trLocation->GetRootItem());
}

int dlgManageFavourites::ManageFavourites()
{
	int r = ShowModal();
	if (r == wxID_OK)
		return 1;
	else
	{
		if (anythingChanged)
			// Need rollback!
			return -1;
		else
			return 0;
	}
}

dlgManageFavourites::~dlgManageFavourites()
{
	SavePosition();
}


void dlgManageFavourites::OnOK(wxCommandEvent &ev)
{
	EndModal(wxID_OK);
}


void dlgManageFavourites::OnCancel(wxCommandEvent &ev)
{
	EndModal(wxID_CANCEL);
}

void dlgManageFavourites::OnTreeChange(wxTreeEvent &ev)
{
	if (!trLocation->GetSelection().IsOk())
	{
		btnRename->Enable(false);
		btnDelete->Enable(false);
		btnNewFolder->Enable(false);
		return;
	}
	if (trLocation->GetSelection() == trLocation->GetRootItem())
	{
		// On root item
		btnRename->Enable(false);
		btnDelete->Enable(false);
		btnNewFolder->Enable(true);
	}
	else
	{
		btnRename->Enable(true);
		btnDelete->Enable(true);
		btnNewFolder->Enable(trLocation->GetItemImage(trLocation->GetSelection()) == 1);
	}
}


void dlgManageFavourites::OnRename(wxCommandEvent &ev)
{
	if (!trLocation->GetSelection().IsOk() ||
	        trLocation->GetSelection() == trLocation->GetRootItem())
		return;

	queryFavouriteItem *item = favourites->FindTreeItem(trLocation->GetSelection());
	if (!item)
		return;

	wxTextEntryDialog dlg(this, _("Enter new name"), (item->GetId() != -2) ? _("Rename favourite") : _("Rename favourites folder"), item->GetTitle());
	if (dlg.ShowModal() != wxID_OK)
		return;

	if (dlg.GetValue() != item->GetTitle())
	{
		item->SetTitle(dlg.GetValue());
		trLocation->SetItemText(trLocation->GetSelection(), dlg.GetValue());
		anythingChanged = true;
	}
}

void dlgManageFavourites::OnDelete(wxCommandEvent &ev)
{
	wxString msg;

	if (!trLocation->GetSelection().IsOk() ||
	        trLocation->GetSelection() == trLocation->GetRootItem())
		return;

	queryFavouriteItem *item = favourites->FindTreeItem(trLocation->GetSelection());
	if (!item)
		return;

	if (item->GetId() != -2)
		msg = wxString::Format(_("Are you sure you want to delete the favourite '%s'?"), item->GetTitle().c_str());
	else
		msg = wxString::Format(_("Are you sure you want to delete the folder '%s'?"), item->GetTitle().c_str());
	if (wxMessageDialog(this, msg, _("Confirm delete"), wxYES_NO | wxICON_QUESTION).ShowModal() != wxID_YES)
		return;

	if (favourites->DeleteTreeItem(trLocation->GetSelection()))
	{
		trLocation->Delete(trLocation->GetSelection());
		anythingChanged = true;
	}
}

void dlgManageFavourites::OnNewFolder(wxCommandEvent &ev)
{
	if (!trLocation->GetSelection().IsOk())
		return;

	queryFavouriteItem *item = favourites->FindTreeItem(trLocation->GetSelection());
	if (!item)
		return;
	if (item->GetId() != -2)
		return;

	wxTextEntryDialog dlg(this, _("Enter name of new folder"), _("Create new favourites folder"));
	if (dlg.ShowModal() != wxID_OK)
		return;

	wxString title = dlg.GetValue().Trim();
	if (title.IsEmpty())
		return;

	queryFavouriteFolder *fld = (queryFavouriteFolder *)item;
	if (fld->ContainsFolder(title))
	{
		wxMessageBox(_("A folder with the specified name already exists."));
		return;
	}


	queryFavouriteFolder *newfld = fld->AddNewFolder(dlg.GetValue());
	newfld->SetTreeId(trLocation->AppendItem(trLocation->GetSelection(), title, 1));
	trLocation->Expand(fld->GetTreeId());
	anythingChanged = true;
}
