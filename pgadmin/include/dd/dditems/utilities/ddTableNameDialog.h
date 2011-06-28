//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddTableNameDialog.h - Utility dialog class to allow user input of table name and short name
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDTABLENAMEDIALOGS_H
#define DDTABLENAMEDIALOGS_H

#include <wx/statline.h>
#include "dd/dditems/figures/ddTextTableItemFigure.h"

enum
{
	DDTABLENAMEDIALOG = 10000,
	DDVALUE1 = 10001,
	DDVALUE2 = 10002,
	DDGENBUTTON = 10003
};

class ddTableNameDialog : public wxDialog
{
	DECLARE_CLASS( ddTableNameDialog )
	DECLARE_EVENT_TABLE()
public:
	ddTableNameDialog();
	~ddTableNameDialog();
	ddTableNameDialog(	wxWindow *parent,
	                    wxWindowID id,
	                    const wxString &caption = wxT("Input Dialog"),
	                    const wxString &captionLabel1 = wxEmptyString,
	                    const wxString &defaultValue1 = wxEmptyString,
	                    const wxString &captionLabel2 = wxEmptyString,
	                    const wxString &defaultValue2 = wxEmptyString,
	                    const wxPoint &pos = wxDefaultPosition,
	                    const wxSize &size = wxDefaultSize,
	                    long style = wxCAPTION,
	                    ddTextTableItemFigure *tableItem = NULL
	                 );

	// Member initialization
	void Init();
	// Creation
	bool Create(	wxWindow *parent,
	                wxWindowID id,
	                const wxString &caption,
	                const wxPoint &pos,
	                const wxSize &size,
	                long style);


	// Creates the controls and sizers
	void CreateControls();

	// Sets the validators for the dialog controls
	//void SetDialogValidators();
	bool TransferDataToWindow();
	bool TransferDataFromWindow();

	// Sets the help text for the dialog controls
	void SetDialogHelp();

	// Value1 accessors
	void SetValue1(wxString value)
	{
		m_value1 = value;
	}
	wxString GetValue1()
	{
		return m_value1;
	}

	// Value1 accessors
	void SetValue2(wxString value)
	{
		m_value2 = value;
	}
	wxString GetValue2()
	{
		return m_value2;
	}

	// CheckBox accessors
	void SetValueGenerate(bool value)
	{
		checkGenerate = value;
	}
	bool GetValueGenerate()
	{
		return checkGenerate;
	}

	//wxEVT_COMMAND_BUTTON_CLICKED event_handler for DDGENBUTTON
	void OnGenButtonClicked( wxCommandEvent &event );
	//wxEVT_COMMAND_TEXT_ENTER event_handle for DDVALUE1
	void OnEnterPressed( wxCommandEvent &event );


protected:
	// Data members
	wxString m_value1, m_value2;
	wxString label1, label2;
	bool checkGenerate;
	ddTextTableItemFigure *tabItem;
	// Dialog controls
	wxBoxSizer *topSizer, *nameGenBox, *boxSizer, *okCancelBox;
	wxStaticText *value1Label, *value2Label;
	wxTextCtrl *value1Ctrl, *value2Ctrl;
	wxButton *generateButton,  *ok, *cancel;
	wxStaticLine *line;

private:

};
#endif

