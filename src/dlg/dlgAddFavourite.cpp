//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgAddFavourite.cpp - Add a favourite
//
//////////////////////////////////////////////////////////////////////////



// App headers
#include "pgAdmin3.h"

#include "dlgAddFavourite.h"
#include "pgConn.h"
#include "pgServer.h"
#include "sysLogger.h"
#include "ctl/ctlTree.h"

#include "images/folder.xpm"

#include "favourites.h"


BEGIN_EVENT_TABLE(dlgAddFavourite, pgDialog)
	EVT_TEXT(XRCID("txtTitle"),			dlgAddFavourite::OnChange)
	EVT_TREE_SEL_CHANGED(XRCID("trLocation"),	dlgAddFavourite::OnTreeChange)
    EVT_BUTTON (wxID_OK,               dlgAddFavourite::OnOK)
    EVT_BUTTON (wxID_CANCEL,           dlgAddFavourite::OnCancel)
END_EVENT_TABLE()


#define btnOK			CTRL_BUTTON("wxID_OK")
#define txtTitle		CTRL_TEXT("txtTitle")
#define trLocation		CTRL_TREE("trLocation")


dlgAddFavourite::dlgAddFavourite(wxWindow *parent, queryFavouriteFolder *favourites) : 
pgDialog()
{
    wxLogInfo(wxT("Creating an add favourite dialogue"));

    wxWindowBase::SetFont(settings->GetSystemFont());
    LoadResource(parent, wxT("dlgAddFavourite"));

    CenterOnParent();

	this->favourites = favourites;

	wxImageList *imgList = new wxImageList(16, 16, true, 2);
	imgList->Add(wxIcon(folder_xpm));
	trLocation->AssignImageList(imgList);

	trLocation->AddRoot(_("Favourites"), 0);
	trLocation->SelectItem(trLocation->GetRootItem());
	favourites->AppendAllToTree(trLocation, trLocation->GetRootItem(), true);
	trLocation->Expand(trLocation->GetRootItem());
}

bool dlgAddFavourite::AddFavourite(wxString newtext)
{
	int r = ShowModal();
	if (r != wxID_OK)
		return false;

	wxString title = txtTitle->GetValue().Trim();
	if (title.IsEmpty())
		return false;

	if (!trLocation->GetSelection().IsOk())
		return false;

	queryFavouriteFolder *fld = (queryFavouriteFolder *)favourites->FindTreeItem(trLocation->GetSelection());

	if (!fld)
		return false;

	fld->AddNewFavourite(title, newtext);
	return true;
}

dlgAddFavourite::~dlgAddFavourite()
{
    wxLogInfo(wxT("Destroying an add favourite dialogue"));
}


void dlgAddFavourite::OnOK(wxCommandEvent& ev)
{
    EndModal(wxID_OK);
}


void dlgAddFavourite::OnCancel(wxCommandEvent& ev)
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
