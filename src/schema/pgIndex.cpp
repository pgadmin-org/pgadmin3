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
        str += wxT("UNIQUE ");
    str += wxT("INDEX ");
    str += qtIdent(GetName()) 
        + wxT(" ON ") + qtIdent(GetIdxSchema()) + wxT(".") + qtIdent(GetIdxTable())+wxT(" (");
//        + wxT(" USING ") + ????
    if (GetProcName().IsNull())
        str += GetQuotedColumns();
    else
    {
        str += qtIdent(GetProcNamespace()) + wxT(".")+qtIdent(GetProcName())+wxT("(")+GetQuotedTypedColumns()+wxT(")");
        if (!this->GetOperatorClasses().IsNull())
            str += wxT(" ") + GetOperatorClasses();
    }

    str += wxT(")");
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

    if (properties)
    {
        properties->ClearAll();
        properties->InsertColumn(0, wxT("Property"), wxLIST_FORMAT_LEFT, 150);
        properties->InsertColumn(1, wxT("Value"), wxLIST_FORMAT_LEFT, 200);

        int pos=0;

        InsertListItem(properties, pos++, wxT("Name"), GetName());
        InsertListItem(properties, pos++, wxT("OID"), NumToStr(GetOid()));
        if (GetProcName().IsNull())
            InsertListItem(properties, pos++, wxT("Columns"), GetColumns());
        else
        {
            InsertListItem(properties, pos++, wxT("Procedure "), GetProcNamespace() + wxT(".")+GetProcName()+wxT("(")+GetTypedColumns()+wxT(")"));
            InsertListItem(properties, pos++, wxT("Operator Classes"), GetOperatorClasses());
        }
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
            "SELECT cls.oid, cls.relname as idxname, indrelid, indkey, indisclustered, indisunique, indisprimary, n.nspname,\n"
            "  proname, tab.relname as tabname, pn.nspname as pronspname, proargtypes, indclass"
            "  FROM pg_index idx\n"
            "  JOIN pg_class cls ON cls.oid=indexrelid\n"
            "  JOIN pg_class tab ON tab.oid=indrelid\n"
            "  JOIN pg_namespace n ON n.oid=tab.relnamespace\n"
            "  LEFT OUTER JOIN pg_proc pr ON pr.oid=indproc\n"
            "  LEFT OUTER JOIN pg_namespace pn ON pn.oid=pr.pronamespace\n"
            " WHERE indrelid = ") + collection->GetOidStr() + wxT("\n"
            "   AND NOT indisprimary\n"
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
                index->iSetProcArgTypeList(indexes->GetVal(wxT("proargtypes")));
                index->iSetProcNamespace(indexes->GetVal(wxT("pronspname")));
                index->iSetProcName(indexes->GetVal(wxT("proname")));
                index->iSetOperatorClassList(indexes->GetVal(wxT("indclass")));

                browser->AppendItem(collection->GetId(), index->GetIdentifier(), PGICON_INDEX, -1, index);

			    indexes->MoveNext();
            }

		    delete indexes;
        }
    }
}

