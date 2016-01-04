//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// edbSynonym.cpp - EnterpriseDB Synonym class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/edbSynonym.h"

edbSynonym::edbSynonym(const wxString &newName)
	: pgDatabaseObject(synonymFactory, newName)
{
}

wxString edbSynonym::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on synonym");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing synonym");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop synonym \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop synonym \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop synonym cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop synonym?");
			break;
		case PROPERTIESREPORT:
			message = _("Synonym properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Synonym properties");
			break;
		case DDLREPORT:
			message = _("Synonym DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Synonym DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("Synonym dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Synonym dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Synonym dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Synonym dependents");
			break;
	}

	return message;
}

bool edbSynonym::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("DROP PUBLIC SYNONYM ") + GetQuotedIdentifier();

	return GetDatabase()->ExecuteVoid(sql);
}

wxString edbSynonym::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Public synonym: ") + GetQuotedIdentifier() + wxT("\n\n")
		      wxT("-- DROP PUBLIC SYNONYM ") + GetQuotedIdentifier();

		sql += wxT("\n\nCREATE OR REPLACE PUBLIC SYNONYM ") + GetQuotedIdentifier() + wxT(" FOR ");

		if (GetTargetSchema() != wxEmptyString)
			sql += qtIdent(GetTargetSchema()) + wxT(".");

		sql += qtIdent(GetTargetObject()) + wxT(";\n");
	}

	return sql;
}

void edbSynonym::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("Owner"), GetOwner());
		properties->AppendItem(_("Target type"), GetTargetType());
		properties->AppendItem(_("Target schema"), GetTargetSchema());
		properties->AppendItem(_("Target object"), GetTargetObject());
		properties->AppendYesNoItem(_("System synonym?"), GetSystemObject());
	}
}



pgObject *edbSynonym::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *synonym = 0;

	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		synonym = synonymFactory.CreateObjects(coll, 0, wxT(" WHERE synname=") + qtDbString(GetName()));

	return synonym;
}



pgObject *edbSynonymFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	edbSynonym *synonym = 0;

	wxString sql = wxT("SELECT *, pg_get_userbyid(synowner) AS owner,\n")
	               wxT("  COALESCE((SELECT relkind \n")
	               wxT("  FROM pg_class c, pg_namespace n\n")
	               wxT("  WHERE c.relnamespace = n.oid\n")
	               wxT("    AND n.nspname = synobjschema\n")
	               wxT("    AND c.relname = synobjname), '') AS targettype\n")
	               wxT("  FROM pg_synonym")
	               + restriction +
	               wxT("  ORDER BY synname;");

	pgSet *synonyms = collection->GetDatabase()->ExecuteSet(sql);

	if (synonyms)
	{
		while (!synonyms->Eof())
		{
			wxString name = synonyms->GetVal(wxT("synname"));
			synonym = new edbSynonym(name);

			synonym->iSetDatabase(collection->GetDatabase());
			synonym->iSetOwner(synonyms->GetVal(wxT("owner")));

			if (synonyms->GetVal(wxT("targettype")) == wxT("r"))
				synonym->iSetTargetType(_("Table"));
			else if (synonyms->GetVal(wxT("targettype")) == wxT("S"))
				synonym->iSetTargetType(_("Sequence"));
			else if (synonyms->GetVal(wxT("targettype")) == wxT("v"))
				synonym->iSetTargetType(_("View"));
			else
				synonym->iSetTargetType(_("Public synonym"));

			synonym->iSetTargetSchema(synonyms->GetVal(wxT("synobjschema")));
			synonym->iSetTargetObject(synonyms->GetVal(wxT("synobjname")));

			if (browser)
			{
				browser->AppendObject(collection, synonym);
				synonyms->MoveNext();
			}
			else
				break;
		}
		delete synonyms;
	}
	return synonym;
}

/////////////////////////////

edbSynonymCollection::edbSynonymCollection(pgaFactory *factory, pgDatabase *db)
	: pgDatabaseObjCollection(factory, db)
{
}

wxString edbSynonymCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on synonyms");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing synonyms");
			break;
		case GRANTWIZARDTITLE:
			message = _("Privileges for synonyms");
			break;
		case OBJECTSLISTREPORT:
			message = _("Synonyms list report");
			break;
	}

	return message;
}

/////////////////////////////

#include "images/synonym.pngc"
#include "images/synonyms.pngc"

edbSynonymFactory::edbSynonymFactory()
	: pgDatabaseObjFactory(__("Public Synonym"), __("New Public Synonym..."), __("Create a new Public Synonym."), synonym_png_img)
{
}


pgCollection *edbSynonymFactory::CreateCollection(pgObject *obj)
{
	return new edbSynonymCollection(GetCollectionFactory(), (pgDatabase *)obj);
}

edbSynonymFactory synonymFactory;
static pgaCollectionFactory cf(&synonymFactory, __("Public Synonyms"), synonyms_png_img);
