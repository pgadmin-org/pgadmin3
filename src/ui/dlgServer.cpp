//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
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
#define txtPassword     CTRL_CHECKBOX("txtPassword")


extern double libpqVersion;


BEGIN_EVENT_TABLE(dlgServer, dlgProperty)
    EVT_TEXT(XRCID("txtDescription"),               dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtService"),                   dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtDatabase"),                  dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtServer"),                    dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtServer"),                    dlgProperty::OnChange)
    EVT_NOTEBOOK_PAGE_CHANGED(XRCID("nbNotebook"),  dlgServer::OnPageSelect)  
    EVT_BUTTON(XRCID("btnOK"),                      dlgServer::OnOK)
END_EVENT_TABLE();


dlgServer::dlgServer(frmMain *frame, pgServer *node)
: dlgProperty(frame, wxT("dlgServer"))
{
    SetIcon(wxIcon(server_xpm));
    server=node;
}


pgObject *dlgServer::GetObject()
{
    return server;
}


void dlgServer::OnOK(wxCommandEvent &ev)
{
    // notice: changes active after reconnect

    if (server)
    {
        server->iSetDescription(txtDescription->GetValue());
        server->iSetServiceID(txtService->GetValue());
        server->iSetPort(StrToLong(txtPort->GetValue()));
        server->iSetSSL(cbSSL->GetSelection());
        server->iSetLastDatabase(cbDatabase->GetValue());
        server->iSetUsername(txtUsername->GetValue());
        server->iSetNeedPwd(chkNeedPwd->GetValue());
        stPassword->Disable();
        txtPassword->Disable();
    }

    dlgProperty::OnOK(ev);
}


void dlgServer::OnPageSelect(wxNotebookEvent &event)
{
    // to prevent dlgProperty from catching it
}


wxString dlgServer::GetHelpPage() const
{
    return dlgProperty::GetHelpPage();
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
        if (connection)
        {
            pgSet *set=connection->ExecuteSet(
                wxT("SELECT datname FROM pg_database WHERE datallowconn ORDER BY datname"));
            if (set)
            {
                while (!set->Eof())
                {
                    cbDatabase->Append(set->GetVal(0));
                    set->MoveNext();
                }
                delete set;
            }
        }
        else
            cbDatabase->Append(server->GetDatabaseName());

        txtDescription->SetValue(server->GetDescription());
        txtService->SetValue(server->GetServiceID());
        txtPort->SetValue(NumToStr((long)server->GetPort()));
        cbSSL->SetSelection(server->GetSSL());
        cbDatabase->SetValue(server->GetDatabaseName());
        txtUsername->SetValue(server->GetUsername());
        chkNeedPwd->SetValue(server->GetNeedPwd());
    }
    else
    {
        cbDatabase->Append(wxT("template1"));

        txtPort->SetValue(wxT("5432"));    
        cbSSL->SetSelection(0);
        cbDatabase->SetSelection(0);
        txtUsername->SetValue(wxT("postgres"));
        chkNeedPwd->SetValue(true);
    }

    return dlgProperty::Go(modal);
}


pgObject *dlgServer::CreateObject(pgCollection *collection)
{
    wxString name=GetName();

    pgObject *obj=0; //pgDatabase::ReadObjects(collection, 0, wxT(" WHERE datname=") + qtString(name) + wxT("\n"));
    return obj;
}


void dlgServer::CheckChange()
{
    wxString name=GetName();
    bool enable=true;

    if (server)
    {
        enable =  name != server->GetName()
               || txtDescription->GetValue() != server->GetDescription()
               || StrToLong(txtPort->GetValue()) != server->GetPort()
               || txtUsername->GetValue() != server->GetUsername()
               || chkNeedPwd->GetValue() != server->GetNeedPwd();
    }

    CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
    CheckValid(enable, !txtDescription->GetValue().IsEmpty(), _("Please specify description."));
    CheckValid(enable, StrToLong(txtPort->GetValue()) > 0, _("Please specify port."));
    CheckValid(enable, !txtUsername->GetValue().IsEmpty(), _("Please specify user name"));

    EnableOK(enable);
}


wxString dlgServer::GetSql()
{
    return wxEmptyString;
}
