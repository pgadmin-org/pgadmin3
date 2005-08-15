//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgConnect.cpp - Connect to a database
//
//////////////////////////////////////////////////////////////////////////



// App headers
#include "pgAdmin3.h"

#include "dlgConnect.h"
#include "pgConn.h"
#include "pgServer.h"
#include "sysLogger.h"



BEGIN_EVENT_TABLE(dlgConnect, DialogWithHelp)
    EVT_BUTTON (wxID_OK,               dlgConnect::OnOK)
    EVT_BUTTON (wxID_CANCEL,           dlgConnect::OnCancel)
END_EVENT_TABLE()


#define stDescription   CTRL_STATIC("stDescription")
#define chkStorePwd     CTRL_CHECKBOX("chkStorePwd")
#define txtPassword     CTRL_TEXT("txtPassword")



dlgConnect::dlgConnect(frmMain *form, const wxString& description, bool storePwd) : 
DialogWithHelp(form)
{
    wxLogInfo(wxT("Creating a connect dialogue"));

    wxWindowBase::SetFont(settings->GetSystemFont());
    LoadResource((wxWindow*)form, wxT("dlgConnect"));

    appearanceFactory->SetIcons(this);
    CenterOnParent();

    // Setup the default values
    stDescription->SetLabel(description);
    chkStorePwd->SetValue(storePwd);
    txtPassword->Enable(true);

}

dlgConnect::~dlgConnect()
{
    wxLogInfo(wxT("Destroying a connect dialogue"));
}


wxString dlgConnect::GetHelpPage() const
{
    return wxT("pg/server-connect");
}


void dlgConnect::OnOK(wxCommandEvent& ev)
{
    EndModal(wxID_OK);
}


void dlgConnect::OnCancel(wxCommandEvent& ev)
{
    EndModal(wxID_CANCEL);
}

int dlgConnect::Go()
{
    // Set focus on the Password textbox and show modal
    txtPassword->SetFocus();
    return ShowModal();
}

wxString dlgConnect::GetPassword()
{
    return txtPassword->GetValue();
}

bool dlgConnect::GetStorePwd()
{
    return chkStorePwd->GetValue();
}
