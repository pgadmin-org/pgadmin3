//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmConnect.cpp - Connect to a database
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


extern double libpqVersion;


BEGIN_EVENT_TABLE(frmConnect, wxDialog)
    EVT_BUTTON (XRCID("btnHelp"),     frmConnect::OnHelp)
    EVT_BUTTON (XRCID("btnOK"),       frmConnect::OnOK)
    EVT_BUTTON (XRCID("btnCancel"),   frmConnect::OnCancel)
    EVT_CHECKBOX(XRCID("chkTrusted"), frmConnect::OnTrustChange)
END_EVENT_TABLE()


#define txtDescription  CTRL("txtDescription", wxTextCtrl)
#define txtServer       CTRL("txtServer", wxTextCtrl)
#define txtDatabase     CTRL("txtDatabase", wxTextCtrl)
#define txtUsername     CTRL("txtUsername", wxTextCtrl)
#define chkTrusted      CTRL("chkTrusted", wxCheckBox)
#define txtPort         CTRL("txtPort", wxTextCtrl)
#define lblSSL          CTRL("lblSSL", wxStaticText)
#define cbSSL           CTRL("cbSSL", wxComboBox)
#define txtPassword     CTRL("txtPassword", wxTextCtrl)



frmConnect::frmConnect(wxFrame *form, const wxString& server, const wxString& description,
                       const wxString& database, const wxString& username, int port, bool trusted, long ssl)
{
    wxLogInfo(wxT("Creating a connect dialogue"));

    wxWindowBase::SetFont(settings->GetSystemFont());
    wxXmlResource::Get()->LoadDialog(this, form, wxT("frmConnect")); 

    // Icon
    SetIcon(wxIcon(pgAdmin3_xpm));
    CenterOnParent();

    // Setup the default values
    txtDescription->SetValue(description);
    txtServer->SetValue(server);
    txtDatabase->SetValue(database);
    txtUsername->SetValue(username);
    chkTrusted->SetValue(trusted);
    txtPort->SetValue(NumToStr((long)port));
    txtPassword->Enable(!trusted);

    cbSSL->Append(wxEmptyString);

#ifdef SSL
    cbSSL->Append(_("require"));
    cbSSL->Append(_("prefer"));

    if (libpqVersion > 7.3)
    {
        cbSSL->Append(_("allow"));
        cbSSL->Append(_("disable"));
    }
#endif

    if (ssl > cbSSL->GetCount() || ssl < 0)
        ssl = 0;
    cbSSL->SetSelection(ssl);
}

frmConnect::~frmConnect()
{
    wxLogInfo(wxT("Destroying a connect dialogue"));
}


void frmConnect::OnHelp(wxCommandEvent &ev)
{
    DisplayHelp(this, wxT("connect"));
}


void frmConnect::OnTrustChange(wxNotifyEvent& ev)
{
    txtPassword->Enable(!chkTrusted->GetValue());
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
    txtPassword->SetFocus();
    return ShowModal();
}

bool frmConnect::TransferDataFromWindow()
{
    // Store the connection settings
    extern sysSettings *settings;
    settings->SetLastDescription(txtDescription->GetValue());
    settings->SetLastServer(txtServer->GetValue());
    settings->SetLastDatabase(txtDatabase->GetValue());
    settings->SetLastUsername(txtUsername->GetValue());
    settings->SetLastPort(StrToLong(txtPort->GetValue()));
    settings->SetLastSSL(cbSSL->GetSelection());
    return true;
}

wxString frmConnect::GetDescription()
{
    return txtDescription->GetValue();
}

wxString frmConnect::GetServer()
{
    return txtServer->GetValue();
}

wxString frmConnect::GetDatabase()
{
    return txtDatabase->GetValue();
}

wxString frmConnect::GetUsername()
{
    return txtUsername->GetValue();
}

wxString frmConnect::GetPassword()
{
    return txtPassword->GetValue();
}

bool frmConnect::GetTrusted()
{
    return chkTrusted->GetValue();
}

long frmConnect::GetPort()
{
    return StrToLong(txtPort->GetValue());
}

long frmConnect::GetSSL()
{
    return cbSSL->GetSelection();
}


void frmConnect::LockFields()
{
    wxColour colBack;
    colBack = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);

    txtDescription->SetEditable(FALSE);
    txtDescription->SetBackgroundColour(colBack);
    txtServer->SetEditable(FALSE);
    txtServer->SetBackgroundColour(colBack);
    txtDatabase->SetEditable(FALSE);
    txtDatabase->SetBackgroundColour(colBack);
    txtPort->SetEditable(FALSE);
    txtPort->SetBackgroundColour(colBack);
    txtUsername->SetEditable(FALSE);
    txtUsername->SetBackgroundColour(colBack);
    chkTrusted->Disable();
    cbSSL->Disable();
    this->Refresh();
}
