//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgIndex.cpp - Index class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgfeatures.h"
#include "pgObject.h"
#include "pgIndex.h"
#include "pgIndexConstraint.h"
#include "pgCollection.h"


pgIndex::pgIndex(pgSchema *newSchema, const wxString& newName, int type)
: pgSchemaObject(newSchema, type, newName)
{
}

pgIndex::~pgIndex()
{
}

bool pgIndex::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(wxT("DROP INDEX ") + GetQuotedFullIdentifier() + wxT(";"));
}

wxString pgIndex::GetCreate()
{
    wxString str;
// no functional indexes so far

    str = wxT("CREATE ");
    if (GetIsUnique())
        str += wxT("UNIQUE ");
    str += wxT("INDEX ");
    str += qtIdent(GetName()) 
        + wxT("\n  ON ") + GetQuotedSchemaPrefix(GetIdxSchema()) + qtIdent(GetIdxTable())
        + wxT("\n  USING ") + GetIndexType()
        + wxT("\n  (");
    if (GetProcName().IsNull())
        str += GetQuotedColumns();
    else
    {
//        str += GetQuotedSchemaPrefix(GetProcNamespace()) + qtIdent(GetProcName())+wxT("(")+GetQuotedTypedColumns()+wxT(")");
        str += GetQuotedSchemaPrefix(GetProcNamespace()) + qtIdent(GetProcName())+wxT("(")+GetQuotedColumns()+wxT(")");
        if (!this->GetOperatorClasses().IsNull())
            str += wxT(" ") + GetOperatorClasses();
    }

    str += wxT(")");
    AppendIfFilled(str, wxT("\n  TABLESPACE "), qtIdent(tablespace));
    AppendIfFilled(str, wxT("\n  WHERE "), GetConstraint());

    str += wxT(";\n");

    if (GetConnection()->BackendMinimumVersion(7, 5))
        if (GetIsClustered())
            str += wxT("ALTER TABLE ") + GetQuotedSchemaPrefix(GetIdxSchema()) + qtIdent(GetIdxTable())
                +  wxT(" CLUSTER ON ") + qtIdent(GetName())
                + wxT(";\n");
    
    return str;
}


wxString pgIndex::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Index: ") + GetQuotedFullIdentifier() + wxT("\n\n")
            + wxT("-- DROP INDEX ") + GetQuotedFullIdentifier() + wxT(";\n\n")
            + GetCreate()
            + GetCommentSql();
    }
    return sql;
}



void pgIndex::ReadColumnDetails()
{
    if (!expandedKids)
    {
        expandedKids = true;

        if (GetConnection()->BackendMinimumVersion(7, 4))
        {
            long i;

            for (i=1 ; i <= columnCount ; i++)
            {
                if (i > 1)
                {
                    columns += wxT(", ");
                    quotedColumns += wxT(", ");
                }
                wxString str=ExecuteScalar(
                    wxT("SELECT pg_get_indexdef(") + GetOidStr() + wxT(", ")
                    + NumToStr(i) + GetDatabase()->GetPrettyOption() + wxT(")"));
                columns += str;
                quotedColumns += str;
            }
        }
        else
        {
            // its a 7.3 db

            // We cannot use SELECT IN (colNumbers) here because we couldn't be sure
            // about the read order
            wxStringTokenizer collist(GetColumnNumbers());
            wxStringTokenizer args(procArgTypeList);
            wxString cn, ct;
            columnCount=0;

            while (collist.HasMoreTokens())
            {
                cn=collist.GetNextToken();
                ct=args.GetNextToken();

                pgSet *colSet=ExecuteSet(
                    wxT("SELECT attname as conattname\n")
                    wxT("  FROM pg_attribute\n")
                    wxT(" WHERE attrelid=") + GetTableOidStr() + wxT(" AND attnum=") + cn);
                if (colSet)
                {
                    if (columnCount)
                    {
                        columns += wxT(", ");
                        quotedColumns += wxT(", ");
                    }
                    wxString colName=colSet->GetVal(0);
                    columns += colName;
                    quotedColumns += qtIdent(colName);

                    if (!ct.IsNull())
                    {
                        pgSet *typeSet=ExecuteSet(wxT(
                            "SELECT typname FROM pg_type where oid=") + ct);
                        if (typeSet)
                        {
                            if (columnCount)
                            {
                                procArgs += wxT(", ");
                                typedColumns += wxT(", ");
                                quotedTypedColumns += wxT(", ");
                            }
                            wxString colType=typeSet->GetVal(0);
                            procArgs += colType;
                            typedColumns += colName + wxT("::") + colType;
                            quotedTypedColumns += qtIdent(colName) + wxT("::") + colType;
                            delete typeSet;
                        }
                    }
                    delete colSet;
                }
                columnCount++;
            }
        }
        wxStringTokenizer ops(operatorClassList);
        wxString op;
        while (ops.HasMoreTokens())
        {
            op = ops.GetNextToken();
            pgSet *set=ExecuteSet(wxT(
                "SELECT opcname FROM pg_opclass WHERE oid=") + op);
            if (set)
            {
                if (!operatorClasses.IsNull())
                    operatorClasses += wxT(", ");
                operatorClasses += set->GetVal(0);
                delete set;
            }
        }
    }
}


void pgIndex::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    ReadColumnDetails();
    if (properties)
    {
        CreateListColumns(properties);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("OID"), GetOid());
        if (!tablespace.IsEmpty())
            properties->AppendItem(_("Tablespace"), tablespace);
        if (!GetProcName().IsNull())
            properties->AppendItem(_("Procedure "), GetSchemaPrefix(GetProcNamespace())+GetProcName()+wxT("(")+GetTypedColumns()+wxT(")"));
        else
            properties->AppendItem(_("Columns"), GetColumns());

        properties->AppendItem(_("Operator classes"), GetOperatorClasses());
        properties->AppendItem(_("Unique?"), GetIsUnique());
        properties->AppendItem(_("Primary?"), GetIsPrimary());
        properties->AppendItem(_("Clustered?"), GetIsClustered());
        properties->AppendItem(_("Access method"), GetIndexType());
        properties->AppendItem(_("Constraint"), GetConstraint());
        properties->AppendItem(_("System index?"), GetSystemObject());
        properties->AppendItem(_("Comment"), GetComment());
    }
}


void pgIndex::ShowStatistics(frmMain *form, ctlListView *statistics)
{
    if (GetConnection()->HasFeature(FEATURE_SIZE))
        DisplayStatistics(statistics, 
            wxT("SELECT pg_size_pretty(pg_relation_size(") + GetOidStr() + wxT(")) AS ") + qtIdent(_("Index Size")));
}


pgObject *pgIndex::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *index=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgCollection *collection=(pgCollection*)browser->GetItemData(parentItem);
        if (collection->IsCollection() && collection->IsCollectionForType(PG_INDEX))
            index = ReadObjects(collection, 0, wxT("\n   AND cls.oid=") + GetOidStr());
    }
    return index;
}



pgObject *pgIndex::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgIndex *index=0;

    wxString proname, projoin;
    if (collection->GetConnection()->BackendMinimumVersion(7, 4))
    {
        proname = wxT("indnatts, ");
        if (collection->GetConnection()->BackendMinimumVersion(7, 5))
        {
            proname += wxT("spcname, ");
            projoin = wxT("  LEFT OUTER JOIN pg_tablespace ta on ta.oid=cls.reltablespace\n");
        }
    }
    else
    {
        proname=wxT("proname, pn.nspname as pronspname, proargtypes, ");
        projoin =   wxT("  LEFT OUTER JOIN pg_proc pr ON pr.oid=indproc\n")
                    wxT("  LEFT OUTER JOIN pg_namespace pn ON pn.oid=pr.pronamespace\n");
    }
    pgSet *indexes= collection->GetDatabase()->ExecuteSet(
        wxT("SELECT DISTINCT ON(cls.relname) cls.oid, cls.relname as idxname, indrelid, indkey, indisclustered, indisunique, indisprimary, n.nspname,\n")
        wxT("       ") + proname + wxT("tab.relname as tabname, indclass, CASE contype WHEN 'p' THEN desp.description ELSE des.description END AS description,\n")
        wxT("       pg_get_expr(indpred, indrelid") + collection->GetDatabase()->GetPrettyOption() + wxT(") as indconstraint, contype, condeferrable, condeferred, amname\n")
        wxT("  FROM pg_index idx\n")
        wxT("  JOIN pg_class cls ON cls.oid=indexrelid\n")
        wxT("  JOIN pg_class tab ON tab.oid=indrelid\n")
        + projoin + 
        wxT("  JOIN pg_namespace n ON n.oid=tab.relnamespace\n")
        wxT("  JOIN pg_am am ON am.oid=cls.relam\n")
        wxT("  LEFT JOIN pg_depend dep ON (dep.classid = cls.tableoid AND dep.objid = cls.oid AND dep.refobjsubid = '0')\n")
        wxT("  LEFT OUTER JOIN pg_constraint con ON (con.tableoid = dep.refclassid AND con.oid = dep.refobjid)\n")
        wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=cls.oid\n")
        wxT("  LEFT OUTER JOIN pg_description desp ON (desp.objoid=con.oid AND desp.objsubid = 0)\n")
        wxT(" WHERE indrelid = ") + collection->GetOidStr()
        + restriction + wxT("\n")
        wxT(" ORDER BY cls.relname"));

    if (indexes)
    {
        while (!indexes->Eof())
        {
            switch (indexes->GetVal(wxT("contype"))[0U])
            {
                case 0:
                    index = new pgIndex(collection->GetSchema(), indexes->GetVal(wxT("idxname")));
                    break;
                case 'p':
                    index = new pgPrimaryKey(collection->GetSchema(), indexes->GetVal(wxT("idxname")));
                    break;
                case 'u':
                    index = new pgUnique(collection->GetSchema(), indexes->GetVal(wxT("idxname")));
                    break;
                default:
                    index=0;
                    break;
            }
            index->iSetOid(indexes->GetOid(wxT("oid")));
            index->iSetTableOid(collection->GetOid());
            index->iSetIsClustered(indexes->GetBool(wxT("indisclustered")));
            index->iSetIsUnique(indexes->GetBool(wxT("indisunique")));
            index->iSetIsPrimary(indexes->GetBool(wxT("indisprimary")));
            index->iSetColumnNumbers(indexes->GetVal(wxT("indkey")));
            index->iSetIdxSchema(indexes->GetVal(wxT("nspname")));
            index->iSetComment(indexes->GetVal(wxT("description")));
            index->iSetIdxTable(indexes->GetVal(wxT("tabname")));
            index->iSetRelTableOid(indexes->GetOid(wxT("indrelid")));
            if (collection->GetConnection()->BackendMinimumVersion(7, 4))
            {
                index->iSetColumnCount(indexes->GetLong(wxT("indnatts")));
                if (collection->GetConnection()->BackendMinimumVersion(7, 5))
                    index->iSetTablespace(indexes->GetVal(wxT("spcname")));
            }
            else
            {
                index->iSetColumnCount(0L);
                index->iSetProcNamespace(indexes->GetVal(wxT("pronspname")));
                index->iSetProcName(indexes->GetVal(wxT("proname")));
                index->iSetProcArgTypeList(indexes->GetVal(wxT("proargtypes")));
            }
            index->iSetOperatorClassList(indexes->GetVal(wxT("indclass")));
            index->iSetDeferrable(indexes->GetBool(wxT("condeferrable")));
            index->iSetDeferred(indexes->GetBool(wxT("condeferred")));
            index->iSetConstraint(indexes->GetVal(wxT("indconstraint")));
            index->iSetIndexType(indexes->GetVal(wxT("amname")));

            if (browser)
            {
                collection->AppendBrowserItem(browser, index);
        		indexes->MoveNext();
            }
            else
                break;
        }

		delete indexes;
    }
    return index;
}



pgObject *pgIndex::ReadObjects(pgCollection *collection, wxTreeCtrl *browser)
{
    return ReadObjects(collection, browser, wxT("\n   AND conname IS NULL"));
}
