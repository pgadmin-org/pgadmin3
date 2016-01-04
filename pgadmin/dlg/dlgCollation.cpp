//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgCollation.cpp - PostgreSQL Collation Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "utils/pgDefs.h"

#include "dlg/dlgCollation.h"
#include "schema/pgSchema.h"
#include "schema/pgCollation.h"
#include "schema/pgDatatype.h"


// pointer to controls
#define txtLocale           CTRL_TEXT("txtLocale")
#define txtLcCollate        CTRL_TEXT("txtLcCollate")
#define txtLcCtype          CTRL_TEXT("txtLcCtype")
#define cbCollation         CTRL_COMBOBOX2("cbCollation")


BEGIN_EVENT_TABLE(dlgCollation, dlgTypeProperty)
	EVT_TEXT(XRCID("txtLocale"),                    dlgProperty::OnChange)
	EVT_TEXT(XRCID("txtLcCollate"),                 dlgProperty::OnChange)
	EVT_TEXT(XRCID("txtLcCtype"),                   dlgProperty::OnChange)
	EVT_TEXT(XRCID("cbCollation"),                  dlgProperty::OnChange)
	EVT_COMBOBOX(XRCID("cbCollation"),              dlgProperty::OnChange)
END_EVENT_TABLE();


dlgProperty *pgCollationFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgCollation(this, frame, (pgCollation *)node, (pgSchema *)parent);
}


dlgCollation::dlgCollation(pgaFactory *f, frmMain *frame, pgCollation *node, pgSchema *sch)
	: dlgTypeProperty(f, frame, wxT("dlgCollation"))
{
	schema = sch;
	collation = node;
}


pgObject *dlgCollation::GetObject()
{
	return collation;
}


int dlgCollation::Go(bool modal)
{
	if (collation)
	{
		// edit mode
		txtLcCollate->SetValue(collation->GetLcCollate());
		txtLcCtype->SetValue(collation->GetLcCtype());

		txtLocale->Disable();
		txtLcCollate->Disable();
		txtLcCtype->Disable();
		cbCollation->Disable();
	}
	else
	{
		// create mode
		// fill collation combobox
		cbCollation->Append(wxEmptyString);
		pgSet *set = connection->ExecuteSet(
		                 wxT("SELECT nspname, collname\n")
		                 wxT("  FROM pg_collation c, pg_namespace n\n")
		                 wxT("  WHERE c.collnamespace=n.oid\n")
		                 wxT("  ORDER BY nspname, collname"));
		if (set)
		{
			while (!set->Eof())
			{
				wxString name = qtIdent(set->GetVal(wxT("nspname"))) + wxT(".") + qtIdent(set->GetVal(wxT("collname")));
				cbCollation->Append(name);
				set->MoveNext();
			}
			delete set;
		}
		cbCollation->SetSelection(0);
	}

	return dlgProperty::Go(modal);
}


pgObject *dlgCollation::CreateObject(pgCollection *collection)
{
	wxString name = GetName();

	pgObject *obj = collationFactory.CreateObjects(collection, 0,
	                wxT("   AND c.collname=") + qtDbString(name) +
	                wxT("\n   AND c.collnamespace=") + schema->GetOidStr() +
	                wxT("\n"));
	return obj;
}


void dlgCollation::CheckChange()
{
	if (collation)
	{
		EnableOK(txtName->GetValue() != collation->GetName()
		         || cbSchema->GetValue() != collation->GetSchema()->GetName()
		         || cbOwner->GetValue() != collation->GetOwner()
		         || txtComment->GetValue() != collation->GetComment());
	}
	else
	{
		bool enable = true;
		CheckValid(enable, !GetName().IsEmpty(), _("Please specify name."));
		CheckValid(enable,
		           !txtLocale->GetValue().IsEmpty() ||
		           !(txtLcCollate->GetValue().IsEmpty() && txtLcCtype->GetValue().IsEmpty()) ||
		           !cbCollation->GetValue().IsEmpty(),
		           _("Please specify a locale, or LC_COLLATE and LC_CTYPE, or a collation"));

		txtLocale->Enable(cbCollation->GetValue().IsEmpty() && txtLcCollate->GetValue().IsEmpty() && txtLcCtype->GetValue().IsEmpty());
		txtLcCollate->Enable(cbCollation->GetValue().IsEmpty() && txtLocale->GetValue().IsEmpty());
		txtLcCtype->Enable(cbCollation->GetValue().IsEmpty() && txtLocale->GetValue().IsEmpty());
		cbCollation->Enable(txtLocale->GetValue().IsEmpty() && txtLcCollate->GetValue().IsEmpty() && txtLcCtype->GetValue().IsEmpty());

		EnableOK(enable);
	}
}


wxString dlgCollation::GetSql()
{
	wxString sql;
	wxString name;

	if (collation)
	{
		// edit mode
		name = schema->GetQuotedPrefix() + qtIdent(GetName());;
		AppendNameChange(sql, wxT("COLLATION ") + collation->GetQuotedFullIdentifier());
		AppendOwnerChange(sql, wxT("COLLATION ") + name);
		AppendSchemaChange(sql, wxT("COLLATION ") + name);
	}
	else
	{
		// create mode
		name = qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName());

		sql = wxT("CREATE COLLATION ") + name;
		if (cbCollation->GetValue().IsEmpty())
		{
			if (txtLocale->GetValue().IsEmpty())
			{
				sql += wxT("(LC_COLLATE=") + qtDbString(txtLcCollate->GetValue())
				       +  wxT(", LC_CTYPE=") + qtDbString(txtLcCtype->GetValue())
				       +  wxT(")");
			}
			else
			{
				sql += wxT("(LOCALE=") + qtDbString(txtLocale->GetValue()) + wxT(")");
			}
		}
		else
		{
			sql += wxT(" FROM ") + cbCollation->GetValue();
		}
		sql += wxT(";\n");

		AppendOwnerNew(sql, wxT("COLLATION ") + schema->GetQuotedPrefix() + qtIdent(name));
	}
	AppendComment(sql, wxT("COLLATION ") + qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()), collation);

	return sql;
}

