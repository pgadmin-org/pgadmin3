//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgMoveTablespace.cpp - Reassign or drop owned objects
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/pgDefs.h"
#include "frm/frmMain.h"
#include "dlg/dlgMoveTablespace.h"
#include "utils/misc.h"
#include "schema/pgTablespace.h"


// pointer to controls
#define cbMoveTo          CTRL_COMBOBOX("cbMoveTo")
#define cbKind            CTRL_COMBOBOX("cbKind")
#define cbOwner           CTRL_COMBOBOX("cbOwner")
#define btnOK             CTRL_BUTTON("wxID_OK")


BEGIN_EVENT_TABLE(dlgMoveTablespace, pgDialog)
	EVT_BUTTON(wxID_OK,                       dlgMoveTablespace::OnOK)
END_EVENT_TABLE()


dlgMoveTablespace::dlgMoveTablespace(frmMain *win, pgConn *conn, pgTablespace *tblspc)
{
	wxString query;

	connection = conn;
	parent = win;

	SetFont(settings->GetSystemFont());
	LoadResource(win, wxT("dlgMoveTablespace"));
	RestorePosition();

	cbKind->Clear();
	cbKind->Append(_("All"));
	cbKind->Append(_("Tables"));
	cbKind->Append(_("Indexes"));
	cbKind->Append(_("Materialized views"));
	cbKind->SetSelection(0);

	cbMoveTo->Clear();
	query = wxT("SELECT spcname FROM pg_tablespace WHERE spcname<>") + conn->qtDbString(tblspc->GetName()) + wxT(" ORDER BY spcname");
	pgSetIterator tblspcs(connection, query);
	while (tblspcs.RowsLeft())
	{
		cbMoveTo->Append(tblspcs.GetVal(wxT("spcname")));
	}
	cbMoveTo->SetSelection(0);

	cbOwner->Clear();
	cbOwner->Append(wxEmptyString);
	query = wxT("SELECT rolname FROM pg_roles ORDER BY rolname");
	pgSetIterator roles(connection, query);
	while (roles.RowsLeft())
	{
		cbOwner->Append(roles.GetVal(wxT("rolname")));
	}
	cbOwner->SetSelection(0);
	cbOwner->Enable(cbOwner->GetStrings().Count() > 0);

	SetSize(330, 160);
}

dlgMoveTablespace::~dlgMoveTablespace()
{
	SavePosition();
}


void dlgMoveTablespace::OnOK(wxCommandEvent &ev)
{
	EndModal(wxID_OK);
}


void dlgMoveTablespace::OnCancel(wxCommandEvent &ev)
{
	EndModal(wxID_CANCEL);
}

wxString dlgMoveTablespace::GetTablespace()
{
	return cbMoveTo->GetValue();
}

wxArrayString dlgMoveTablespace::GetKind()
{
	wxArrayString kinds;
	if (cbKind->GetValue().Cmp(_("Tables")) == 0)
		kinds.Add(wxT("TABLE"));
	else if (cbKind->GetValue().Cmp(_("Indexes")) == 0)
		kinds.Add(wxT("INDEX"));
	else if (cbKind->GetValue().Cmp(_("Materialized views")) == 0)
		kinds.Add(wxT("MATERIALIZED VIEW"));
	else
	{
		kinds.Add(wxT("TABLE"));
		kinds.Add(wxT("INDEX"));
		kinds.Add(wxT("MATERIALIZED VIEW"));
	}
	return kinds;
}

wxString dlgMoveTablespace::GetOwner()
{
	return cbOwner->GetValue();
}

