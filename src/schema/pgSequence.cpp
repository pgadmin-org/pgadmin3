//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
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

bool pgSequence::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(wxT("DROP SEQUENCE ") + GetQuotedFullIdentifier() + wxT(";"));
}


void pgSequence::UpdateValues()
{
    pgSet *sequence=ExecuteSet(
        wxT("SELECT last_value, min_value, max_value, cache_value, is_cycled, increment_by\n")
        wxT("  FROM ") + GetQuotedFullIdentifier());
    if (sequence)
    {
        lastValue = sequence->GetLongLong(wxT("last_value"));
        minValue = sequence->GetLongLong(wxT("min_value"));
        maxValue = sequence->GetLongLong(wxT("max_value"));
        cacheValue = sequence->GetLongLong(wxT("cache_value"));
        increment = sequence->GetLongLong(wxT("increment_by"));
        cycled = sequence->GetBool(wxT("is_cycled"));

        delete sequence;
    }
}


wxString pgSequence::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        UpdateValues();
        sql = wxT("-- Sequence: ") + GetQuotedFullIdentifier() + wxT("\n\n")
            + wxT("-- DROP SEQUENCE ") + GetQuotedFullIdentifier() + wxT(";")
            + wxT("\n\nCREATE SEQUENCE ") + GetQuotedFullIdentifier()
            + wxT("\n  INCREMENT ") + GetIncrement().ToString()
            + wxT("\n  MINVALUE ") + GetMinValue().ToString()
            + wxT("\n  MAXVALUE ") + GetMaxValue().ToString()
            + wxT("\n  START ") + GetLastValue().ToString()
            + wxT("\n  CACHE ") + GetCacheValue().ToString();
        if (GetCycled())
            sql += wxT("\n  CYCLE");
        AppendIfFilled(sql, wxT("\n  TABLESPACE "), qtIdent(tablespace));
        sql += wxT(";\n")
            + GetGrant(wxT("arwdRxt"), wxT("TABLE"))
            + GetCommentSql();
    }

    return sql;
}

void pgSequence::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    UpdateValues();
    if (properties)
    {
        CreateListColumns(properties);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("OID"), GetOid());
        properties->AppendItem(_("Owner"), GetOwner());
        if (!tablespace.IsEmpty())
            properties->AppendItem(_("Tablespace"), tablespace);
        properties->AppendItem(_("ACL"), GetAcl());
        properties->AppendItem(_("Current value"), GetLastValue());
        properties->AppendItem(_("Minimum"), GetMinValue());
        properties->AppendItem(_("Maximum"), GetMaxValue());
        properties->AppendItem(_("Increment"), GetIncrement());
        properties->AppendItem(_("Cache"), GetCacheValue());
        properties->AppendItem(_("Cycled"), GetCycled());
        properties->AppendItem(_("System sequence?"), GetSystemObject());
        properties->AppendItem(_("Comment"), GetComment());
    }
}



void pgSequence::ShowStatistics(ctlListView *statistics)
{
    wxLogInfo(wxT("Displaying statistics for sequence on ") +GetSchema()->GetIdentifier());

    // Add the statistics view columns
    CreateListColumns(statistics, _("Statistic"), _("Value"));

    pgSet *stats = GetSchema()->GetDatabase()->ExecuteSet(wxT(
        "SELECT blks_read, blks_hit FROM pg_statio_all_sequences WHERE relid = ") + GetOidStr());

    if (stats)
    {
        statistics->InsertItem(0, wxT("Blocks Read"), PGICON_STATISTICS);
        statistics->SetItem(0l, 1, stats->GetVal(wxT("blks_read")));
        statistics->InsertItem(1, wxT("Blocks Hit"), PGICON_STATISTICS);
        statistics->SetItem(1, 1, stats->GetVal(wxT("blks_hit")));

        delete stats;
    }
}


pgObject *pgSequence::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *sequence=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgObject *obj=(pgObject*)browser->GetItemData(parentItem);
        if (obj->GetType() == PG_SEQUENCES)
            sequence = ReadObjects((pgCollection*)obj, 0, wxT("\n   AND cl.oid=") + GetOidStr());
    }
    return sequence;
}



pgObject *pgSequence::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgSequence *sequence=0;

    pgSet *sequences;
    if (collection->GetConnection()->BackendMinimumVersion(7, 5))
    {
        sequences = collection->GetDatabase()->ExecuteSet(
            wxT("SELECT cl.oid, relname, spcname, pg_get_userbyid(relowner) AS seqowner, relacl, description\n")
            wxT("  FROM pg_class cl\n")
            wxT("  LEFT OUTER JOIN pg_tablespace ta on ta.oid=cl.reltablespace\n")
            wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=cl.oid\n")
            wxT(" WHERE relkind = 'S' AND relnamespace  = ") + collection->GetSchema()->GetOidStr()
            + restriction + wxT("\n")
            wxT(" ORDER BY relname"));
    }
    else
    {
        sequences = collection->GetDatabase()->ExecuteSet(
            wxT("SELECT cl.oid, relname, pg_get_userbyid(relowner) AS seqowner, relacl, description\n")
            wxT("  FROM pg_class cl\n")
            wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=cl.oid\n")
            wxT(" WHERE relkind = 'S' AND relnamespace  = ") + collection->GetSchema()->GetOidStr()
            + restriction + wxT("\n")
            wxT(" ORDER BY relname"));
    }
    if (sequences)
    {
        while (!sequences->Eof())
        {
            sequence = new pgSequence(collection->GetSchema(), 
                                            sequences->GetVal(wxT("relname")));

            sequence->iSetOid(sequences->GetOid(wxT("oid")));
            sequence->iSetComment(sequences->GetVal(wxT("description")));
            sequence->iSetOwner(sequences->GetVal(wxT("seqowner")));
            sequence->iSetAcl(sequences->GetVal(wxT("relacl")));
            if (collection->GetConnection()->BackendMinimumVersion(7, 5))
                sequence->iSetTablespace(sequences->GetVal(wxT("spcname")));

            if (browser)
            {
                collection->AppendBrowserItem(browser, sequence);
	  			sequences->MoveNext();
            }
            else
                break;
        }
		delete sequences;
    }
    return sequence;
}
