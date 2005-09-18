//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id:  $
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgSelectConnection.cpp - Connect to a database
//
//////////////////////////////////////////////////////////////////////////



// App headers
#include "pgAdmin3.h"

#include "frmMain.h"
#include "dlgSelectConnection.h"
#include "pgServer.h"



BEGIN_EVENT_TABLE(dlgSelectConnection, DialogWithHelp)
    EVT_COMBOBOX(XRCID("cbServer"),   dlgSelectConnection::OnChangeServer) 
    EVT_COMBOBOX(XRCID("cbDatabase"),   dlgSelectConnection::OnChangeDatabase) 
    EVT_BUTTON (wxID_OK,               dlgSelectConnection::OnOK)
    EVT_BUTTON (wxID_CANCEL,           dlgSelectConnection::OnCancel)
END_EVENT_TABLE()


#define cbServer        CTRL_COMBOBOX("cbServer")
#define cbDatabase      CTRL_COMBOBOX("cbDatabase")



dlgSelectConnection::dlgSelectConnection(frmMain *form) : 
DialogWithHelp(form)
{
    wxLogInfo(wxT("Creating a select connection dialogue"));

    wxWindowBase::SetFont(settings->GetSystemFont());
    LoadResource((wxWindow*)form, wxT("dlgSelectConnection"));

    appearanceFactory->SetIcons(this);
    CenterOnParent();
}


dlgSelectConnection::~dlgSelectConnection()
{
    wxLogInfo(wxT("Destroying a select connection dialogue"));
}


wxString dlgSelectConnection::GetHelpPage() const
{
    return wxT("pg/server-connect");
}


void dlgSelectConnection::OnChangeServer(wxCommandEvent& ev)
{
    cbDatabase->Clear();

    int sel=cbServer->GetSelection();
    if (sel >= 0)
    {
        remoteServer = (pgServer*)cbServer->GetClientData(sel);

        if (!remoteServer->GetConnected())
        {
            remoteServer->Connect(mainForm, remoteServer->GetStorePwd());
            if (!remoteServer->GetConnected())
            {
                wxLogError(remoteServer->GetLastError());
                return;
            }
        }
        if (remoteServer->GetConnected())
        {
            pgSet *set=remoteServer->ExecuteSet(
                wxT("SELECT DISTINCT datname\n")
                wxT("  FROM pg_database db\n")
                wxT(" WHERE datallowconn ORDER BY datname"));
            if (set)
            {
                while (!set->Eof())
                {
                    cbDatabase->Append(set->GetVal(wxT("datname")));
                    set->MoveNext();
                }
                delete set;

                cbDatabase->SetSelection(0);
            }
        }

    }
    OnChangeDatabase(ev);
}


wxString dlgSelectConnection::GetDatabase()
{
    return cbDatabase->GetValue();
}


void dlgSelectConnection::OnChangeDatabase(wxCommandEvent& ev)
{
    btnOK->Enable(cbDatabase->GetCount() > 0 && cbDatabase->GetSelection() >= 0);
}


void dlgSelectConnection::OnOK(wxCommandEvent& ev)
{
    EndModal(wxID_OK);
}


void dlgSelectConnection::OnCancel(wxCommandEvent& ev)
{
    EndModal(wxID_CANCEL);
}


int dlgSelectConnection::Go()
{
    treeObjectIterator servers(mainForm->GetBrowser(), mainForm->GetServerCollection());
    pgServer *s;

    while ((s=(pgServer*)servers.GetNextObject()) != 0)
        cbServer->Append(s->GetIdentifier(), (void*)s);
    
    cbServer->SetFocus();
    btnOK->Disable();
    return ShowModal();
}

