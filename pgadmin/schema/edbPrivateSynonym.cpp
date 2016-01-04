//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// edbPrivateSynonym.cpp - EnterpriseDB Private/Public Synonym class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/edbPrivateSynonym.h"

edbPrivateSynonym::edbPrivateSynonym(pgSchema *newSchema, const wxString &newName)
	: pgSchemaObject(newSchema, edbPrivFactory, newName)
{
	isPublic = false;
	if (newSchema && newSchema->GetName() == wxT("public"))
		isPublic = true;
}

bool edbPrivateSynonym::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql;
	if (isPublic)
		sql = wxT("DROP PUBLIC SYNONYM ") + GetQuotedIdentifier();
	else
		sql = wxT("DROP SYNONYM ") + this->GetSchema()->GetQuotedIdentifier() + wxT(".") + GetQuotedIdentifier();

	return GetDatabase()->ExecuteVoid(sql);
}

wxString edbPrivateSynonym::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop synonym \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPTITLE:
			message = _("Drop synonym?");
			break;
	}

	return message;
}

wxString edbPrivateSynonym::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		if (isPublic)
			sql = wxT("-- Public synonym: ") + GetQuotedIdentifier() + wxT("\n\n") +
			      wxT("-- DROP PUBLIC SYNONYM ") + GetQuotedIdentifier();
		else
			sql = wxT("-- Private synonym: ") + GetSchema()->GetQuotedIdentifier() + wxT(".") + GetQuotedIdentifier() + wxT("\n\n") +
			      wxT("-- DROP SYNONYM ") + GetSchema()->GetQuotedIdentifier() + wxT(".") + GetQuotedIdentifier();

		if (isPublic)
			sql += wxT("\n\nCREATE OR REPLACE PUBLIC SYNONYM ") + GetQuotedIdentifier() + wxT(" FOR ");
		else
			sql += wxT("\n\nCREATE OR REPLACE SYNONYM ") + GetSchema()->GetQuotedIdentifier() + wxT(".") + GetQuotedIdentifier() + wxT(" FOR ");

		if (GetTargetSchema() != wxEmptyString)
			sql += qtIdent(GetTargetSchema()) + wxT(".");

		sql += qtIdent(GetTargetObject()) + wxT(";\n");
	}

	return sql;
}

void edbPrivateSynonym::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Public Synonym"), isPublic ? _("Yes") : _("No"));
		properties->AppendItem(_("Name"), GetName());
		if (!isPublic)
			properties->AppendItem(_("Schema"), GetSchema()->GetName());
		properties->AppendItem(_("Owner"), GetOwner());
		properties->AppendItem(_("Target type"), GetTargetType());
		properties->AppendItem(_("Target schema"), GetTargetSchema());
		properties->AppendItem(_("Target object"), GetTargetObject());
		properties->AppendYesNoItem(_("System synonym?"), GetSystemObject());
	}
}



pgObject *edbPrivateSynonym::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *synonym = 0;

	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		synonym = edbPrivFactory.CreateObjects(coll, 0, wxT(" WHERE s.synname=") + qtDbString(GetName()) +
		                                       wxT(" AND s.synnamespace=") + coll->GetSchema()->GetOidStr() + wxT(" \n"));

	return synonym;
}



pgObject *edbPrivateSynonymFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	edbPrivateSynonym *synonym = 0;

	wxString sql = wxT("SELECT *, pg_get_userbyid(synowner) AS owner,\n")
	               wxT("  COALESCE((SELECT relkind \n")
	               wxT("  FROM pg_class c, pg_namespace n\n")
	               wxT("  WHERE c.relnamespace = n.oid\n")
	               wxT("    AND n.nspname = synobjschema\n")
	               wxT("    AND c.relname = synobjname),\n")
	               wxT("  (SELECT CASE WHEN p.protype = '0' THEN 'f'::\"char\" ELSE 'p'::\"char\" END \n")
	               wxT("  FROM pg_proc p, pg_namespace n\n")
	               wxT("    WHERE p.pronamespace = n.oid\n")
	               wxT("      AND n.nspname = synobjschema\n")
	               wxT("      AND p.proname = synobjname LIMIT 1), '') AS targettype\n")
	               wxT("  FROM pg_synonym s")
	               wxT("  JOIN pg_namespace ns ON s.synnamespace = ns.oid AND ns.nspname = ")
	               + qtConnString(collection->GetSchema()->GetName()) + wxT(" \n")
	               + restriction +
	               wxT("  ORDER BY synname;");

	pgSet *synonyms = collection->GetDatabase()->ExecuteSet(sql);

	if (synonyms)
	{
		while (!synonyms->Eof())
		{
			wxString name = synonyms->GetVal(wxT("synname"));
			synonym = new edbPrivateSynonym(collection->GetSchema(), name);

			synonym->iSetDatabase(collection->GetDatabase());
			synonym->iSetOwner(synonyms->GetVal(wxT("owner")));

			if (synonyms->GetVal(wxT("targettype")) == wxT("r"))
				synonym->iSetTargetType(_("Table"));
			else if (synonyms->GetVal(wxT("targettype")) == wxT("S"))
				synonym->iSetTargetType(_("Sequence"));
			else if (synonyms->GetVal(wxT("targettype")) == wxT("v"))
				synonym->iSetTargetType(_("View"));
			else if (synonyms->GetVal(wxT("targettype")) == wxT("f"))
				synonym->iSetTargetType(_("Function"));
			else if (synonyms->GetVal(wxT("targettype")) == wxT("p"))
				synonym->iSetTargetType(_("Procedure"));
			else
				synonym->iSetTargetType(_("Synonym"));

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

edbPrivateSynonymCollection::edbPrivateSynonymCollection(pgaFactory *factory, pgSchema *sch)
	: pgSchemaObjCollection(factory, sch)
{
}

wxString edbPrivateSynonymCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on private synonyms");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing private synonyms");
			break;
		case GRANTWIZARDTITLE:
			message = _("Privileges for private synonyms");
			break;
		case OBJECTSLISTREPORT:
			message = _("Private synonyms list report");
			break;
	}

	return message;
}

/////////////////////////////

#include "images/synonym.pngc"
#include "images/synonyms.pngc"

edbPrivateSynonymFactory::edbPrivateSynonymFactory()
	: pgSchemaObjFactory(__("Synonym"), __("New Synonym..."), __("Create a new Synonym."), synonym_png_img)
{
	metaType = EDB_SYNONYM;
}

pgCollection *edbPrivateSynonymFactory::CreateCollection(pgObject *obj)
{
	return new edbPrivateSynonymCollection(GetCollectionFactory(), (pgSchema *)obj);
}

edbPrivateSynonymFactory edbPrivFactory;

static pgaCollectionFactory cf(&edbPrivFactory, __("Synonyms"), synonyms_png_img);

