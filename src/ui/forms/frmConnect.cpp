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
#include "../../schema/pg/pgServer.h"
#include "../../utils/sysLogger.h"

// Icons
#include "../../images/pgAdmin3.xpm"

BEGIN_EVENT_TABLE(frmConnect, wxDialog)
    EVT_INIT_DIALOG(frmConnect::Init)
    EVT_BUTTON (XRCID("btnOK"), frmConnect::OK)
    EVT_BUTTON (XRCID("btnCancel"), frmConnect::Cancel)
END_EVENT_TABLE()

frmConnect::frmConnect(pgServer *parent, const wxString& szServer, const wxString& szDatabase, const wxString& szUsername, int iPort)
{

    wxLogInfo(wxT("Creating a connect dialogue"));
    extern sysSettings *objSettings;

    wxXmlResource::Get()->LoadDialog(this, (wxFrame *) NULL, "frmConnect"); 

    // Icon
    SetIcon(wxIcon(pgAdmin3_xpm));
    Center();

    // Setup the default values
    XRCCTRL(*this, "txtServer", wxTextCtrl)->SetValue(szServer);
    XRCCTRL(*this, "txtDatabase", wxTextCtrl)->SetValue(szDatabase);
    XRCCTRL(*this, "txtUsername", wxTextCtrl)->SetValue(szUsername);
    wxString szPort;
    szPort.Printf("%d", iPort);
    XRCCTRL(*this, "txtPort", wxTextCtrl)->SetValue(szPort);
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

void frmConnect::OK()
{
    // Store the connection settings
    extern sysSettings *objSettings;
    objSettings->SetLastServer(XRCCTRL(*this, "txtServer", wxTextCtrl)->GetValue());
    objSettings->SetLastDatabase(XRCCTRL(*this, "txtDatabase", wxTextCtrl)->GetValue());
    objSettings->SetLastUsername(XRCCTRL(*this, "txtUsername", wxTextCtrl)->GetValue());
    objSettings->SetLastPort(atoi(XRCCTRL(*this, "txtPort", wxTextCtrl)->GetValue().c_str()));

    // Close the dialogue
    this->EndModal(0);
}

void frmConnect::Cancel()
{
    this->EndModal(1);
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
