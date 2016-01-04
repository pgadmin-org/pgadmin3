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

#ifndef DDPRECISIONSCALEDIALOG_H
#define DDPRECISIONSCALEDIALOG_H

class ddPrecisionScaleDialog : public pgDialog
{
public:
	ddPrecisionScaleDialog();
	~ddPrecisionScaleDialog();
	ddPrecisionScaleDialog(	wxWindow *parent,
	                        const wxString &defaultValue1 = wxT("0"),
	                        const wxString &defaultValue2 = wxT("0")
	                      );

	// Member initialization
	void Init();

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

	//wxEVT_COMMAND_TEXT_ENTER event_handle for DDVALUE1
	void OnEnterPressed( wxCommandEvent &event );


protected:
	// Data members
	wxString m_value1, m_value2;

	DECLARE_EVENT_TABLE()

};
#endif

