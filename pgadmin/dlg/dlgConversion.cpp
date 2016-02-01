//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgConversion.cpp - PostgreSQL Conversion Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "utils/pgDefs.h"

#include "dlg/dlgConversion.h"
#include "schema/pgConversion.h"
#include "schema/pgSchema.h"


// pointer to controls
#define cbSourceEncoding    CTRL_COMBOBOX("cbSourceEncoding")
#define cbTargetEncoding    CTRL_COMBOBOX("cbTargetEncoding")
#define cbFunction          CTRL_COMBOBOX("cbFunction")
#define chkDefault          CTRL_CHECKBOX("chkDefault")



BEGIN_EVENT_TABLE(dlgConversion, dlgProperty)
	EVT_TEXT(XRCID("cbSourceEncoding"),     dlgProperty::OnChange)
	EVT_COMBOBOX(XRCID("cbSourceEncoding"), dlgProperty::OnChange)
	EVT_TEXT(XRCID("cbTargetEncoding"),     dlgProperty::OnChange)
	EVT_COMBOBOX(XRCID("cbTargetEncoding"), dlgProperty::OnChange)
	EVT_TEXT(XRCID("cbFunction"),           dlgProperty::OnChange)
	EVT_COMBOBOX(XRCID("cbFunction"),       dlgProperty::OnChange)
END_EVENT_TABLE();


dlgProperty *pgConversionFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgConversion(this, frame, (pgConversion *)node, (pgSchema *)parent);
}



dlgConversion::dlgConversion(pgaFactory *f, frmMain *frame, pgConversion *node, pgSchema *sch)
	: dlgProperty(f, frame, wxT("dlgConversion"))
{
	conversion = node;
	schema = sch;
}


pgObject *dlgConversion::GetObject()
{
	return conversion;
}


int dlgConversion::Go(bool modal)
{
	if (!connection->BackendMinimumVersion(7, 4))
		txtComment->Disable();

	if (!connection->BackendMinimumVersion(7, 5))
		cbOwner->Disable();

	if (conversion)
	{
		// edit mode
		cbSchema->Enable(connection->BackendMinimumVersion(9, 1));
		cbSourceEncoding->Append(conversion->GetForEncoding());
		cbSourceEncoding->SetSelection(0);
		cbTargetEncoding->Append(conversion->GetToEncoding());
		cbTargetEncoding->SetSelection(0);

		cbFunction->Append(database->GetSchemaPrefix(conversion->GetProcNamespace()) + conversion->GetProc());
		cbFunction->SetSelection(0);

		if (!connection->BackendMinimumVersion(7, 4))
			txtName->Disable();
		chkDefault->SetValue(conversion->GetDefaultConversion());
		cbSourceEncoding->Disable();
		cbTargetEncoding->Disable();
		cbFunction->Disable();
		chkDefault->Disable();
	}
	else
	{
		// create mode

		wxString qry =
		    wxT("SELECT proname, nspname\n")
		    wxT("  FROM pg_proc p\n")
		    wxT("  JOIN pg_namespace n ON n.oid=pronamespace")
		    wxT("\n WHERE prorettype = ") + NumToStr(PGOID_TYPE_VOID) +
		    wxT("\n    AND pronargs = 5");

		pgSet *set = connection->ExecuteSet(qry);
		if (set)
		{
			while (!set->Eof())
			{
				functions.Add(database->GetQuotedSchemaPrefix(set->GetVal(wxT("nspname"))) + qtIdent(set->GetVal(wxT("proname"))));
				cbFunction->Append(database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("proname")));

				set->MoveNext();
			}
			delete set;
		}

		long encNo = 0;
		wxString encStr;
		do
		{
			encStr = connection->ExecuteScalar(
			             wxT("SELECT pg_encoding_to_char(") + NumToStr(encNo) + wxT(")"));
			if (!encStr.IsEmpty())
			{
				cbSourceEncoding->Append(encStr);
				cbTargetEncoding->Append(encStr);
			}
			encNo++;
		}
		while (!encStr.IsEmpty());
	}

	return dlgProperty::Go(modal);
}


pgObject *dlgConversion::CreateObject(pgCollection *collection)
{
	pgObject *obj = conversionFactory.CreateObjects(collection, 0,
	                wxT("\n AND conname = ") + qtDbString(GetName()));

	return obj;
}


void dlgConversion::CheckChange()
{
	if (conversion)
	{
		EnableOK(txtName->GetValue() != conversion->GetName()
		         || cbSchema->GetValue() != conversion->GetSchema()->GetName()
		         || txtComment->GetValue() != conversion->GetComment()
		         || cbOwner->GetValue() != conversion->GetOwner());
	}
	else
	{
		bool enable = true;
		CheckValid(enable, !GetName().IsEmpty(), _("Please specify name."));
		CheckValid(enable, !cbSourceEncoding->GetValue().IsEmpty(), _("Please specify source encoding."));
		CheckValid(enable, !cbTargetEncoding->GetValue().IsEmpty(), _("Please specify target encoding."));
		CheckValid(enable, cbFunction->GetCurrentSelection() >= 0, _("Please specify conversion function."));

		EnableOK(enable);
	}
}




wxString dlgConversion::GetSql()
{
	wxString sql;
	wxString name;

	if (conversion)
	{
		// edit mode
		name = GetName();

		AppendNameChange(sql);
		AppendOwnerChange(sql, wxT("CONVERSION ") + schema->GetQuotedPrefix() + qtIdent(name));

		if (cbSchema->GetValue() != conversion->GetSchema()->GetName())
		{
			sql += wxT("ALTER CONVERSION ") + qtIdent(conversion->GetSchema()->GetName()) + wxT(".") + qtIdent(name)
			       +  wxT("\n  SET SCHEMA ") + qtIdent(cbSchema->GetValue())
			       +  wxT(";\n");
		}
	}
	else
	{
		name = qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName());

		// create mode
		sql = wxT("CREATE ");
		if (chkDefault->GetValue())
			sql += wxT("DEFAULT ");
		sql += wxT("CONVERSION ") + name
		       + wxT("\n   FOR ") + qtDbString(cbSourceEncoding->GetValue())
		       + wxT(" TO ") + qtDbString(cbTargetEncoding->GetValue())
		       + wxT("\n   FROM ") + functions.Item(cbFunction->GetCurrentSelection())
		       + wxT(";\n");

		AppendOwnerNew(sql, wxT("CONVERSION ") + schema->GetQuotedPrefix() + qtIdent(name));
	}
	AppendComment(sql, wxT("CONVERSION ") + qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()), conversion);

	return sql;
}
