//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
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
    EVT_BUTTON (XRCID("btnOK"),       frmPassword::OnOK)
    EVT_BUTTON (XRCID("btnCancel"),   frmPassword::OnCancel)
END_EVENT_TABLE()

frmPassword::frmPassword(wxFrame *parent)
{

    wxLogInfo(wxT("Creating a change password dialogue"));

    wxXmlResource::Get()->LoadDialog(this, parent, "frmPassword"); 

    // Icon
    SetIcon(wxIcon(pgAdmin3_xpm));
    CenterOnParent();
}

frmPassword::~frmPassword()
{
    wxLogInfo(wxT("Destroying a change password dialogue"));
}

void frmPassword::OnOK(wxCommandEvent& event)
{

    // Is the old password right?
    if (XRCCTRL(*this, "txtCurrent", wxTextCtrl)->GetValue() != server->GetPassword()) {
        wxLogError(__("Incorrect password!"));
        return;
    }

    // Did we confirm the password OK?
    if (XRCCTRL(*this, "txtNew", wxTextCtrl)->GetValue() != XRCCTRL(*this, "txtConfirm", wxTextCtrl)->GetValue()) {
        wxLogError(__("Passwords do not match!"));
        return;
    }

    // Set the new password
    if (!server->SetPassword(XRCCTRL(*this, "txtNew", wxTextCtrl)->GetValue()))
    {
        wxLogError(__("The password could not be changed!"));
        return;
    }

    // All must have gone well!
    wxLogMessage(__("Password successfully changed!"));
    this->Destroy();
}

void frmPassword::OnCancel(wxCommandEvent& event)
{
    this->Destroy();
}

void frmPassword::SetServer(pgServer *newServer)
{
    server = newServer;
}
