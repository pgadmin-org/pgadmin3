//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdRemoveDeleteDialog.cpp - Utility dialog class to allow user to select between delete / remove a figure
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/statline.h>

// App headers
#include "hotdraw/utilities/hdRemoveDeleteDialog.h"

IMPLEMENT_CLASS( hdRemoveDeleteDialog, wxDialog )

BEGIN_EVENT_TABLE(hdRemoveDeleteDialog, wxDialog)
	EVT_BUTTON(DD_REMOVE, hdRemoveDeleteDialog::OnRemove)
	EVT_BUTTON(DD_DELETE, hdRemoveDeleteDialog::OnDelete)
	EVT_BUTTON(wxID_CANCEL, hdRemoveDeleteDialog::OnCancel)
END_EVENT_TABLE()

hdRemoveDeleteDialog::hdRemoveDeleteDialog(	const wxString &message,
        const wxString &caption,
        wxWindow *parent, bool allowRemove
                                          )
{
	allowRemoveButton = allowRemove;
	SetFont(settings->GetSystemFont());
	Init();
	Create(parent, wxID_ANY, message, caption);
	cancelButton->SetFocus();
}

hdRemoveDeleteDialog::~hdRemoveDeleteDialog()
{
	if(staticText)
		delete staticText;
	if(staticText2)
		delete staticText2;
	if(staticText3)
		delete staticText3;
	if(line)
		delete line;
	if(removeButton)
		delete removeButton;
	if(deleteButton)
		delete deleteButton;
	if(cancelButton)
		delete cancelButton;
}

hdRemoveDeleteDialog::hdRemoveDeleteDialog()
{
	Init();
}


void hdRemoveDeleteDialog::Init( )
{
}

// Creation
bool hdRemoveDeleteDialog::Create(	wxWindow *parent,
                                    wxWindowID id,
                                    const wxString &caption,
                                    const wxString &message
                                 )
{
	SetFont(settings->GetSystemFont());

	if (!wxDialog::Create( parent, id, message, wxDefaultPosition, wxDefaultSize, wxCAPTION))
		return false;

	CreateControls(caption);

	// This fits the dialog to the minimum size dictated by
	// the sizers
	GetSizer()->Fit(this);

	// This ensures that the dialog cannot be sized smaller
	// than the minimum size
	GetSizer()->SetSizeHints(this);

	// Centre the dialog on the parent or (if none) screen

	Centre();

	return true;
}


// Creates the controls and sizers
void hdRemoveDeleteDialog::CreateControls(const wxString &message)
{
	// A top-level sizer
	topSizer = new wxBoxSizer(wxVERTICAL );
	this->SetSizer(topSizer);
	topSizer->AddSpacer(10);
	//Message Sizer
	messageSizer = new wxBoxSizer(wxHORIZONTAL );
	topSizer->Add(messageSizer);
	messageSizer->AddSpacer(25);

	staticText = new wxStaticText(this, wxID_STATIC, message, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
	messageSizer->Add(staticText, 0, wxALIGN_CENTER, 5);

	messageSizer->AddSpacer(45);

	// Add important user info
	wxString info = _("     Choose Remove from Diagram to remove only from current diagram.     ");
	wxString info2 = _("     Choose Remove from Model to delete permanently.     ");

	this->SetForegroundColour(wxColour(wxT("GREY")));
	staticText2 = new wxStaticText(this, wxID_STATIC, info, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	topSizer->Add(staticText2, 0, wxALIGN_LEFT, 5);

	staticText3 = new wxStaticText(this, wxID_STATIC, info2, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	topSizer->Add(staticText3, 0, wxALIGN_LEFT, 5);

	// A space and a dividing line before the Remove Delete and Cancel buttons
	topSizer->AddSpacer(10);
	line = new wxStaticLine ( this, wxID_STATIC,
	                          wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	topSizer->Add(line, 0, wxGROW | wxALL, 5);

	//Buttons Sizer
	buttonsSizer = new wxBoxSizer(wxHORIZONTAL );

	topSizer->Add(buttonsSizer, 0, wxALIGN_CENTER, 5);

	removeButton = new wxButton ( this, DD_REMOVE, wxT("&Remove from Diagram"),
	                              wxDefaultPosition, wxDefaultSize, 0 );

	if(!allowRemoveButton)
		removeButton->Enable(false);

	buttonsSizer->Add(removeButton, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	deleteButton = new wxButton ( this, DD_DELETE, wxT("&Remove from Model"),
	                              wxDefaultPosition, wxDefaultSize, 0 );
	buttonsSizer->Add(deleteButton, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	cancelButton = new wxButton ( this, wxID_CANCEL, wxT("&Cancel"),
	                              wxDefaultPosition, wxDefaultSize, 0 );
	buttonsSizer->Add(cancelButton, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	topSizer->AddSpacer(10);
}

void hdRemoveDeleteDialog::OnRemove(wxCommandEvent &WXUNUSED(event))
{
	EndModal( DD_REMOVE );
}

void hdRemoveDeleteDialog::OnDelete(wxCommandEvent &WXUNUSED(event))
{
	EndModal( DD_DELETE );
}

void hdRemoveDeleteDialog::OnCancel(wxCommandEvent &WXUNUSED(event))
{
	EndModal( wxID_CANCEL );
}
