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


// For some unknown reasons, if compiled as Win32-Release, this event table will not 
// accept any frmConnect-functions! A crash will result if you try.

BEGIN_EVENT_TABLE(frmConnect, wxDialog)
EVT_BUTTON (XRCID("btnOK"),     wxDialog::OnOK)
EVT_BUTTON (XRCID("btnCancel"), wxDialog::OnCancel)
END_EVENT_TABLE()




frmConnect::frmConnect(wxFrame *form, const wxString& server, const wxString& database, const wxString& username, int port)
{
    wxLogInfo(wxT("Creating a connect dialogue"));
    extern sysSettings *settings;

    wxXmlResource::Get()->LoadDialog(this, form, "frmConnect"); 

    // Icon
    SetIcon(wxIcon(pgAdmin3_xpm));
    CenterOnParent();

    // Setup the default values
    XRCCTRL(*this, "txtServer", wxTextCtrl)->SetValue(server);
    XRCCTRL(*this, "txtDatabase", wxTextCtrl)->SetValue(database);
    XRCCTRL(*this, "txtUsername", wxTextCtrl)->SetValue(username);
    wxString sport;
    sport.Printf("%d", port);
    XRCCTRL(*this, "txtPort", wxTextCtrl)->SetValue(sport);
}

frmConnect::~frmConnect()
{
    wxLogInfo(wxT("Destroying a connect dialogue"));
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

void frmConnect::LockFields()
{
    wxColour colBack;
    colBack = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);

    XRCCTRL(*this, "txtServer", wxTextCtrl)->SetEditable(FALSE);
    XRCCTRL(*this, "txtServer", wxTextCtrl)->SetBackgroundColour(colBack);
    XRCCTRL(*this, "txtDatabase", wxTextCtrl)->SetEditable(FALSE);
    XRCCTRL(*this, "txtDatabase", wxTextCtrl)->SetBackgroundColour(colBack);
    XRCCTRL(*this, "txtPort", wxTextCtrl)->SetEditable(FALSE);
    XRCCTRL(*this, "txtPort", wxTextCtrl)->SetBackgroundColour(colBack);
    XRCCTRL(*this, "txtUsername", wxTextCtrl)->SetEditable(FALSE);
    XRCCTRL(*this, "txtUsername", wxTextCtrl)->SetBackgroundColour(colBack);

    this->Refresh();
}