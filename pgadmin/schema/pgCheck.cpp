//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgCheck.cpp - Check class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgCheck.h"


pgCheck::pgCheck(pgTable *newTable, const wxString &newName)
	: pgTableObject(newTable, checkFactory, newName)
{
}

pgCheck::~pgCheck()
{
}


bool pgCheck::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("ALTER TABLE ") + qtIdent(fkSchema) + wxT(".") + qtIdent(fkTable)
	               + wxT(" DROP CONSTRAINT ") + GetQuotedIdentifier();
	if (cascaded)
		sql += wxT(" CASCADE");
	return GetDatabase()->ExecuteVoid(sql);
}


wxString pgCheck::GetConstraint()
{
	return GetQuotedIdentifier() +  wxT(" CHECK (") + GetDefinition() + wxT(")");
}


wxString pgCheck::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Check: ") + GetQuotedFullIdentifier() + wxT("\n\n")
		      + wxT("-- ALTER TABLE ") + GetQuotedSchemaPrefix(fkSchema) + qtIdent(fkTable)
		      + wxT(" DROP CONSTRAINT ") + GetQuotedIdentifier()
		      + wxT(";\n\nALTER TABLE ") + GetQuotedSchemaPrefix(fkSchema) + qtIdent(fkTable)
		      + wxT("\n  ADD CONSTRAINT ") + GetConstraint()
		      + wxT(";\n");

		if (!GetComment().IsNull())
		{
			sql += wxT("COMMENT ON CONSTRAINT ") + GetQuotedIdentifier() + wxT(" ON ") + GetQuotedSchemaPrefix(fkSchema) + qtIdent(fkTable)
			       + wxT(" IS ") + qtDbString(GetComment()) + wxT(";\n");
		}
	}

	return sql;
}


void pgCheck::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetOid());
		properties->AppendItem(_("Definition"), GetDefinition());
		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
	}
}


pgObject *pgCheck::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *check = 0;

	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		check = checkFactory.CreateObjects(coll, 0, wxT("\n   AND c.oid=") + GetOidStr());

	return check;
}



pgObject *pgCheckFactory::CreateObjects(pgCollection *coll, ctlTree *browser, const wxString &restriction)
{
	pgTableObjCollection *collection = (pgTableObjCollection *)coll;
	pgCheck *check = 0;
	pgSet *checks = collection->GetDatabase()->ExecuteSet(
	                    wxT("SELECT c.oid, conname, relname, nspname, description,\n")
	                    wxT("       pg_get_expr(conbin, conrelid") + collection->GetDatabase()->GetPrettyOption() + wxT(") as consrc\n")
	                    wxT("  FROM pg_constraint c\n")
	                    wxT("  JOIN pg_class cl ON cl.oid=conrelid\n")
	                    wxT("  JOIN pg_namespace nl ON nl.oid=relnamespace\n")
	                    wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=c.oid\n")
	                    wxT(" WHERE contype = 'c' AND conrelid =  ") + NumToStr(collection->GetOid())
	                    + restriction + wxT("::oid\n")
	                    wxT(" ORDER BY conname"));

	if (checks)
	{
		while (!checks->Eof())
		{
			check = new pgCheck(collection->GetTable(), checks->GetVal(wxT("conname")));

			check->iSetOid(checks->GetOid(wxT("oid")));
			check->iSetDefinition(checks->GetVal(wxT("consrc")));
			check->iSetFkTable(checks->GetVal(wxT("relname")));
			check->iSetFkSchema(checks->GetVal(wxT("nspname")));
			check->iSetComment(checks->GetVal(wxT("description")));

			if (browser)
			{
				browser->AppendObject(collection, check);
				checks->MoveNext();
			}
			else
				break;
		}

		delete checks;
	}
	return check;
}


/////////////////////////////

#include "images/check.xpm"

pgCheckFactory::pgCheckFactory()
	: pgTableObjFactory(__("Check"), __("New Check..."), __("Create a new Check constraint."), check_xpm)
{
	metaType = PGM_CHECK;
	collectionFactory = &constraintCollectionFactory;
}


pgCheckFactory checkFactory;
