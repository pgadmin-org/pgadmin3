//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgSelectConnection.cpp - Connect to a database
//
//////////////////////////////////////////////////////////////////////////

// App headers
#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/bmpcbox.h>

// App headers
#include "frm/frmMain.h"
#include "dlg/dlgSelectConnection.h"
#include "dlg/dlgConnect.h"
#include "schema/pgServer.h"
#include "images/connect.pngc"

#define CTRLID_CBSERVER 4242
#define CTRLID_CBDATABASE 4243
#define CTRLID_CBUSERNAME 4244
#define CTRLID_CBROLENAME 4245

BEGIN_EVENT_TABLE(dlgSelectConnection, DialogWithHelp)
	EVT_COMBOBOX(CTRLID_CBSERVER,      dlgSelectConnection::OnChangeServer)
	EVT_COMBOBOX(CTRLID_CBDATABASE,    dlgSelectConnection::OnChangeDatabase)
	EVT_TEXT(CTRLID_CBSERVER,          dlgSelectConnection::OnTextChange)
	EVT_TEXT(CTRLID_CBDATABASE,        dlgSelectConnection::OnTextChange)
	EVT_TEXT(CTRLID_CBUSERNAME,        dlgSelectConnection::OnTextChange)
	EVT_BUTTON (wxID_OK,               dlgSelectConnection::OnOK)
	EVT_BUTTON (wxID_CANCEL,           dlgSelectConnection::OnCancel)
END_EVENT_TABLE()

#define stUsername		CTRL_STATIC("stUsername")


dlgSelectConnection::dlgSelectConnection(wxWindow *parent, frmMain *form) :
	DialogWithHelp(form)
{
	long style = wxCB_DROPDOWN;
	remoteServer = NULL;

	SetFont(settings->GetSystemFont());
	LoadResource(parent, wxT("dlgSelectConnection"));

	SetIcon(*connect_png_ico);
	RestorePosition();

	if (form != NULL)
		style |= wxCB_READONLY;

	cbServer = new ctlComboBoxFix(this, CTRLID_CBSERVER, ConvertDialogToPixels(wxPoint(65, 5)), ConvertDialogToPixels(wxSize(135, 12)), style);
	cbDatabase = new wxComboBox(this, CTRLID_CBDATABASE, wxEmptyString, ConvertDialogToPixels(wxPoint(65, 20)), ConvertDialogToPixels(wxSize(135, 12)), wxArrayString(), style);
	cbUsername = new wxComboBox(this, CTRLID_CBUSERNAME, wxEmptyString, ConvertDialogToPixels(wxPoint(65, 35)), ConvertDialogToPixels(wxSize(135, 12)), wxArrayString(), style);
	cbRolename = new wxComboBox(this, CTRLID_CBROLENAME, wxEmptyString, ConvertDialogToPixels(wxPoint(65, 50)), ConvertDialogToPixels(wxSize(135, 12)), wxArrayString(), style);

	if (form == NULL)
	{
		cbServer->SetValue(settings->Read(wxT("QuickConnect/server"), wxEmptyString));
		cbDatabase->SetValue(settings->Read(wxT("QuickConnect/database"), wxEmptyString));
		cbUsername->SetValue(settings->Read(wxT("QuickConnect/username"), wxEmptyString));
		cbRolename->SetValue(settings->Read(wxT("QuickConnect/rolename"), wxEmptyString));
	}

	btnOK->Enable(cbServer->GetValue().Length() > 0 && cbDatabase->GetValue().Length() > 0 && cbUsername->GetValue().Length() > 0);
}


dlgSelectConnection::~dlgSelectConnection()
{
	SavePosition();
}


wxString dlgSelectConnection::GetHelpPage() const
{
	return wxT("connect");
}


void dlgSelectConnection::OnChangeServer(wxCommandEvent &ev)
{
	int item;
	wxString olddatabase, oldusername;

	if (!GetServer())
		return;

	// Keep old value for these comboboxes so that we can restore them if needed
	olddatabase = cbDatabase->GetValue();
	oldusername = cbUsername->GetValue();

	// Clear the comboboxes
	cbDatabase->Clear();
	cbUsername->Clear();
	cbRolename->Clear();

	int sel = cbServer->GetCurrentSelection();
	if (sel >= 0)
	{
		remoteServer = (pgServer *)cbServer->wxItemContainer::GetClientData(sel);

		if (!remoteServer->GetConnected())
		{
			remoteServer->Connect(mainForm, remoteServer->GetStorePwd());
			if (!remoteServer->GetConnected())
			{
				wxLogError(wxT("%s"), remoteServer->GetLastError().c_str());
				return;
			}
		}
		if (remoteServer->GetConnected())
		{
			pgSetIterator set1(remoteServer->GetConnection(),
			                   wxT("SELECT DISTINCT datname\n")
			                   wxT("  FROM pg_database db\n")
			                   wxT(" WHERE datallowconn ORDER BY datname"));

			item = 0;
			while(set1.RowsLeft())
			{
				cbDatabase->Append(set1.GetVal(wxT("datname")));
				if (set1.GetVal(wxT("datname")) == olddatabase)
					item = cbDatabase->GetCount() - 1;
			}

			if (cbDatabase->GetCount())
				cbDatabase->SetSelection(item);

			pgSetIterator set2(remoteServer->GetConnection(),
			                   wxT("SELECT DISTINCT usename\n")
			                   wxT("FROM pg_user db\n")
			                   wxT("ORDER BY usename"));

			item = 0;
			while(set2.RowsLeft())
			{
				cbUsername->Append(set2.GetVal(wxT("usename")));
				if (set2.GetVal(wxT("usename")) == oldusername)
					item = cbDatabase->GetCount() - 1;
			}

			if (cbUsername->GetCount())
				cbUsername->SetSelection(item);

			if (remoteServer->GetConnection()->BackendMinimumVersion(8, 1))
			{
				pgSetIterator set3(remoteServer->GetConnection(),
				                   wxT("SELECT DISTINCT rolname\n")
				                   wxT("FROM pg_roles db\n")
				                   wxT("ORDER BY rolname"));

				cbRolename->Append(wxEmptyString);

				while(set3.RowsLeft())
					cbRolename->Append(set3.GetVal(wxT("rolname")));

				cbRolename->Enable(true);
			}
			else
				cbRolename->Disable();

			cbRolename->SetValue(wxEmptyString);
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

void dlgSelectConnection::OnChangeDatabase(wxCommandEvent &ev)
{
	btnOK->Enable(cbServer->GetValue().Length() > 0 && cbDatabase->GetValue().Length() > 0 && cbUsername->GetValue().Length() > 0);
}

void dlgSelectConnection::OnTextChange(wxCommandEvent &ev)
{
	btnOK->Enable(cbServer->GetValue().Length() > 0 && cbDatabase->GetValue().Length() > 0 && cbUsername->GetValue().Length() > 0);
}

void dlgSelectConnection::OnOK(wxCommandEvent &ev)
{
#ifdef __WXGTK__
	if (!btnOK->IsEnabled())
		return;
#endif

	if (cbDatabase->GetCurrentSelection() == wxNOT_FOUND ||
	        cbServer->GetCurrentSelection() == wxNOT_FOUND)
		remoteServer = NULL;

	EndModal(wxID_OK);
}


void dlgSelectConnection::OnCancel(wxCommandEvent &ev)
{
	EndModal(wxID_CANCEL);
}

pgConn *dlgSelectConnection::CreateConn(wxString &applicationname, bool &createdNew)
{
	/* gcc requires that we store this in temporary variables for some reason... */
	wxString serv = cbServer->GetValue();
	wxString db = cbDatabase->GetValue();

	createdNew = true;

	long port = 0;
	if (serv.Find(':') > 0)
	{
		if (!serv.Mid(serv.Find(':') + 1).ToLong(&port))
		{
			wxMessageBox(_("Invalid port number specified."));
			return NULL;
		}
		serv = serv.Mid(0, serv.Find(':'));
	}

	wxString user = cbUsername->GetValue();
	wxString role = cbRolename->GetValue();

	if (cbConnection)
	{
		/* Check if selected combination already exists */
		for (unsigned int index = 0; index < cbConnection->GetCount() - 1; index++)
		{
			pgConn *conn = (pgConn *)cbConnection->GetClientData(index);
			if (conn &&
			        conn->GetHost() == serv &&
			        conn->GetPort() == port &&
			        conn->GetUser() == user &&
			        conn->GetRole() == role &&
			        conn->GetDbname() == db)
			{
				createdNew = false;
				return conn;
			}
		}
	}


	int sslmode = remoteServer ? remoteServer->GetSSL() : 0;

	return CreateConn(serv, db, user, port, role, sslmode, applicationname, true);
}

pgConn *dlgSelectConnection::CreateConn(wxString &server, wxString &dbname, wxString &username, int port, wxString &rolename, int sslmode, wxString &applicationname, bool writeMRU)
{
	pgConn *newconn;
	newconn = new pgConn(server, wxEmptyString, wxEmptyString, dbname, username, wxT(""), port, rolename, sslmode, 0, applicationname);
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

		newconn = new pgConn(server, wxEmptyString, wxEmptyString, dbname, username, dlg.GetPassword(), port, rolename, sslmode, 0, applicationname);
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
			settings->Write(wxT("QuickConnect/username"), cbUsername->GetValue());
			settings->Write(wxT("QuickConnect/rolename"), cbRolename->GetValue());
		}
	}
	return newconn;
}

int dlgSelectConnection::Go(pgConn *conn, wxBitmapComboBox *cb)
{
	bool foundServer = false;
	cbConnection = cb;
	if (mainForm != NULL)
	{
		ctlTree *browser = mainForm->GetBrowser();
		wxTreeItemIdValue foldercookie, servercookie;
		wxTreeItemId folderitem, serveritem;
		pgObject *object;
		pgServer *server;

		folderitem = browser->GetFirstChild(browser->GetRootItem(), foldercookie);
		while (folderitem)
		{
			if (browser->ItemHasChildren(folderitem))
			{
				serveritem = browser->GetFirstChild(folderitem, servercookie);
				while (serveritem)
				{
					object = browser->GetObject(serveritem);
					if (object && object->IsCreatedBy(serverFactory))
					{
						server = (pgServer *)object;
						cbServer->Append(server->GetIdentifier(), (void *)server);
						if (server->GetConnected() &&
						        server->GetConnection()->GetHost() == conn->GetHost() &&
						        server->GetConnection()->GetPort() == conn->GetPort())
						{
							cbServer->SetSelection(cbServer->GetCount() - 1);
							remoteServer = server;
							foundServer = true;
						}
					}
					serveritem = browser->GetNextChild(folderitem, servercookie);
				}
			}
			folderitem = browser->GetNextChild(browser->GetRootItem(), foldercookie);
		}
	}

	cbServer->SetFocus();

	wxCommandEvent ev;
	OnChangeServer(ev);

	if (foundServer)
	{

		unsigned int index = 0;
		for (; index < cbDatabase->GetCount(); index++)
		{
			if (cbDatabase->GetString(index) == conn->GetDbname())
			{
				cbDatabase->SetSelection(index);
				break;
			}
		}
		for (index = 0; index < cbUsername->GetCount(); index++)
		{
			if (cbUsername->GetString(index) == conn->GetUser())
			{
				cbUsername->SetSelection(index);
				break;
			}
		}
		for (index = 0; index < cbRolename->GetCount(); index++)
		{
			if (cbRolename->GetString(index) == conn->GetRole())
			{
				cbRolename->SetSelection(index);
				break;
			}
		}
	}

	return ShowModal();
}

