//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgOperator.cpp - Operator class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "pgOperator.h"
#include "pgCollection.h"


pgOperator::pgOperator(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, PG_OPERATOR, newName)
{
}

pgOperator::~pgOperator()
{
}

bool pgOperator::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    wxString sql = wxT("DROP OPERATOR ") + GetFullIdentifier();
    
    if (GetLeftType().Length() > 0)
        sql += wxT(" (") + qtIdent(GetLeftType());
    else
        sql += wxT(") (NONE");

    if (GetRightType().Length() > 0)
        sql += wxT(", ") + qtIdent(GetLeftType()) + wxT(")");
    else
        sql += wxT(", NONE)");

    return GetDatabase()->ExecuteVoid(sql);
}

wxString pgOperator::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Operator: ") + GetFullIdentifier() + wxT(" (") + GetLeftType() + wxT(", ") + GetRightType() + wxT(")\n")
            + wxT("CREATE OPERATOR ") + GetFullIdentifier()
            + wxT("(\n  PROCEDURE = ") + qtIdent(GetOperatorFunction());
        AppendIfFilled(sql, wxT(",\n  LEFTARG = "), qtIdent(GetLeftType()));
        AppendIfFilled(sql, wxT(",\n  RIGHTARG = "), qtIdent(GetRightType()));
        AppendIfFilled(sql, wxT(",\n  COMMUTATOR = "), GetCommutator());
        AppendIfFilled(sql, wxT(",\n  RESTRICT = "), qtIdent(GetRestrictFunction()));
        if (GetHashJoins())  sql += wxT(",\n  HASHES");
        if (!leftSortOperator.IsNull() || !rightSortOperator.IsNull() ||
            !lessOperator.IsNull() || !greaterOperator.IsNull())
            sql += wxT(",\n  MERGES");
        AppendIfFilled(sql, wxT(",\n  SORT1 = "), GetLeftSortOperator());
        AppendIfFilled(sql, wxT(",\n  SORT2 = "), GetRightSortOperator());
        AppendIfFilled(sql, wxT(",\n  LTCMP = "), GetLessOperator());
        AppendIfFilled(sql, wxT(",\n  GTCMP = "), GetGreaterOperator());
        sql += wxT(";\n")
            + GetCommentSql();
    }

    return sql;
}


wxString pgOperator::GetFullName() const
{
    return GetName() + wxT(" (") + GetLeftType() + wxT(", ") + GetRightType() + wxT(")");
}

void pgOperator::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (properties)
    {
        CreateListColumns(properties);
        int pos=0;

        InsertListItem(properties, pos++, wxT("Name"), GetName());
        InsertListItem(properties, pos++, wxT("OID"), GetOid());
        InsertListItem(properties, pos++, wxT("Owner"), GetOwner());
        InsertListItem(properties, pos++, wxT("Kind"), GetKind());
        if (!leftType.IsNull())
            InsertListItem(properties, pos++, wxT("Left Type"), GetLeftType());
        if (!rightType.IsNull())
            InsertListItem(properties, pos++, wxT("Right Type"), GetRightType());
        InsertListItem(properties, pos++, wxT("Result Type"), GetResultType());
        InsertListItem(properties, pos++, wxT("Operator Function"), GetOperatorFunction());
        InsertListItem(properties, pos++, wxT("Join Function"), GetJoinFunction());
        InsertListItem(properties, pos++, wxT("Restrict Function"), GetRestrictFunction());
        InsertListItem(properties, pos++, wxT("Commutator"), GetCommutator());
        InsertListItem(properties, pos++, wxT("Negator"), GetNegator());
        InsertListItem(properties, pos++, wxT("Left Sort Operator"), GetLeftSortOperator());
        InsertListItem(properties, pos++, wxT("Right Sort Operator"), GetRightSortOperator());
        InsertListItem(properties, pos++, wxT("Less Than Operator"), GetLessOperator());
        InsertListItem(properties, pos++, wxT("Greater Than Operator"), GetGreaterOperator());
        InsertListItem(properties, pos++, wxT("Hash Joins?"), GetHashJoins());
        InsertListItem(properties, pos++, wxT("Comment"), GetComment());
    }
}



pgObject *pgOperator::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *oper=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgObject *obj=(pgObject*)browser->GetItemData(parentItem);
        if (obj->GetType() == PG_OPERATORS)
            oper = ReadObjects((pgCollection*)obj, 0, wxT("\n   AND op.oid=") + GetOidStr());
    }
    return oper;
}



pgObject *pgOperator::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgOperator *oper;

    pgSet *operators= collection->GetDatabase()->ExecuteSet(wxT(
        "SELECT op.oid, op.oprname, pg_get_userbyid(op.oprowner) as opowner, op.oprkind, op.oprcanhash,\n"
        "       lt.typname as lefttype, rt.typname as righttype, et.typname as resulttype,\n"
        "       co.oprname as compop, ne.oprname as negop, lso.oprname as leftsortop, rso.oprname as rightsortop,\n"
        "       lco.oprname as lscmpop, gco.oprname as gtcmpop,\n"
        "       po.proname as operproc, pj.proname as joinproc, pr.proname as restrproc, description\n"
        "  FROM pg_operator op\n"
        "  JOIN pg_type lt ON lt.oid=op.oprleft\n"
        "  JOIN pg_type rt ON rt.oid=op.oprright\n"
        "  JOIN pg_type et on et.oid=op.oprresult\n"
        "  LEFT OUTER JOIN pg_operator co ON co.oid=op.oprcom\n"
        "  LEFT OUTER JOIN pg_operator ne ON ne.oid=op.oprnegate\n"
        "  LEFT OUTER JOIN pg_operator lso ON lso.oid=op.oprlsortop\n"
        "  LEFT OUTER JOIN pg_operator rso ON rso.oid=op.oprrsortop\n"
        "  LEFT OUTER JOIN pg_operator lco ON lco.oid=op.oprltcmpop\n"
        "  LEFT OUTER JOIN pg_operator gco ON gco.oid=op.oprgtcmpop\n"
        "  JOIN pg_proc po ON po.oid=op.oprcode\n"
        "  LEFT OUTER JOIN pg_proc pr ON pr.oid=op.oprrest\n"
        "  LEFT OUTER JOIN pg_proc pj ON pj.oid=op.oprjoin\n"
        "  LEFT OUTER JOIN pg_description des ON des.objoid=op.oid\n"
        " WHERE op.oprnamespace = ") + collection->GetSchema()->GetOidStr() 
        + restriction + wxT("\n"
        " ORDER BY op.oprname"));

    if (operators)
    {
        while (!operators->Eof())
        {
            oper = new pgOperator(collection->GetSchema(), operators->GetVal(wxT("oprname")));
            oper->iSetOid(operators->GetOid(wxT("oid")));
            oper->iSetOwner(operators->GetVal(wxT("opowner")));
            oper->iSetComment(operators->GetVal(wxT("description")));
            oper->iSetLeftType(operators->GetVal(wxT("lefttype")));
            oper->iSetRightType(operators->GetVal(wxT("righttype")));
            oper->iSetResultType(operators->GetVal(wxT("resulttype")));
            oper->iSetOperatorFunction(operators->GetVal(wxT("operproc")));
            oper->iSetJoinFunction(operators->GetVal(wxT("joinproc")));
            oper->iSetRestrictFunction(operators->GetVal(wxT("restrproc")));
            oper->iSetLeftSortOperator(operators->GetVal(wxT("leftsortop")));
            oper->iSetRightSortOperator(operators->GetVal(wxT("rightsortop")));
            oper->iSetLessOperator(operators->GetVal(wxT("lscmpop")));
            oper->iSetGreaterOperator(operators->GetVal(wxT("gtcmpop")));
            oper->iSetCommutator(operators->GetVal(wxT("compop")));
            oper->iSetNegator(operators->GetVal(wxT("negop")));
            wxString kind=operators->GetVal(wxT("oprkind"));
            oper->iSetKind(kind.IsSameAs("b") ? wxT("infix") :
                           kind.IsSameAs("l") ? wxT("prefix") :
                           kind.IsSameAs("r") ? wxT("postfix") : wxT("unknown"));
            oper->iSetHashJoins(operators->GetBool(wxT("oprcanhash")));

            if (browser)
            {
                browser->AppendItem(collection->GetId(), oper->GetFullName(), PGICON_OPERATOR, -1, oper);
			    operators->MoveNext();
            }
            else
                break;
        }

		delete operators;
    }
    return oper;
}


void pgOperator::ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (browser->GetChildrenCount(collection->GetId(), FALSE) == 0)
    {
        // Log
        wxLogInfo(wxT("Adding Operators to schema %s"), collection->GetSchema()->GetIdentifier().c_str());

        // Get the Operators
        ReadObjects(collection, browser);
    }
}

