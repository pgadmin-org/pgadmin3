//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgLanguage.cpp - Language class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgLanguage.h"


pgLanguage::pgLanguage(const wxString &newName)
	: pgDatabaseObject(languageFactory, newName)
{
}

wxString pgLanguage::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on language");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing language");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop language \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop language \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop language cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop language?");
			break;
		case PROPERTIESREPORT:
			message = _("Language properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Language properties");
			break;
		case DDLREPORT:
			message = _("Language DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Language DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("Language dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Language dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Language dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Language dependents");
			break;
	}

	return message;
}

bool pgLanguage::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("DROP LANGUAGE ") + GetQuotedFullIdentifier();
	if (cascaded)
		sql += wxT(" CASCADE");
	return GetDatabase()->ExecuteVoid(sql);
}

wxString pgLanguage::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Language: ") + GetQuotedFullIdentifier() + wxT("\n\n")
		      + wxT("-- DROP LANGUAGE ") + GetQuotedFullIdentifier() + wxT(";")
		      + wxT("\n\nCREATE ");
		if (GetTrusted())
			sql += wxT("TRUSTED ");
		sql += wxT("PROCEDURAL LANGUAGE '") + GetName()
		       +  wxT("'\n  HANDLER ") + qtIdent(GetHandlerProc());

		if (!GetInlineProc().IsEmpty())
			sql += wxT("\n  INLINE ") + qtIdent(GetInlineProc());

		if (!GetValidatorProc().IsEmpty())
			sql += wxT("\n  VALIDATOR ") + qtIdent(GetValidatorProc());

		sql += wxT(";\n")
		       +  GetOwnerSql(8, 3, wxT("LANGUAGE ") + GetName())
		       +  GetGrant(wxT("U"), wxT("LANGUAGE ") + GetQuotedFullIdentifier());

		if (GetConnection()->BackendMinimumVersion(9, 1))
			sql += GetSeqLabelsSql();
	}
	return sql;
}

void pgLanguage::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetOid());
		if (GetConnection()->BackendMinimumVersion(8, 3))
			properties->AppendItem(_("Owner"), GetOwner());
		properties->AppendItem(_("ACL"), GetAcl());
		properties->AppendYesNoItem(_("Trusted?"), GetTrusted());
		properties->AppendItem(_("Handler function"), GetHandlerProc());
		if (GetConnection()->BackendMinimumVersion(9, 0))
			properties->AppendItem(_("Inline function"), GetInlineProc());
		properties->AppendItem(_("Validator function"), GetValidatorProc());
		properties->AppendYesNoItem(_("System language?"), GetSystemObject());
		if (GetConnection()->BackendMinimumVersion(7, 5))
			properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));

		if (!GetLabels().IsEmpty())
		{
			wxArrayString seclabels = GetProviderLabelArray();
			if (seclabels.GetCount() > 0)
			{
				for (unsigned int index = 0 ; index < seclabels.GetCount() - 1 ; index += 2)
				{
					properties->AppendItem(seclabels.Item(index), seclabels.Item(index + 1));
				}
			}
		}
	}
}

pgObject *pgLanguage::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *language = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		language = languageFactory.CreateObjects(coll, 0, wxT("\n   AND lan.oid=") + GetOidStr());

	return language;
}

pgObject *pgLanguageFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	wxString sql;
	pgLanguage *language = 0;

	sql = wxT("SELECT lan.oid, lanname, lanpltrusted, lanacl, hp.proname as lanproc, vp.proname as lanval, description");
	if (collection->GetConnection()->BackendMinimumVersion(8, 3))
		sql += wxT(", pg_get_userbyid(lan.lanowner) as languageowner");
	if (collection->GetConnection()->BackendMinimumVersion(9, 0))
		sql += wxT(", ip.proname as laninl");
	if (collection->GetDatabase()->BackendMinimumVersion(9, 1))
	{
		sql += wxT(",\n(SELECT array_agg(label) FROM pg_seclabels sl1 WHERE sl1.objoid=lan.oid) AS labels");
		sql += wxT(",\n(SELECT array_agg(provider) FROM pg_seclabels sl2 WHERE sl2.objoid=lan.oid) AS providers");
	}
	sql += wxT("\n  FROM pg_language lan\n")
	       wxT("  JOIN pg_proc hp on hp.oid=lanplcallfoid\n");
	if (collection->GetConnection()->BackendMinimumVersion(9, 0))
		sql += wxT("  LEFT OUTER JOIN pg_proc ip on ip.oid=laninline\n");
	sql += wxT("  LEFT OUTER JOIN pg_proc vp on vp.oid=lanvalidator\n")
	       wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=lan.oid AND des.objsubid=0 AND des.classoid='pg_language'::regclass)\n")
	       wxT(" WHERE lanispl IS TRUE")
	       + restriction + wxT("\n")
	       wxT(" ORDER BY lanname");
	pgSet *languages = collection->GetDatabase()->ExecuteSet(sql);

	if (languages)
	{
		while (!languages->Eof())
		{

			language = new pgLanguage(languages->GetVal(wxT("lanname")));
			language->iSetDatabase(collection->GetDatabase());
			language->iSetOid(languages->GetOid(wxT("oid")));
			if (collection->GetConnection()->BackendMinimumVersion(8, 3))
			{
				language->iSetOwner(languages->GetVal(wxT("languageowner")));
			}
			language->iSetAcl(languages->GetVal(wxT("lanacl")));
			language->iSetComment(languages->GetVal(wxT("description")));
			language->iSetHandlerProc(languages->GetVal(wxT("lanproc")));
			if (collection->GetConnection()->BackendMinimumVersion(9, 0))
			{
				language->iSetInlineProc(languages->GetVal(wxT("laninl")));
			}
			language->iSetValidatorProc(languages->GetVal(wxT("lanval")));
			language->iSetTrusted(languages->GetBool(wxT("lanpltrusted")));

			if (collection->GetDatabase()->BackendMinimumVersion(9, 1))
			{
				language->iSetProviders(languages->GetVal(wxT("providers")));
				language->iSetLabels(languages->GetVal(wxT("labels")));
			}

			if (browser)
			{
				browser->AppendObject(collection, language);
				languages->MoveNext();
			}
			else
				break;
		}

		delete languages;
	}
	return language;
}

/////////////////////////////

pgLanguageCollection::pgLanguageCollection(pgaFactory *factory, pgDatabase *db)
	: pgDatabaseObjCollection(factory, db)
{
}

wxString pgLanguageCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on languages");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing languages");
			break;
		case OBJECTSLISTREPORT:
			message = _("Languages list report");
			break;
	}

	return message;
}

///////////////////////////////////////////////////

#include "images/language.pngc"
#include "images/language-sm.pngc"
#include "images/languages.pngc"

pgLanguageFactory::pgLanguageFactory()
	: pgDatabaseObjFactory(__("Language"), __("New Language..."), __("Create a new Language."), language_png_img, language_sm_png_img)
{
	metaType = PGM_LANGUAGE;
}

pgCollection *pgLanguageFactory::CreateCollection(pgObject *obj)
{
	return new pgLanguageCollection(GetCollectionFactory(), (pgDatabase *)obj);
}

pgLanguageFactory languageFactory;
static pgaCollectionFactory cf(&languageFactory, __("Languages"), languages_png_img);
