//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgIndex.cpp - Index class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "pgIndex.h"
#include "pgCollection.h"


pgIndex::pgIndex(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, PG_INDEX, newName)
{
}

pgIndex::~pgIndex()
{
}



wxString pgIndex::GetCreate()
{
    wxString str;
// no functional indexes so far

    str = wxT("CREATE ");
    if (GetIsUnique())
        str += wxT("INDEX ");
    str += qtIdent(GetName()) 
        + wxT(" ON ") + qtIdent(GetIdxSchema()) + wxT(".") + qtIdent(GetIdxTable())
//        + wxT(" USING ") + ????
        + wxT(" (") + GetQuotedColumns() + wxT(")");
    if (!GetConstraint().IsNull())
        str += wxT(" WHERE ") + GetConstraint();
    str += wxT(";\n");
    
    return str;
}


wxString pgIndex::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- DROP INDEX ") + qtIdent(GetIdxSchema()) + wxT(".") + qtIdent(GetName())
            + wxT(";\n")
            + GetCreate();
    }
    return sql;
}



void pgIndex::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    SetButtons(form);

    if (!expandedKids)
    {
        expandedKids = true;
        wxStringTokenizer collist(GetColumnNumbers());
        wxString cn;
        columnCount=0;

        while (collist.HasMoreTokens())
        {
            cn=collist.GetNextToken();
            pgSet *set=ExecuteSet(wxT(
                "SELECT attname as conattname\n"
                "  FROM pg_attribute\n"
                " WHERE attrelid=") + GetTableOidStr() + wxT(" AND attnum=") + cn);
            if (set)
            {
                if (columnCount)
                {
                    columns += wxT(", ");
                    quotedColumns += wxT(", ");
                }
                columns += set->GetVal(0);
                quotedColumns += qtIdent(set->GetVal(0));
                columnCount++;
                delete set;
            }
        }
    }

    if (properties)
    {
        properties->ClearAll();
        properties->InsertColumn(0, wxT("Property"), wxLIST_FORMAT_LEFT, 150);
        properties->InsertColumn(1, wxT("Value"), wxLIST_FORMAT_LEFT, 200);

        int pos=0;

        InsertListItem(properties, pos++, wxT("Name"), GetName());
        InsertListItem(properties, pos++, wxT("OID"), NumToStr(GetOid()));
        InsertListItem(properties, pos++, wxT("Columns"), GetColumns());
        InsertListItem(properties, pos++, wxT("Unique?"), BoolToYesNo(GetIsUnique()));
        InsertListItem(properties, pos++, wxT("Primary?"), BoolToYesNo(GetIsPrimary()));
        InsertListItem(properties, pos++, wxT("Clustered?"), BoolToYesNo(GetIsClustered()));
        InsertListItem(properties, pos++, wxT("Constraint"), GetConstraint());
        InsertListItem(properties, pos++, wxT("Index Type"), GetIndexType());
        InsertListItem(properties, pos++, wxT("System index?"), BoolToYesNo(GetSystemObject()));
        InsertListItem(properties, pos++, wxT("Comment"), GetComment());
    }
}



void pgIndex::ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    wxString msg;
    pgIndex *index;

    if (browser->GetChildrenCount(collection->GetId(), FALSE) == 0)
    {
        // Log
        msg.Printf(wxT("Adding Indexs to schema %s"), collection->GetSchema()->GetIdentifier().c_str());
        wxLogInfo(msg);

        // Get the Indexes

        pgSet *indexes= collection->GetDatabase()->ExecuteSet(wxT(
            "SELECT cls.oid, cls.relname as idxname, indrelid, indkey, indisclustered, indisunique, indisprimary, proname, tab.relname as tabname, nspname"
            "  FROM pg_index idx\n"
            "  JOIN pg_class cls ON cls.oid=indexrelid\n"
            "  JOIN pg_class tab ON tab.oid=indrelid\n"
            "  JOIN pg_namespace n ON n.oid=tab.relnamespace\n"
            "  LEFT OUTER JOIN pg_proc pr ON pr.oid=indproc\n"
            " WHERE indrelid = ") + collection->GetOidStr() + wxT("\n"
            " ORDER BY cls.relname"));

        if (indexes)
        {
            while (!indexes->Eof())
            {
                index = new pgIndex(collection->GetSchema(), indexes->GetVal(wxT("idxname")));
                index->iSetOid(StrToDouble(indexes->GetVal(wxT("oid"))));
                index->iSetTableOid(collection->GetOid());
                index->iSetIsClustered(StrToBool(indexes->GetVal(wxT("indisclustered"))));
                index->iSetIsUnique(StrToBool(indexes->GetVal(wxT("indisunique"))));
                index->iSetIsPrimary(StrToBool(indexes->GetVal(wxT("indisprimary"))));
                index->iSetColumnNumbers(indexes->GetVal(wxT("indkey")));
                index->iSetIdxSchema(indexes->GetVal(wxT("nspname")));
                index->iSetIdxTable(indexes->GetVal(wxT("tabname")));
                index->iSetRelTableOid(StrToDouble(indexes->GetVal(wxT("indrelid"))));

                browser->AppendItem(collection->GetId(), index->GetIdentifier(), PGICON_INDEX, -1, index);
	    
			    indexes->MoveNext();
            }

		    delete indexes;
        }
    }
}

