//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgGroup.cpp - PostgreSQL Group Property
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "utils/misc.h"
#include "dlg/dlgGroup.h"
#include "schema/pgGroup.h"


// pointer to controls
#define txtID           CTRL_TEXT("txtID")

#define lbUsersNotIn    CTRL_LISTBOX("lbUsersNotIn")
#define lbUsersIn       CTRL_LISTBOX("lbUsersIn")
#define btnAddUser      CTRL_BUTTON("btnAddUser")
#define btnDelUser      CTRL_BUTTON("btnDelUser")


dlgProperty *pgGroupFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgGroup(this, frame, (pgGroup *)node);
}



BEGIN_EVENT_TABLE(dlgGroup, dlgProperty)
	EVT_LISTBOX_DCLICK(XRCID("lbUsersNotIn"),       dlgGroup::OnUserAdd)
	EVT_LISTBOX_DCLICK(XRCID("lbUsersIn"),          dlgGroup::OnUserRemove)
	EVT_BUTTON(XRCID("btnAddUser"),                 dlgGroup::OnUserAdd)
	EVT_BUTTON(XRCID("btnDelUser"),                 dlgGroup::OnUserRemove)
END_EVENT_TABLE();



dlgGroup::dlgGroup(pgaFactory *f, frmMain *frame, pgGroup *node)
	: dlgProperty(f, frame, wxT("dlgGroup"))
{
	group = node;
}


pgObject *dlgGroup::GetObject()
{
	return group;
}


int dlgGroup::Go(bool modal)
{
	pgSet *set = connection->ExecuteSet(wxT("SELECT usename FROM pg_user"));
	if (set)
	{
		while (!set->Eof())
		{
			wxString userName = set->GetVal(wxT("usename"));

			if (group && group->GetUsersIn().Index(userName) >= 0)
				lbUsersIn->Append(userName);
			else
				lbUsersNotIn->Append(userName);

			set->MoveNext();
		}
		delete set;
	}

	if (group)
	{
		// Edit Mode
		readOnly = !group->GetServer()->GetSuperUser();

		txtID->SetValue(NumToStr(group->GetGroupId()));
		if (!connection->BackendMinimumVersion(7, 4))
			txtName->Disable();
		txtID->Disable();
		if (readOnly)
		{
			btnAddUser->Disable();
			btnDelUser->Disable();
		}
	}
	else
	{
		txtID->SetValidator(numericValidator);
	}

	return dlgProperty::Go(modal);
}


void dlgGroup::CheckChange()
{
	if (group)
	{
		EnableOK(!GetSql().IsEmpty());
	}
	else
	{
		wxString name = GetName();

		bool enable = true;
		CheckValid(enable, !name.IsEmpty(), _("Please specify name."));

		EnableOK(enable);
	}
}


void dlgGroup::OnUserAdd(wxCommandEvent &ev)
{
	if (!readOnly)
	{
		int pos = lbUsersNotIn->GetSelection();
		if (pos >= 0)
		{
			lbUsersIn->Append(lbUsersNotIn->GetString(pos));
			lbUsersNotIn->Delete(pos);
		}
		CheckChange();
	}
}


void dlgGroup::OnUserRemove(wxCommandEvent &ev)
{
	if (!readOnly)
	{
		int pos = lbUsersIn->GetSelection();
		if (pos >= 0)
		{
			lbUsersNotIn->Append(lbUsersIn->GetString(pos));
			lbUsersIn->Delete(pos);
		}
		CheckChange();
	}
}


pgObject *dlgGroup::CreateObject(pgCollection *collection)
{
	wxString name = GetName();

	pgObject *obj = groupFactory.CreateObjects(collection, 0, wxT("\n WHERE groname=") + qtDbString(name));
	return obj;
}


wxString dlgGroup::GetSql()
{
	wxString sql;
	wxString name = GetName();
	int cnt, pos;

	if (group)
	{
		// Edit Mode

		AppendNameChange(sql);

		cnt = lbUsersIn->GetCount();
		wxArrayString tmpUsers = group->GetUsersIn();

		// check for added users
		for (pos = 0 ; pos < cnt ; pos++)
		{
			wxString userName = lbUsersIn->GetString(pos);

			int index = tmpUsers.Index(userName);
			if (index >= 0)
				tmpUsers.RemoveAt(index);
			else
				sql += wxT("ALTER GROUP ") + qtIdent(name)
				       +  wxT("\n  ADD USER ") + qtIdent(userName) + wxT(";\n");
		}

		// check for removed users
		for (pos = 0 ; pos < (int)tmpUsers.GetCount() ; pos++)
		{
			sql += wxT("ALTER GROUP ") + qtIdent(name)
			       +  wxT("\n  DROP USER ") + qtIdent(tmpUsers.Item(pos)) + wxT(";\n");
		}
	}
	else
	{
		// Create Mode
		wxString name = GetName();

		long id = StrToLong(txtID->GetValue());

		sql = wxT(
		          "CREATE GROUP ") + qtIdent(name);
		if (id)
			sql += wxT("\n  WITH SYSID ") + NumToStr(id);
		cnt = lbUsersIn->GetCount();
		if (cnt)
			sql += wxT("\n   USER ");
		for (pos = 0 ; pos < cnt ; pos++)
		{
			if (pos)
				sql += wxT(", ");
			sql += qtIdent(lbUsersIn->GetString(pos));
		}
		sql += wxT(";\n");

	}
	return sql;
}

