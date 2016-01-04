//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgConversion.cpp - Conversion class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgConversion.h"


pgConversion::pgConversion(pgSchema *newSchema, const wxString &newName)
	: pgSchemaObject(newSchema, conversionFactory, newName)
{
}

pgConversion::~pgConversion()
{
}

wxString pgConversion::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on conversion");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing conversion");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop conversion \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop conversion \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop conversion cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop conversion?");
			break;
		case PROPERTIESREPORT:
			message = _("Conversion properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Conversion properties");
			break;
		case DDLREPORT:
			message = _("Conversion DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Conversion DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("Conversion dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Conversion dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Conversion dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Conversion dependents");
			break;
	}

	return message;
}


bool pgConversion::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("DROP CONVERSION ") + this->GetSchema()->GetQuotedIdentifier() + wxT(".") + this->GetQuotedIdentifier();
	if (cascaded)
		sql += wxT(" CASCADE");
	return GetDatabase()->ExecuteVoid(sql);
}

wxString pgConversion::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Conversion: ") + GetQuotedFullIdentifier() + wxT("\n\n")
		      + wxT("-- DROP CONVERSION ") + GetQuotedFullIdentifier() + wxT(";")
		      + wxT("\n\nCREATE ");
		if (GetDefaultConversion())
			sql += wxT("DEFAULT ");
		sql += wxT("CONVERSION ") + qtIdent(GetName())
		       + wxT("\n  FOR '") + GetForEncoding() + wxT("'")
		       + wxT("\n  TO '") + GetToEncoding() + wxT("'")
		       + wxT("\n  FROM ") + GetDatabase()->GetQuotedSchemaPrefix(GetProcNamespace())
		       + qtIdent(GetProc()) + wxT(";\n")
		       + GetOwnerSql(8, 0);
	}

	return sql;
}

void pgConversion::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetOid());
		properties->AppendItem(_("Owner"), GetOwner());
		properties->AppendItem(_("From"), GetForEncoding());
		properties->AppendItem(_("To"), GetToEncoding());
		properties->AppendItem(_("Function"), GetSchemaPrefix(GetProcNamespace()) + GetProc());
		properties->AppendYesNoItem(_("Default?"), GetDefaultConversion());
		properties->AppendYesNoItem(_("System conversion?"), GetSystemObject());
		if (GetConnection()->BackendMinimumVersion(7, 5))
			properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
	}
}




pgObject *pgConversion::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *conversion = 0;

	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		conversion = conversionFactory.CreateObjects(coll, 0, wxT("\n   AND co.oid=") + GetOidStr());

	return conversion;
}



pgObject *pgConversionFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	pgConversion *conversion = 0;

	pgSet *conversions = collection->GetDatabase()->ExecuteSet(
	                         wxT("SELECT co.oid, co.*, pg_encoding_to_char(conforencoding) as forencoding, pg_get_userbyid(conowner) as owner,")
	                         wxT("pg_encoding_to_char(contoencoding) as toencoding, proname, nspname, description\n")
	                         wxT("  FROM pg_conversion co\n")
	                         wxT("  JOIN pg_proc pr ON pr.oid=conproc\n")
	                         wxT("  JOIN pg_namespace na ON na.oid=pr.pronamespace\n")
	                         wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=co.oid AND des.objsubid=0 AND des.classoid='pg_conversion'::regclass)\n")
	                         wxT(" WHERE connamespace = ") + collection->GetSchema()->GetOidStr()
	                         + restriction + wxT("\n")
	                         wxT(" ORDER BY conname"));

	if (conversions)
	{
		while (!conversions->Eof())
		{
			conversion = new pgConversion(collection->GetSchema(),
			                              conversions->GetVal(wxT("conname")));

			conversion->iSetOid(conversions->GetOid(wxT("oid")));
			conversion->iSetOwner(conversions->GetVal(wxT("owner")));
			conversion->iSetComment(conversions->GetVal(wxT("description")));
			conversion->iSetForEncoding(conversions->GetVal(wxT("forencoding")));
			conversion->iSetToEncoding(conversions->GetVal(wxT("toencoding")));
			conversion->iSetProc(conversions->GetVal(wxT("proname")));
			conversion->iSetProcNamespace(conversions->GetVal(wxT("nspname")));
			conversion->iSetDefaultConversion(conversions->GetBool(wxT("condefault")));

			if (browser)
			{
				browser->AppendObject(collection, conversion);
				conversions->MoveNext();
			}
			else
				break;
		}

		delete conversions;
	}
	return conversion;
}

/////////////////////////////

pgConversionCollection::pgConversionCollection(pgaFactory *factory, pgSchema *sch)
	: pgSchemaObjCollection(factory, sch)
{
}


wxString pgConversionCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on conversions");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing conversions");
			break;
		case OBJECTSLISTREPORT:
			message = _("Conversions list report");
			break;
	}

	return message;
}

/////////////////////////////

#include "images/conversion.pngc"
#include "images/conversions.pngc"

pgConversionFactory::pgConversionFactory()
	: pgSchemaObjFactory(__("Conversion"), __("New Conversion..."), __("Create a new Conversion."), conversion_png_img)
{
}


pgCollection *pgConversionFactory::CreateCollection(pgObject *obj)
{
	return new pgConversionCollection(GetCollectionFactory(), (pgSchema *)obj);
}

pgConversionFactory conversionFactory;
static pgaCollectionFactory cf(&conversionFactory, __("Conversions"), conversions_png_img);
