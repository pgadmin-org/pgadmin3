//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgRepCluster.cpp - PostgreSQL Slony-I Cluster Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/pgDefs.h"
#include <wx/textbuf.h>
#include <wx/file.h>

#include "frm/frmMain.h"
#include "slony/dlgRepCluster.h"
#include "slony/slCluster.h"
#include "slony/slSet.h"
#include "slony/slCluster.h"
#include "schema/pgDatatype.h"
#include "utils/sysProcess.h"

#define cbServer            CTRL_COMBOBOX("cbServer")
#define cbDatabase          CTRL_COMBOBOX("cbDatabase")
#define cbClusterName       CTRL_COMBOBOX("cbClusterName")

BEGIN_EVENT_TABLE(dlgRepClusterBase, dlgProperty)
	EVT_COMBOBOX(XRCID("cbServer"),         dlgRepClusterBase::OnChangeServer)
	EVT_COMBOBOX(XRCID("cbDatabase"),       dlgRepClusterBase::OnChangeDatabase)
END_EVENT_TABLE();



dlgRepClusterBase::dlgRepClusterBase(pgaFactory *f, frmMain *frame, const wxString &dlgName, slCluster *node, pgDatabase *db)
	: dlgProperty(f, frame, dlgName)
{
	cluster = node;
	remoteServer = 0;
	remoteConn = 0;

	pgObject *obj = db;
	servers = obj->GetId();
	while (obj && obj != frame->GetServerCollection())
	{
		servers = frame->GetBrowser()->GetItemParent(servers);
		if (servers)
			obj = frame->GetBrowser()->GetObject(servers);
	}
}


dlgRepClusterBase::~dlgRepClusterBase()
{
	if (remoteConn)
	{
		delete remoteConn;
		remoteConn = 0;
	}
}


pgObject *dlgRepClusterBase::GetObject()
{
	return cluster;
}


bool dlgRepClusterBase::AddScript(wxString &sql, const wxString &fn)
{
	wxFileName filename;
	filename.Assign(settings->GetSlonyPath(), fn);

	if (!wxFile::Exists(filename.GetFullPath()))
		return false;

	wxFile file(filename.GetFullPath(), wxFile::read);
	if (!file.IsOpened())
		return false;

	char *buffer;
	size_t done;

	buffer = new char[file.Length() + 1];
	done = file.Read(buffer, file.Length());
	buffer[done] = 0;
	sql += wxTextBuffer::Translate(wxString::FromAscii(buffer), wxTextFileType_Unix);
	delete[] buffer;

	return done > 0;
}


int dlgRepClusterBase::Go(bool modal)
{
	return dlgProperty::Go(modal);
}


void dlgRepClusterBase::OnChangeServer(wxCommandEvent &ev)
{
	cbDatabase->Clear();
	if (remoteConn)
	{
		delete remoteConn;
		remoteConn = 0;
	}
	int sel = cbServer->GetCurrentSelection();
	if (sel >= 0)
	{
		remoteServer = (pgServer *)cbServer->wxItemContainer::GetClientData(sel);

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
			pgSet *set = remoteServer->ExecuteSet(
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

				if (cbDatabase->GetCount())
					cbDatabase->SetSelection(0);
			}
		}

	}
	OnChangeDatabase(ev);
}



void dlgRepClusterBase::OnChangeDatabase(wxCommandEvent &ev)
{
	cbClusterName->Clear();

	int sel = cbDatabase->GetCurrentSelection();
	if (remoteServer && sel >= 0)
	{
		if (remoteConn)
		{
			delete remoteConn;
			remoteConn = 0;
		}
		remoteConn = remoteServer->CreateConn(cbDatabase->GetValue());
		if (remoteConn)
		{
			pgSet *set = remoteConn->ExecuteSet(
			                 wxT("SELECT substr(nspname, 2) as clustername\n")
			                 wxT("  FROM pg_namespace nsp\n")
			                 wxT("  JOIN pg_proc pro ON pronamespace=nsp.oid AND proname = 'slonyversion'\n")
			                 wxT(" ORDER BY nspname"));

			if (set)
			{
				while (!set->Eof())
				{
					cbClusterName->Append(set->GetVal(wxT("clustername")));
					set->MoveNext();
				}
				delete set;
			}

			if (cbClusterName->GetCount())
				cbClusterName->SetSelection(0);
		}
	}
	OnChangeCluster(ev);
}

////////////////////////////////////////////////////////////////////////////////7

// pointer to controls
#define chkJoinCluster      CTRL_CHECKBOX("chkJoinCluster")
#define txtClusterName      CTRL_TEXT("txtClusterName")
#define txtNodeID           CTRL_TEXT("txtNodeID")
#define txtNodeName         CTRL_TEXT("txtNodeName")
#define txtAdminNodeID      CTRL_TEXT("txtAdminNodeID")
#define txtAdminNodeName    CTRL_TEXT("txtAdminNodeName")
#define cbAdminNode         CTRL_COMBOBOX("cbAdminNode")


BEGIN_EVENT_TABLE(dlgRepCluster, dlgRepClusterBase)
	EVT_BUTTON(wxID_OK,                     dlgRepCluster::OnOK)
	EVT_CHECKBOX(XRCID("chkJoinCluster"),   dlgRepCluster::OnChangeJoin)
	EVT_COMBOBOX(XRCID("cbClusterName"),    dlgRepCluster::OnChangeCluster)
	EVT_TEXT(XRCID("txtClusterName"),       dlgRepCluster::OnChange)
	EVT_TEXT(XRCID("txtNodeID"),            dlgRepCluster::OnChange)
	EVT_TEXT(XRCID("txtNodeName"),          dlgRepCluster::OnChange)
	EVT_COMBOBOX(XRCID("cbAdminNode"),      dlgRepCluster::OnChange)
	EVT_END_PROCESS(-1,                     dlgRepCluster::OnEndProcess)
END_EVENT_TABLE();


dlgProperty *pgaSlClusterFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgRepCluster(this, frame, (slCluster *)node, (pgDatabase *)parent);
}


dlgRepCluster::dlgRepCluster(pgaFactory *f, frmMain *frame, slCluster *node, pgDatabase *db)
	: dlgRepClusterBase(f, frame, wxT("dlgRepCluster"), node, db)
{
	process = 0;
}



wxString dlgRepCluster::GetHelpPage() const
{
	wxString page = wxT("slony-install");
	if (chkJoinCluster->GetValue())
		page += wxT("#join");

	return page;
}

bool dlgRepCluster::SlonyMaximumVersion(const wxString &series, long minor)
{

	wxString slonySeries;
	long slonyMinorVersion;

	slonySeries = slonyVersion.BeforeLast('.');
	slonyVersion.AfterLast('.').ToLong(&slonyMinorVersion);

	return slonySeries == series && slonyMinorVersion <= minor;
}



int dlgRepCluster::Go(bool modal)
{
	chkJoinCluster->SetValue(false);

	if (cluster)
	{
		// edit mode
		txtClusterName->SetValue(cluster->GetName());
		txtNodeID->SetValue(NumToStr(cluster->GetLocalNodeID()));
		txtClusterName->Disable();
		txtNodeID->Disable();
		txtNodeName->SetValue(cluster->GetLocalNodeName());
		txtNodeName->Disable();
		chkJoinCluster->Disable();

		txtAdminNodeID->Hide();
		txtAdminNodeName->Hide();

		wxString sql =
		    wxT("SELECT no_id, no_comment\n")
		    wxT("  FROM ") + cluster->GetSchemaPrefix() + wxT("sl_node\n")
		    wxT("  JOIN ") + cluster->GetSchemaPrefix() + wxT("sl_path ON no_id = pa_client\n")
		    wxT(" WHERE pa_server = ") + NumToStr(cluster->GetLocalNodeID()) +
		    wxT("   AND pa_conninfo LIKE ") + qtDbString(wxT("%host=") + cluster->GetServer()->GetName() + wxT("%")) +
		    wxT("   AND pa_conninfo LIKE ") + qtDbString(wxT("%dbname=") + cluster->GetDatabase()->GetName() + wxT("%"));

		if (cluster->GetServer()->GetPort() != 5432)
			sql += wxT("   AND pa_conninfo LIKE ") + qtDbString(wxT("%port=") + NumToStr((long)cluster->GetServer()->GetPort()) + wxT("%"));

		sql += wxT(" ORDER BY no_id");

		pgSet *set = connection->ExecuteSet(sql);
		if (set)
		{
			while (!set->Eof())
			{
				long id = set->GetLong(wxT("no_id"));
				cbAdminNode->Append(IdAndName(id, set->GetVal(wxT("no_comment"))), (void *)id);
				if (id == cluster->GetAdminNodeID())
					cbAdminNode->SetSelection(cbAdminNode->GetCount() - 1);

				set->MoveNext();
			}
			delete set;
		}
		if (!cbAdminNode->GetCount())
		{
			cbAdminNode->Append(_("<none>"), (void *) - 1);
			cbAdminNode->SetSelection(0);
		}

		cbServer->Append(cluster->GetServer()->GetName());
		cbServer->SetSelection(0);
		cbDatabase->Append(cluster->GetDatabase()->GetName());
		cbDatabase->SetSelection(0);
		cbClusterName->Append(cluster->GetName());
		cbClusterName->SetSelection(0);
	}
	else
	{
		// create mode
		cbAdminNode->Hide();

		wxString scriptVersion = wxEmptyString;
		wxString xxidVersion = wxEmptyString;

		txtNodeID->SetValidator(numericValidator);
		txtAdminNodeID->SetValidator(numericValidator);
		txtClusterName->Hide();

		//We need to find the exact Slony Version.
		//NOTE: We are not supporting Slony versions less than 1.2.0
		wxString tempScript = wxEmptyString;
		bool isSlonyVersionBefore2_2_0 = false;

		if(!AddScript(tempScript, wxT("slony1_funcs.sql")))
		{
			if(!AddScript(tempScript, wxT("slony1_funcs.2.2.0.sql")))
			{
				isSlonyVersionBefore2_2_0 = true;
			}
			else
			{
				isSlonyVersionBefore2_2_0 = false;
			}
		}
		else
		{
			isSlonyVersionBefore2_2_0 = true;
		}

		if (tempScript.Contains(wxT("@MODULEVERSION@")) && slonyVersion.IsEmpty())
		{
			bool hasVerFunc = false;
			this->database->ExecuteVoid(wxT("RESET SEARCH_PATH;"));

			if (isSlonyVersionBefore2_2_0)
				hasVerFunc = this->database->ExecuteVoid(wxT("CREATE OR REPLACE FUNCTION pgadmin_slony_version() returns text as '$libdir/slony1_funcs', '_Slony_I_getModuleVersion' LANGUAGE C"), false);
			else
				hasVerFunc = this->database->ExecuteVoid(wxT("CREATE OR REPLACE FUNCTION pgadmin_slony_version() returns text as '$libdir/slony1_funcs.2.2.0', '_Slony_I_2_2_0_getModuleVersion' LANGUAGE C"), false);

			if (hasVerFunc)
			{
				slonyVersion = this->database->ExecuteScalar(wxT("SELECT pgadmin_slony_version();"));
				this->database->ExecuteVoid(wxT("DROP FUNCTION pgadmin_slony_version()"), false);
			}
			else
			{
				tempScript.Empty();
			}

			if (slonyVersion.IsEmpty())
			{
				wxLogError(_("Couldn't test for the Slony version. Assuming 1.2.0"));
				slonyVersion = wxT("1.2.0");
			}
		}

		//Here we are finding the exact slony scripts version, which is based on Slony Version and PG Version.
		// For Slony 1.2.0 to 1.2.21 and 2.0.0 if PG 7.3 script version is v73
		// For Slony 1.2.0 to 1.2.21 and 2.0.0 if PG 7.4 script version is v74
		// For Slony 1.2.0 to 1.2.6 if PG 8.0+ script version is v80
		// For Slony 1.2.7 to 1.2.21 and 2.0.0 if PG 8.0 script version is v80
		// For Slony 1.2.7 to 1.2.21 and 2.0.0 if PG 8.1+ script version is v81
		// For Slony 2.0.1 and 2.0.2 if PG 8.3+ script version is v83. (These version onwards do not support PG Version less than 8.3)
		// For Slony 2.0.3 if PG 8.3 script version is v83.
		// For Slony 2.0.3 if PG 8.4+ script version is v84.
		// For Slony 2.1.0 to 2.2.0 if PG 8.3 script version v83
		// For Slony 2.1.0 to 2.2.0 if PG 8.4+ script version v84
		// Since both 1.2 and 2.0 series is increasing, the following code needs to be updated with each Slony or PG update.
		// For Slony 1.2.22 onwards if PG 7.4 script version v74
		// For Slony 1.2.22 onwards if PG 8.0 script version v80
		// For Slony 1.2.22 onwards if PG 8.3 script version v81
		// For Slony 1.2.22 onwards if PG 8.4+ script version v84
		if (!tempScript.IsEmpty())
		{
			wxString slonySeries;
			long slonyMinorVersion;
			slonySeries = slonyVersion.BeforeLast('.');
			slonyVersion.AfterLast('.').ToLong(&slonyMinorVersion);

			//Set the slony_base and slony_funcs script version.
			if (SlonyMaximumVersion(wxT("1.2"), 6))
			{
				if (connection->BackendMinimumVersion(8, 0))
					scriptVersion = wxT("v80");
				else
				{
					if (connection->BackendMinimumVersion(7, 4))
						scriptVersion = wxT("v74");
					else
						scriptVersion = wxT("v73");
				}
			}
			else
			{
				// For slony verion 1.2.22 and above set the script version
				if (slonySeries == wxT("1.2") && slonyMinorVersion >= 22)
				{
					if (connection->BackendMinimumVersion(8, 4))
						scriptVersion = wxT("v84");
					else
					{
						if (connection->BackendMinimumVersion(8, 1))
							scriptVersion = wxT("v81");
						else
						{
							if (connection->BackendMinimumVersion(8, 0))
								scriptVersion = wxT("v80");
							else
								scriptVersion = wxT("v74");
						}
					}
				}

				// For slony major version 1.2, minor version <= 21 and slony version 2.0, set the script version
				if (SlonyMaximumVersion(wxT("1.2"), 21) || SlonyMaximumVersion(wxT("2.0"), 0))
				{
					if (connection->BackendMinimumVersion(8, 1))
						scriptVersion = wxT("v81");
					else
					{
						if (connection->BackendMinimumVersion(8, 0))
							scriptVersion = wxT("v80");
						else
						{
							if (connection->BackendMinimumVersion(7, 4))
								scriptVersion = wxT("v74");
							else
								scriptVersion = wxT("v73");
						}
					}
				}
				else
				{
					if (SlonyMaximumVersion(wxT("2.0"), 2))
						scriptVersion = wxT("v83");
					else
					{
						if (SlonyMaximumVersion(wxT("2.0"), 8))
						{
							if (connection->BackendMinimumVersion(8, 4))
								scriptVersion = wxT("v84");
						}

						if (SlonyMaximumVersion(wxT("2.1"), 4) || SlonyMaximumVersion(wxT("2.2"), 0))
						{
							if (connection->BackendMinimumVersion(8, 4))
								scriptVersion = wxT("v84");
						}
						else
						{
							if (scriptVersion.IsEmpty())
								scriptVersion = wxT("v83");
						}
					}
				}
			}

			//Set the correct xxid version if applicable
			// For Slony 1.2.0 to 1.2.17 and 2.0.0 if PG 7.3 xxid version is v73
			// For Slony 1.2.1 to 1.2.17 and 2.0.0 if PG 7.4+ xxid version is v74
			// For Slony 1.2.0 if PG 8.0 xxid version is v80
			// For Slony 2.0.1+ and PG8.4+ xxid is obsolete.

			if (SlonyMaximumVersion(wxT("1.2"), 0))
			{
				if (connection->BackendMinimumVersion(8, 0))
					xxidVersion = wxT("v80");
				else
				{
					if (connection->BackendMinimumVersion(7, 4))
						xxidVersion = wxT("v74");
					else
						xxidVersion = wxT("v73");
				}
			}
			else
			{
				// For Slony 1.2.22 and above if PG 7.4 xxid version is v74
				// For Slony 1.2.22 and above if PG 8.0 xxid version is v80
				// For Slony 1.2.22 and above if PG 8.1 xxid version is v81
				// For Slony 1.2.22 and above if PG 8.4+ xxid version is v84
				if (slonySeries == wxT("1.2") && slonyMinorVersion >= 22)
				{
					if (connection->BackendMinimumVersion(8, 4))
						xxidVersion = wxT("v84");
					else
					{
						if (connection->BackendMinimumVersion(8, 1))
							xxidVersion = wxT("v81");
						else
						{
							if (connection->BackendMinimumVersion(8, 0))
								xxidVersion = wxT("v80");
							else
								xxidVersion = wxT("v74");
						}
					}
				}

				if (SlonyMaximumVersion(wxT("1.2"), 21) || SlonyMaximumVersion(wxT("2.0"), 0))
				{
					if (!connection->BackendMinimumVersion(8, 4))
					{
						if (connection->BackendMinimumVersion(7, 4))
							xxidVersion = wxT("v74");
						else
							xxidVersion = wxT("v73");
					}
				}
			}

			wxString slonyBaseVersionFilename;
			wxString slonyFuncsVersionFilename;

			if (SlonyMaximumVersion(wxT("2.2"), 0))
			{
				slonyBaseVersionFilename = wxT("slony1_base.") + scriptVersion + wxT(".2.2.0.sql");
				slonyFuncsVersionFilename = wxT("slony1_funcs.") + scriptVersion + wxT(".2.2.0.sql");
			}
			else
			{
				slonyBaseVersionFilename = wxT("slony1_base.") + scriptVersion + wxT(".sql");
				slonyFuncsVersionFilename = wxT("slony1_funcs.") + scriptVersion + wxT(".sql");
			}

			wxString xxidVersionFilename;

			if (!xxidVersion.IsEmpty())
				xxidVersionFilename = wxT("xxid.") + xxidVersion + wxT(".sql");

			if (SlonyMaximumVersion(wxT("2.2"), 0))
			{
				if (((!xxidVersion.IsEmpty() && !AddScript(createScript, xxidVersionFilename)) ||
				        !AddScript(createScript, wxT("slony1_base.2.2.0.sql")) ||
				        !AddScript(createScript, slonyBaseVersionFilename) ||
				        !AddScript(createScript, wxT("slony1_funcs.2.2.0.sql")) ||
				        !AddScript(createScript, slonyFuncsVersionFilename)))
					createScript = wxEmptyString;
			}
			else
			{
				if (((!xxidVersion.IsEmpty() && !AddScript(createScript, xxidVersionFilename)) ||
				        !AddScript(createScript, wxT("slony1_base.sql")) ||
				        !AddScript(createScript, slonyBaseVersionFilename) ||
				        !AddScript(createScript, wxT("slony1_funcs.sql")) ||
				        !AddScript(createScript, slonyFuncsVersionFilename)))
					createScript = wxEmptyString;
			}
		}

		// Populate the server combo box
		ctlTree *browser = mainForm->GetBrowser();
		wxTreeItemIdValue foldercookie, servercookie;
		wxTreeItemId folderitem, serveritem;
		pgObject *object;
		pgServer *server;
		int sel = -1;

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
						if (server == database->GetServer())
							sel = cbServer->GetCount();
						cbServer->Append(browser->GetItemText(server->GetId()), (void *)server);
					}
					serveritem = browser->GetNextChild(folderitem, servercookie);
				}
			}
			folderitem = browser->GetNextChild(browser->GetRootItem(), foldercookie);
		}

		if (sel >= 0)
			cbServer->SetSelection(sel);
	}

	wxCommandEvent ev;
	OnChangeJoin(ev);

	return dlgRepClusterBase::Go(modal);
}


void dlgRepCluster::OnChangeJoin(wxCommandEvent &ev)
{
	bool joinCluster = chkJoinCluster->GetValue();
	txtClusterName->Show(!joinCluster);
	cbClusterName->Show(joinCluster);

	cbServer->Enable(joinCluster);
	cbDatabase->Enable(joinCluster);

	txtAdminNodeID->Show(!joinCluster && !cluster);
	txtAdminNodeName->Show(!joinCluster && !cluster);
	cbAdminNode->Show(joinCluster || cluster);
	cbAdminNode->Move(txtAdminNodeID->GetPosition());

	// Force the dialogue to resize to prevent a drawing issue on GTK
#ifdef __WXGTK__
	SetSize(GetSize().x + 1, GetSize().y + 1);
	Layout();
	SetSize(GetSize().x - 1, GetSize().y - 1);
#endif

	if (joinCluster && !cbDatabase->GetCount())
	{
		OnChangeServer(ev);
		return;
	}

	OnChange(ev);
}


void dlgRepCluster::OnChangeCluster(wxCommandEvent &ev)
{
	clusterBackup = wxEmptyString;
	remoteVersion = wxEmptyString;

	cbAdminNode->Clear();
	cbAdminNode->Append(_("<none>"), (void *) - 1);

	int sel = cbClusterName->GetCurrentSelection();
	if (remoteConn && sel >= 0)
	{
		wxString schemaPrefix = qtIdent(wxT("_") + cbClusterName->GetValue()) + wxT(".");
		long adminNodeID = settings->Read(wxT("Replication/") + cbClusterName->GetValue() + wxT("/AdminNode"), -1L);

		remoteVersion = remoteConn->ExecuteScalar(wxT("SELECT ") + schemaPrefix + wxT("slonyVersion();"));

		wxString sql =
		    wxT("SELECT no_id, no_comment\n")
		    wxT("  FROM ") + schemaPrefix + wxT("sl_node\n")
		    wxT("  JOIN ") + schemaPrefix + wxT("sl_path ON no_id = pa_client\n")
		    wxT(" WHERE pa_server = (SELECT last_value FROM ") + schemaPrefix + wxT("sl_local_node_id)\n")
		    wxT("   AND pa_conninfo ILIKE ") + qtDbString(wxT("%host=") + remoteServer->GetName() + wxT("%")) + wxT("\n")
		    wxT("   AND pa_conninfo LIKE ") + qtDbString(wxT("%dbname=") + cbDatabase->GetValue() + wxT("%")) + wxT("\n");

		if (remoteServer->GetPort() != 5432)
			sql += wxT("   AND pa_conninfo LIKE ") + qtDbString(wxT("%port=") + NumToStr((long)remoteServer->GetPort()) + wxT("%"));

		pgSet *set = remoteConn->ExecuteSet(sql);
		if (set)
		{
			if (!set->Eof())
			{
				long id = set->GetLong(wxT("no_id"));
				cbAdminNode->Append(IdAndName(id, set->GetVal(wxT("no_comment"))), (void *)id);
				if (adminNodeID == id)
					cbAdminNode->SetSelection(cbAdminNode->GetCount() - 1);
			}
		}


		usedNodes.Clear();
		set = remoteConn->ExecuteSet(
		          wxT("SELECT no_id FROM ") + schemaPrefix + wxT("sl_node"));

		if (set)
		{
			while (!set->Eof())
			{
				usedNodes.Add(set->GetLong(wxT("no_id")));
				set->MoveNext();
			}
			delete set;
		}
	}
	OnChange(ev);
}



bool dlgRepCluster::CopyTable(pgConn *from, pgConn *to, const wxString &table)
{
	bool ok = true;

	pgSet *set = from->ExecuteSet(wxT("SELECT * FROM ") + table);
	if (!set)
		return false;

	while (ok && !set->Eof())
	{
		wxString sql = wxT("INSERT INTO ") + table + wxT("(");
		wxString vals;
		int i;

		for (i = 0 ; i < set->NumCols() ; i++)
		{
			if (i)
			{
				sql += wxT(", ");;
				vals += wxT(", ");
			}

			sql += set->ColName(i);

			if (set->IsNull(i))
				vals += wxT("NULL");
			else
			{
				switch (set->ColTypeOid(i))
				{
					case PGOID_TYPE_BOOL:
					case PGOID_TYPE_BYTEA:
					case PGOID_TYPE_CHAR:
					case PGOID_TYPE_NAME:
					case PGOID_TYPE_TEXT:
					case PGOID_TYPE_VARCHAR:
					case PGOID_TYPE_TIME:
					case PGOID_TYPE_TIMESTAMP:
					case PGOID_TYPE_TIME_ARRAY:
					case PGOID_TYPE_TIMESTAMPTZ:
					case PGOID_TYPE_INTERVAL:
					case PGOID_TYPE_TIMETZ:
						vals += qtDbString(set->GetVal(i));
						break;
					default:
						vals += set->GetVal(i);
				}
			}
		}

		ok = to->ExecuteVoid(
		         sql + wxT(")\n VALUES (") + vals + wxT(");"));


		set->MoveNext();
	}
	delete set;
	return ok;
}


void dlgRepCluster::OnOK(wxCommandEvent &ev)
{
#ifdef __WXGTK__
	if (!btnOK->IsEnabled())
		return;
#endif
	EnableOK(false);

	bool done = true;
	done = connection->ExecuteVoid(wxT("BEGIN TRANSACTION;"));

	if (remoteConn)
		done = remoteConn->ExecuteVoid(wxT("BEGIN TRANSACTION;"));

	// initialize cluster on local node
	done = connection->ExecuteVoid(GetSql());

	if (done && chkJoinCluster->GetValue())
	{
		// we're joining an existing cluster

		wxString schemaPrefix = qtIdent(wxT("_") + cbClusterName->GetValue()) + wxT(".");

		wxString clusterVersion = remoteConn->ExecuteScalar(
		                              wxT("SELECT ") + schemaPrefix + wxT("slonyversion()"));

		wxString newVersion = connection->ExecuteScalar(
		                          wxT("SELECT ") + schemaPrefix + wxT("slonyversion()"));

		if (clusterVersion != newVersion)
		{
			wxMessageDialog msg(this,
			                    wxString::Format(_("The newly created cluster version (%s)\n doesn't match the existing cluster's version (%s)"),
			                                     newVersion.c_str(), clusterVersion.c_str()),
			                    _("Error while joining replication cluster"), wxICON_ERROR);
			msg.ShowModal();
			done = false;
		}

		if (done)
			done = CopyTable(remoteConn, connection, schemaPrefix + wxT("sl_node"));
		if (done)
			done = CopyTable(remoteConn, connection, schemaPrefix + wxT("sl_path"));
		if (done)
			done = CopyTable(remoteConn, connection, schemaPrefix + wxT("sl_listen"));
		if (done)
			done = CopyTable(remoteConn, connection, schemaPrefix + wxT("sl_set"));
		if (done)
			done = CopyTable(remoteConn, connection, schemaPrefix + wxT("sl_subscribe"));


		// make sure event seqno starts correctly after node reusage
		if (done)
		{
			pgSet *set = connection->ExecuteSet(
			                 wxT("SELECT ev_origin, MAX(ev_seqno) as seqno\n")
			                 wxT("  FROM ") + schemaPrefix + wxT("sl_event\n")
			                 wxT(" GROUP BY ev_origin"));
			if (set)
			{
				while (done && !set->Eof())
				{
					if (set->GetVal(wxT("ev_origin")) == txtNodeID->GetValue())
					{
						done = connection->ExecuteVoid(
						           wxT("SELECT pg_catalog.setval(") +
						           qtDbString(wxT("_") + cbClusterName->GetValue() + wxT(".sl_event_seq")) +
						           wxT(", ") + set->GetVal(wxT("seqno")) + wxT("::int8 +1)"));
					}
					else
					{
						done = connection->ExecuteVoid(
						           wxT("INSERT INTO ") + schemaPrefix + wxT("sl_confirm(con_origin, con_received, con_seqno, con_timestamp\n")
						           wxT(" VALUES (") + set->GetVal(wxT("ev_origin")) +
						           wxT(", ") + txtNodeID->GetValue() +
						           wxT(", ") + set->GetVal(wxT("seqno")) +
						           wxT(", current_timestamp"));

					}
					set->MoveNext();
				}
				delete set;
			}
		}


		// make sure rowid seq starts correctly
		if (done)
		{
			wxString seqno = connection->ExecuteScalar(
			                     wxT("SELECT MAX(seql_last_value)\n")
			                     wxT("  FROM ") + schemaPrefix + wxT("sl_seqlog\n")
			                     wxT(" WHERE seql_seqid = 0 AND seql_origin = ") + txtNodeID->GetValue());

			if (!seqno.IsEmpty())
			{
				done = connection->ExecuteVoid(
				           wxT("SELECT pg_catalog.setval(") +
				           qtDbString(wxT("_") + cbClusterName->GetValue() + wxT(".sl_rowid_seq")) +
				           wxT(", ") + seqno + wxT(")"));
			}
		}

		// create new node on the existing cluster
		if (done)
		{
			wxString sql =
			    wxT("SELECT ") + schemaPrefix + wxT("storenode(")
			    + txtNodeID->GetValue() + wxT(", ")
			    + qtDbString(txtNodeName->GetValue());

			// When user has not selected cluster drop down in that case "schemaPrefix" will be NULL,
			// we have to use slonyVersion instead of remoteVersion
			if (remoteVersion.IsEmpty())
			{
				if (StrToDouble(slonyVersion) >= 1.1 && StrToDouble(slonyVersion) < 2.0)
					sql += wxT(", false");
			}
			else
			{
				if (StrToDouble(remoteVersion) >= 1.1 && StrToDouble(remoteVersion) < 2.0)
					sql += wxT(", false");
			}

			sql += wxT(");\n")
			       wxT("SELECT ") + schemaPrefix + wxT("enablenode(")
			       + txtNodeID->GetValue() + wxT(");\n");

			done = remoteConn->ExecuteVoid(sql);
		}

		// add admin info to cluster

		if (done && cbAdminNode->GetCurrentSelection() > 0)
		{
			done = remoteConn->ExecuteVoid(
			           wxT("SELECT ") + schemaPrefix + wxT("storepath(") +
			           txtNodeID->GetValue() + wxT(", ") +
			           NumToStr((long)cbAdminNode->wxItemContainer::GetClientData(cbAdminNode->GetCurrentSelection())) + wxT(", ") +
			           qtDbString(wxT("host=") + database->GetServer()->GetName() +
			                      wxT(" port=") + NumToStr((long)database->GetServer()->GetPort()) +
			                      wxT(" dbname=") + database->GetName()) + wxT(", ")
			           wxT("0);\n"));
		}
	}
	if (!done)
	{
		if (remoteConn)
			done = remoteConn->ExecuteVoid(wxT("ROLLBACK TRANSACTION;"));
		done = connection->ExecuteVoid(wxT("ROLLBACK TRANSACTION;"));
		EnableOK(true);
		return;
	}

	if (remoteConn)
		done = remoteConn->ExecuteVoid(wxT("COMMIT TRANSACTION;"));
	done = connection->ExecuteVoid(wxT("COMMIT TRANSACTION;"));

	ShowObject();
	Destroy();
}


pgObject *dlgRepCluster::CreateObject(pgCollection *collection)
{
	pgObject *obj = slClusterFactory.CreateObjects(collection, 0,
	                wxT(" WHERE nspname = ") + qtDbString(wxT("_") + GetName()));

	return obj;
}


void dlgRepCluster::CheckChange()
{
	if (cluster)
	{
		int sel = cbAdminNode->GetCurrentSelection();
		bool changed = (sel >= 0 && (long)cbAdminNode->wxEvtHandler::GetClientData() != cluster->GetAdminNodeID());

		EnableOK(changed || txtComment->GetValue() != cluster->GetComment());
	}
	else
	{
		size_t i;
		bool enable = true;

		CheckValid(enable, chkJoinCluster->GetValue() || (!createScript.IsEmpty()),
		           _("Slony-I creation scripts not available; only joining possible."));

		if (chkJoinCluster->GetValue())
			CheckValid(enable, !cbClusterName->GetValue().IsEmpty(), _("Please select a cluster name."));
		else
			CheckValid(enable, !txtClusterName->GetValue().IsEmpty(), _("Please specify name."));

		long nodeId = StrToLong(txtNodeID->GetValue());
		CheckValid(enable, nodeId > 0, _("Please specify local node ID."));
		for (i = 0 ; i < usedNodes.GetCount() && enable; i++)
			CheckValid(enable, nodeId != usedNodes[i], _("Node ID is already in use."));

		CheckValid(enable, !txtNodeName->GetValue().IsEmpty(), _("Please specify local node name."));

		txtAdminNodeName->Enable(nodeId != StrToLong(txtAdminNodeID->GetValue()));

		EnableOK(enable);
	}
}


void dlgRepCluster::OnEndProcess(wxProcessEvent &ev)
{
	if (process)
	{
		wxString error = process->ReadErrorStream();
		clusterBackup += process->ReadInputStream();
		delete process;
		process = 0;
	}
}


// this is necessary because wxString::Replace is ridiculously slow on large strings.

void AppendBuf(wxChar *&buf, int &buflen, int &len, const wxChar *str, int slen = -1)
{
	if (slen < 0)
		slen = wxStrlen(str);
	if (!slen)
		return;
	if (buflen < len + slen)
	{
		buflen = (len + slen) * 6 / 5;
		wxChar *tmp = new wxChar[buflen + 1];
		memcpy(tmp, buf, len * sizeof(wxChar));
		delete[] buf;
		buf = tmp;
	}
	memcpy(buf + len, str, slen * sizeof(wxChar));
	len += slen;
}


wxString ReplaceString(const wxString &str, const wxString &oldStr, const wxString &newStr)
{
	int buflen = str.Length() + 100;
	int len = 0;

	wxChar *buf = new wxChar[buflen + 1];

	const wxChar *ptrIn = str.c_str();
	const wxChar *ptrFound = wxStrstr(ptrIn, oldStr);

	while (ptrFound)
	{
		AppendBuf(buf, buflen, len, ptrIn, ptrFound - ptrIn);
		AppendBuf(buf, buflen, len, newStr.c_str());
		ptrIn = ptrFound + oldStr.Length();
		ptrFound = wxStrstr(ptrIn, oldStr);
	}

	AppendBuf(buf, buflen, len, ptrIn);
	buf[len] = 0;
	wxString tmpstr(buf);
	delete[] buf;

	return tmpstr;
}


wxString dlgRepCluster::GetSql()
{
	wxString sql;
	wxString name;
	if (chkJoinCluster->GetValue())
		name = wxT("_") + cbClusterName->GetValue();
	else
		name = wxT("_") + txtClusterName->GetValue();

	wxString quotedName = qtIdent(name);


	if (cluster)
	{
		// edit mode
		int sel = cbAdminNode->GetCurrentSelection();
		if (sel >= 0)
		{
			long id = (long)cbAdminNode->wxItemContainer::GetClientData(sel);
			if (id != cluster->GetAdminNodeID())
				settings->WriteLong(wxT("Replication/") + cluster->GetName() + wxT("/AdminNode"), id);
		}
	}
	else
	{
		// create mode
		wxString backupExecutable;
		if (remoteServer && remoteServer->GetConnection()->EdbMinimumVersion(8, 0))
			backupExecutable = edbBackupExecutable;
		else if (remoteServer && remoteServer->GetConnection()->GetIsGreenplum())
			backupExecutable = gpBackupExecutable;
		else
			backupExecutable = pgBackupExecutable;

		if (remoteServer && clusterBackup.IsEmpty() && !backupExecutable.IsEmpty())
		{
			wxArrayString environment;
			if (!remoteServer->GetPasswordIsStored())
				environment.Add(wxT("PGPASSWORD=") + remoteServer->GetPassword());

			process = sysProcess::Create(backupExecutable +
			                             wxT(" -i -F p -h ") + remoteServer->GetName() +
			                             wxT(" -p ") + NumToStr((long)remoteServer->GetPort()) +
			                             wxT(" -U ") + remoteServer->GetUsername() +
			                             wxT(" -s -O -n ") + name +
			                             wxT(" ") + cbDatabase->GetValue(),
			                             this, &environment);

			wxBusyCursor wait;
			while (process)
			{
				wxSafeYield();
				if (process)
					clusterBackup += process->ReadInputStream();
				wxSafeYield();
				wxMilliSleep(10);
			}
		}

		if (!clusterBackup.IsEmpty())
		{
			int opclassPos = clusterBackup.Find(wxT("CREATE OPERATOR CLASS"));
			sql = wxT("-- Extracted schema from existing cluster\n\n") +
			      clusterBackup.Left(opclassPos > 0 ? opclassPos : 99999999);
			if (opclassPos > 0)
			{
				sql +=  wxT("----------- inserted by pgadmin: add public operators\n")
				        wxT("CREATE OPERATOR public.< (PROCEDURE = xxidlt,")
				        wxT("    LEFTARG = xxid, RIGHTARG = xxid,")
				        wxT("    COMMUTATOR = public.\">\", NEGATOR = public.\">=\",")
				        wxT("    RESTRICT = scalarltsel, JOIN = scalarltjoinsel);\n")
				        wxT("CREATE OPERATOR public.= (PROCEDURE = xxideq,")
				        wxT("    LEFTARG = xxid, RIGHTARG = xxid,")
				        wxT("    COMMUTATOR = public.\"=\", NEGATOR = public.\"<>\",")
				        wxT("    RESTRICT = eqsel, JOIN = eqjoinsel,")
				        wxT("    SORT1 = public.\"<\", SORT2 = public.\"<\", HASHES);\n")
				        wxT("CREATE OPERATOR public.<> (PROCEDURE = xxidne,")
				        wxT("    LEFTARG = xxid, RIGHTARG = xxid,")
				        wxT("    COMMUTATOR = public.\"<>\", NEGATOR = public.\"=\",")
				        wxT("    RESTRICT = neqsel, JOIN = neqjoinsel);\n")
				        wxT("CREATE OPERATOR public.> (PROCEDURE = xxidgt,")
				        wxT("    LEFTARG = xxid, RIGHTARG = xxid,")
				        wxT("    COMMUTATOR = public.\"<\", NEGATOR = public.\"<=\",")
				        wxT("    RESTRICT = scalargtsel, JOIN = scalargtjoinsel);\n")
				        wxT("CREATE OPERATOR public.<= (PROCEDURE = xxidle,")
				        wxT("    LEFTARG = xxid, RIGHTARG = xxid,")
				        wxT("    COMMUTATOR = public.\">=\", NEGATOR = public.\">\",")
				        wxT("    RESTRICT = scalarltsel, JOIN = scalarltjoinsel);\n")
				        wxT("CREATE OPERATOR public.>= (PROCEDURE = xxidge,")
				        wxT("    LEFTARG = xxid, RIGHTARG = xxid,")
				        wxT("    COMMUTATOR = public.\"<=\", NEGATOR = public.\"<\",")
				        wxT("    RESTRICT = scalargtsel, JOIN = scalargtjoinsel);\n")
				        wxT("------------- continue with backup script\n")
				        + clusterBackup.Mid(opclassPos);
			}
		}
		else
		{
			sql = wxT("CREATE SCHEMA ") + quotedName + wxT(";\n\n")
			      + ReplaceString(createScript, wxT("@NAMESPACE@"), quotedName);

			if (chkJoinCluster->GetValue())
				sql = ReplaceString(sql, wxT("@CLUSTERNAME@"), cbClusterName->GetValue());
			else
				sql = ReplaceString(sql, wxT("@CLUSTERNAME@"), txtClusterName->GetValue());

			// From Slony 1.2 onwards, the scripts include the module version.
			// To figure it out, temporarily load and use _Slony_I_getModuleVersion.
			// We'll cache the result to save doing it again.
			if (sql.Contains(wxT("@MODULEVERSION@")) && slonyVersion.IsEmpty())
			{
				bool hasVerFunc = this->database->ExecuteVoid(wxT("CREATE OR REPLACE FUNCTION pgadmin_slony_version() returns text as '$libdir/slony1_funcs', '_Slony_I_getModuleVersion' LANGUAGE C"), false);
				if (!hasVerFunc)
				{
					hasVerFunc = this->database->ExecuteVoid(wxT("CREATE OR REPLACE FUNCTION pgadmin_slony_version() returns text as '$libdir/slony1_funcs.2.2.0', '_Slony_I_2_2_0_getModuleVersion' LANGUAGE C"), false);
				}
				if (hasVerFunc)
				{
					slonyVersion = this->database->ExecuteScalar(wxT("SELECT pgadmin_slony_version();"));
					this->database->ExecuteVoid(wxT("DROP FUNCTION pgadmin_slony_version()"), false);
				}

				if (slonyVersion.IsEmpty())
				{
					wxLogError(_("Couldn't test for the Slony version. Assuming 1.2.0"));
					slonyVersion = wxT("1.2.0");
				}
			}
			sql = ReplaceString(sql, wxT("@MODULEVERSION@"), slonyVersion);

			// If slony version is greater then equal to 2.2 then replace @FUNCVERSION to 2_2_0
			if (sql.Contains(wxT("@FUNCVERSION@")) && SlonyMaximumVersion(wxT("2.2"), 0))
			{
				wxString slonyFuncVersion = slonyVersion;
				slonyFuncVersion.Replace(wxT("."), wxT("_"));
				sql = ReplaceString(sql, wxT("@FUNCVERSION@"), slonyFuncVersion);
			}
		}

		sql += wxT("\n")
		       wxT("SELECT ") + quotedName + wxT(".initializelocalnode(") +
		       txtNodeID->GetValue() + wxT(", ") + qtDbString(txtNodeName->GetValue()) +
		       wxT(");\n")
		       wxT("SELECT ") + quotedName;

		if (chkJoinCluster->GetValue())
			sql += wxT(".enablenode_int(");
		else
			sql += wxT(".enablenode(");

		sql += txtNodeID->GetValue() +
		       wxT(");\n");
	}

	if ((!cluster && !txtComment->IsEmpty()) || (cluster &&
	        cluster->GetComment() != txtComment->GetValue()))
	{
		sql += wxT("\n")
		       wxT("COMMENT ON SCHEMA ") + quotedName + wxT(" IS ")
		       + qtDbString(txtComment->GetValue()) + wxT(";\n");
	}

	if (chkJoinCluster->GetValue())
		sql += wxT("\n\n-- In addition, the configuration is copied from the existing cluster.\n");
	else
	{
		wxString schemaPrefix = qtIdent(wxT("_") + txtClusterName->GetValue()) + wxT(".");
		long adminNode = StrToLong(txtAdminNodeID->GetValue());
		if (adminNode > 0 && adminNode != StrToLong(txtNodeID->GetValue()))
		{
			sql +=
			    wxT("\n-- Create admin node\n")
			    wxT("SELECT ") + schemaPrefix + wxT("storeNode(") +
			    NumToStr(adminNode) + wxT(", ") +
			    qtDbString(txtAdminNodeName->GetValue());


			// When user has not selected cluster drop down in that case "schemaPrefix" will be NULL,
			// we have to use slonyVersion instead of remoteVersion
			if (remoteVersion.IsEmpty())
			{
				if (StrToDouble(slonyVersion) >= 1.1 && StrToDouble(slonyVersion) < 2.0)
					sql += wxT(", false");
			}
			else
			{
				// storeNode API contains three argument in slony version 1.1 and 1.2 (storeNode(int4,text,boolean)),
				// slony version 2.0 onwards, storeNode API contains only two arguments e.g. storeNode(int4,text)
				if (StrToDouble(remoteVersion) >= 1.1 && StrToDouble(remoteVersion) < 2.0)
					sql += wxT(", false");
			}

			sql += wxT(");\n")
			       wxT("SELECT ") + schemaPrefix + wxT("storepath(") +
			       txtNodeID->GetValue() + wxT(", ") +
			       NumToStr(adminNode) + wxT(", ") +
			       qtDbString(wxT("host=") + database->GetServer()->GetName() +
			                  wxT(" port=") + NumToStr((long)database->GetServer()->GetPort()) +
			                  wxT(" dbname=") + database->GetName()) + wxT(", ")
			       wxT("0);\n");
		}
	}
	return sql;
}



////////////////////////////////////////////////////////////////////////////////7


#define txtCurrentVersion   CTRL_TEXT("txtCurrentVersion")
#define txtVersion          CTRL_TEXT("txtVersion")

BEGIN_EVENT_TABLE(dlgRepClusterUpgrade, dlgRepClusterBase)
	EVT_COMBOBOX(XRCID("cbClusterName"),    dlgRepClusterUpgrade::OnChangeCluster)
END_EVENT_TABLE();

// no factory needed; called by slFunction

dlgRepClusterUpgrade::dlgRepClusterUpgrade(pgaFactory *f, frmMain *frame, slCluster *cl)
	: dlgRepClusterBase(f, frame, wxT("dlgRepClusterUpgrade"), cl, cl->GetDatabase())
{
}


int dlgRepClusterUpgrade::Go(bool modal)
{
	txtCurrentVersion->SetValue(cluster->GetClusterVersion());
	txtCurrentVersion->Disable();
	txtVersion->Disable();

	// Populate the server combo box
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
					cbServer->Append(browser->GetItemText(server->GetId()), (void *)server);
				}
				serveritem = browser->GetNextChild(folderitem, servercookie);
			}
		}
		folderitem = browser->GetNextChild(browser->GetRootItem(), foldercookie);
	}

	if (cbServer->GetCount())
		cbServer->SetSelection(0);

	wxCommandEvent ev;
	OnChangeServer(ev);

	return dlgRepClusterBase::Go(modal);
}


void dlgRepClusterUpgrade::CheckChange()
{
	bool enable = true;
	CheckValid(enable, cluster->GetSlonPid() == 0, _("Slon process running on node; stop it before upgrading."));
	CheckValid(enable, cbDatabase->GetCount() > 0, _("Select server with Slony-I cluster installed."));
	CheckValid(enable, cbClusterName->GetCount() > 0, _("Select database with Slony-I cluster installed."));
	CheckValid(enable, cbClusterName->GetCurrentSelection() >= 0, _("Select Slony-I cluster."));
	CheckValid(enable, version > cluster->GetClusterVersion(), _("Selected cluster doesn't contain newer software."));
	EnableOK(enable);
}


wxString dlgRepClusterUpgrade::GetSql()
{
	if (sql.IsEmpty() && !version.IsEmpty() && remoteConn)
	{
		wxString remoteCluster = wxT("_") + cbClusterName->GetValue();
		sql = wxT("SET SEARCH_PATH = ") + qtIdent(wxT("_") + cluster->GetName()) + wxT(", pg_catalog;\n\n");

		bool upgradeSchemaAvailable = false;

		{
			// update functions
			pgSetIterator func(remoteConn,
			                   wxT("SELECT proname, proisagg, prosecdef, proisstrict, proretset, provolatile, pronargs, prosrc, probin,\n")
			                   wxT("       lanname, tr.typname as rettype,\n")
			                   wxT("       t0.typname AS arg0, t1.typname AS arg1, t2.typname AS arg2, t3.typname AS arg3, t4.typname AS arg4,\n")
			                   wxT("       t5.typname AS arg5, t6.typname AS arg6, t7.typname AS arg7, t8.typname AS arg8, t9.typname AS arg9, \n")
			                   wxT("       proargnames[0] AS an0, proargnames[1] AS an1, proargnames[2] AS an2, proargnames[3] AS an3, proargnames[4] AS an4,\n")
			                   wxT("       proargnames[5] AS an5, proargnames[6] AS an6, proargnames[7] AS an7, proargnames[8] AS an8, proargnames[9] AS an9\n")
			                   wxT("  FROM pg_proc\n")
			                   wxT("  JOIN pg_namespace nsp ON nsp.oid=pronamespace\n")
			                   wxT("  JOIN pg_language l ON l.oid=prolang\n")
			                   wxT("  JOIN pg_type tr ON tr.oid=prorettype\n")
			                   wxT("  LEFT JOIN pg_type t0 ON t0.oid=proargtypes[0]\n")
			                   wxT("  LEFT JOIN pg_type t1 ON t1.oid=proargtypes[1]\n")
			                   wxT("  LEFT JOIN pg_type t2 ON t2.oid=proargtypes[2]\n")
			                   wxT("  LEFT JOIN pg_type t3 ON t3.oid=proargtypes[3]\n")
			                   wxT("  LEFT JOIN pg_type t4 ON t4.oid=proargtypes[4]\n")
			                   wxT("  LEFT JOIN pg_type t5 ON t5.oid=proargtypes[5]\n")
			                   wxT("  LEFT JOIN pg_type t6 ON t6.oid=proargtypes[6]\n")
			                   wxT("  LEFT JOIN pg_type t7 ON t7.oid=proargtypes[7]\n")
			                   wxT("  LEFT JOIN pg_type t8 ON t8.oid=proargtypes[8]\n")
			                   wxT("  LEFT JOIN pg_type t9 ON t9.oid=proargtypes[9]\n")
			                   wxT(" WHERE nspname = ") + qtDbString(remoteCluster)
			                  );

			while (func.RowsLeft())
			{
				wxString proname = func.GetVal(wxT("proname"));
				if (proname == wxT("upgradeschema"))
					upgradeSchemaAvailable = true;

				sql += wxT("CREATE OR REPLACE FUNCTION " + qtIdent(proname) + wxT("(");

				           wxString language = func.GetVal(wxT("lanname"));
				           wxString volat = func.GetVal(wxT("provolatile"));
				           long numArgs = func.GetLong(wxT("pronargs"));

				           long i;

				           for (i = 0 ; i < numArgs ; i++)
			{
				if (i)
						sql += wxT(", ");
					wxString argname = func.GetVal(wxT("an") + NumToStr(i));
					if (!argname.IsEmpty())
						sql += qtIdent(argname) + wxT(" ");

					sql += qtIdent(func.GetVal(wxT("arg") + NumToStr(i)));
				}
				sql += wxT(")\n")
				       wxT("  RETURNS ");
				       if (func.GetBool(wxT("proretset")))
				       sql += wxT("SETOF "));
				sql += qtIdent(func.GetVal(wxT("rettype")));

				if (language == wxT("c"))
					sql += wxT("\n")
					       wxT("AS '" + func.GetVal(wxT("probin")) + wxT("', '") + func.GetVal(wxT("prosrc")) + wxT("'"));
				else
					sql += wxT(" AS\n")
					       wxT("$BODY$") + func.GetVal(wxT("prosrc")) + wxT("$BODY$");

				sql += wxT(" LANGUAGE ") + language;

				if (volat == wxT("v"))
					sql += wxT(" VOLATILE");
				else if (volat == wxT("i"))
					sql += wxT(" IMMUTABLE");
				else
					sql += wxT(" STABLE");

				if (func.GetBool(wxT("proisstrict")))
					sql += wxT(" STRICT");

				if (func.GetBool(wxT("prosecdef")))
					sql += wxT(" SECURITY DEFINER");

				sql += wxT(";\n\n");
			}
		}

		if (upgradeSchemaAvailable)
			sql += wxT("SELECT upgradeSchema(") + qtDbString(cluster->GetClusterVersion()) + wxT(");\n\n");

		{
			// Create missing tables and columns
			// we don't expect column names and types to change

			pgSetIterator srcCols(remoteConn,
			                      wxT("SELECT relname, attname, attndims, atttypmod, attnotnull, adsrc, ty.typname, tn.nspname as typnspname,\n")
			                      wxT("  (SELECT count(1) FROM pg_type t2 WHERE t2.typname=ty.typname) > 1 AS isdup\n")
			                      wxT("  FROM pg_attribute\n")
			                      wxT("  JOIN pg_class c ON c.oid=attrelid\n")
			                      wxT("  JOIN pg_namespace n ON n.oid=relnamespace")
			                      wxT("  LEFT JOIN pg_attrdef d ON adrelid=attrelid and adnum=attnum\n")
			                      wxT("  JOIN pg_type ty ON ty.oid=atttypid\n")
			                      wxT("  JOIN pg_namespace tn ON tn.oid=ty.typnamespace\n")
			                      wxT(" WHERE n.nspname = ") + qtDbString(remoteCluster) +
			                      wxT("   AND attnum>0 and relkind='r'\n")
			                      wxT(" ORDER BY (relname != 'sl_confirm'), relname, attname")
			                     );

			pgSetIterator destCols(connection,
			                       wxT("SELECT relname, attname, adsrc\n")
			                       wxT("  FROM pg_attribute\n")
			                       wxT("  JOIN pg_class c ON c.oid=attrelid\n")
			                       wxT("  JOIN pg_namespace n ON n.oid=relnamespace")
			                       wxT("  LEFT JOIN pg_attrdef d ON adrelid=attrelid and adnum=attnum\n")
			                       wxT(" WHERE n.nspname = ") + qtDbString(wxT("_") + cluster->GetName()) +
			                       wxT("   AND attnum>0 and relkind='r'\n")
			                       wxT(" ORDER BY (relname != 'sl_confirm'), relname, attname")
			                      );

			if (!destCols.RowsLeft())
				return wxT("error");

			wxString lastTable;
			while (srcCols.RowsLeft())
			{
				wxString table = srcCols.GetVal(wxT("relname"));
				wxString column = srcCols.GetVal(wxT("attname"));
				wxString defVal = srcCols.GetVal(wxT("adsrc"));

				if (table == wxT("sl_node"))
				{
					table = wxT("sl_node");
				}
				pgDatatype dt(srcCols.GetVal(wxT("typnspname")), srcCols.GetVal(wxT("typname")),
				              srcCols.GetBool(wxT("isdup")),
				              srcCols.GetLong(wxT("attndims")), srcCols.GetLong(wxT("atttypmod")));


				if (destCols.Set()->Eof() ||
				        destCols.GetVal(wxT("relname")) != table ||
				        destCols.GetVal(wxT("attname")) != column)
				{
					if (table == lastTable || table == destCols.GetVal(wxT("relname")))
					{
						// just an additional column
						sql += wxT("ALTER TABLE ") + qtIdent(table)
						       +  wxT(" ADD COLUMN ") + qtIdent(column)
						       + wxT(" ") + dt.GetQuotedSchemaPrefix(0) + dt.QuotedFullName();

						if (!defVal.IsEmpty())
							sql += wxT(" DEFAULT ") + defVal;
						if (srcCols.GetBool(wxT("attnotnull")))
							sql += wxT(" NOT NULL");

						sql += wxT(";\n");
					}
					else
					{
						// new table
						// sl_confirm will always exist and be the first so no need for special
						// precautions in case a new table is the very first in the set

						sql += wxT("CREATE TABLE ") + qtIdent(table)
						       +  wxT(" (") + qtIdent(column)
						       + wxT(" ") + dt.GetQuotedSchemaPrefix(0) + dt.QuotedFullName();

						if (!defVal.IsEmpty())
							sql += wxT(" DEFAULT ") + defVal;

						sql += wxT(");\n");
					}
				}
				else
				{
					// column is found
					if (destCols.GetVal(wxT("adsrc")) != defVal)
					{
						sql += wxT("ALTER TABLE ") + qtIdent(table)
						       +  wxT(" ALTER COLUMN ") + qtIdent(column);
						if (defVal.IsEmpty())
							sql += wxT(" DROP DEFAULT;\n");
						else
							sql += wxT(" SET DEFAULT ") + defVal + wxT(";\n");
					}
					destCols.RowsLeft();
				}
				lastTable = table;
			}
		}

		{
			// check missing indexes
			pgSetIterator srcIndexes(remoteConn,
			                         wxT("SELECT t.relname, indkey, ti.relname as indname, pg_get_indexdef(indexrelid) AS inddef\n")
			                         wxT("  FROM pg_index i\n")
			                         wxT("  JOIN pg_class ti ON indexrelid=ti.oid\n")
			                         wxT("  JOIN pg_class t ON indrelid=t.oid\n")
			                         wxT("  JOIN pg_namespace n ON n.oid=t.relnamespace\n")
			                         wxT(" WHERE nspname = ") + qtDbString(remoteCluster) +
			                         wxT(" ORDER BY t.relname, ti.relname, indkey"));

			pgSetIterator destIndexes(remoteConn,
			                          wxT("SELECT t.relname, indkey, ti.relname as indnamen")
			                          wxT("  FROM pg_index i\n")
			                          wxT("  JOIN pg_class ti ON indexrelid=ti.oid\n")
			                          wxT("  JOIN pg_class t ON indrelid=t.oid\n")
			                          wxT("  JOIN pg_namespace n ON n.oid=t.relnamespace\n")
			                          wxT(" WHERE nspname = ") + qtDbString(wxT("_") + cluster->GetName()) +
			                          wxT(" ORDER BY t.relname, ti.relname, indkey"));

			if (!destIndexes.RowsLeft())
				return wxT("error");

			while (srcIndexes.RowsLeft())
			{
				wxString table = srcIndexes.GetVal(wxT("relname"));

				bool needUpdate = destIndexes.Set()->Eof() ||
				                  destIndexes.GetVal(wxT("relname")) != table;

				if (!needUpdate && destIndexes.GetVal(wxT("indkey")) != srcIndexes.GetVal(wxT("indkey")))
				{
					// better ignore index name and check column names here
					needUpdate = destIndexes.GetVal(wxT("indname")) != srcIndexes.GetVal(wxT("indname"));
				}
				if (needUpdate)
				{
					wxString inddef = srcIndexes.GetVal(wxT("inddef"));
					inddef.Replace(qtIdent(remoteCluster) + wxT("."), qtIdent(wxT("_") + cluster->GetName()) + wxT("."));
					sql += inddef + wxT(";\n");
				}
				else
					destIndexes.RowsLeft();
			}
		}

		{
			// check missing constraints
			// we don't expect constraint definitions to change

			pgSetIterator srcConstraints(remoteConn,
			                             wxT("SELECT t.relname, contype, conkey, conname,\n")
			                             wxT("       pg_get_constraintdef(c.oid) AS condef\n")
			                             wxT("  FROM pg_constraint c\n")
			                             wxT("  JOIN pg_class t ON c.conrelid=t.oid\n")
			                             wxT("  JOIN pg_namespace n ON n.oid=relnamespace\n")
			                             wxT(" WHERE nspname = ") + qtDbString(remoteCluster) + wxT("\n")
			                             wxT(" ORDER BY (contype != 'p'), relname, contype, conname, conkey")
			                            );

			pgSetIterator destConstraints(connection,
			                              wxT("SELECT t.relname, contype, conkey, conname\n")
			                              wxT("  FROM pg_constraint c\n")
			                              wxT("  JOIN pg_class t ON c.conrelid=t.oid\n")
			                              wxT("  JOIN pg_namespace n ON n.oid=relnamespace\n")
			                              wxT(" WHERE nspname = ") + qtDbString(wxT("_") + cluster->GetName()) + wxT("\n")
			                              wxT(" ORDER BY (contype != 'p'), relname, contype, conname, conkey")
			                             );

			if (!destConstraints.RowsLeft())
				return wxT("error");

			while (srcConstraints.RowsLeft())
			{
				wxString table = srcConstraints.GetVal(wxT("relname"));
				wxString contype = srcConstraints.GetVal(wxT("contype"));

				bool needUpdate = destConstraints.Set()->Eof() ||
				                  destConstraints.GetVal(wxT("relname")) != table ||
				                  destConstraints.GetVal(wxT("contype")) != contype;
				if (!needUpdate && destConstraints.GetVal(wxT("conkey"))  != srcConstraints.GetVal(wxT("conkey")))
				{
					// better ignore constraint name and compare column names here
					needUpdate = destConstraints.GetVal(wxT("conname")) != srcConstraints.GetVal(wxT("conname"));
				}
				if (needUpdate)
				{
					wxString condef = srcConstraints.GetVal(wxT("condef"));
					condef.Replace(qtIdent(remoteCluster) + wxT("."), qtIdent(wxT("_") + cluster->GetName()) + wxT("."));

					sql += wxT("ALTER TABLE ") + qtIdent(table)
					       +  wxT(" ADD CONSTRAINT ") + qtIdent(srcConstraints.GetVal(wxT("conname")))
					       + wxT(" ") + condef
					       + wxT(";\n");
				}
				else
					destConstraints.RowsLeft();
			}

			sql += wxT("\nNOTIFY ") + qtIdent(wxT("_") + cluster->GetName() + wxT("_Restart"))
			       +  wxT(";\n\n");
		}
	}
	return sql;
}


pgObject *dlgRepClusterUpgrade::CreateObject(pgCollection *collection)
{
	return 0;
}


void dlgRepClusterUpgrade::OnChangeCluster(wxCommandEvent &ev)
{
	version = wxEmptyString;
	sql = wxEmptyString;

	int sel = cbClusterName->GetCurrentSelection();
	if (remoteConn && sel >= 0)
	{
		wxString schemaPrefix = qtIdent(wxT("_") + cbClusterName->GetValue()) + wxT(".");

		version = remoteConn->ExecuteScalar(wxT("SELECT ") + schemaPrefix + wxT("slonyversion();"));
	}
	OnChange(ev);


	txtVersion->SetValue(version);
	OnChange(ev);
}
