//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddPrecisionScaleDialog.cpp - Utility dialog class to allow user input of table name and short name
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/spinctrl.h>

// App headers
#include "dd/dditems/utilities/ddPrecisionScaleDialog.h"
#include "dd/dditems/figures/ddTableFigure.h"

IMPLEMENT_CLASS( ddPrecisionScaleDialog, wxDialog )

BEGIN_EVENT_TABLE( ddPrecisionScaleDialog, wxDialog )
EVT_TEXT_ENTER(DDVALUE1, ddPrecisionScaleDialog::OnEnterPressed )
END_EVENT_TABLE()

ddPrecisionScaleDialog::ddPrecisionScaleDialog(	wxWindow* parent,
										wxWindowID id,
										const wxString& caption,
										const wxString& captionLabel1,
										const int& defaultValue1,
										const wxString& captionLabel2,
										const int& defaultValue2,
										const wxPoint& pos,
										const wxSize& size,
										long style
										)
{	
	Init();
	label1=captionLabel1;
	SetValue1(defaultValue1);
	label2=captionLabel2;
	SetValue2(defaultValue2);
    Create(parent, id, caption, pos, size, style);    
}

ddPrecisionScaleDialog::~ddPrecisionScaleDialog()
{
	delete value1Label;
	delete value2Label;
	delete value1Ctrl;
	delete value2Ctrl;
	delete ok;
	delete cancel;
	delete line;
}

ddPrecisionScaleDialog::ddPrecisionScaleDialog()
{
	Init();
}

void ddPrecisionScaleDialog::Init( )
{
    m_value1 = 0;
    m_value2 = 0;
}

bool ddPrecisionScaleDialog::Create(	wxWindow* parent,
					wxWindowID id,
					const wxString& caption = wxT("Input Dialog"),
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxDefaultSize,
					long style = wxCAPTION )
{
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

void ddPrecisionScaleDialog::CreateControls()
{
    // A top-level sizer

    topSizer = new wxBoxSizer(wxHORIZONTAL );
    this->SetSizer(topSizer);

	// A horizontal box sizer to contain auto generate short name checkbox
	nameGenBox = new wxBoxSizer(wxVERTICAL);
    topSizer->Add(nameGenBox, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    // A second box sizer to give more space around the controls
    boxSizer = new wxBoxSizer(wxHORIZONTAL );
    nameGenBox->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    // Label for the Value 1
   value1Label = new wxStaticText ( this, wxID_STATIC,
        label1, wxDefaultPosition, wxDefaultSize, 0 );
    boxSizer->Add(value1Label, 0, wxALIGN_LEFT|wxALL, 5);

    // A text control for Value 1
	value1Ctrl = new wxSpinCtrl ( this, DDVALUE1,wxString::Format(wxT("(%d)"),m_value1) , wxDefaultPosition,
		wxDefaultSize, wxSP_ARROW_KEYS, 0, 1000, 0 );
    boxSizer->Add(value1Ctrl, 0, wxGROW|wxALL, 5);

    // Label for the Value 2
    value2Label = new wxStaticText ( this, wxID_STATIC,
        label2, wxDefaultPosition, wxDefaultSize, 0 );
    boxSizer->Add(value2Label, 0, wxALIGN_LEFT|wxALL, 5);

    // A text control for Value 2
   value2Ctrl = new wxSpinCtrl ( this, DDVALUE2, wxString::Format(wxT("(%d)"),m_value2), wxDefaultPosition,
		 wxDefaultSize, wxSP_ARROW_KEYS, 0, 1000, 0 );
  
   boxSizer->Add(value2Ctrl, 0, wxGROW|wxALL, 5);

    // A dividing line before the OK and Cancel buttons

    line = new wxStaticLine ( this, wxID_STATIC,
        wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    nameGenBox->Add(line, 0, wxGROW|wxALL, 5);

     // A horizontal box sizer to contain Reset, OK, Cancel and Help

	okCancelBox = new wxBoxSizer(wxHORIZONTAL);
    nameGenBox->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

	// The OK button

    ok = new wxButton ( this, wxID_OK, wxT("&OK"),
        wxDefaultPosition, wxDefaultSize, 0 );
    okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // The Cancel button

    cancel = new wxButton ( this, wxID_CANCEL,
        wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

}

//Transfer data to the window
bool ddPrecisionScaleDialog::TransferDataToWindow()
{
    wxSpinCtrl* val1Ctrl = (wxSpinCtrl*) FindWindow(DDVALUE1);
	wxSpinCtrl* val2Ctrl = (wxSpinCtrl*) FindWindow(DDVALUE2);

    val1Ctrl->SetValue(m_value1);
	val2Ctrl->SetValue(m_value2);

    return true;
}

//Transfer data from the window
bool ddPrecisionScaleDialog::TransferDataFromWindow()
{
    wxSpinCtrl* val1Ctrl = (wxSpinCtrl*) FindWindow(DDVALUE1);
	wxSpinCtrl* val2Ctrl = (wxSpinCtrl*) FindWindow(DDVALUE2);

    m_value1 = val1Ctrl->GetValue();
	m_value2 = val2Ctrl->GetValue();

    return true;
}

void ddPrecisionScaleDialog::OnEnterPressed( wxCommandEvent& event )
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
}