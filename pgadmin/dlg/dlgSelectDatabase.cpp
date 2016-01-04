//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgSelectDatabase.cpp - Database Selection for connection string
//
//////////////////////////////////////////////////////////////////////////

// App headers
#include "pgAdmin3.h"

#include <wx/regex.h>

#include "utils/misc.h"
#include "dlg/dlgSelectDatabase.h"
#include "schema/pgServer.h"
#include "schema/pgDatabase.h"
#include "frm/frmMain.h"

extern frmMain *winMain;
wxWindowID TCSERVER_ID = ::wxNewId();

BEGIN_EVENT_TABLE(dlgSelectDatabase, wxDialog)
	EVT_TREE_ITEM_ACTIVATED (TCSERVER_ID, dlgSelectDatabase::OnSelActivate)
	EVT_TREE_SEL_CHANGED    (TCSERVER_ID, dlgSelectDatabase::OnSelect)
END_EVENT_TABLE()


dlgSelectDatabase::dlgSelectDatabase(wxWindow *parent, int id, const wxPoint &pos, const wxSize &size, long style):
	wxDialog(parent, id, wxT("Select Database"), pos, size, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{

#ifdef __WXMSW__
	SetWindowStyleFlag(GetWindowStyleFlag() & ~wxMAXIMIZE_BOX);
#endif

	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

	tcServers = new wxTreeCtrl(this, TCSERVER_ID);
	mainSizer->Add(tcServers, 1, wxEXPAND, 0);

	wxBoxSizer *bottomSizer = new wxBoxSizer(wxHORIZONTAL);

	bottomSizer->AddStretchSpacer();

	wxButton *btnOk = new wxButton(this, wxID_OK, wxT("&OK"));
	bottomSizer->Add(btnOk);

	wxButton *btnCANCEL = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));
	bottomSizer->Add(btnCANCEL, 0, wxLEFT, 10);

	mainSizer->Add(bottomSizer, 0, wxALL | wxALIGN_RIGHT, 5);
	SetSizer(mainSizer);

	SetSize(wxSize(200, 240));

	Layout();
	Centre();

	Initialize();
}

void dlgSelectDatabase::Initialize()
{
	// Add the root node
	pgServerCollection *serversObj = new pgServerCollection(serverFactory.GetCollectionFactory());
	wxTreeItemId rootItemID = tcServers->AddRoot(wxGetTranslation(serverFactory.GetCollectionFactory()->GetTypeName()),
	                          serversObj->GetIconId(), -1, serversObj);
	tcServers->SetImageList(imageList);

	ctlTree *browser = winMain->GetBrowser();

	wxTreeItemId servers = tcServers->GetRootItem();

	wxTreeItemIdValue foldercookie;
	wxTreeItemId folderitem = browser->GetFirstChild(browser->GetRootItem(), foldercookie);
	while (folderitem)
	{
		if (browser->ItemHasChildren(folderitem))
		{
			wxCookieType cookie;
			wxTreeItemId serverItem = browser->GetFirstChild(folderitem, cookie);

			wxTreeItemId tcGroupsItem = tcServers->AppendItem(rootItemID, browser->GetItemText(folderitem), serversObj->GetIconId());
			while (serverItem)
			{
				pgServer *server = (pgServer *)browser->GetObject(serverItem);

				if (server && server->IsCreatedBy(serverFactory))
				{
					dlgSelDBNode *cnInfo = new dlgSelDBNode(server);
					wxTreeItemId itm = tcServers->AppendItem(tcGroupsItem, server->GetFullName(), server->GetIconId(), -1, cnInfo);

					pgConn *conn = server->connection();

					if (conn && conn->IsAlive())
					{
						pgSet *res = conn->ExecuteSet(wxT("SELECT datname, datallowconn FROM pg_catalog.pg_database"));
						if (res)
						{
							while (!res->Eof())
							{
								if (res->GetBool(wxT("datallowconn")))
								{
									dlgSelDBNode *cnInfo = new dlgSelDBNode(server, res->GetVal(wxT("datname")));
									tcServers->AppendItem(itm, cnInfo->getDatabase(), databaseFactory.GetIconId(), -1, cnInfo);
								}

								res->MoveNext();
							}
							delete res;
						}
					}
				}

				serverItem = browser->GetNextChild(folderitem, cookie);
			}
			folderitem = browser->GetNextChild(browser->GetRootItem(), foldercookie);
		}
	}

	tcServers->Expand(servers);
	selectedConn = NULL;
}

void dlgSelectDatabase::OnSelect(wxTreeEvent &ev)
{
	wxTreeItemId sel = tcServers->GetSelection();

	if (sel.IsOk() && sel != tcServers->GetRootItem() && tcServers->GetItemParent(sel) != tcServers->GetRootItem())
	{
		selectedConn = (dlgSelDBNode *)tcServers->GetItemData(sel);
	}
	else
	{
		selectedConn = NULL;
	}
}

void dlgSelectDatabase::OnSelActivate(wxTreeEvent &ev)
{
	wxTreeItemId selID = tcServers->GetSelection();

	if (selID.IsOk() && selID != tcServers->GetRootItem())
	{
		selectedConn = (dlgSelDBNode *)tcServers->GetItemData(selID);

		if (selectedConn->dbname.IsEmpty() && tcServers->GetChildrenCount(selID, false) == 0)
		{
			winMain->ReconnectServer(selectedConn->server, true);

			pgConn *conn = selectedConn->server->connection();
			if (conn && conn->GetStatus() == PGCONN_OK)
			{
				pgSet *res = conn->ExecuteSet(wxT("SELECT datname, datallowconn FROM pg_catalog.pg_database"));
				if (res)
				{
					while (!res->Eof())
					{
						if (res->GetBool(wxT("datallowconn")))
						{
							dlgSelDBNode *cnInfo = new dlgSelDBNode(selectedConn->server, res->GetVal(wxT("datname")));
							tcServers->AppendItem(selID, cnInfo->getDatabase(), databaseFactory.GetIconId(), -1, cnInfo);
						}

						res->MoveNext();
					}
				}
				delete res;
			}
			tcServers->Expand(selID);
		}
	}
	else
	{
		selectedConn = NULL;
	}
}


wxString dlgSelectDatabase::getConnInfo()
{
	if (selectedConn)
	{
		return selectedConn->getConnectionString();
	}

	return wxEmptyString;
}

bool dlgSelectDatabase::getValidConnectionString(wxString connStr, wxString &resultStr)
{
	wxString      user;
	wxString      dbname;
	wxString      host;
	unsigned long port = 0;
	wxString      password;
	unsigned long connection_timeout = 0;

	wxRegEx propertyExp;

	// Remove white-spaces ahead the '="
	bool res = propertyExp.Compile(wxT("(([ ]*[\t]*)+)="));
	propertyExp.ReplaceAll(&connStr, wxT("="));

	// Remove white-spaces after the '="
	res = propertyExp.Compile(wxT("=(([ ]*[\t]*)+)"));
	propertyExp.ReplaceAll(&connStr, wxT("="));
	wxArrayString tokens = wxStringTokenize(connStr, wxT("\t \n\r"));

	unsigned int index = 0;
	while (index < tokens.Count())
	{
		wxString prop, value;

		// Find pairs
		//   i.e. user=xxx
		//        password=xxx
		wxArrayString pairs = wxStringTokenize(tokens[index++], wxT("="));

		// pair must exist in pair=value format
		if (pairs.GetCount() != 2)
			return false;

		prop = pairs[0];
		value = pairs[1];

		if (prop.CmpNoCase(wxT("user")) == 0)
			user = value;
		else if (prop.CmpNoCase(wxT("host")) == 0 || prop.CmpNoCase(wxT("hostAddr")) == 0)
			host = value;
		else if (prop.CmpNoCase(wxT("port")) == 0)
		{
			if (!value.ToULong(&port))
				// port must be an unsigned integer
				return false;
		}
		else if (prop.CmpNoCase(wxT("password")) == 0)
			password = value;
		else if (prop.CmpNoCase(wxT("connection_timeout")) == 0)
		{
			if (!value.ToULong(&connection_timeout))
				// connection timeout must be an unsigned interger
				return false;
		}
		else if (prop.CmpNoCase(wxT("dbname")) == 0)
			dbname = value;
		else
			// Not valid property found
			return false;
	}

	if (dbname.IsEmpty())
		return false;

	if (!user.IsEmpty())
		resultStr = wxT("user=") + user + wxT(" ");

	if (!host.IsEmpty())
	{
		resultStr += wxT("host=") + host + wxT(" ");
	}


	if (!resultStr.IsEmpty())
		resultStr += wxT(" ");
	resultStr += wxT("dbname=") + dbname + wxT(" ");

	if (port != 0)
	{
		wxString portStr;
		portStr.Printf(wxT("port=%ld"), port);
		resultStr += portStr + wxT(" ");
	}


	if (connection_timeout != 0)
	{
		wxString strConnTimeOut;
		strConnTimeOut.Printf(wxT("connection_timeout=%ld"), connection_timeout);
		resultStr += strConnTimeOut + wxT(" ");
	}

	if (!password.IsEmpty())
	{
		resultStr += wxT("password=") + password + wxT(" ");
	}

	resultStr = resultStr.Trim();

	return true;
}

dlgSelDBNode::dlgSelDBNode (pgServer *_server, const wxString &_dbname)
{
	server = _server;
	dbname = _dbname;
}

wxString dlgSelDBNode::getConnectionString()
{
	if (dbname.IsEmpty())
		return wxEmptyString;

	pgConn *conn = server->connection();
	wxString connStr;

	if (conn && conn->GetStatus() == PGCONN_OK)
	{
		connStr += wxT("user=") + conn->GetUser() + wxT(" ");
		connStr += wxT("host=") + conn->GetHostName() + wxT(" ");

		wxString port;
		port.Printf(wxT("port=%d "), conn->GetPort());
		connStr += port;

		connStr += wxT("dbname=") + dbname;
	}

	return connStr;
}

