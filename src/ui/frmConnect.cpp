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
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>


// App headers
#include "pgAdmin3.h"
#include "frmConnect.h"
#include "pgConn.h"
#include "pgServer.h"
#include "sysLogger.h"

// Icons
#include "images/pgAdmin3.xpm"



BEGIN_EVENT_TABLE(frmConnect, wxDialog)
    EVT_BUTTON (XRCID("btnOK"),       frmConnect::OnOK)
    EVT_BUTTON (XRCID("btnCancel"),   frmConnect::OnCancel)
    EVT_CHECKBOX(XRCID("chkTrusted"), frmConnect::OnTrustChange)
END_EVENT_TABLE()




frmConnect::frmConnect(wxFrame *form, const wxString& server, const wxString& description,
                       const wxString& database, const wxString& username, int port, bool trusted)
{
    wxLogInfo(wxT("Creating a connect dialogue"));

    wxXmlResource::Get()->LoadDialog(this, form, "frmConnect"); 

    // Icon
    SetIcon(wxIcon(pgAdmin3_xpm));
    CenterOnParent();

    // Setup the default values
    XRCCTRL(*this, "txtDescription", wxTextCtrl)->SetValue(description);
    XRCCTRL(*this, "txtServer", wxTextCtrl)->SetValue(server);
    XRCCTRL(*this, "txtDatabase", wxTextCtrl)->SetValue(database);
    XRCCTRL(*this, "txtUsername", wxTextCtrl)->SetValue(username);
    XRCCTRL(*this, "chkTrusted", wxCheckBox)->SetValue(trusted);
    wxString sport;
    sport.Printf("%d", port);
    XRCCTRL(*this, "txtPort", wxTextCtrl)->SetValue(sport);
    XRCCTRL(*this, "txtPassword", wxTextCtrl)->Enable(!trusted);
}

frmConnect::~frmConnect()
{
    wxLogInfo(wxT("Destroying a connect dialogue"));
}



void frmConnect::OnTrustChange(wxNotifyEvent& ev)
{
    XRCCTRL(*this, "txtPassword", wxTextCtrl)->Enable(
        ! XRCCTRL(*this, "chkTrusted", wxCheckBox)->GetValue());
}


void frmConnect::OnOK(wxCommandEvent& ev)
{
    TransferDataFromWindow();
    EndModal(wxID_OK);
    Destroy();
}


void frmConnect::OnCancel(wxCommandEvent& ev)
{
    EndModal(wxID_CANCEL);
    Destroy();
}

int frmConnect::Go()
{
    // Set focus on the Password textbox and show modal
    XRCCTRL(*this, "txtPassword", wxTextCtrl)->SetFocus();
    return ShowModal();
}

bool frmConnect::TransferDataFromWindow()
{
    // Store the connection settings
    extern sysSettings *settings;
    settings->SetLastServer(XRCCTRL(*this, "txtServer", wxTextCtrl)->GetValue());
    settings->SetLastDatabase(XRCCTRL(*this, "txtDatabase", wxTextCtrl)->GetValue());
    settings->SetLastUsername(XRCCTRL(*this, "txtUsername", wxTextCtrl)->GetValue());
    settings->SetLastPort(atoi(XRCCTRL(*this, "txtPort", wxTextCtrl)->GetValue().c_str()));
    return true;
}

wxString frmConnect::GetDescription()
{
    return XRCCTRL(*this, "txtDescription", wxTextCtrl)->GetValue();
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

bool frmConnect::GetTrusted()
{
    return XRCCTRL(*this, "chkTrusted", wxCheckBox)->GetValue();
}

long frmConnect::GetPort()
{
    return atoi(XRCCTRL(*this, "txtPort", wxTextCtrl)->GetValue());
}

void frmConnect::LockFields()
{
    wxColour colBack;
    colBack = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);

    XRCCTRL(*this, "txtDescription", wxTextCtrl)->SetEditable(FALSE);
    XRCCTRL(*this, "txtDescription", wxTextCtrl)->SetBackgroundColour(colBack);
    XRCCTRL(*this, "txtServer", wxTextCtrl)->SetEditable(FALSE);
    XRCCTRL(*this, "txtServer", wxTextCtrl)->SetBackgroundColour(colBack);
    XRCCTRL(*this, "txtDatabase", wxTextCtrl)->SetEditable(FALSE);
    XRCCTRL(*this, "txtDatabase", wxTextCtrl)->SetBackgroundColour(colBack);
    XRCCTRL(*this, "txtPort", wxTextCtrl)->SetEditable(FALSE);
    XRCCTRL(*this, "txtPort", wxTextCtrl)->SetBackgroundColour(colBack);
    XRCCTRL(*this, "txtUsername", wxTextCtrl)->SetEditable(FALSE);
    XRCCTRL(*this, "txtUsername", wxTextCtrl)->SetBackgroundColour(colBack);
    XRCCTRL(*this, "chkTrusted", wxCheckBox)->Disable();
    this->Refresh();
}
