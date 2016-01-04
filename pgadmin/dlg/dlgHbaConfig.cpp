//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgHbaConfig.cpp - Configure setting
//
//////////////////////////////////////////////////////////////////////////



// App headers
#include "pgAdmin3.h"

#include "dlg/dlgHbaConfig.h"
#include "db/pgConn.h"
#include "db/pgSet.h"

// Icons
#include "images/property.pngc"





BEGIN_EVENT_TABLE(dlgHbaConfig, DialogWithHelp)
	EVT_BUTTON (wxID_OK,                dlgHbaConfig::OnOK)
	EVT_BUTTON (wxID_CANCEL,            dlgHbaConfig::OnCancel)
	EVT_BUTTON(wxID_REFRESH,	     	dlgHbaConfig::OnAddValue)
	EVT_CHECKBOX(XRCID("chkEnabled"),   dlgHbaConfig::OnChange)
	EVT_COMBOBOX(XRCID("cbType"),       dlgHbaConfig::OnChange)
	EVT_TEXT(XRCID("cbDatabase"),       dlgHbaConfig::OnChange)
	EVT_TEXT(XRCID("cbUser"),           dlgHbaConfig::OnChange)
	EVT_COMBOBOX(XRCID("cbDatabase"),   dlgHbaConfig::OnAddDatabase)
	EVT_COMBOBOX(XRCID("cbUser"),       dlgHbaConfig::OnAddUser)
	EVT_COMBOBOX(XRCID("cbMethod"),     dlgHbaConfig::OnChange)
	EVT_TEXT(XRCID("txtIPaddress"),     dlgHbaConfig::OnChange)
	EVT_TEXT(XRCID("txtOption"),        dlgHbaConfig::OnChange)
END_EVENT_TABLE()


#define chkEnabled          CTRL_CHECKBOX("chkEnabled")
#define cbType              CTRL_COMBOBOX("cbType")
#define cbDatabase          CTRL_COMBOBOX("cbDatabase")
#define cbUser              CTRL_COMBOBOX("cbUser")
#define cbMethod            CTRL_COMBOBOX("cbMethod")
#define txtIPaddress        CTRL_TEXT("txtIPaddress")
#define txtOption           CTRL_TEXT("txtOption")
#define stIPaddress         CTRL_STATIC("stIPaddress")
#define stOption            CTRL_STATIC("stOption")


dlgHbaConfig::dlgHbaConfig(pgFrame *parent, pgHbaConfigLine *_line, pgConn *_conn) :
	DialogWithHelp((frmMain *)parent)
{
	SetFont(settings->GetSystemFont());
	LoadResource((wxWindow *)parent, wxT("dlgHbaConfig"));

	conn = _conn;

	userAdding = databaseAdding = false;

	// Icon
	SetIcon(*property_png_ico);
	RestorePosition();

	line = _line;

	cbType->Append(wxT("local"));
	cbType->Append(wxT("host"));
	cbType->Append(wxT("hostssl"));
	cbType->Append(wxT("hostnossl"));

	cbDatabase->Append(wxT("all"));
	cbDatabase->Append(wxT("sameuser"));
	cbDatabase->Append(wxT("@<filename>"));
	if (conn)
	{
		// role is supported from 8.1
		if (conn->BackendMinimumVersion(8, 1))
			cbDatabase->Append(wxT("samerole"));
		else
			cbDatabase->Append(wxT("samegroup"));

		// replication is supported from 9.0
		if (conn->BackendMinimumVersion(9, 0))
			cbDatabase->Append(wxT("replication"));
	}
	else
	{
		cbDatabase->Append(wxT("samegroup"));
		cbDatabase->Append(wxT("samerole"));
		cbDatabase->Append(wxT("replication"));
	}

	cbUser->Append(wxT("all"));

	cbMethod->Append(wxT("trust"));
	cbMethod->Append(wxT("reject"));
	cbMethod->Append(wxT("md5"));
	cbMethod->Append(wxT("password"));
	cbMethod->Append(wxT("krb4"));
	cbMethod->Append(wxT("krb5"));
	cbMethod->Append(wxT("ident"));
	cbMethod->Append(wxT("pam"));

	if (conn)
	{
		// LDAP is supported from 8.2
		if (conn->BackendMinimumVersion(8, 2))
			cbMethod->Append(wxT("ldap"));

		// GSS/SSPI are supported from 8.3
		if (conn->BackendMinimumVersion(8, 3))
		{
			cbMethod->Append(wxT("gss"));
			cbMethod->Append(wxT("sspi"));
		}

		// CERT is supported from 8.4
		// but crypt is no longer supported in 8.4
		if (conn->BackendMinimumVersion(8, 4))
		{
			cbMethod->Append(wxT("cert"));
		}
		else
		{
			cbMethod->Append(wxT("crypt"));
		}

		// Radius is supported from 9.0
		if (conn->BackendMinimumVersion(9, 0))
		{
			cbMethod->Append(wxT("radius"));
		}

		// Peer is supported from 9.1
		if (conn->BackendMinimumVersion(9, 1))
		{
			cbMethod->Append(wxT("peer"));
		}
	}
	else
	{
		// Add all version-dependent methods if we don't know what version we have.
		cbMethod->Append(wxT("ldap"));
		cbMethod->Append(wxT("gss"));
		cbMethod->Append(wxT("sspi"));
		cbMethod->Append(wxT("cert"));
		cbMethod->Append(wxT("crypt"));
		cbMethod->Append(wxT("radius"));
		cbMethod->Append(wxT("peer"));
	}

	if (conn)
	{
		pgSet *set = conn->ExecuteSet(wxT("SELECT datname FROM pg_database"));
		if (set)
		{
			while (!set->Eof())
			{
				cbDatabase->Append(set->GetVal(0));
				set->MoveNext();
			}
			delete set;
		}

		wxString sql = wxT("SELECT usename FROM pg_user\n")
		               wxT("UNION\n")
		               wxT("SELECT 'group ' || groname FROM pg_group");
		set = conn->ExecuteSet(sql);
		if (set)
		{
			while (!set->Eof())
			{
				cbUser->Append(set->GetVal(0));
				set->MoveNext();
			}
			delete set;
		}
	}

	// Setup the default values

	chkEnabled->SetValue(!line->isComment);
	if(line->connectType != pgHbaConfigLine::PGC_INVALIDCONF)
	{
		database = line->database;
		user = line->user;

		cbType->SetSelection(line->connectType);
		cbMethod->SetSelection(line->method);
		cbDatabase->SetValue(database);
		cbUser->SetValue(user);
		txtIPaddress->SetValue(line->ipaddress);
		txtOption->SetValue(line->option);
	}
	wxCommandEvent noEvent;
	OnChange(noEvent);
}


dlgHbaConfig::~dlgHbaConfig()
{
	SavePosition();
}


wxString dlgHbaConfig::GetHelpPage() const
{
	return wxT("pg/client-authentication");
	// auth-methods#auth-trust #auth-password #kerberos-auth #auth-ident #auth-pam
}


void dlgHbaConfig::OnAddDatabase(wxCommandEvent &ev)
{
	int sel = cbDatabase->GetCurrentSelection();
	if (sel < 3)
		return;

	wxString newDatabase;
	if (database == wxT("all") || database == wxT("sameuser") || database == wxT("samegroup")
	        || database == wxT("samerole") || database == wxT("replication") || database.Left(1) == wxT("@"))
		database = wxEmptyString;

	if (sel == 3)    // file
		newDatabase = wxT("@");
	else
	{
		wxString str = cbDatabase->GetString(sel);
		if (str.Find(' ') >= 0)
			str = wxT("\"") + str + wxT("\"");

		int pos = database.Find(str);
		if (pos >= 0)
		{
			if (pos > 0  && database.Mid(pos - 1, 1) != wxT(","))
				pos = -1;
			if (database.Length() > str.Length() + pos && database.Mid(pos + str.Length(), 1) != wxT(","))
				pos = -1;
		}
		if (pos < 0)
		{
			if (!database.IsEmpty())
				database += wxT(",");

			newDatabase = database + str;
		}
		else
			newDatabase = database;
	}

	wxTheApp->Yield(true);

	database = newDatabase;
	databaseAdding = true;
	wxCommandEvent buttonEvent(wxEVT_COMMAND_BUTTON_CLICKED, wxID_REFRESH);
	AddPendingEvent(buttonEvent);
}


void dlgHbaConfig::OnAddUser(wxCommandEvent &ev)
{
	int sel = cbUser->GetCurrentSelection();
	if (sel < 1)
		return;

	wxString newUser;

	wxString str = cbUser->GetString(sel);
	if (str.Left(6) == wxT("group "))
	{
		if (str.Find(' ', true) > 5)
			str = wxT("+\"") + str.Mid(6) + wxT("\"");
		else
			str = wxT("+") + str.Mid(6);
	}
	else
	{
		if (str.Find(' ') >= 0)
			str = wxT("\"") + str + wxT("\"");
	}

	if (user == wxT("all"))
		newUser = str;
	else
	{
		int pos = user.Find(str);
		if (pos >= 0)
		{
			if (pos > 0  && user.Mid(pos - 1, 1) != wxT(","))
				pos = -1;
			if (user.Length() > str.Length() + pos && user.Mid(pos + str.Length(), 1) != wxT(","))
				pos = -1;
		}

		if (pos < 0)
		{
			if (!user.IsEmpty())
				user += wxT(",");
			newUser = user + str;
		}
		else
			newUser = user;
	}

	wxTheApp->Yield(true);

	user = newUser;
	userAdding = true;
	wxCommandEvent buttonEvent(wxEVT_COMMAND_BUTTON_CLICKED, wxID_REFRESH);
	AddPendingEvent(buttonEvent);
}



void dlgHbaConfig::OnAddValue(wxCommandEvent &ev)
{
	if (databaseAdding)
	{
		cbDatabase->SetSelection(-1);
		cbDatabase->SetValue(database);
		cbDatabase->SetInsertionPointEnd();
		databaseAdding = false;
	}

	if (userAdding)
	{
		cbUser->SetSelection(-1);
		cbUser->SetValue(user);
		cbUser->SetInsertionPointEnd();
		userAdding = false;
	}
	OnChange(ev);
}


void dlgHbaConfig::OnChange(wxCommandEvent &ev)
{
	if (databaseAdding || userAdding)
		return;

	database = cbDatabase->GetValue();
	user = cbUser->GetValue();

	bool needIp = (cbType->GetCurrentSelection() != 0);

	stIPaddress->Enable(needIp);
	txtIPaddress->Enable(needIp);

	bool needOption = false;
	// IDENT and LDAP always take an option
	if (cbMethod->GetCurrentSelection() == pgHbaConfigLine::PGC_IDENT ||
	        cbMethod->GetCurrentSelection() == pgHbaConfigLine::PGC_LDAP)
	{
		needOption = true;
	}
	else if (cbMethod->GetCurrentSelection() == pgHbaConfigLine::PGC_GSS ||
	         cbMethod->GetCurrentSelection() == pgHbaConfigLine::PGC_SSPI ||
	         cbMethod->GetCurrentSelection() == pgHbaConfigLine::PGC_KRB5 ||
	         cbMethod->GetCurrentSelection() == pgHbaConfigLine::PGC_PAM ||
	         cbMethod->GetCurrentSelection() == pgHbaConfigLine::PGC_CERT)
	{
		// GSS/SSPI/KRB5/PAM/CERT take options from 8.4 onwards. If we don't know the version
		// then allow the option to be specified.
		if (conn)
		{
			if (conn->BackendMinimumVersion(8, 4))
				needOption = true;
		}
		else
			needOption = true;
	}

	// On 8.4 and above, any hostssl lines can take an option
	if (cbType->GetCurrentSelection() == pgHbaConfigLine::PGC_HOSTSSL)
	{
		if (conn)
		{
			if (conn->BackendMinimumVersion(8, 4))
				needOption = true;
		}
		else
			needOption = true;
	}

	stOption->Enable(needOption);
	txtOption->Enable(needOption);

	bool ipValid = !chkEnabled->GetValue() || !needIp;
	if (!ipValid)
	{
		// we should check for validity of txtIPaddress->GetValue() here
		ipValid = true;
	}
	btnOK->Enable(cbType->GetCurrentSelection() >= 0 && !database.IsEmpty() && !user.IsEmpty() &&
	              cbMethod->GetCurrentSelection() >= 0 && ipValid);
}


void dlgHbaConfig::OnOK(wxCommandEvent &ev)
{
#ifdef __WXGTK__
	if (!btnOK->IsEnabled())
		return;
#endif
	line->isComment = !chkEnabled->GetValue();
	line->connectType = (pgHbaConfigLine::pgHbaConnectType)cbType->GetCurrentSelection();
	line->database = database;
	line->user = user;
	line->ipaddress = txtIPaddress->GetValue();
	line->method = (pgHbaConfigLine::pgHbaMethod)cbMethod->GetCurrentSelection();
	if (txtOption->IsEnabled())
		line->option = txtOption->GetValue();
	else
		line->option = wxEmptyString;
	line->changed = true;

	EndModal(wxID_OK);
}


void dlgHbaConfig::OnCancel(wxCommandEvent &ev)
{
	EndModal(wxID_CANCEL);
}


int dlgHbaConfig::Go()
{
	// Set focus on the Password textbox and show modal
	return ShowModal();
}
