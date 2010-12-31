//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgSchema.cpp - PostgreSQL Schema Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "dlg/dlgSchema.h"
#include "schema/pgSchema.h"


// pointer to controls

BEGIN_EVENT_TABLE(dlgSchema, dlgDefaultSecurityProperty)
END_EVENT_TABLE();

dlgProperty *pgSchemaBaseFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgSchema(this, frame, (pgSchema *)node);
}

dlgSchema::dlgSchema(pgaFactory *f, frmMain *frame, pgSchema *node)
	: dlgDefaultSecurityProperty(f, frame, node, wxT("dlgSchema"), wxT("USAGE,CREATE"), "UC")
{
	schema = node;
}


pgObject *dlgSchema::GetObject()
{
	return schema;
}


int dlgSchema::Go(bool modal)
{
	wxString strDefPrivsOnTables, strDefPrivsOnSeqs, strDefPrivsOnFuncs;

	if (!schema)
		cbOwner->Append(wxT(""));

	AddGroups(cbOwner);
	AddUsers(cbOwner);
	if (schema)
	{
		if (connection->BackendMinimumVersion(9, 0))
		{
			strDefPrivsOnTables = schema->GetDefPrivsOnTables();
			strDefPrivsOnSeqs   = schema->GetDefPrivsOnSequences();
			strDefPrivsOnFuncs  = schema->GetDefPrivsOnFunctions();
		}

		// edit mode
		if (!connection->BackendMinimumVersion(7, 5))
			cbOwner->Disable();

		if (schema->GetMetaType() == PGM_CATALOG)
		{
			cbOwner->Disable();
			txtName->Disable();
		}
	}
	else
	{
		// create mode
	}

	return dlgDefaultSecurityProperty::Go(modal, true, strDefPrivsOnTables, strDefPrivsOnSeqs, strDefPrivsOnFuncs);
}


pgObject *dlgSchema::CreateObject(pgCollection *collection)
{
	wxString name = GetName();

	pgObject *obj = schemaFactory.CreateObjects(collection, 0, wxT(" WHERE nspname=") + qtDbString(name) + wxT("\n"));
	return obj;
}


#ifdef __WXMAC__
void dlgSchema::OnChangeSize(wxSizeEvent &ev)
{
	SetPrivilegesLayout();
	if (GetAutoLayout())
	{
		Layout();
	}
}
#endif


void dlgSchema::CheckChange()
{
	wxString name = GetName();
	if (schema)
	{
		EnableOK(name != schema->GetName() || txtComment->GetValue() != schema->GetComment()
		         || cbOwner->GetValue() != schema->GetOwner());
	}
	else
	{

		bool enable = true;
		CheckValid(enable, !name.IsEmpty(), _("Please specify name."));

		EnableOK(enable);
	}
}



wxString dlgSchema::GetSql()
{
	wxString sql, name;
	name = qtIdent(GetName());

	if (schema)
	{
		// edit mode
		AppendNameChange(sql);
		AppendOwnerChange(sql, wxT("SCHEMA ") + name);
	}
	else
	{
		// create mode
		sql = wxT("CREATE SCHEMA ") + name;
		AppendIfFilled(sql, wxT("\n       AUTHORIZATION "), qtIdent(cbOwner->GetValue()));
		sql += wxT(";\n");

	}
	AppendComment(sql, wxT("SCHEMA"), 0, schema);

	sql += GetGrant(wxT("UC"), wxT("SCHEMA ") + name);

	if (connection->BackendMinimumVersion(9, 0) && defaultSecurityChanged)
		sql += GetDefaultPrivileges(name);

	return sql;
}


