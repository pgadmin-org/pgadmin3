//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
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

	wxString sql = wxT("SELECT s.*, pg_get_userbyid(s.synowner) AS owner,\n")
	               wxT("  COALESCE((SELECT relkind \n")
	               wxT("  FROM pg_class c, pg_namespace n\n")
	               wxT("  WHERE c.relnamespace = n.oid\n")
	               wxT("    AND n.nspname = s.synobjschema\n")
	               wxT("    AND c.relname = s.synobjname), '') AS targettype\n")
	               wxT("  FROM pg_synonym s\n")
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

#include "images/synonym.xpm"
#include "images/synonyms.xpm"

edbPrivateSynonymFactory::edbPrivateSynonymFactory()
	: pgSchemaObjFactory(__("Synonym"), __("New Synonym..."), __("Create a new Synonym."), synonym_xpm)
{}

edbPrivateSynonymFactory edbPrivFactory;

static pgaCollectionFactory cf(&edbPrivFactory, __("Synonyms"), synonyms_xpm);

