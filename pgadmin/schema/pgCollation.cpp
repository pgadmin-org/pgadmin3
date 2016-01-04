//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgCollation.cpp - Collation class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgCollation.h"
#include "schema/pgDatatype.h"


/*
dlgProperty *pgCollationFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return 0L;
}
*/


pgCollation::pgCollation(pgSchema *newSchema, const wxString &newName)
	: pgSchemaObject(newSchema, collationFactory, newName)
{
}

pgCollation::~pgCollation()
{
}

wxString pgCollation::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on collation");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing collation");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop collation \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop collation \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop collation cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop collation?");
			break;
		case PROPERTIESREPORT:
			message = _("Collation properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Collation properties");
			break;
		case DDLREPORT:
			message = _("Collation DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Collation DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("Collation dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Collation dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Collation dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Collation dependents");
			break;
	}

	return message;
}


bool pgCollation::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("DROP COLLATION ") + this->GetSchema()->GetQuotedIdentifier() + wxT(".") + this->GetQuotedIdentifier();
	if (cascaded)
		sql += wxT(" CASCADE");
	return GetDatabase()->ExecuteVoid(sql);
}

wxString pgCollation::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Collation: ") + GetQuotedFullIdentifier() + wxT("\n\n")
		      + wxT("-- DROP COLLATION ") + GetQuotedFullIdentifier() + wxT(";")
		      + wxT("\n\nCREATE COLLATION ") + GetQuotedFullIdentifier()
		      + wxT("\n  (LC_COLLATE=") + qtDbString(GetLcCollate())
		      + wxT(", LC_CTYPE=") + qtDbString(GetLcCtype())
		      + wxT(");\n");

		sql += GetOwnerSql(9, 1)
		       + GetCommentSql();
	}

	return sql;
}


void pgCollation::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetOid());
		properties->AppendItem(_("Owner"), GetOwner());
		properties->AppendItem(_("LC_COLLATE"), GetLcCollate());
		properties->AppendItem(_("LC_CTYPE"), GetLcCtype());
		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
	}
}



pgObject *pgCollation::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *collation = 0;

	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		collation = collationFactory.CreateObjects(coll, 0, wxT("   AND c.oid=") + GetOidStr() + wxT("\n"));

	return collation;
}


////////////////////////////////////////////////////



pgObject *pgCollationFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	pgCollation *collation = 0;

	pgDatabase *db = collection->GetDatabase();

	pgSet *collations = db->ExecuteSet(
	                        wxT("SELECT c.oid, c.collname, c.collcollate, c.collctype, \n")
	                        wxT("       pg_get_userbyid(c.collowner) as cowner, description\n")
	                        wxT("  FROM pg_collation c\n")
	                        wxT("  JOIN pg_namespace n ON n.oid=c.collnamespace\n")
	                        wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=c.oid AND des.classoid='pg_collation'::regclass)\n")
	                        wxT(" WHERE c.collnamespace = ") + NumToStr(collection->GetSchema()->GetOid()) + wxT("::oid\n")
	                        + restriction +
	                        wxT(" ORDER BY c.collname"));

	if (collations)
	{
		while (!collations->Eof())
		{
			collation = new pgCollation(collection->GetSchema(), collations->GetVal(wxT("collname")));

			collation->iSetOid(collations->GetOid(wxT("oid")));
			collation->iSetOwner(collations->GetVal(wxT("cowner")));
			collation->iSetLcCollate(collations->GetVal(wxT("collcollate")));
			collation->iSetLcCtype(collations->GetVal(wxT("collctype")));
			collation->iSetComment(collations->GetVal(wxT("description")));

			if (browser)
			{
				browser->AppendObject(collection, collation);
				collations->MoveNext();
			}
			else
				break;
		}

		delete collations;
	}
	return collation;
}

/////////////////////////////

pgCollationCollection::pgCollationCollection(pgaFactory *factory, pgSchema *sch)
	: pgSchemaObjCollection(factory, sch)
{
}


wxString pgCollationCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on collations");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing collations");
			break;
		case OBJECTSLISTREPORT:
			message = _("Collations list report");
			break;
	}

	return message;
}

/////////////////////////////

#include "images/collation.pngc"
#include "images/collation-sm.pngc"
#include "images/collations.pngc"

pgCollationFactory::pgCollationFactory()
	: pgSchemaObjFactory(__("Collation"), __("New Collation..."), __("Create a new Collation."), collation_png_img, collation_sm_png_img)
{
}


pgCollection *pgCollationFactory::CreateCollection(pgObject *obj)
{
	return new pgCollationCollection(GetCollectionFactory(), (pgSchema *)obj);
}

pgCollationFactory collationFactory;
static pgaCollectionFactory cf(&collationFactory, __("Collations"), collations_png_img);
