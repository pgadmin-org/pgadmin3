//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgAggregate.cpp - Aggregate class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgAggregate.h"


pgAggregate::pgAggregate(pgSchema *newSchema, const wxString &newName)
	: pgSchemaObject(newSchema, aggregateFactory, newName)
{
}

wxString pgAggregate::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on aggregate");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing aggregate");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop aggregate \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop aggregate \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop aggregate cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop aggregate?");
			break;
		case PROPERTIESREPORT:
			message = _("Aggregate properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Aggregate properties");
			break;
		case DDLREPORT:
			message = _("Aggregate DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Aggregate DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("Aggregate dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Aggregate dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Aggregate dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Aggregate dependents");
			break;
	}

	return message;
}

bool pgAggregate::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("DROP AGGREGATE ") + GetSchema()->GetQuotedIdentifier() + wxT(".") + GetFullName();
	if (cascaded)
		sql += wxT(" CASCADE");
	return GetDatabase()->ExecuteVoid(sql);
}

wxString pgAggregate::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Aggregate: ") + GetQuotedFullName() + wxT("\n\n")
		      + wxT("-- DROP AGGREGATE ") + GetQuotedFullName() + wxT(";");

		if (GetDatabase()->BackendMinimumVersion(8, 2))
		{
			sql += wxT("\n\nCREATE AGGREGATE ") + GetQuotedFullName() + wxT(" (");
		}
		else
		{
			sql += wxT("\n\nCREATE AGGREGATE ") + GetQuotedFullIdentifier()
			       + wxT("(\n  BASETYPE=") + GetInputTypesList() + wxT(",");
		}

		sql += wxT("\n  SFUNC=") + GetStateFunction()
		       + wxT(",\n  STYPE=") + GetStateType();

		AppendIfFilled(sql, wxT(",\n  FINALFUNC="), qtIdent(GetFinalFunction()));

		if (GetInitialCondition().length() > 0)
		{
			if (GetInitialCondition() == wxT("''"))
				sql += wxT(",\n  INITCOND=''");
			else if (GetInitialCondition() == wxT("\\'\\'"))
				sql += wxT(",\n  INITCOND=''''''");
			else
				sql += wxT(",\n  INITCOND=") + qtDbString(GetInitialCondition());
		}

		AppendIfFilled(sql, wxT(",\n  SORTOP="), GetQuotedSortOp());

		sql += wxT("\n);\n");
		sql += GetOwnerSql(8, 0, wxT("AGGREGATE ") + GetQuotedFullName())
		       +  GetGrant(wxT("X"), wxT("FUNCTION ") + GetQuotedFullIdentifier());

		if (!GetComment().IsNull())
		{
			sql += wxT("COMMENT ON AGGREGATE ") + GetQuotedFullName()
			       + wxT(" IS ") + qtDbString(GetComment()) + wxT(";\n");
		}

		if (GetConnection()->BackendMinimumVersion(9, 1))
			sql += GetSeqLabelsSql();
	}

	return sql;
}

void pgAggregate::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetOid());
		properties->AppendItem(_("Owner"), GetOwner());
		properties->AppendItem(_("ACL"), GetAcl());
		properties->AppendItem(_("Input types"), GetInputTypesList());
		properties->AppendItem(_("State type"), GetStateType());
		properties->AppendItem(_("State function"), GetStateFunction());
		properties->AppendItem(_("Final type"), GetFinalType());
		properties->AppendItem(_("Final function"), GetFinalFunction());
		if (GetConnection()->BackendMinimumVersion(8, 1))
			properties->AppendItem(_("Sort operator"), GetSortOp());

		if (GetInitialCondition() == wxT(""))
			properties->AppendItem(_("Initial condition"), _("<null>"));
		else if (GetInitialCondition() == wxT("''"))
			properties->AppendItem(_("Initial condition"), _("<empty string>"));
		else if (GetInitialCondition() == wxT("\\'\\'"))
			properties->AppendItem(_("Initial condition"), _("''"));
		else
			properties->AppendItem(_("Initial condition"), GetInitialCondition());

		properties->AppendYesNoItem(_("System aggregate?"), GetSystemObject());
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

pgObject *pgAggregate::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *aggregate = 0;

	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		aggregate = aggregateFactory.CreateObjects(coll, 0, wxT("\n   AND aggfnoid::oid=") + GetOidStr());

	return aggregate;
}

wxString pgAggregate::GetQuotedFullName()
{
	return GetQuotedFullIdentifier() + wxT("(") + GetInputTypesList() + wxT(")");
}

wxString pgAggregate::GetFullName()
{
	return GetName() + wxT("(") + GetInputTypesList() + wxT(")");
}

// Return the list of input types
wxString pgAggregate::GetInputTypesList()
{
	wxString types;

	for (unsigned int i = 0; i < inputTypes.Count(); i++)
	{
		if (i > 0)
			types += wxT(", ");

		types += inputTypes.Item(i);
	}
	return types;
}

pgObject *pgAggregateFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	pgAggregate *aggregate = 0;

	// Build a cache of data types
	pgSet *types = collection->GetDatabase()->ExecuteSet(wxT(
	                   "SELECT oid, format_type(oid, typtypmod) AS typname FROM pg_type"));
	cacheMap map;
	while(!types->Eof())
	{
		map[types->GetVal(wxT("oid"))] = types->GetVal(wxT("typname"));
		types->MoveNext();
	}

	delete types;
	// Build the query to get all objects
	wxString sql =
	    wxT("SELECT aggfnoid::oid, proname AS aggname, pg_get_userbyid(proowner) AS aggowner, aggtransfn,\n")
	    wxT(        "aggfinalfn, proargtypes, aggtranstype, proacl, ")
	    wxT(        "CASE WHEN (tt.typlen = -1 AND tt.typelem != 0) THEN (SELECT at.typname FROM pg_type at WHERE at.oid = tt.typelem) || '[]' ELSE tt.typname END as transname, ")
	    wxT(        "prorettype AS aggfinaltype, ")
	    wxT(        "CASE WHEN (tf.typlen = -1 AND tf.typelem != 0) THEN (SELECT at.typname FROM pg_type at WHERE at.oid = tf.typelem) || '[]' ELSE tf.typname END as finalname, ")
	    wxT(        "agginitval, description");

	if (collection->GetDatabase()->BackendMinimumVersion(9, 1))
	{
		sql += wxT(",\n(SELECT array_agg(label) FROM pg_seclabels sl1 WHERE sl1.objoid=aggfnoid) AS labels");
		sql += wxT(",\n(SELECT array_agg(provider) FROM pg_seclabels sl2 WHERE sl2.objoid=aggfnoid) AS providers");
	}

	if (collection->GetDatabase()->BackendMinimumVersion(8, 1))
	{
		sql += wxT(", oprname, opn.nspname as oprnsp\n")
		       wxT("  FROM pg_aggregate ag\n")
		       wxT("  LEFT OUTER JOIN pg_operator op ON op.oid=aggsortop\n")
		       wxT("  LEFT OUTER JOIN pg_namespace opn ON opn.oid=op.oprnamespace");
	}
	else
		sql +=  wxT("\n  FROM pg_aggregate ag\n");

	pgSet *aggregates = collection->GetDatabase()->ExecuteSet(sql +
	                    wxT("  JOIN pg_proc pr ON pr.oid = ag.aggfnoid\n")
	                    wxT("  JOIN pg_type tt on tt.oid=aggtranstype\n")
	                    wxT("  JOIN pg_type tf on tf.oid=prorettype\n")
	                    wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=aggfnoid::oid AND des.classoid='pg_aggregate'::regclass)\n")
	                    wxT(" WHERE pronamespace = ") + collection->GetSchema()->GetOidStr()
	                    + restriction
	                    + wxT("\n ORDER BY aggname"));

	if (aggregates)
	{
		while (!aggregates->Eof())
		{
			aggregate = new pgAggregate(collection->GetSchema(), aggregates->GetVal(wxT("aggname")));

			aggregate->iSetOid(aggregates->GetOid(wxT("aggfnoid")));
			aggregate->iSetOwner(aggregates->GetVal(wxT("aggowner")));
			aggregate->iSetAcl(aggregates->GetVal(wxT("proacl")));

			// Get the input type names. From 8.2 onwards there might be
			// multiple types in the array. In any case, we must properly
			// quote "any"

			// Tokenize the arguments
			wxStringTokenizer argTypes(wxEmptyString);

			if (aggregates->GetVal(wxT("proargtypes")) == wxEmptyString)
			{
				if (collection->GetDatabase()->BackendMinimumVersion(8, 2))
					aggregate->iAddInputType(wxT("*"));
				else
					aggregate->iAddInputType(wxT("\"any\""));
			}
			else
			{
				argTypes.SetString(aggregates->GetVal(wxT("proargtypes")));

				while (argTypes.HasMoreTokens())
				{
					// Add the arg type. This is a type oid, so
					// look it up in the hashmap
					wxString type = argTypes.GetNextToken();
					if (map[type] == wxT("any"))
						aggregate->iAddInputType(wxT("\"any\""));
					else
						aggregate->iAddInputType(qtTypeIdent(map[type]));
				}
			}

			aggregate->iSetStateType(aggregates->GetVal(wxT("transname")));
			aggregate->iSetStateFunction(aggregates->GetVal(wxT("aggtransfn")));
			aggregate->iSetFinalType(aggregates->GetVal(wxT("finalname")));

			wxString final = aggregates->GetVal(wxT("aggfinalfn"));
			if (final != wxT("-"))
				aggregate->iSetFinalFunction(final);

			if (!aggregates->IsNull(aggregates->ColNumber(wxT("agginitval"))))
			{
				if (aggregates->GetVal(wxT("agginitval")).IsEmpty())
					aggregate->iSetInitialCondition(wxT("''"));
				else if (aggregates->GetVal(wxT("agginitval")) == wxT("''"))
					aggregate->iSetInitialCondition(wxT("\\'\\'"));
				else
					aggregate->iSetInitialCondition(aggregates->GetVal(wxT("agginitval")));
			}

			aggregate->iSetComment(aggregates->GetVal(wxT("description")));
			if (collection->GetDatabase()->BackendMinimumVersion(8, 1))
			{
				wxString oprname = aggregates->GetVal(wxT("oprname"));
				if (!oprname.IsEmpty())
				{
					wxString oprnsp = aggregates->GetVal(wxT("oprnsp"));
					aggregate->iSetSortOp(collection->GetDatabase()->GetSchemaPrefix(oprnsp) + oprname);
					aggregate->iSetQuotedSortOp(collection->GetDatabase()->GetQuotedSchemaPrefix(oprnsp)
					                            + qtIdent(oprname));
				}
			}

			if (collection->GetDatabase()->BackendMinimumVersion(9, 1))
			{
				aggregate->iSetProviders(aggregates->GetVal(wxT("providers")));
				aggregate->iSetLabels(aggregates->GetVal(wxT("labels")));
			}

			if (browser)
			{
				browser->AppendObject(collection, aggregate);
				aggregates->MoveNext();
			}
			else
				break;
		}

		delete aggregates;
	}
	return aggregate;
}

/////////////////////////////

pgAggregateCollection::pgAggregateCollection(pgaFactory *factory, pgSchema *sch)
	: pgSchemaObjCollection(factory, sch)
{
}

wxString pgAggregateCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on aggregates");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing aggregates");
			break;
		case OBJECTSLISTREPORT:
			message = _("Aggregates list report");
			break;
	}

	return message;
}

/////////////////////////////

#include "images/aggregate.pngc"
#include "images/aggregate-sm.pngc"
#include "images/aggregates.pngc"

pgAggregateFactory::pgAggregateFactory()
	: pgSchemaObjFactory(__("Aggregate"), __("New Aggregate..."), __("Create a new Aggregate."), aggregate_png_img, aggregate_sm_png_img)
{
}

pgCollection *pgAggregateFactory::CreateCollection(pgObject *obj)
{
	return new pgAggregateCollection(GetCollectionFactory(), (pgSchema *)obj);
}

pgAggregateFactory aggregateFactory;
static pgaCollectionFactory cf(&aggregateFactory, __("Aggregates"), aggregates_png_img);
