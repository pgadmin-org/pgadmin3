//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
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


wxString pgAggregate::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Aggregate: ") + GetQuotedFullIdentifier() + wxT("\n")
            + wxT("CREATE AGGREGATE ") + GetQuotedFullIdentifier() 
            + wxT("(\n  BASETYPE=") + GetInputType()
            + wxT(",\n  SFUNC=") + GetStateFunction()
            + wxT(",\n  STYPE=") + GetStateType();
        AppendIfFilled(sql, wxT(",\n  FFUNC="), qtIdent(GetFinalFunction()));
        if (GetInitialCondition().length() > 0)
          sql += wxT(",\n  INITCOND=") + GetInitialCondition() + wxT("'");
        sql += wxT("\n);\n");
    }

    return sql;
}


wxString pgAggregate::GetFullName() const
{
    return GetName() + wxT("(") + GetInputType() + wxT(")");
}



void pgAggregate::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    SetButtons(form);

    if (properties)
    {
        CreateListColumns(properties);
        int pos=0;

        InsertListItem(properties, pos++, wxT("Name"), GetName());
        InsertListItem(properties, pos++, wxT("Input Type"), GetInputType());
        InsertListItem(properties, pos++, wxT("OID"), GetOid());
        InsertListItem(properties, pos++, wxT("Owner"), GetOwner());
        InsertListItem(properties, pos++, wxT("State Type"), GetStateType());
        InsertListItem(properties, pos++, wxT("State Function"), GetStateFunction());
        InsertListItem(properties, pos++, wxT("Final Type"), GetFinalType());
        InsertListItem(properties, pos++, wxT("Final Function"), GetFinalFunction());
        InsertListItem(properties, pos++, wxT("Initial Condition"), GetInitialCondition());
        InsertListItem(properties, pos++, wxT("System Aggregate?"), GetSystemObject());
        InsertListItem(properties, pos++, wxT("Comment"), GetComment());
    }
}



void pgAggregate::ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    wxString msg;
    pgAggregate *aggregate;

    if (browser->GetChildrenCount(collection->GetId(), FALSE) == 0)
    {
        // Log
        msg.Printf(wxT("Adding Aggregates to schema %s"), collection->GetSchema()->GetIdentifier().c_str());
        wxLogInfo(msg);

        // Get the Aggregates
        pgSet *aggregates= collection->GetDatabase()->ExecuteSet(wxT(
            "SELECT pr.oid, proname AS aggname, pg_get_userbyid(proowner) AS aggowner, aggtransfn,\n"
                    "aggfinalfn, "
                    "proargtypes[0] AS aggbasetype, "
                    "CASE WHEN (ti.typlen = -1 AND ti.typelem != 0) THEN (SELECT at.typname FROM pg_type at WHERE at.oid = ti.typelem) || '[]' ELSE ti.typname END as inputname, "
                    "aggtranstype, "
                    "CASE WHEN (tt.typlen = -1 AND tt.typelem != 0) THEN (SELECT at.typname FROM pg_type at WHERE at.oid = tt.typelem) || '[]' ELSE tt.typname END as transname, "
                    "prorettype AS aggfinaltype, "
                    "CASE WHEN (tf.typlen = -1 AND tf.typelem != 0) THEN (SELECT at.typname FROM pg_type at WHERE at.oid = tf.typelem) || '[]' ELSE tf.typname END as finalname, "
                    "agginitval\n"
            "  FROM pg_aggregate ag\n"
            "  JOIN pg_proc pr ON pr.oid = ag.aggfnoid\n"
            "  JOIN pg_type ti on ti.oid=proargtypes[0]\n"
            "  JOIN pg_type tt on tt.oid=aggtranstype\n"
            "  JOIN pg_type tf on tf.oid=prorettype\n"
            " WHERE pronamespace = ") + NumToStr(collection->GetSchema()->GetOid()) + wxT("::oid\n"
            " ORDER BY aggname"));

        if (aggregates)
        {
            while (!aggregates->Eof())
            {
                aggregate = new pgAggregate(collection->GetSchema(), aggregates->GetVal(wxT("aggname")));

                aggregate->iSetOid(aggregates->GetOid(wxT("oid")));
                aggregate->iSetOwner(aggregates->GetVal(wxT("aggowner")));
                aggregate->iSetInputType(aggregates->GetVal(wxT("inputname")));
                aggregate->iSetStateType(aggregates->GetVal(wxT("transname")));
                aggregate->iSetStateFunction(aggregates->GetVal(wxT("aggtransfn")));
                aggregate->iSetFinalType(aggregates->GetVal(wxT("finalname")));
                aggregate->iSetFinalFunction(aggregates->GetVal(wxT("aggfinalfn")));
                aggregate->iSetInitialCondition(aggregates->GetVal(wxT("agginitval")));


                browser->AppendItem(collection->GetId(), aggregate->GetFullName(), PGICON_AGGREGATE, -1, aggregate);
	    
			    aggregates->MoveNext();
            }

		    delete aggregates;
        }
    }
}
