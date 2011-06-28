//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddTableNameDialog.cpp - Utility dialog class to allow user input of table name and short name
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/statline.h>
// App headers
#include "dd/dditems/utilities/ddTableNameDialog.h"
#include "dd/dditems/figures/ddTableFigure.h"

IMPLEMENT_CLASS( ddTableNameDialog, wxDialog )

BEGIN_EVENT_TABLE( ddTableNameDialog, wxDialog )
	EVT_BUTTON(DDGENBUTTON, ddTableNameDialog::OnGenButtonClicked )
	EVT_TEXT_ENTER(DDVALUE1, ddTableNameDialog::OnEnterPressed )
END_EVENT_TABLE()

ddTableNameDialog::ddTableNameDialog(	wxWindow *parent,
                                        wxWindowID id,
                                        const wxString &caption,
                                        const wxString &captionLabel1,
                                        const wxString &defaultValue1,
                                        const wxString &captionLabel2,
                                        const wxString &defaultValue2,
                                        const wxPoint &pos,
                                        const wxSize &size,
                                        long style,
                                        ddTextTableItemFigure *tableItem
                                    )
{
	Init();
	tabItem = tableItem;
	checkGenerate = false;
	label1 = captionLabel1;
	SetValue1(defaultValue1);
	label2 = captionLabel2;
	SetValue2(defaultValue2);
	Create(parent, id, caption, pos, size, style);
}

ddTableNameDialog::~ddTableNameDialog()
{
	delete value1Label;
	delete value2Label;
	delete value1Ctrl;
	delete value2Ctrl;
	delete generateButton;
	delete ok;
	delete cancel;
	delete line;
}

ddTableNameDialog::ddTableNameDialog()
{
	Init();
}

void ddTableNameDialog::Init( )
{
	m_value1 = wxEmptyString;
	m_value2 = wxEmptyString;
}

bool ddTableNameDialog::Create(	wxWindow *parent,
                                wxWindowID id = DDTABLENAMEDIALOG,
                                const wxString &caption = wxT("Input Dialog"),
                                const wxPoint &pos = wxDefaultPosition,
                                const wxSize &size = wxDefaultSize,
                                long style = wxCAPTION )
{

	// We have to set extra styles before creating the
	// dialog

	SetExtraStyle(wxWS_EX_BLOCK_EVENTS | wxDIALOG_EX_CONTEXTHELP);
	if (!wxDialog::Create( parent, id, caption, pos, size, style ))
		return false;

	CreateControls();
	//SetDialogValidators();

	// This fits the dialog to the minimum size dictated by
	// the sizers

	GetSizer()->Fit(this);

	// This ensures that the dialog cannot be sized smaller
	// than the minimum size

	GetSizer()->SetSizeHints(this);

	// Centre the dialog on the parent or (if none) screen

	value1Ctrl->SetFocus();

	Centre();

	return true;
}

void ddTableNameDialog::CreateControls()
{
	// A top-level sizer

	topSizer = new wxBoxSizer(wxHORIZONTAL );
	this->SetSizer(topSizer);

	// A horizontal box sizer to contain auto generate short name checkbox
	nameGenBox = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(nameGenBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// A second box sizer to give more space around the controls
	boxSizer = new wxBoxSizer(wxHORIZONTAL );
	nameGenBox->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Label for the Value 1
	value1Label = new wxStaticText ( this, wxID_STATIC,
	                                 label1, wxDefaultPosition, wxDefaultSize, 0 );
	boxSizer->Add(value1Label, 0, wxALIGN_LEFT | wxALL, 5);

	// A text control for Value 1
	value1Ctrl = new wxTextCtrl ( this, DDVALUE1, m_value1 , wxDefaultPosition,
	                              wxDefaultSize, wxTE_PROCESS_ENTER );
	boxSizer->Add(value1Ctrl, 0, wxGROW | wxALL, 5);

	// Label for the Value 2
	value2Label = new wxStaticText ( this, wxID_STATIC,
	                                 label2, wxDefaultPosition, wxDefaultSize, 0 );
	boxSizer->Add(value2Label, 0, wxALIGN_LEFT | wxALL, 5);

	// A text control for Value 2
	value2Ctrl = new wxTextCtrl ( this, DDVALUE2, m_value2, wxDefaultPosition,
	                              wxDefaultSize, 0 );
	value2Ctrl->SetMaxLength(3);
	boxSizer->Add(value2Ctrl, 0, wxGROW | wxALL, 5);

	//A check box to allow select automatic short name generation

	generateButton = new wxButton ( this, DDGENBUTTON, wxT("&Generate"),
	                                wxDefaultPosition, wxDefaultSize, 0 );
	boxSizer->Add(generateButton, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// A dividing line before the OK and Cancel buttons

	line = new wxStaticLine ( this, wxID_STATIC,
	                          wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	nameGenBox->Add(line, 0, wxGROW | wxALL, 5);

	// A horizontal box sizer to contain Reset, OK, Cancel and Help

	okCancelBox = new wxBoxSizer(wxHORIZONTAL);
	nameGenBox->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// The OK button

	ok = new wxButton ( this, wxID_OK, wxT("&OK"),
	                    wxDefaultPosition, wxDefaultSize, 0 );
	okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	// The Cancel button

	cancel = new wxButton ( this, wxID_CANCEL,
	                        wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

}

//Transfer data to the window
bool ddTableNameDialog::TransferDataToWindow()
{
	wxTextCtrl *val1Ctrl = (wxTextCtrl *) FindWindow(DDVALUE1);
	wxTextCtrl *val2Ctrl = (wxTextCtrl *) FindWindow(DDVALUE2);

	if (val1Ctrl)
		val1Ctrl->SetValue(m_value1);
	if (val2Ctrl)
		val2Ctrl->SetValue(m_value2);

	if(val1Ctrl && val2Ctrl)
		return true;
	else
		return false;
}

//Transfer data from the window
bool ddTableNameDialog::TransferDataFromWindow()
{
	wxTextCtrl *val1Ctrl = (wxTextCtrl *) FindWindow(DDVALUE1);
	wxTextCtrl *val2Ctrl = (wxTextCtrl *) FindWindow(DDVALUE2);

	if (val1Ctrl)
		m_value1 = val1Ctrl->GetValue();
	if(val2Ctrl)
		m_value2 = val2Ctrl->GetValue();

	if(val1Ctrl && val2Ctrl)
		return true;
	else
		return false;
}

//Generation CheckBox Event to generata short name when checkbox clicked
void ddTableNameDialog::OnGenButtonClicked( wxCommandEvent &event )
{
	wxTextCtrl *val1Ctrl = (wxTextCtrl *) FindWindow(DDVALUE1);
	wxTextCtrl *val2Ctrl = (wxTextCtrl *) FindWindow(DDVALUE2);
	wxString shortName = ddTableFigure::generateShortName(val1Ctrl->GetValue());
	val2Ctrl->SetValue(shortName);
}


void ddTableNameDialog::OnEnterPressed( wxCommandEvent &event )
{
	if (event.GetEventType() == wxEVT_COMMAND_TEXT_ENTER)
	{
		if ( Validate() && TransferDataFromWindow() )
		{
			if ( IsModal() )
				EndModal(wxID_OK); // If modal
			else
			{
				SetReturnCode(wxID_OK);
				this->Show(false); // If modeless
			}
		}

	}
	event.Skip();
}
