//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgExtension.cpp - PostgreSQL Extension Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "utils/pgDefs.h"

#include "dlg/dlgExtension.h"
#include "schema/pgExtension.h"


// pointer to controls
#define cbName          CTRL_COMBOBOX("cbName")
#define cbObjectsSchema CTRL_COMBOBOX("cbObjectsSchema")
#define cbVersion       CTRL_COMBOBOX("cbVersion")


dlgProperty *pgExtensionFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgExtension(this, frame, (pgExtension *)node);
}


BEGIN_EVENT_TABLE(dlgExtension, dlgProperty)
	EVT_TEXT(XRCID("cbName"),                       dlgExtension::OnChangeName)
	EVT_COMBOBOX(XRCID("cbName"),                   dlgExtension::OnChangeName)
	EVT_TEXT(XRCID("cbObjectsSchema"),                     dlgProperty::OnChange)
	EVT_COMBOBOX(XRCID("cbObjectsSchema"),                 dlgProperty::OnChange)
	EVT_TEXT(XRCID("cbVersion"),                    dlgProperty::OnChange)
	EVT_COMBOBOX(XRCID("cbVersion"),                dlgProperty::OnChange)
END_EVENT_TABLE();


dlgExtension::dlgExtension(pgaFactory *f, frmMain *frame, pgExtension *node)
	: dlgProperty(f, frame, wxT("dlgExtension"))
{
	extension = node;
}


pgObject *dlgExtension::GetObject()
{
	return extension;
}


int dlgExtension::Go(bool modal)
{
	txtComment->Disable();

	// add all schemas
	cbObjectsSchema->Append(wxEmptyString);
	pgSetIterator schemas(connection,
	                      wxT("SELECT nspname FROM pg_namespace\n")
	                      wxT(" ORDER BY nspname"));

	while (schemas.RowsLeft())
		cbObjectsSchema->Append(schemas.GetVal(wxT("nspname")));
	cbObjectsSchema->SetSelection(0);

	if (extension)
	{
		// edit mode
		cbName->Append(extension->GetName());
		cbName->SetSelection(0);
		cbName->Disable();

		cbObjectsSchema->SetValue(extension->GetSchemaStr());
		cbObjectsSchema->Enable(extension->GetIsRelocatable());

		// add all versions
		cbVersion->Clear();
		cbVersion->Append(wxEmptyString);
		pgSetIterator versions(connection,
		                       wxT("SELECT version, relocatable FROM pg_available_extension_versions\n")
		                       wxT(" WHERE name=") + qtDbString(cbName->GetValue()) + wxT(" ")
		                       wxT(" ORDER BY version"));

		while (versions.RowsLeft())
			cbVersion->Append(versions.GetVal(wxT("version")));
		cbVersion->SetValue(extension->GetVersion());
	}
	else
	{
		// create mode

		// add available extensions (but not the installed ones)
		cbName->Append(wxEmptyString);
		pgSetIterator extensions(connection,
		                         wxT("SELECT name FROM pg_available_extensions\n")
		                         wxT(" WHERE installed_version IS NULL\n")
		                         wxT(" ORDER BY name"));

		while (extensions.RowsLeft())
			cbName->Append(extensions.GetVal(wxT("name")));
		cbName->SetSelection(0);
	}

	return dlgProperty::Go(modal);
}


pgObject *dlgExtension::CreateObject(pgCollection *collection)
{
	wxString name = cbName->wxComboBox::GetValue();

	pgObject *obj = extensionFactory.CreateObjects(collection, 0, wxT("\n   AND extname ILIKE ") + qtDbString(name));
	return obj;
}


void dlgExtension::OnChangeName(wxCommandEvent &ev)
{
	bool relocatable;

	// add all versions
	cbVersion->Clear();
	cbVersion->Append(wxEmptyString);
	pgSetIterator versions(connection,
	                       wxT("SELECT version, relocatable FROM pg_available_extension_versions\n")
	                       wxT(" WHERE name=") + qtDbString(cbName->GetValue()) + wxT(" ")
	                       wxT(" ORDER BY version"));

	while (versions.RowsLeft())
	{
		relocatable = versions.GetBool(wxT("relocatable"));
		cbVersion->Append(versions.GetVal(wxT("version")));
	}
	cbVersion->SetSelection(0);

	if (relocatable)
	{
		cbObjectsSchema->Enable();
	}
	else
	{
		cbObjectsSchema->SetSelection(0);
		cbObjectsSchema->Disable();
	}

	OnChange(ev);
}


void dlgExtension::CheckChange()
{
	bool didChange = true;
	if (extension)
	{
		didChange = cbObjectsSchema->GetValue() != extension->GetSchemaStr()
		            || cbVersion->GetValue() != extension->GetVersion();
		EnableOK(didChange);
	}
	else
	{
		bool enable = true;

		CheckValid(enable, !cbName->GetValue().IsEmpty(), _("Please specify name."));
		EnableOK(enable);
	}
}


wxString dlgExtension::GetSql()
{
	wxString sql;

	if (extension)
	{
		// edit mode
		if (cbObjectsSchema->GetValue() != extension->GetSchemaStr())
			sql += wxT("ALTER EXTENSION ") + qtIdent(extension->GetName())
			       +  wxT("\n  SET SCHEMA ") + qtIdent(cbObjectsSchema->GetValue()) + wxT(";\n");
		if (cbVersion->GetValue() != extension->GetVersion())
			sql += wxT("ALTER EXTENSION ") + qtIdent(extension->GetName())
			       +  wxT("\n  UPDATE TO ") + qtIdent(cbVersion->GetValue()) + wxT(";\n");
	}
	else
	{
		sql = wxT("CREATE EXTENSION ") + qtIdent(cbName->GetValue());
		AppendIfFilled(sql, wxT("\n   SCHEMA "), qtIdent(cbObjectsSchema->GetValue()));
		AppendIfFilled(sql, wxT("\n   VERSION "), qtIdent(cbVersion->GetValue()));
	}

	return sql;
}
