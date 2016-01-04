//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddPrecisionScaleDialog.h - Utility dialog class to allow user input of precision and scale
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDREMOVEDELETEDIALOG_H
#define HDREMOVEDELETEDIALOG_H

#include <wx/statline.h>
#include <wx/stattext.h>

enum ddRemoveDeleteButtons
{
	DD_REMOVE = 31000,
	DD_DELETE = 32000
};

class hdRemoveDeleteDialog : public wxDialog
{
	DECLARE_CLASS( hdRemoveDeleteDialog )
	DECLARE_EVENT_TABLE()
public:
	hdRemoveDeleteDialog();
	hdRemoveDeleteDialog(	const wxString &message,
	                        const wxString &caption = _("Title"),
	                        wxWindow *parent = NULL,
	                        bool allowRemove = true
	                    );
	~hdRemoveDeleteDialog();

	// Member initialization
	void Init();

	// Creation
	bool Create(	wxWindow *parent,
	                wxWindowID id,
	                const wxString &message,
	                const wxString &caption
	           );

	// Creates the controls and sizers
	void CreateControls(const wxString &message);

	//Buttons Events
	void OnRemove(wxCommandEvent &WXUNUSED(event));
	void OnDelete(wxCommandEvent &WXUNUSED(event));
	void OnCancel(wxCommandEvent &WXUNUSED(event));

private:
	bool allowRemoveButton;
	wxBoxSizer *topSizer, *messageSizer, *buttonsSizer;
	wxStaticText *staticText, *staticText2, *staticText3;
	wxStaticLine *line;
	wxButton *removeButton, *deleteButton, *cancelButton;

};
#endif

