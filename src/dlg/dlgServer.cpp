//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgServer.cpp - PostgreSQL Database Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "frmMain.h"
#include "dlgServer.h"
#include "pgDatabase.h"

// Images
#include "images/server.xpm"


// pointer to controls
#define txtDescription  CTRL_TEXT("txtDescription")
#define txtService      CTRL_TEXT("txtService")
#define cbDatabase      CTRL_COMBOBOX("cbDatabase")
#define txtPort         CTRL_TEXT("txtPort")
#define cbSSL           CTRL_COMBOBOX("cbSSL")
#define txtUsername     CTRL_TEXT("txtUsername")
#define chkNeedPwd      CTRL_CHECKBOX("chkNeedPwd")
#define stPassword      CTRL_STATIC("stPassword")
#define txtPassword     CTRL_TEXT("txtPassword")


extern double libpqVersion;


BEGIN_EVENT_TABLE(dlgServer, dlgProperty)
    EVT_NOTEBOOK_PAGE_CHANGED(XRCID("nbNotebook"),  dlgServer::OnPageSelect)  
    EVT_TEXT(XRCID("txtDescription"),               dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtService"),                   dlgProperty::OnChange)
    EVT_TEXT(XRCID("cbDatabase"),                   dlgProperty::OnChange)
    EVT_COMBOBOX(XRCID("cbDatabase"),               dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtPort")  ,                    dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtUsername"),                  dlgProperty::OnChange)
    EVT_COMBOBOX(XRCID("cbSSL"),                    dlgProperty::OnChange)
    EVT_CHECKBOX(XRCID("chkNeedPwd"),               dlgServer::OnChangeNeedPwd)
    EVT_BUTTON(wxID_OK,                             dlgServer::OnOK)
END_EVENT_TABLE();


dlgServer::dlgServer(frmMain *frame, pgServer *node)
: dlgProperty(frame, wxT("dlgServer"))
{
    SetIcon(wxIcon(server_xpm));
    server=node;
    objectType = PG_SERVER;

    cbDatabase->Append(settings->GetLastDatabase());
    cbDatabase->SetSelection(0);
    txtPort->SetValue(NumToStr((long)settings->GetLastPort()));    
    cbSSL->SetSelection(settings->GetLastSSL());
    txtUsername->SetValue(settings->GetLastUsername());
    chkNeedPwd->SetValue(true);
}


dlgServer::~dlgServer()
{
    if (!server)
    {
        settings->SetLastDatabase(cbDatabase->GetValue());
        settings->SetLastPort(StrToLong(txtPort->GetValue()));
        settings->SetLastSSL(cbSSL->GetSelection());
        settings->SetLastUsername(txtUsername->GetValue());
    }
}


pgObject *dlgServer::GetObject()
{
    return server;
}


void dlgServer::OnOK(wxCommandEvent &ev)
{
    // notice: changes active after reconnect

    EnableOK(false);


    if (server)
    {
        server->iSetName(GetName());
        server->iSetDescription(txtDescription->GetValue());
        if (txtService->GetValue() != server->GetServiceID())
        {
            mainForm->StartMsg(_("Checking server status"));
            server->iSetServiceID(txtService->GetValue());
            mainForm->EndMsg();
        }
        server->iSetPort(StrToLong(txtPort->GetValue()));
        server->iSetSSL(cbSSL->GetSelection());
        server->iSetLastDatabase(cbDatabase->GetValue());
        server->iSetUsername(txtUsername->GetValue());
        server->iSetNeedPwd(chkNeedPwd->GetValue());
        mainForm->execSelChange(server->GetId(), true);
        mainForm->GetBrowser()->SetItemText(item, server->GetFullName());
    }

    if (IsModal())
    {
        EndModal(wxID_OK);
        return;
    }
    else
        Destroy();
}


void dlgServer::OnPageSelect(wxNotebookEvent &event)
{
    // to prevent dlgProperty from catching it
}


wxString dlgServer::GetHelpPage() const
{
    return wxT("pgadmin/connect");
}


int dlgServer::GoNew()
{
    if (cbSSL->IsEmpty())
        return Go(true);
    else
    {
        CheckChange();
        return ShowModal();
    }
}


int dlgServer::Go(bool modal)
{
    cbSSL->Append(wxT(" "));

#ifdef SSL
    cbSSL->Append(_("require"));
    cbSSL->Append(_("prefer"));

    if (libpqVersion > 7.3)
    {
        cbSSL->Append(_("allow"));
        cbSSL->Append(_("disable"));
    }
#endif

    if (server)
    {
        cbDatabase->Append(server->GetDatabaseName());
        txtDescription->SetValue(server->GetDescription());
        txtService->SetValue(server->GetServiceID());
        txtPort->SetValue(NumToStr((long)server->GetPort()));
        cbSSL->SetSelection(server->GetSSL());
        cbDatabase->SetValue(server->GetDatabaseName());
        txtUsername->SetValue(server->GetUsername());
        chkNeedPwd->SetValue(server->GetNeedPwd());
        stPassword->Disable();
        txtPassword->Disable();
        if (connection)
        {
            txtName->Disable();
            cbDatabase->Disable();
            txtPort->Disable();
            cbSSL->Disable();
            txtUsername->Disable();
            chkNeedPwd->Disable();
        }
    }
    else
    {
        SetTitle(_("Add server"));
    }

    int rc=dlgProperty::Go(modal);

    CheckChange();

    return rc;
}


wxString dlgServer::GetPassword()
{
    if (chkNeedPwd->GetValue())
        return txtPassword->GetValue();
    return wxEmptyString;
}


pgObject *dlgServer::CreateObject(pgCollection *collection)
{
    wxString name=GetName();

    pgObject *obj=new pgServer(GetName(), txtDescription->GetValue(), cbDatabase->GetValue(), 
        txtUsername->GetValue(), StrToLong(txtPort->GetValue()), !chkNeedPwd->GetValue(), cbSSL->GetSelection());

    return obj;
}


void dlgServer::OnChangeNeedPwd(wxCommandEvent &ev)
{
    if (!server)
        txtPassword->Enable(chkNeedPwd->GetValue());
    OnChange(ev);
}


void dlgServer::CheckChange()
{
    wxString name=GetName();
    bool enable=true;

    if (server)
    {
        enable =  name != server->GetName()
               || txtDescription->GetValue() != server->GetDescription()
               || txtService->GetValue() != server->GetServiceID()
               || StrToLong(txtPort->GetValue()) != server->GetPort()
               || cbDatabase->GetValue() != server->GetDatabaseName()
               || txtUsername->GetValue() != server->GetUsername()
               || cbSSL->GetSelection() != server->GetSSL()
               || chkNeedPwd->GetValue() != server->GetNeedPwd();
    }

    CheckValid(enable, !txtDescription->GetValue().IsEmpty(), _("Please specify description."));

#ifdef __WXMSW__
    CheckValid(enable, !name.IsEmpty(), _("Please specify address."));
#else
    bool isPipe = (name.IsEmpty() || name.StartsWith(wxT("/")));
    cbSSL->Enable(!isPipe);
#endif
    CheckValid(enable, StrToLong(txtPort->GetValue()) > 0, _("Please specify port."));
    CheckValid(enable, !txtUsername->GetValue().IsEmpty(), _("Please specify user name"));

    EnableOK(enable);
}


wxString dlgServer::GetSql()
{
    return wxEmptyString;
}
