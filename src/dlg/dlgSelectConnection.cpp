//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id:  $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgSelectConnection.cpp - Connect to a database
//
//////////////////////////////////////////////////////////////////////////



// App headers
#include "pgAdmin3.h"

#include "frmMain.h"
#include "dlgSelectConnection.h"
#include "dlgConnect.h"
#include "pgServer.h"
#include "images/connect.xpm"


BEGIN_EVENT_TABLE(dlgSelectConnection, DialogWithHelp)
    EVT_COMBOBOX(XRCID("cbServer"),   dlgSelectConnection::OnChangeServer) 
    EVT_COMBOBOX(XRCID("cbDatabase"),   dlgSelectConnection::OnChangeDatabase) 
	EVT_TEXT(XRCID("cbServer"),        dlgSelectConnection::OnTextChange)
	EVT_TEXT(XRCID("cbDatabase"),      dlgSelectConnection::OnTextChange)
    EVT_BUTTON (wxID_OK,               dlgSelectConnection::OnOK)
    EVT_BUTTON (wxID_CANCEL,           dlgSelectConnection::OnCancel)
END_EVENT_TABLE()


#define cbServer        CTRL_COMBOBOX("cbServer")
#define cbDatabase      CTRL_COMBOBOX("cbDatabase")
#define stUsername		CTRL_STATIC("stUsername")
#define txtUsername		CTRL_TEXT("txtUsername")



dlgSelectConnection::dlgSelectConnection(wxWindow *parent, frmMain *form) : 
DialogWithHelp(form)
{
    wxLogInfo(wxT("Creating a select connection dialogue"));

	remoteServer = NULL;

    wxWindowBase::SetFont(settings->GetSystemFont());
    LoadResource(parent, wxT("dlgSelectConnection"));

    SetIcon(wxIcon(connect_xpm));
    CenterOnParent();

	if (form != NULL)
	{
		stUsername->Hide();
		txtUsername->Hide();
	}
	else
	{
		cbServer->SetValue(settings->Read(wxT("QuickConnect/server"), wxEmptyString));
		cbDatabase->SetValue(settings->Read(wxT("QuickConnect/database"), wxEmptyString));
		txtUsername->SetValue(settings->Read(wxT("QuickConnect/username"), wxEmptyString));
	}
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
	if (!GetServer())
		return;

    cbDatabase->Clear();

    int sel=cbServer->GetCurrentSelection();
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
            pgSetIterator set(remoteServer->GetConnection(), 
                wxT("SELECT DISTINCT datname\n")
                wxT("  FROM pg_database db\n")
                wxT(" WHERE datallowconn ORDER BY datname"));

            while(set.RowsLeft())
            {
                wxString dbName=set.GetVal(wxT("datname"));

                bool alreadyConnected=false;

                if (cbConnection)
                {
                    unsigned int i;

                    for (i=0 ; i < cbConnection->GetCount()-1 ; i++)
                    {
                        pgConn *conn=(pgConn*)cbConnection->GetClientData(i);
                        if (conn->GetHost() == remoteServer->GetName() && conn->GetDbname() == dbName)
                        {
                            alreadyConnected=true;
                            break;
                        }
                    }
                }
                if (!alreadyConnected)
                    cbDatabase->Append(dbName);
            }
            if (cbDatabase->GetCount())
                cbDatabase->SetSelection(0);
        }

    }
    OnChangeDatabase(ev);
}


wxString dlgSelectConnection::GetDatabase()
{
    return cbDatabase->GetValue();
}

wxString dlgSelectConnection::GetServerName()
{
	return cbServer->GetValue();
}

void dlgSelectConnection::OnChangeDatabase(wxCommandEvent& ev)
{
    btnOK->Enable(cbDatabase->GetCount() > 0 && cbDatabase->GetCurrentSelection() >= 0);
}

void dlgSelectConnection::OnTextChange(wxCommandEvent& ev)
{
	btnOK->Enable(cbDatabase->GetValue().Len() > 0 && cbServer->GetValue().Len() > 0 && txtUsername->GetValue().Len() > 0);
}

void dlgSelectConnection::OnOK(wxCommandEvent& ev)
{
	if (cbDatabase->GetCurrentSelection() == wxNOT_FOUND ||
		cbServer->GetCurrentSelection() == wxNOT_FOUND)
		remoteServer = NULL;
    EndModal(wxID_OK);
}


void dlgSelectConnection::OnCancel(wxCommandEvent& ev)
{
    EndModal(wxID_CANCEL);
}

pgConn *dlgSelectConnection::CreateConn()
{
	if (GetServer())	/* Running with access to the main form with the object tree */
		return GetServer()->CreateConn(GetDatabase());
	else
    {
        /* gcc requires that we store this in temporary variables for some reason... */
        wxString serv = cbServer->GetValue();
        wxString db = cbServer->GetValue();
        wxString user = txtUsername->GetValue();
		return CreateConn(serv, db, user, 0, 0, true);
    }
}

pgConn *dlgSelectConnection::CreateConn(wxString& server, wxString& dbname, wxString& username, int port, int sslmode, bool writeMRU)
{
	pgConn *newconn;
	newconn = new pgConn(server, dbname, username, wxT(""), port, sslmode);
	if (newconn->GetStatus() != PGCONN_OK &&
		newconn->GetLastError().Cmp(wxString(PQnoPasswordSupplied, wxConvUTF8)) == 0)
	{
		/* Needs password */
		delete newconn;
		newconn = NULL;

        wxString txt;
        txt.Printf(_("Please enter password for user %s\non server %s"), username.c_str(), server.c_str());
		dlgConnect dlg(NULL, txt, false);
		if (dlg.Go() != wxID_OK)
			return NULL;

		newconn = new pgConn(server, dbname, username, dlg.GetPassword(), port, sslmode);
	}

	if (newconn)
	{
		if (newconn->GetStatus() != PGCONN_OK)
		{
			wxMessageBox(wxT("Connection failed: ") + newconn->GetLastError());
			return NULL;
		}

		if (writeMRU)
		{
			settings->Write(wxT("QuickConnect/server"), cbServer->GetValue());
			settings->Write(wxT("QuickConnect/database"), cbDatabase->GetValue());
			settings->Write(wxT("QuickConnect/username"), txtUsername->GetValue());
		}
	}
	return newconn;
}

int dlgSelectConnection::Go(pgConn *conn, ctlComboBoxFix *cb)
{
    cbConnection=cb;
	if (mainForm != NULL)
	{
		treeObjectIterator servers(mainForm->GetBrowser(), mainForm->GetServerCollection());
		pgServer *s;

		while ((s=(pgServer*)servers.GetNextObject()) != 0)
		{
			cbServer->Append(s->GetIdentifier(), (void*)s);
			if (s->GetConnected() && s->GetConnection()->GetHost() == conn->GetHost() && s->GetConnection()->GetPort() == conn->GetPort())
			{
				 cbServer->SetSelection(cbServer->GetCount()-1);
			}
		}    
		cbServer->SetFocus();
	}

    wxCommandEvent ev;
    OnChangeServer(ev);

    return ShowModal();
}

