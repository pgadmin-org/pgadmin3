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
    return GetDatabase()->ExecuteVoid(wxT("DROP INDEX ") + GetQuotedFullIdentifier());
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
        + wxT("\n  ON ") + qtIdent(GetIdxSchema()) + wxT(".") + qtIdent(GetIdxTable())
        + wxT("\n  USING ") + GetIndexType()
        + wxT("\n  (");
    if (GetProcName().IsNull())
        str += GetQuotedColumns();
    else
    {
        str += qtIdent(GetProcNamespace()) + wxT(".")+qtIdent(GetProcName())+wxT("(")+GetQuotedTypedColumns()+wxT(")");
        if (!this->GetOperatorClasses().IsNull())
            str += wxT(" ") + GetOperatorClasses();
    }

    str += wxT(")");
    AppendIfFilled(str, wxT("\n  WHERE "), GetConstraint());

    str += wxT(";\n");
    
    return str;
}


wxString pgIndex::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Index: ") + GetQuotedFullIdentifier() + wxT("\n")
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

            pgSet *colSet=ExecuteSet(wxT(
                "SELECT attname as conattname\n"
                "  FROM pg_attribute\n"
                " WHERE attrelid=") + GetTableOidStr() + wxT(" AND attnum=") + cn);
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
                columnCount++;
                delete colSet;
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
}


void pgIndex::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    ReadColumnDetails();
    if (properties)
    {
        CreateListColumns(properties);
        int pos=0;

        InsertListItem(properties, pos++, wxT("Name"), GetName());
        InsertListItem(properties, pos++, wxT("OID"), GetOid());
        if (GetProcName().IsNull())
            InsertListItem(properties, pos++, wxT("Columns"), GetColumns());
        else
        {
            InsertListItem(properties, pos++, wxT("Procedure "), GetProcNamespace() + wxT(".")+GetProcName()+wxT("(")+GetTypedColumns()+wxT(")"));
            InsertListItem(properties, pos++, wxT("Operator Classes"), GetOperatorClasses());
        }
        InsertListItem(properties, pos++, wxT("Unique?"), GetIsUnique());
        InsertListItem(properties, pos++, wxT("Primary?"), GetIsPrimary());
        InsertListItem(properties, pos++, wxT("Clustered?"), GetIsClustered());
        InsertListItem(properties, pos++, wxT("Index Type"), GetIndexType());
        InsertListItem(properties, pos++, wxT("Constraint"), GetConstraint());
        InsertListItem(properties, pos++, wxT("System index?"), GetSystemObject());
        InsertListItem(properties, pos++, wxT("Comment"), GetComment());
    }
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

        pgSet *indexes= collection->GetDatabase()->ExecuteSet(wxT(
        "SELECT cls.oid, cls.relname as idxname, indrelid, indkey, indisclustered, indisunique, indisprimary, n.nspname,\n"
        "       proname, tab.relname as tabname, pn.nspname as pronspname, proargtypes, indclass, description,\n"
        "       ") + collection->GetDatabase()->GetExprFunction() + wxT("(indpred, indrelid) as indconstraint, contype, condeferrable, condeferred, amname\n"
        "  FROM pg_index idx\n"
        "  JOIN pg_class cls ON cls.oid=indexrelid\n"
        "  JOIN pg_class tab ON tab.oid=indrelid\n"
        "  JOIN pg_namespace n ON n.oid=tab.relnamespace\n"
        "  JOIN pg_am am ON am.oid=cls.relam\n"
        "  LEFT OUTER JOIN pg_proc pr ON pr.oid=indproc\n"
        "  LEFT OUTER JOIN pg_namespace pn ON pn.oid=pr.pronamespace\n"
        "  LEFT OUTER JOIN pg_description des ON des.objoid=cls.oid\n"
        "  LEFT OUTER JOIN pg_constraint con ON con.conrelid=indrelid AND conname=cls.relname\n"
        " WHERE indrelid = ") + collection->GetOidStr() 
        + restriction + wxT("\n"
        " ORDER BY cls.relname"));

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
            index->iSetProcArgTypeList(indexes->GetVal(wxT("proargtypes")));
            index->iSetProcNamespace(indexes->GetVal(wxT("pronspname")));
            index->iSetProcName(indexes->GetVal(wxT("proname")));
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
