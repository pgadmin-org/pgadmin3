//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
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


pgAggregate::pgAggregate(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, aggregateFactory, newName)
{
}

pgAggregate::~pgAggregate()
{
}

bool pgAggregate::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
    wxString sql=wxT("DROP AGGREGATE ") + this->GetSchema()->GetQuotedIdentifier() + wxT(".") + this->GetQuotedIdentifier() + wxT("(") + GetInputTypesList() + wxT(")");
    if (cascaded)
        sql += wxT(" CASCADE");
    return GetDatabase()->ExecuteVoid(sql);
}

wxString pgAggregate::GetSql(ctlTree *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Aggregate: ") + GetQuotedFullIdentifier() + wxT("\n\n")
            + wxT("-- DROP AGGREGATE ") + GetQuotedFullIdentifier() + wxT("(") + GetInputTypesList() + wxT(");");

        if (GetDatabase()->BackendMinimumVersion(8, 2))
        {
            sql += wxT("\n\nCREATE AGGREGATE ") + GetQuotedFullIdentifier() + wxT("(") + GetInputTypesList() + wxT(") (");
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
          sql += wxT(",\n  INITCOND=") + qtDbString(GetInitialCondition());
        AppendIfFilled(sql, wxT(",\n  SORTOP="), GetQuotedSortOp());

        sql += wxT("\n);\n")
            + GetOwnerSql(8, 0, wxT("AGGREGATE ") + GetQuotedFullIdentifier() 
                + wxT("(") + GetInputTypesList()
                + wxT(")"));

        if (!GetComment().IsNull())
        {
            sql += wxT("COMMENT ON AGGREGATE ") + GetQuotedFullIdentifier() 
                + wxT("(") + GetInputTypesList()
                + wxT(") IS ") + qtDbString(GetComment()) + wxT(";\n");
        }
    }

    return sql;
}


wxString pgAggregate::GetFullName()
{
    return GetName() + wxT("(") + GetInputTypesList() + wxT(")");
}

// Return the list of input types
wxString pgAggregate::GetInputTypesList()
{
    wxString types;

    for (unsigned int i=0; i < inputTypes.Count(); i++)
    {
        if (i > 0)
            types += wxT(", ");

        types += inputTypes.Item(i);
    }
    return types;
}

void pgAggregate::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (properties)
    {
        CreateListColumns(properties);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("Input types"), GetInputTypesList());
        properties->AppendItem(_("OID"), GetOid());
        properties->AppendItem(_("Owner"), GetOwner());
        properties->AppendItem(_("State type"), GetStateType());
        properties->AppendItem(_("State function"), GetStateFunction());
        properties->AppendItem(_("Final type"), GetFinalType());
        properties->AppendItem(_("Final function"), GetFinalFunction());
        if (GetConnection()->BackendMinimumVersion(8,1))
            properties->AppendItem(_("Sort operator"), GetSortOp());
        properties->AppendItem(_("Initial condition"), GetInitialCondition());
        properties->AppendItem(_("System aggregate?"), GetSystemObject());
        properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
    }
}


pgObject *pgAggregate::Refresh(ctlTree *browser, const wxTreeItemId item)
{
    pgObject *aggregate=0;

    pgCollection *coll=browser->GetParentCollection(item);
    if (coll)
        aggregate = aggregateFactory.CreateObjects(coll, 0, wxT("\n   AND aggfnoid::oid=") + GetOidStr());

    return aggregate;
}


////////////////////////////////////////////////////////////////////////


pgObject *pgAggregateFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
    pgAggregate *aggregate=0;
    wxString sql=
        wxT("SELECT aggfnoid::oid, proname AS aggname, pg_get_userbyid(proowner) AS aggowner, aggtransfn,\n")
        wxT(        "aggfinalfn, proargtypes, aggtranstype, ")
        wxT(        "CASE WHEN (tt.typlen = -1 AND tt.typelem != 0) THEN (SELECT at.typname FROM pg_type at WHERE at.oid = tt.typelem) || '[]' ELSE tt.typname END as transname, ")
        wxT(        "prorettype AS aggfinaltype, ")
        wxT(        "CASE WHEN (tf.typlen = -1 AND tf.typelem != 0) THEN (SELECT at.typname FROM pg_type at WHERE at.oid = tf.typelem) || '[]' ELSE tf.typname END as finalname, ")
        wxT(        "agginitval, description");

    if (collection->GetDatabase()->BackendMinimumVersion(8, 1))
    {
        sql += wxT(", oprname, opn.nspname as oprnsp\n")
               wxT("  FROM pg_aggregate ag\n")
               wxT("  LEFT OUTER JOIN pg_operator op ON op.oid=aggsortop\n")
               wxT("  LEFT OUTER JOIN pg_namespace opn ON opn.oid=op.oprnamespace");
    }
    else
        sql +=  wxT("\n  FROM pg_aggregate ag\n");


    pgSet *aggregates= collection->GetDatabase()->ExecuteSet(sql +
        wxT("  JOIN pg_proc pr ON pr.oid = ag.aggfnoid\n")
        wxT("  JOIN pg_type tt on tt.oid=aggtranstype\n")
        wxT("  JOIN pg_type tf on tf.oid=prorettype\n")
        wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=aggfnoid::oid\n")
        wxT(" WHERE pronamespace = ") + collection->GetSchema()->GetOidStr()
        + restriction
        + wxT("\n ORDER BY aggname"));

    // Build a cache of data types
    pgSet *types = collection->GetDatabase()->ExecuteSet(wxT(
                    "SELECT oid, format_type(oid, typtypmod) AS typname FROM pg_type"));

    typeMap map;

    while(!types->Eof())
    {
        map[types->GetVal(wxT("oid"))] = types->GetVal(wxT("typname"));
        types->MoveNext();
    }

    if (aggregates)
    {
        while (!aggregates->Eof())
        {
            aggregate = new pgAggregate(collection->GetSchema(), aggregates->GetVal(wxT("aggname")));

            aggregate->iSetOid(aggregates->GetOid(wxT("aggfnoid")));
            aggregate->iSetOwner(aggregates->GetVal(wxT("aggowner")));

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

            wxString final=aggregates->GetVal(wxT("aggfinalfn"));
            if (final != wxT("-"))
                aggregate->iSetFinalFunction(final);
            aggregate->iSetInitialCondition(aggregates->GetVal(wxT("agginitval")));
            aggregate->iSetComment(aggregates->GetVal(wxT("description")));
            if (collection->GetDatabase()->BackendMinimumVersion(8, 1))
            {
                wxString oprname=aggregates->GetVal(wxT("oprname"));
                if (!oprname.IsEmpty())
                {
                    wxString oprnsp=aggregates->GetVal(wxT("oprnsp"));
                    aggregate->iSetSortOp(collection->GetDatabase()->GetSchemaPrefix(oprnsp) + oprname);
                    aggregate->iSetQuotedSortOp(collection->GetDatabase()->GetQuotedSchemaPrefix(oprnsp)
                        + qtIdent(oprname));
                }
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


#include "images/aggregate.xpm"
#include "images/aggregate-sm.xpm"
#include "images/aggregates.xpm"

pgAggregateFactory::pgAggregateFactory() 
: pgaFactory(__("Aggregate"), __("New Aggregate..."), __("Create a new Aggregate."), aggregate_xpm, aggregate_sm_xpm)
{
}

pgCollection *pgAggregateFactory::CreateCollection(pgObject *obj)
{
    return new pgSchemaObjCollection(GetCollectionFactory(), (pgSchema*)obj);
}

pgAggregateFactory aggregateFactory;
static pgaCollectionFactory cf(&aggregateFactory, __("Aggregates"), aggregates_xpm);
