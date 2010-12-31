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


wxString pgCheck::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on check constraint");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing check constraint");
			message += wxT(" ") + GetName();
			break;
		case GRANTWIZARDTITLE:
			message = _("Privileges for check constraint");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop check constraint \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop check constraint \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop check constraint cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop check constraint?");
			break;
		case PROPERTIESREPORT:
			message = _("Check constraint properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Check constraint properties");
			break;
		case DDLREPORT:
			message = _("Check constraint DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Check constraint DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("Check constraint dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Check constraint dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Check constraint dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Check constraint dependents");
			break;
	}

	return message;
}


bool pgCheck::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	return GetDatabase()->ExecuteVoid(
	           wxT("ALTER TABLE ") + qtIdent(fkSchema) + wxT(".") + qtIdent(fkTable)
	           + wxT(" DROP CONSTRAINT ") + GetQuotedIdentifier());

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

wxString pgCheckCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on check constraints");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing check constraints");
			break;
		case OBJECTSLISTREPORT:
			message = _("Check constraints list report");
			break;
	}

	return message;
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
