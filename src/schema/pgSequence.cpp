//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgSequence.cpp - Sequence class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "pgSequence.h"
#include "pgCollection.h"


pgSequence::pgSequence(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, PG_SEQUENCE, newName)
{
}

pgSequence::~pgSequence()
{
}


// we dont have an appropriate wxLongLong method
#define atolonglong _atoi64

void pgSequence::UpdateValues()
{
    pgSet *sequence=ExecuteSet(wxT(
        "SELECT last_value, min_value, max_value, cache_value, is_cycled, increment_by\n"
        "  FROM ") + qtIdent(GetName()));
    if (sequence)
    {
        lastValue = atolonglong(sequence->GetVal(wxT("last_value")));
        minValue = atolonglong(sequence->GetVal(wxT("min_value")));
        maxValue = atolonglong(sequence->GetVal(wxT("max_value")));
        cacheValue = atolonglong(sequence->GetVal(wxT("cache_value")));
        increment = atolonglong(sequence->GetVal(wxT("increment_by")));
        cycled = StrToBool(sequence->GetVal(wxT("is_cycled")));

        delete sequence;
    }
}


wxString pgSequence::GetSql(wxTreeCtrl *browser)
{
//    if (sql.IsNull())
    {
        UpdateValues();
        sql = wxT("CREATE SEQUENCE ") + GetQuotedFullIdentifier()
            + wxT(" INCREMENT ") + GetIncrement().ToString()
            + wxT(" MINVALUE ") + GetMinValue().ToString()
            + wxT(" MAXVALUE ") + GetMaxValue().ToString()
            + wxT(" START ") + GetLastValue().ToString()
            + wxT(" CACHE ") + GetCacheValue().ToString();
        if (GetCycled())
            sql += wxT(" CYCLE");
        sql += wxT(";\n")
            + GetGrant()
            + GetCommentSql();
    }

    return sql;
}

void pgSequence::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    SetButtons(form);

    UpdateValues();
    properties->ClearAll();
    properties->InsertColumn(0, wxT("Property"), wxLIST_FORMAT_LEFT, 150);
    properties->InsertColumn(1, wxT("Value"), wxLIST_FORMAT_LEFT, 200);

    int pos=0;

    InsertListItem(properties, pos++, wxT("Name"), GetName());
    InsertListItem(properties, pos++, wxT("OID"), NumToStr(GetOid()));
    InsertListItem(properties, pos++, wxT("Owner"), GetOwner());
    InsertListItem(properties, pos++, wxT("ACL"), GetAcl());
    InsertListItem(properties, pos++, wxT("Last Value"), GetLastValue().ToString());
    InsertListItem(properties, pos++, wxT("Minimum"), GetMinValue().ToString());
    InsertListItem(properties, pos++, wxT("Maximum"), GetMaxValue().ToString());
    InsertListItem(properties, pos++, wxT("Increment"), GetIncrement().ToString());
    InsertListItem(properties, pos++, wxT("Cached"), GetCacheValue().ToString());
    InsertListItem(properties, pos++, wxT("Cycled"), BoolToYesNo(GetCycled()));
    InsertListItem(properties, pos++, wxT("Comment"), GetComment());


    wxLogInfo(wxT("Displaying statistics for sequence on ") +GetSchema()->GetIdentifier());

    // Add the statistics view columns
    statistics->ClearAll();
    statistics->InsertColumn(0, wxT("Statistic"), wxLIST_FORMAT_LEFT, 100);
    statistics->InsertColumn(1, wxT("Value"), wxLIST_FORMAT_LEFT, 200);

    pgSet *stats = GetSchema()->GetDatabase()->ExecuteSet(wxT(
        "SELECT blks_read, blks_hit FROM pg_statio_all_sequences WHERE relid = ") + GetOidStr());

    if (stats)
    {
        statistics->InsertItem(0, wxT("Blocks Read"), 0);
        statistics->SetItem(0l, 1, stats->GetVal(wxT("blks_read")));
        statistics->InsertItem(1, wxT("Blocks Hit"), 0);
        statistics->SetItem(1, 1, stats->GetVal(wxT("blks_hit")));

        delete stats;
    }
}



void pgSequence::ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    wxString msg;
    pgSequence *sequence;

    if (browser->GetChildrenCount(collection->GetId(), FALSE) == 0)
    {
        // Log
        msg.Printf(wxT("Adding Sequences to schema %s"), collection->GetSchema()->GetIdentifier().c_str());
        wxLogInfo(msg);

        // Get the Sequences
        pgSet *sequences= collection->GetDatabase()->ExecuteSet(wxT(
            "SELECT oid, relname, pg_get_userbyid(relowner) AS seqowner, relacl\n"
            "  FROM pg_class WHERE relkind = 'S' AND relnamespace  = ") + collection->GetSchema()->GetOidStr() + wxT("\n"
            " ORDER BY relname"));

        if (sequences)
        {
            while (!sequences->Eof())
            {
                sequence = new pgSequence(collection->GetSchema(), sequences->GetVal(wxT("relname")));

                sequence->iSetOid(StrToDouble(sequences->GetVal(wxT("oid"))));
                sequence->iSetOwner(sequences->GetVal(wxT("seqowner")));
                sequence->iSetAcl(sequences->GetVal(wxT("relacl")));

                browser->AppendItem(collection->GetId(), sequence->GetIdentifier(), PGICON_SEQUENCE, -1, sequence);
	    
			    sequences->MoveNext();
            }

		    delete sequences;
        }
    }
}

