//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgAggregate.cpp - Aggregate class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "pgAggregate.h"
#include "pgCollection.h"


pgAggregate::pgAggregate(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, PG_AGGREGATE, newName)
{
}

pgAggregate::~pgAggregate()
{
}

bool pgAggregate::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(wxT("DROP AGGREGATE ") + GetQuotedFullIdentifier() + wxT("(") + GetInputType() + wxT(")"));
}

wxString pgAggregate::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Aggregate: ") + GetQuotedFullIdentifier() + wxT("\n\n")
            + wxT("-- DROP AGGREGATE ") + GetQuotedFullIdentifier() + wxT("(") + GetInputType() + wxT(")")
            + wxT("\n\nCREATE AGGREGATE ") + GetQuotedFullIdentifier() 
            + wxT("(\n  BASETYPE=") + GetInputType()
            + wxT(",\n  SFUNC=") + GetStateFunction()
            + wxT(",\n  STYPE=") + GetStateType();
        AppendIfFilled(sql, wxT(",\n  FFUNC="), qtIdent(GetFinalFunction()));
        if (GetInitialCondition().length() > 0)
          sql += wxT(",\n  INITCOND=") + qtString(GetInitialCondition());
        sql += wxT("\n);\n")
            + GetCommentSql();
    }

    return sql;
}


wxString pgAggregate::GetFullName() const
{
    return GetName() + wxT("(") + GetInputType() + wxT(")");
}



void pgAggregate::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (properties)
    {
        CreateListColumns(properties);
        int pos=0;

        InsertListItem(properties, pos++, _("Name"), GetName());
        InsertListItem(properties, pos++, _("Input Type"), GetInputType());
        InsertListItem(properties, pos++, _("OID"), GetOid());
        InsertListItem(properties, pos++, _("Owner"), GetOwner());
        InsertListItem(properties, pos++, _("State Type"), GetStateType());
        InsertListItem(properties, pos++, _("State Function"), GetStateFunction());
        InsertListItem(properties, pos++, _("Final Type"), GetFinalType());
        InsertListItem(properties, pos++, _("Final Function"), GetFinalFunction());
        InsertListItem(properties, pos++, _("Initial Condition"), GetInitialCondition());
        InsertListItem(properties, pos++, _("System Aggregate?"), GetSystemObject());
        InsertListItem(properties, pos++, _("Comment"), GetComment());
    }
}


pgObject *pgAggregate::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *aggregate=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgObject *obj=(pgObject*)browser->GetItemData(parentItem);
        if (obj->GetType() == PG_AGGREGATES)
            aggregate = ReadObjects((pgCollection*)obj, 0, wxT("\n   AND aggfnoid::oid=") + GetOidStr());
    }
    return aggregate;
}



pgObject *pgAggregate::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgAggregate *aggregate=0;
    pgSet *aggregates= collection->GetDatabase()->ExecuteSet(
        wxT("SELECT aggfnoid::oid, proname AS aggname, pg_get_userbyid(proowner) AS aggowner, aggtransfn,\n")
        wxT(        "aggfinalfn, proargtypes[0] AS aggbasetype, ")
        wxT(        "CASE WHEN (ti.typlen = -1 AND ti.typelem != 0) THEN (SELECT at.typname FROM pg_type at WHERE at.oid = ti.typelem) || '[]' ELSE ti.typname END as inputname, ")
        wxT(        "aggtranstype, ")
        wxT(        "CASE WHEN (tt.typlen = -1 AND tt.typelem != 0) THEN (SELECT at.typname FROM pg_type at WHERE at.oid = tt.typelem) || '[]' ELSE tt.typname END as transname, ")
        wxT(        "prorettype AS aggfinaltype, ")
        wxT(        "CASE WHEN (tf.typlen = -1 AND tf.typelem != 0) THEN (SELECT at.typname FROM pg_type at WHERE at.oid = tf.typelem) || '[]' ELSE tf.typname END as finalname, ")
        wxT(        "agginitval, description\n")
        wxT("  FROM pg_aggregate ag\n")
        wxT("  JOIN pg_proc pr ON pr.oid = ag.aggfnoid\n")
        wxT("  JOIN pg_type ti on ti.oid=proargtypes[0]\n")
        wxT("  JOIN pg_type tt on tt.oid=aggtranstype\n")
        wxT("  JOIN pg_type tf on tf.oid=prorettype\n")
        wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=aggfnoid::oid\n")
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
            aggregate->iSetInputType(aggregates->GetVal(wxT("inputname")));
            aggregate->iSetStateType(aggregates->GetVal(wxT("transname")));
            aggregate->iSetStateFunction(aggregates->GetVal(wxT("aggtransfn")));
            aggregate->iSetFinalType(aggregates->GetVal(wxT("finalname")));
            aggregate->iSetFinalFunction(aggregates->GetVal(wxT("aggfinalfn")));
            aggregate->iSetInitialCondition(aggregates->GetVal(wxT("agginitval")));
            aggregate->iSetComment(aggregates->GetVal(wxT("description")));


            if (browser)
            {
                collection->AppendBrowserItem(browser, aggregate);
    			aggregates->MoveNext();
            }
            else
                break;
        }

		delete aggregates;
    }
    return aggregate;
}
