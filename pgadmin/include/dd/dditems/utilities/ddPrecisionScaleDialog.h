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

#ifndef DDPRECISIONSCALEDIALOG_H
#define DDPRECISIONSCALEDIALOG_H

#include <wx/statline.h>

enum
{
	DDPRECISIONSCALEDIALOG = 10000,
	DDVALUE1 = 10001,
	DDVALUE2 = 10002
};

class ddPrecisionScaleDialog : public wxDialog
{
	DECLARE_CLASS( ddPrecisionScaleDialog )
	DECLARE_EVENT_TABLE()
public:
	ddPrecisionScaleDialog();
	~ddPrecisionScaleDialog();
	ddPrecisionScaleDialog(	wxWindow *parent,
	                        wxWindowID id = DDPRECISIONSCALEDIALOG,
	                        const wxString &caption = wxT("Two Numeric Values Input Dialog"),
	                        const wxString &captionLabel1 = wxEmptyString,
	                        const int &defaultValue1 = 0,
	                        const wxString &captionLabel2 = wxEmptyString,
	                        const int &defaultValue2 = 0,
	                        const wxPoint &pos = wxDefaultPosition,
	                        const wxSize &size = wxDefaultSize,
	                        long style = wxCAPTION
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
	void SetValue1(int value)
	{
		m_value1 = value;
	}
	int GetValue1()
	{
		return m_value1;
	}

	// Value1 accessors
	void SetValue2(int value)
	{
		m_value2 = value;
	}
	int GetValue2()
	{
		return m_value2;
	}

	//wxEVT_COMMAND_TEXT_ENTER event_handle for DDVALUE1
	void OnEnterPressed( wxCommandEvent &event );


protected:
	// Data members
	int m_value1, m_value2;
	wxString label1, label2;
	// Dialog controls
	wxBoxSizer *topSizer, *nameGenBox, *boxSizer, *okCancelBox;
	wxStaticText *value1Label, *value2Label;
	wxSpinCtrl *value1Ctrl, *value2Ctrl;
	wxButton *ok, *cancel;
	wxStaticLine *line;

private:

};
#endif

