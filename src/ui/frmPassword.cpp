//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmPassword.cpp - Change password
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/xrc/xmlres.h>


// App headers
#include "pgAdmin3.h"
#include "frmPassword.h"
#include "pgServer.h"

// Icons
#include "images/pgAdmin3.xpm"

BEGIN_EVENT_TABLE(frmPassword, wxDialog)
  EVT_BUTTON (XRCID("btnOK"), frmPassword::OnOK)
  EVT_BUTTON (XRCID("btnCancel"), frmPassword::OnCancel)
END_EVENT_TABLE()

frmPassword::frmPassword(wxFrame *parent)
{

    wxLogInfo(wxT("Creating a change password dialogue"));

    wxXmlResource::Get()->LoadDialog(this, parent, "frmPassword"); 

    // Icon
    SetIcon(wxIcon(pgAdmin3_xpm));
    Center();
}

frmPassword::~frmPassword()
{
    wxLogInfo(wxT("Destroying a change password dialogue"));
}

void frmPassword::OnOK()
{

    // Is the old password right?
    if (XRCCTRL(*this, "txtCurrent", wxTextCtrl)->GetValue() != objServer->GetPassword()) {
        wxLogError(wxT("Incorrect password!"));
        return;
    }

    // Did we confirm the password OK?
    if (XRCCTRL(*this, "txtNew", wxTextCtrl)->GetValue() != XRCCTRL(*this, "txtConfirm", wxTextCtrl)->GetValue()) {
        wxLogError(wxT("Passwords do not match!"));
        return;
    }

    // Set the new password
    if (!objServer->SetPassword(XRCCTRL(*this, "txtNew", wxTextCtrl)->GetValue())) {
        wxString szMsg;
        szMsg.Printf(wxT("The password could not be changed!"));
        wxLogError(szMsg);
        return;
    }

    // All must have gone well!
    wxLogMessage(wxT("Password successfully changed!"));
    this->Destroy();
}

void frmPassword::OnCancel()
{
    this->Destroy();
}

void frmPassword::SetServer(pgServer *objNewServer)
{
    objServer = objNewServer;
}
