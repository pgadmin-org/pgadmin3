//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgExtension.cpp - Extension class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgExtension.h"


pgExtension::pgExtension(const wxString &newName)
	: pgDatabaseObject(extensionFactory, newName)
{
}

wxString pgExtension::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on extension");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing extension");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop extension \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop extension \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop extension cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop extension?");
			break;
		case PROPERTIESREPORT:
			message = _("Extension properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Extension properties");
			break;
		case DDLREPORT:
			message = _("Extension DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Extension DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("Extension dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Extension dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Extension dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Extension dependents");
			break;
	}

	return message;
}

bool pgExtension::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("DROP EXTENSION ") + GetQuotedIdentifier();
	if (cascaded)
		sql += wxT(" CASCADE");
	return GetDatabase()->ExecuteVoid(sql);
}


wxString pgExtension::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Extension: ") + GetQuotedIdentifier() + wxT("\n\n")
		      + wxT("-- DROP EXTENSION ") + GetQuotedIdentifier() + wxT(";")
		      + wxT("\n\n CREATE EXTENSION ") + GetName();

		if (!GetSchemaStr().IsEmpty())
			sql += wxT("\n  SCHEMA ") + qtIdent(GetSchemaStr());
		if (!GetVersion().IsEmpty())
			sql += wxT("\n  VERSION ") + qtIdent(GetVersion());

		sql += wxT(";\n");
	}
	return sql;
}


void pgExtension::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetOid());
		properties->AppendItem(_("Owner"), GetOwner());
		properties->AppendItem(_("Schema"), GetSchemaStr());
		properties->AppendYesNoItem(_("Relocatable?"), GetIsRelocatable());
		properties->AppendItem(_("Version"), GetVersion());
		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
	}
}



pgObject *pgExtension::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *language = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		language = extensionFactory.CreateObjects(coll, 0, wxT("\n   AND x.oid=") + GetOidStr());

	return language;
}



pgObject *pgExtensionFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	wxString sql;
	pgExtension *extension = 0;

	sql = wxT("select x.oid, pg_get_userbyid(extowner) AS owner, x.extname, n.nspname, x.extrelocatable, x.extversion, e.comment")
	      wxT("  FROM pg_extension x\n")
	      wxT("  JOIN pg_namespace n on x.extnamespace=n.oid\n")
	      wxT("  join pg_available_extensions() e(name, default_version, comment) ON x.extname=e.name\n")
	      + restriction + wxT("\n")
	      wxT(" ORDER BY x.extname");
	pgSet *extensions = collection->GetDatabase()->ExecuteSet(sql);

	if (extensions)
	{
		while (!extensions->Eof())
		{

			extension = new pgExtension(extensions->GetVal(wxT("extname")));
			extension->iSetDatabase(collection->GetDatabase());
			extension->iSetOid(extensions->GetOid(wxT("oid")));
			extension->iSetOwner(extensions->GetVal(wxT("owner")));
			extension->iSetSchemaStr(extensions->GetVal(wxT("nspname")));
			extension->iSetIsRelocatable(extensions->GetBool(wxT("extrelocatable")));
			extension->iSetVersion(extensions->GetVal(wxT("extversion")));
			extension->iSetComment(extensions->GetVal(wxT("comment")));

			if (browser)
			{
				browser->AppendObject(collection, extension);

				extensions->MoveNext();
			}
			else
				break;
		}

		delete extensions;
	}
	return extension;
}


/////////////////////////////

pgExtensionCollection::pgExtensionCollection(pgaFactory *factory, pgDatabase *db)
	: pgDatabaseObjCollection(factory, db)
{
}


wxString pgExtensionCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on extensions");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing extensions");
			break;
		case OBJECTSLISTREPORT:
			message = _("Extensions list report");
			break;
	}

	return message;
}

///////////////////////////////////////////////////

#include "images/extension.pngc"
#include "images/extension-sm.pngc"
#include "images/extensions.pngc"

pgExtensionFactory::pgExtensionFactory()
	: pgDatabaseObjFactory(__("Extension"), __("New Extension..."), __("Create a new Extension."), extension_png_img, extension_sm_png_img)
{
}


pgCollection *pgExtensionFactory::CreateCollection(pgObject *obj)
{
	return new pgExtensionCollection(GetCollectionFactory(), (pgDatabase *)obj);
}

pgExtensionFactory extensionFactory;
static pgaCollectionFactory cf(&extensionFactory, __("Extensions"), extensions_png_img);
