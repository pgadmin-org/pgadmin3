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
    EVT_INIT_DIALOG(frmConnect::Init)
    EVT_BUTTON (XRCID("btnOK"), frmConnect::OnOK)
    EVT_BUTTON (XRCID("btnCancel"), frmConnect::OnCancel)
END_EVENT_TABLE()

frmConnect::frmConnect(pgServer *parent, const wxString& server, const wxString& database, const wxString& username, int port)
{

    wxLogInfo(wxT("Creating a connect dialogue"));
    extern sysSettings *settings;

    wxXmlResource::Get()->LoadDialog(this, (wxFrame *) NULL, "frmConnect"); 

    // Icon
    SetIcon(wxIcon(pgAdmin3_xpm));
    Center();

    // Setup the default values
    XRCCTRL(*this, "txtServer", wxTextCtrl)->SetValue(server);
    XRCCTRL(*this, "txtDatabase", wxTextCtrl)->SetValue(database);
    XRCCTRL(*this, "txtUsername", wxTextCtrl)->SetValue(username);
    wxString sport;
    sport.Printf("%d", port);
    XRCCTRL(*this, "txtPort", wxTextCtrl)->SetValue(sport);

    objParent = parent;
}

frmConnect::~frmConnect()
{
    wxLogInfo(wxT("Destroying a connect dialogue"));
}


void frmConnect::Init()
{

    // Set focus on the Password texbox.
    XRCCTRL(*this, "txtPassword", wxTextCtrl)->SetFocus();
}

void frmConnect::OnOK()
{

    // Store the connection settings
    extern sysSettings *settings;
    settings->SetLastServer(XRCCTRL(*this, "txtServer", wxTextCtrl)->GetValue());
    settings->SetLastDatabase(XRCCTRL(*this, "txtDatabase", wxTextCtrl)->GetValue());
    settings->SetLastUsername(XRCCTRL(*this, "txtUsername", wxTextCtrl)->GetValue());
    settings->SetLastPort(atoi(XRCCTRL(*this, "txtPort", wxTextCtrl)->GetValue().c_str()));

    // Pass the settings back to the pgServer that called me
    objParent->iSetName(XRCCTRL(*this, "txtServer", wxTextCtrl)->GetValue());
    objParent->iSetDatabase(XRCCTRL(*this, "txtDatabase", wxTextCtrl)->GetValue());
    objParent->iSetUsername(XRCCTRL(*this, "txtUsername", wxTextCtrl)->GetValue());
    objParent->iSetPassword(XRCCTRL(*this, "txtPassword", wxTextCtrl)->GetValue());
    objParent->iSetPort(atoi(XRCCTRL(*this, "txtPort", wxTextCtrl)->GetValue().c_str()));

    // Close the dialogue
    this->EndModal(wxID_OK);
}

void frmConnect::OnCancel()
{
    this->EndModal(wxID_CANCEL);
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