//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmOptions.cpp - The main options dialogue
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/xrc/xmlres.h>


// App headers
#include "../../pgAdmin3.h"
#include "frmConnect.h"
#include "../../db/pg/pgConn.h"
#include "../../utils/sysLogger.h"

// Icons
#include "../../images/pgAdmin3.xpm"

BEGIN_EVENT_TABLE(frmConnect, wxDialog)
  EVT_BUTTON (XRCID("btnOK"), frmConnect::OK)
  EVT_BUTTON (XRCID("btnCancel"), frmConnect::Cancel)
END_EVENT_TABLE()

frmConnect::frmConnect(wxFrame *parent)
{

    wxLogInfo(wxT("Creating a connect dialogue"));
    extern sysSettings *objSettings;

    wxXmlResource::Get()->LoadDialog(this, parent, "frmConnect"); 

    // Icon
    SetIcon(wxIcon(pgAdmin3_xpm));
    Center();
}

frmConnect::~frmConnect()
{
    wxLogInfo(wxT("Destroying a connect dialogue"));
}

void frmConnect::OK()
{
	wxLogInfo("User clicked OK...");
    bCancelled = FALSE;
    this->Show(FALSE);
}

void frmConnect::Cancel()
{
	wxLogInfo("User clicked Cancel...");
    bCancelled = TRUE;
    this->Show(FALSE);
}

wxString frmConnect::GetServer()
{
    return XRCCTRL(*this, "txtServer", wxTextCtrl)->GetValue();
}

wxString frmConnect::GetDatabase()
{
    return XRCCTRL(*this, "txtDatabase", wxTextCtrl)->GetValue();
}

wxString frmConnect::GetUsername()
{
    return XRCCTRL(*this, "txtUsername", wxTextCtrl)->GetValue();
}

wxString frmConnect::GetPassword()
{
    return XRCCTRL(*this, "txtPassword", wxTextCtrl)->GetValue();
}

long frmConnect::GetPort()
{
    return atoi(XRCCTRL(*this, "txtPort", wxTextCtrl)->GetValue());
}

bool frmConnect::GetCancelled()
{
    return bCancelled;
}