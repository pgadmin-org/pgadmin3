//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
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

#define txtPrecision    CTRL_TEXT("txtPrecision")
#define txtScale        CTRL_TEXT("txtScale")


BEGIN_EVENT_TABLE(ddPrecisionScaleDialog, pgDialog)
END_EVENT_TABLE()

ddPrecisionScaleDialog::ddPrecisionScaleDialog(	wxWindow *parent,
        const wxString &defaultValue1,
        const wxString &defaultValue2
                                              )
{
	SetFont(settings->GetSystemFont());
	LoadResource(parent, wxT("ddPrecisionScaleDialog"));
	RestorePosition();
	Init();
	SetValue1(defaultValue1);
	SetValue2(defaultValue2);

	txtPrecision->SetFocus();
}

ddPrecisionScaleDialog::~ddPrecisionScaleDialog()
{
}

ddPrecisionScaleDialog::ddPrecisionScaleDialog()
{
	Init();
}

void ddPrecisionScaleDialog::Init( )
{
	m_value1 = wxT("0");
	m_value2 = wxT("0");
}

//Transfer data to the window
bool ddPrecisionScaleDialog::TransferDataToWindow()
{
	txtPrecision->SetValue(m_value1);
	txtScale->SetValue(m_value2);

	return true;
}

//Transfer data from the window
bool ddPrecisionScaleDialog::TransferDataFromWindow()
{
	m_value1 = txtPrecision->GetValue();
	m_value2 = txtScale->GetValue();

	return true;
}
