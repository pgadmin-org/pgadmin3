//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
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

// Icons
#include "images/pgAdmin3.xpm"





BEGIN_EVENT_TABLE(dlgConnect, DialogWithHelp)
    EVT_BUTTON (XRCID("btnOK"),       dlgConnect::OnOK)
    EVT_BUTTON (XRCID("btnCancel"),   dlgConnect::OnCancel)
    EVT_CHECKBOX(XRCID("chkNeedPwd"), dlgConnect::OnTrustChange)
END_EVENT_TABLE()


#define stDescription   CTRL_STATIC("stDescription")
#define chkNeedPwd      CTRL_CHECKBOX("chkNeedPwd")
#define txtPassword     CTRL_TEXT("txtPassword")



dlgConnect::dlgConnect(frmMain *form, const wxString& description, bool needPwd) : 
DialogWithHelp(form)
{
    wxLogInfo(wxT("Creating a connect dialogue"));

    wxWindowBase::SetFont(settings->GetSystemFont());
    LoadResource((wxWindow*)form, wxT("dlgConnect"));

    // Icon
    SetIcon(wxIcon(pgAdmin3_xpm));
    CenterOnParent();

    // Setup the default values
    stDescription->SetLabel(description);
    chkNeedPwd->SetValue(needPwd);
    txtPassword->Enable(needPwd);

}

dlgConnect::~dlgConnect()
{
    wxLogInfo(wxT("Destroying a connect dialogue"));
}


wxString dlgConnect::GetHelpPage() const
{
    return wxT("server-connect");
}


void dlgConnect::OnTrustChange(wxCommandEvent& ev)
{
    txtPassword->Enable(chkNeedPwd->GetValue());
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

bool dlgConnect::GetNeedPwd()
{
    return chkNeedPwd->GetValue();
}
