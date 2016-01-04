//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddTableNameDialog.h - Utility dialog class to allow user input of table name and short name
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDTABLENAMEDIALOGS_H
#define DDTABLENAMEDIALOGS_H

#include "dlg/dlgClasses.h"
#include "dd/dditems/figures/ddTextTableItemFigure.h"

class ddTableNameDialog : public pgDialog
{
public:
	ddTableNameDialog();
	~ddTableNameDialog();
	ddTableNameDialog(wxWindow *parent,
	                  const wxString &defaultValue1 = wxEmptyString,
	                  const wxString &defaultValue2 = wxEmptyString,
	                  ddTextTableItemFigure *tableItem = NULL
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


protected:
	// Data members
	wxString m_value1, m_value2;
	wxString label1, label2;
	bool checkGenerate;
	ddTextTableItemFigure *tabItem;

	DECLARE_EVENT_TABLE()

private:

};
#endif

