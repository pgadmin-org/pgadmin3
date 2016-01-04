//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgCatalogObject.cpp - EnterpriseDB catalog class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgCatalogObject.h"
#include "schema/pgColumn.h"


pgCatalogObject::pgCatalogObject(pgSchema *newSchema, const wxString &newName)
	: pgSchemaObject(newSchema, catalogObjectFactory, newName)
{
}

wxString pgCatalogObject::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on catalog object");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing catalog object");
			message += wxT(" ") + GetName();
			break;
		case PROPERTIESREPORT:
			message = _("Catalog object properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Catalog object properties");
			break;
		case DEPENDENCIESREPORT:
			message = _("Catalog object dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Catalog object dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Catalog object dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Catalog object dependents");
			break;
	}

	return message;
}


wxString pgCatalogObject::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Catalog Object: ") + GetQuotedIdentifier() + wxT("\n");
	}
	return sql;
}


void pgCatalogObject::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (!expandedKids)
	{
		expandedKids = true;

		browser->AppendCollection(this, columnFactory);
	}

	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetOid());
		properties->AppendItem(_("Owner"), GetOwner());
		properties->AppendItem(_("Comment"), GetComment());
	}
}



pgObject *pgCatalogObject::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *catalog = 0;
	pgCollection *parent = (pgCollection *)browser->GetItemData(browser->GetItemParent(item));
	if (parent)
		catalog = catalogObjectFactory.CreateObjects(parent, 0, wxT("\n AND c.oid=") + GetOidStr());

	return catalog;
}



pgObject *pgCatalogObjectFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	pgCatalogObject *catalog = 0;

	wxString qry = wxT("SELECT c.oid, c.relname, pg_get_userbyid(relowner) AS owner, description\n")
	               wxT("  FROM pg_class c\n")
	               wxT("  LEFT OUTER JOIN pg_description d ON (d.objoid=c.oid AND d.classoid='pg_class'::regclass)\n")
	               wxT("  WHERE relnamespace = ") + NumToStr(collection->GetSchema()->GetOid()) + wxT("::oid\n");

	qry += restriction +
	       wxT(" ORDER BY relname");

	pgSet *catalogs = collection->GetServer()->ExecuteSet(qry);

	if (catalogs)
	{
		while (!catalogs->Eof())
		{
			// On EnterpriseDB we need to ignore some objects in the sys
			// catalog, namely, _*, dual and type_object_source.
			if (!settings->GetShowSystemObjects() &&
			        collection->GetSchema()->GetName() == wxT("sys") &&
			        (catalogs->GetVal(wxT("relname")).StartsWith(wxT("_")) ||
			         catalogs->GetVal(wxT("relname")) == wxT("dual") ||
			         catalogs->GetVal(wxT("relname")) == wxT("type_object_source")))
			{
				catalogs->MoveNext();
				continue;
			}

			catalog = new pgCatalogObject(collection->GetSchema(), catalogs->GetVal(wxT("relname")));
			catalog->iSetOid(catalogs->GetOid(wxT("oid")));
			catalog->iSetOwner(catalogs->GetVal(wxT("owner")));
			catalog->iSetComment(catalogs->GetVal(wxT("description")));

			if (browser)
			{
				browser->AppendObject(collection, catalog);
				catalogs->MoveNext();
			}
			else
				break;
		}

		delete catalogs;
	}
	return catalog;
}

/////////////////////////////

pgCatalogObjectCollection::pgCatalogObjectCollection(pgaFactory *factory, pgSchema *sch)
	: pgSchemaObjCollection(factory, sch)
{
}


wxString pgCatalogObjectCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on catalog objects");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing catalog objects");
			break;
		case OBJECTSLISTREPORT:
			message = _("Catalog objects list report");
			break;
	}

	return message;
}

/////////////////////////////

#include "images/catalogobject.pngc"
#include "images/catalogobject-sm.pngc"
#include "images/catalogobjects.pngc"

pgCatalogObjectFactory::pgCatalogObjectFactory()
	: pgSchemaObjFactory(__("Catalog Object"), __("New Catalog Object..."), __("Create a new Catalog Object."), catalogobject_png_img, catalogobject_sm_png_img)
{
	metaType = PGM_CATALOGOBJECT;
}


pgCollection *pgCatalogObjectFactory::CreateCollection(pgObject *obj)
{
	return new pgCatalogObjectCollection(GetCollectionFactory(), (pgSchema *)obj);
}

pgCatalogObjectFactory catalogObjectFactory;
static pgaCollectionFactory cf(&catalogObjectFactory, __("Catalog Objects"), catalogobjects_png_img);
