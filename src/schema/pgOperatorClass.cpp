//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgOperatorClass.cpp - OperatorClass class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "pgOperatorClass.h"
#include "pgCollection.h"
#include "pgFunction.h"


pgOperatorClass::pgOperatorClass(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, PG_OPERATORCLASS, newName)
{
}

pgOperatorClass::~pgOperatorClass()
{
}

bool pgOperatorClass::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(wxT("DROP OPERATOR CLASS ") + GetQuotedFullIdentifier() + wxT(" USING ") + GetAccessMethod());
}

wxString pgOperatorClass::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Operator Class: \"") + GetName() + wxT("\"\n");
        sql += wxT("CREATE OPERATOR CLASS ") + GetQuotedFullIdentifier();
        if (GetOpcDefault())
            sql += wxT("DEFAULT ");
        sql += wxT(" FOR TYPE ") + GetInType()
            +  wxT(" USING ") + GetAccessMethod() 
            +  wxT(" AS");
        unsigned int i;
        bool needComma=false;

        for (i=0 ; i < operators.Count() ; i++)
        {
            if (needComma)
                sql += wxT(",");

            sql += wxT("\n   OPERATOR ") + operators.Item(i);
            needComma=true;
        }
        for (i=0 ; i < functions.Count() ; i++)
        {
            if (needComma)
                sql += wxT(",");

            sql += wxT("\n   FUNCTION ") + functions.Item(i);
            needComma=true;
        }
        sql += wxT(";\n");
    }

    return sql;
}


void pgOperatorClass::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids=true;

        pgSet *set;
        set=ExecuteSet(wxT(
            "SELECT amopstrategy, amopreqcheck, oprname, lt.typname as lefttype, rt.typname as righttype\n"
            "  FROM pg_amop am\n"
            "  JOIN pg_operator op ON amopoid=op.oid\n"
            "  LEFT OUTER JOIN pg_type lt ON lt.oid=oprleft\n"
            "  LEFT OUTER JOIN pg_type rt ON rt.oid=oprright\n"
            " WHERE amopclaid=") + GetOidStr()+ wxT("\n"
            " ORDER BY amopstrategy"));
        if (set)
        {
            while (!set->Eof())
            {
                wxString str=set->GetVal(wxT("ampostrategy")) + wxT(" ") + qtIdent(set->GetVal(wxT("oprname")));
                wxString lt=set->GetVal(wxT("lefttype"));
                wxString rt=set->GetVal(wxT("righttype"));
                if (!lt.IsEmpty() || !rt.IsEmpty())
                {
                    str += wxT("(");
                    if (!lt.IsEmpty())
                    {
                        str += lt;
                        if (!rt.IsEmpty())
                            str += wxT(", ");
                    }
                    if (!rt.IsEmpty())
                        str += rt;
                    str += wxT(")");
                }
                if (set->GetBool(wxT("amopreqcheck")))
                    str += wxT(" RECHECK");
                operators.Add(str);
                set->MoveNext();
            }
            delete set;
        }

        set=ExecuteSet(wxT(
            "SELECT amprocnum, amproc::oid\n"
            "  FROM pg_amproc am\n"
            " WHERE amopclaid=") + GetOidStr() +wxT("\n"
            " ORDER BY amprocnum"));
        if (set)
        {
            while (!set->Eof())
            {
                wxString amproc=set->GetVal(wxT("amproc"));
                functionOids.Add(amproc);

                // We won't build a PG_FUNCTIONS collection under OperatorClass, so we create
                // temporary function items
                pgFunction *function=pgFunction::AppendFunctions(this, GetSchema(), 0, wxT("   AND pr.oid=") + amproc);
                if (function)
                {
                    wxString str=set->GetVal(wxT("amprocnum")) + wxT(" ") + function->GetQuotedFullIdentifier();

                    functions.Add(str);
                    delete function;
                }

                set->MoveNext();
            }
            delete set;
        }
    }
    if (properties)
    {
        CreateListColumns(properties);
        int pos=0;

        InsertListItem(properties, pos++, wxT("Name"), GetName());
        InsertListItem(properties, pos++, wxT("Default?"), GetOpcDefault());
        InsertListItem(properties, pos++, wxT("For Type"), GetInType());
        InsertListItem(properties, pos++, wxT("Access Method"), GetAccessMethod());
// do we need this?
//        InsertListItem(properties, pos++, wxT("Key Type"), GetKeyType());  
        unsigned int i;
        for (i=0 ; i < operators.Count() ; i++)
            InsertListItem(properties, pos++, wxT("OPERATOR"), operators.Item(i));
        for (i=0 ; i < functions.Count() ; i++)
            InsertListItem(properties, pos++, wxT("FUNCTION"), functions.Item(i));
    }
}



pgObject *pgOperatorClass::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *operatorClass=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgObject *obj=(pgObject*)browser->GetItemData(parentItem);
        if (obj->GetType() == PG_OPERATORCLASSES)
            operatorClass = ReadObjects((pgCollection*)obj, 0, wxT("\n   AND op.oid=") + GetOidStr());
    }
    return operatorClass;
}



pgObject *pgOperatorClass::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgOperatorClass *operatorClass=0;

    pgSet *operatorClasses= collection->GetDatabase()->ExecuteSet(wxT(
        "SELECT op.*, it.typname as intypename, dt.typname as keytypename, amname\n"
        "  FROM pg_opclass op\n"
        "  JOIN pg_am am ON am.oid=opcamid\n"
        "  JOIN pg_type it ON it.oid=opcintype\n"
        "  LEFT OUTER JOIN pg_type dt ON dt.oid=opckeytype\n"
        " WHERE opcnamespace = ") + collection->GetSchema()->GetOidStr() 
        + restriction + wxT("\n"
        " ORDER BY opcname"));

    if (operatorClasses)
    {
        while (!operatorClasses->Eof())
        {
            operatorClass = new pgOperatorClass(
                        collection->GetSchema(), operatorClasses->GetVal(wxT("opcname")));

            operatorClass->iSetOwner(operatorClasses->GetVal(wxT("opcowner")));
            operatorClass->iSetAccessMethod(operatorClasses->GetVal(wxT("amname")));
            operatorClass->iSetInType(operatorClasses->GetVal(wxT("intypename")));
            operatorClass->iSetKeyType(operatorClasses->GetVal(wxT("keytypename")));
            operatorClass->iSetOpcDefault(operatorClasses->GetBool(wxT("opcdefault")));

            if (browser)
            {
                browser->AppendItem(collection->GetId(), operatorClass->GetIdentifier(), PGICON_OPERATORCLASS, -1, operatorClass);
				operatorClasses->MoveNext();
            }
            else
                break;
        }

		delete operatorClasses;
    }
    return operatorClass;
}



void pgOperatorClass::ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (browser->GetChildrenCount(collection->GetId(), FALSE) == 0)
    {
        // Log
        wxLogInfo(wxT("Adding OperatorClasss to schema ") + collection->GetSchema()->GetIdentifier());

        // Get the OperatorClasss
        ReadObjects(collection, browser);
    }
}

