//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgReassignDropOwned.cpp - Reassign or drop owned objects
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/pgDefs.h"
#include "frm/frmMain.h"
#include "utils/misc.h"
#include "schema/pgRole.h"
#include "dlg/dlgReassignDropOwned.h"


// pointer to controls
#define rbReassignTo      CTRL_RADIOBUTTON("rbReassignTo")
#define rbDrop            CTRL_RADIOBUTTON("rbDrop")
#define cbRoles           CTRL_COMBOBOX("cbRoles")
#define cbDatabases       CTRL_COMBOBOX("cbDatabases")
#define btnOK             CTRL_BUTTON("wxID_OK")

BEGIN_EVENT_TABLE(dlgReassignDropOwned, pgDialog)
	EVT_RADIOBUTTON(XRCID("rbReassignTo"),    dlgReassignDropOwned::OnChange)
	EVT_RADIOBUTTON(XRCID("rbDrop"),          dlgReassignDropOwned::OnChange)
	EVT_BUTTON(wxID_OK,                       dlgReassignDropOwned::OnOK)
END_EVENT_TABLE()


dlgReassignDropOwned::dlgReassignDropOwned(frmMain *win, pgConn *conn,
        pgRole *role, wxString dbrestriction)
{
	wxString query;

	connection = conn;
	parent = win;

	SetFont(settings->GetSystemFont());
	LoadResource(win, wxT("dlgReassignDropOwned"));
	RestorePosition();

	cbRoles->Clear();
	query = wxT("SELECT rolname FROM pg_roles WHERE rolname<>") + conn->qtDbString(role->GetName()) + wxT(" ORDER BY rolname");
	pgSetIterator roles(connection, query);
	while (roles.RowsLeft())
	{
		cbRoles->Append(roles.GetVal(wxT("rolname")));
	}
	cbRoles->SetSelection(0);
	cbRoles->Enable(cbRoles->GetStrings().Count() > 0);

	cbDatabases->Clear();
	query = wxT("SELECT DISTINCT datname FROM pg_database WHERE datallowconn");
	if (!dbrestriction.IsEmpty())
	{
		query += wxT(" AND datname NOT IN (") + dbrestriction + wxT(")");
	}
	query += wxT(" ORDER BY datname");

	pgSetIterator databases(connection, query);
	while (databases.RowsLeft())
	{
		cbDatabases->Append(databases.GetVal(wxT("datname")));
	}
	cbDatabases->SetSelection(0);

	if(rbReassignTo->GetValue() && cbRoles->GetStrings().Count() <= 0)
		btnOK->Disable();

	SetSize(330, 160);
}

dlgReassignDropOwned::~dlgReassignDropOwned()
{
	SavePosition();
}


void dlgReassignDropOwned::OnOK(wxCommandEvent &ev)
{
	EndModal(wxID_OK);
}


void dlgReassignDropOwned::OnCancel(wxCommandEvent &ev)
{
	EndModal(wxID_CANCEL);
}

void dlgReassignDropOwned::OnChange(wxCommandEvent &ev)
{
	cbRoles->Enable(rbReassignTo->GetValue() && cbRoles->GetStrings().Count() > 0);
	if(rbReassignTo->GetValue() && cbRoles->GetStrings().Count() <= 0)
		btnOK->Disable();
	else
		btnOK->Enable();
}

wxString dlgReassignDropOwned::GetDatabase()
{
	return cbDatabases->GetValue();
}

wxString dlgReassignDropOwned::GetRole()
{
	return cbRoles->GetValue();
}

bool dlgReassignDropOwned::IsReassign()
{
	return rbReassignTo->GetValue();
}
