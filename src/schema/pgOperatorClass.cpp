//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
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
    return GetDatabase()->ExecuteVoid(wxT("DROP OPERATOR CLASS ") + GetQuotedFullIdentifier() + wxT(" USING ") + GetAccessMethod() + wxT(";"));
}

wxString pgOperatorClass::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Operator Class: \"") + GetName() + wxT("\"\n\n")
            + wxT("-- DROP OPERATOR CLASS ") + GetQuotedFullIdentifier() + wxT(" USING ") + GetAccessMethod() + wxT(";")
            + wxT("\n\nCREATE OPERATOR CLASS ") + GetQuotedFullIdentifier();
        if (GetOpcDefault())
            sql += wxT(" DEFAULT");
        sql += wxT("\n   FOR TYPE ") + GetInType()
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

            sql += wxT("\n   FUNCTION ") + quotedFunctions.Item(i);
            needComma=true;
        }
        AppendIfFilled(sql, wxT("\n   STORAGE "), GetKeyType());
        sql += wxT(";\n");
    }

    return sql;
}


void pgOperatorClass::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlListView *statistics, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids=true;

        pgSet *set;
        set=ExecuteSet(
            wxT("SELECT amopstrategy, amopreqcheck, oprname, lt.typname as lefttype, rt.typname as righttype\n")
            wxT("  FROM pg_amop am\n")
            wxT("  JOIN pg_operator op ON amopopr=op.oid\n")
            wxT("  LEFT OUTER JOIN pg_type lt ON lt.oid=oprleft\n")
            wxT("  LEFT OUTER JOIN pg_type rt ON rt.oid=oprright\n")
            wxT(" WHERE amopclaid=") + GetOidStr()+ wxT("\n")
            wxT(" ORDER BY amopstrategy"));
        if (set)
        {
            while (!set->Eof())
            {
                wxString str=set->GetVal(wxT("amopstrategy")) + wxT("  ") + set->GetVal(wxT("oprname"));
                wxString lt=set->GetVal(wxT("lefttype"));
                wxString rt=set->GetVal(wxT("righttype"));
                if (lt == GetInType() && (rt.IsEmpty() || rt == GetInType()))
                    lt=wxEmptyString;
                if (rt == GetInType() && lt.IsEmpty())
                    rt = wxEmptyString;

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

        set=ExecuteSet(
            wxT("SELECT amprocnum, amproc::oid\n")
            wxT("  FROM pg_amproc am\n")
            wxT(" WHERE amopclaid=") + GetOidStr() +wxT("\n")
            wxT(" ORDER BY amprocnum"));
        if (set)
        {
            while (!set->Eof())
            {
                wxString amproc=set->GetVal(wxT("amproc"));
                functionOids.Add(amproc);

                // We won't build a PG_FUNCTIONS collection under OperatorClass, so we create
                // temporary function items
                pgFunction *function=pgFunction::AppendFunctions(this, GetSchema(), 0, wxT(" WHERE pr.oid=") + amproc);
                if (function)
                {
                    functions.Add(set->GetVal(wxT("amprocnum")) + wxT("  ") + function->GetFullName());
                    quotedFunctions.Add(set->GetVal(wxT("amprocnum")) + wxT("  ") 
                        + function->GetQuotedFullIdentifier() + wxT("(") + function->GetArgTypes() + wxT(")"));
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

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("OID"), GetOid());
        properties->AppendItem(_("Owner"), GetOwner());
        properties->AppendItem(_("Default?"), GetOpcDefault());
        properties->AppendItem(_("For type"), GetInType());
        properties->AppendItem(_("Access method"), GetAccessMethod());
        if (!GetKeyType().IsEmpty())
            properties->AppendItem(_("Storage"), GetKeyType());  
        unsigned int i;
        for (i=0 ; i < operators.Count() ; i++)
            properties->AppendItem(wxT("OPERATOR"), operators.Item(i));
        for (i=0 ; i < functions.Count() ; i++)
            properties->AppendItem(wxT("FUNCTION"), functions.Item(i));
        properties->AppendItem(_("System operator class?"), GetSystemObject());
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

    pgSet *operatorClasses= collection->GetDatabase()->ExecuteSet(
        wxT("SELECT op.oid, op.*, pg_get_userbyid(op.opcowner) as opowner, it.typname as intypename, dt.typname as keytypename, amname\n")
        wxT("  FROM pg_opclass op\n")
        wxT("  JOIN pg_am am ON am.oid=opcamid\n")
        wxT("  JOIN pg_type it ON it.oid=opcintype\n")
        wxT("  LEFT OUTER JOIN pg_type dt ON dt.oid=opckeytype\n")
        wxT(" WHERE opcnamespace = ") + collection->GetSchema()->GetOidStr()
        + restriction + wxT("\n")
        wxT(" ORDER BY opcname"));

    if (operatorClasses)
    {
        while (!operatorClasses->Eof())
        {
            operatorClass = new pgOperatorClass(
                        collection->GetSchema(), operatorClasses->GetVal(wxT("opcname")));

            operatorClass->iSetOid(operatorClasses->GetOid(wxT("oid")));
            operatorClass->iSetOwner(operatorClasses->GetVal(wxT("opowner")));
            operatorClass->iSetAccessMethod(operatorClasses->GetVal(wxT("amname")));
            operatorClass->iSetInType(operatorClasses->GetVal(wxT("intypename")));
            operatorClass->iSetKeyType(operatorClasses->GetVal(wxT("keytypename")));
            operatorClass->iSetOpcDefault(operatorClasses->GetBool(wxT("opcdefault")));

            if (browser)
            {
                collection->AppendBrowserItem(browser, operatorClass);
				operatorClasses->MoveNext();
            }
            else
                break;
        }

		delete operatorClasses;
    }
    return operatorClass;
}
