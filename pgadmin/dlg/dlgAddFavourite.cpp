//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgAddFavourite.cpp - Add a favourite
//
//////////////////////////////////////////////////////////////////////////



// App headers
#include "pgAdmin3.h"

#include "dlg/dlgAddFavourite.h"
#include "db/pgConn.h"
#include "schema/pgServer.h"
#include "utils/sysLogger.h"
#include "ctl/ctlTree.h"

#include "images/folder.pngc"

#include "utils/favourites.h"

#include <wx/imaglist.h>

BEGIN_EVENT_TABLE(dlgAddFavourite, pgDialog)
	EVT_TEXT(XRCID("txtTitle"),			dlgAddFavourite::OnChange)
	EVT_TREE_SEL_CHANGED(XRCID("trLocation"),	dlgAddFavourite::OnTreeChange)
	EVT_BUTTON (wxID_OK,               dlgAddFavourite::OnOK)
	EVT_BUTTON (wxID_CANCEL,           dlgAddFavourite::OnCancel)
	EVT_BUTTON (XRCID("btnNewFolder"), dlgAddFavourite::OnNewFolder)
END_EVENT_TABLE()


#define btnOK			CTRL_BUTTON("wxID_OK")
#define txtTitle		CTRL_TEXT("txtTitle")
#define trLocation		CTRL_TREE("trLocation")
#define btnNewFolder	CTRL_BUTTON("btnNewFolder")


dlgAddFavourite::dlgAddFavourite(wxWindow *parent, queryFavouriteFolder *favourites) :
	pgDialog()
{
	SetFont(settings->GetSystemFont());
	LoadResource(parent, wxT("dlgAddFavourite"));
	RestorePosition();

	anythingChanged = false;

	this->favourites = favourites;

	wxImageList *imgList = new wxImageList(16, 16, true, 2);
	imgList->Add(*folder_png_ico);
	trLocation->AssignImageList(imgList);

	trLocation->AddRoot(_("Favourites"), 0);
	trLocation->SelectItem(trLocation->GetRootItem());
	favourites->AppendAllToTree(trLocation, trLocation->GetRootItem(), true);
	trLocation->Expand(trLocation->GetRootItem());
}

int dlgAddFavourite::AddFavourite(wxString newtext)
{
	int ret = 1;
	int r = ShowModal();
	if (r != wxID_OK)
	{
		if (anythingChanged)
			// Need rollback!
			ret = -1;
		else
			ret = 0;
	}

	wxString title = txtTitle->GetValue().Trim();
	if (title.IsEmpty())
	{
		if (anythingChanged)
			// Need rollback!
			ret = -1;
		else
			ret = 0;
	}

	if (!trLocation->GetSelection().IsOk())
	{
		if (anythingChanged)
			// Need rollback!
			ret = -1;
		else
			ret = 0;
	}

	queryFavouriteFolder *fld = (queryFavouriteFolder *)favourites->FindTreeItem(trLocation->GetSelection());

	if (!fld)
	{
		if (anythingChanged)
			// Need rollback!
			ret = -1;
		else
			ret = 0;
	}

	if (r == wxID_OK)
		fld->AddNewFavourite(title, newtext);
	return ret;
}

dlgAddFavourite::~dlgAddFavourite()
{
	SavePosition();
}


void dlgAddFavourite::OnOK(wxCommandEvent &ev)
{
#ifdef __WXGTK__
	if (!btnOK->IsEnabled())
		return;
#endif
	EndModal(wxID_OK);
}


void dlgAddFavourite::OnCancel(wxCommandEvent &ev)
{
	EndModal(wxID_CANCEL);
}

void dlgAddFavourite::OnChange(wxCommandEvent &ev)
{
	bool ok = true;

	if (txtTitle->GetValue().Trim().IsEmpty())
		ok = false;
	else if (!trLocation->GetSelection().IsOk())
		ok = false;
	else if (favourites->FindTreeItem(trLocation->GetSelection()) == NULL)
		ok = false;

	btnOK->Enable(ok);
}

void dlgAddFavourite::OnTreeChange(wxTreeEvent &ev)
{
	wxCommandEvent evt;
	OnChange(evt);
}

void dlgAddFavourite::OnNewFolder(wxCommandEvent &ev)
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
	newfld->SetTreeId(trLocation->AppendItem(trLocation->GetSelection(), title, 0));
	trLocation->Expand(fld->GetTreeId());
	anythingChanged = true;
}
