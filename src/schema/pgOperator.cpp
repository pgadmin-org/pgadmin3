//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
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
        sql = wxT("-- Operator: ") + GetFullIdentifier() + wxT("(") + GetOperands() + wxT(")\n\n")
            + wxT("-- DROP OPERATOR ") + GetFullIdentifier()
	    + wxT("(") + GetOperands() + wxT(");\n\n")
              wxT("CREATE OPERATOR ") + GetFullIdentifier()
            + wxT("(\n  PROCEDURE = ") + qtIdent(GetOperatorFunction());
        AppendIfFilled(sql, wxT(",\n  LEFTARG = "), qtIdent(GetLeftType()));
        AppendIfFilled(sql, wxT(",\n  RIGHTARG = "), qtIdent(GetRightType()));
        AppendIfFilled(sql, wxT(",\n  COMMUTATOR = "), GetCommutator());
        AppendIfFilled(sql, wxT(",\n  RESTRICT = "), qtIdent(GetRestrictFunction()));
        AppendIfFilled(sql, wxT(",\n  JOIN = "), qtIdent(GetJoinFunction()));
        if (GetHashJoins())  sql += wxT(",\n  HASHES");
        if (!leftSortOperator.IsNull() || !rightSortOperator.IsNull() ||
            !lessOperator.IsNull() || !greaterOperator.IsNull())
            sql += wxT(",\n  MERGES");
        AppendIfFilled(sql, wxT(",\n  SORT1 = "), GetLeftSortOperator());
        AppendIfFilled(sql, wxT(",\n  SORT2 = "), GetRightSortOperator());
        AppendIfFilled(sql, wxT(",\n  LTCMP = "), GetLessOperator());
        AppendIfFilled(sql, wxT(",\n  GTCMP = "), GetGreaterOperator());
        sql += wxT(");\n");

	if (!GetComment().IsNull())
	    sql += wxT("COMMENT ON OPERATOR ") + GetFullIdentifier()
	    + wxT("(") + GetOperands() + wxT(") IS ")
		+ qtString(GetComment()) + wxT(";\n");
    }

    return sql;
}


wxString pgOperator::GetOperands() const
{
    wxString sql;
    if (GetLeftType().IsEmpty())
	sql = wxT("NONE");
    else
	sql = qtIdent(GetLeftType());
    sql += wxT(", ");

    if (GetRightType().IsEmpty())
	sql += wxT("NONE");
    else
	sql += qtIdent(GetRightType());

    return sql;
}
wxString pgOperator::GetFullName() const
{
    if (leftType.IsEmpty() || rightType.IsEmpty())
        return GetName() + wxT(" (") + leftType + rightType + wxT(")");
    else
        return GetName() + wxT(" (") + leftType + wxT(", ") + rightType + wxT(")");
}


void pgOperator::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (properties)
    {
        CreateListColumns(properties);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("OID"), GetOid());
        properties->AppendItem(_("Owner"), GetOwner());
        properties->AppendItem(_("Kind"), GetKind());
        if (!leftType.IsNull())
            properties->AppendItem(_("Left type"), GetLeftType());
        if (!rightType.IsNull())
            properties->AppendItem(_("Right type"), GetRightType());
        properties->AppendItem(_("Result type"), GetResultType());
        properties->AppendItem(_("Operator function"), GetOperatorFunction());
        properties->AppendItem(_("Commutator"), GetCommutator());
        properties->AppendItem(_("Negator"), GetNegator());
        properties->AppendItem(_("Join function"), GetJoinFunction());
        properties->AppendItem(_("Restrict function"), GetRestrictFunction());
        properties->AppendItem(_("Left Sort operator"), GetLeftSortOperator());
        properties->AppendItem(_("Right Sort operator"), GetRightSortOperator());
        properties->AppendItem(_("Less Than operator"), GetLessOperator());
        properties->AppendItem(_("Greater than operator"), GetGreaterOperator());
        properties->AppendItem(_("Supports hash?"), GetHashJoins());
        properties->AppendItem(_("System operator?"), GetSystemObject());
        properties->AppendItem(_("Comment"), GetComment());
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
    pgOperator *oper=0;

    pgSet *operators= collection->GetDatabase()->ExecuteSet(
        wxT("SELECT op.oid, op.oprname, pg_get_userbyid(op.oprowner) as opowner, op.oprkind, op.oprcanhash,\n")
        wxT("       op.oprleft, op.oprright, lt.typname as lefttype, rt.typname as righttype, et.typname as resulttype,\n")
        wxT("       co.oprname as compop, ne.oprname as negop, lso.oprname as leftsortop, rso.oprname as rightsortop,\n")
        wxT("       lco.oprname as lscmpop, gco.oprname as gtcmpop,\n")
        wxT("       op.oprcode as operproc, op.oprjoin as joinproc, op.oprrest as restrproc, description\n")
        wxT("  FROM pg_operator op\n")
        wxT("  LEFT OUTER JOIN pg_type lt ON lt.oid=op.oprleft\n")
        wxT("  LEFT OUTER JOIN pg_type rt ON rt.oid=op.oprright\n")
        wxT("  JOIN pg_type et on et.oid=op.oprresult\n")
        wxT("  LEFT OUTER JOIN pg_operator co ON co.oid=op.oprcom\n")
        wxT("  LEFT OUTER JOIN pg_operator ne ON ne.oid=op.oprnegate\n")
        wxT("  LEFT OUTER JOIN pg_operator lso ON lso.oid=op.oprlsortop\n")
        wxT("  LEFT OUTER JOIN pg_operator rso ON rso.oid=op.oprrsortop\n")
        wxT("  LEFT OUTER JOIN pg_operator lco ON lco.oid=op.oprltcmpop\n")
        wxT("  LEFT OUTER JOIN pg_operator gco ON gco.oid=op.oprgtcmpop\n")
        wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=op.oid\n")
        wxT(" WHERE op.oprnamespace = ") + collection->GetSchema()->GetOidStr() 
        + restriction + wxT("\n")
        wxT(" ORDER BY op.oprname"));

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
            oper->iSetLeftTypeOid(operators->GetOid(wxT("oprleft")));
            oper->iSetRightTypeOid(operators->GetOid(wxT("oprright")));
            oper->iSetResultType(operators->GetVal(wxT("resulttype")));
            oper->iSetOperatorFunction(operators->GetVal(wxT("operproc")));
	    wxString tmp=operators->GetVal(wxT("joinproc"));
	    if (tmp != wxT("-"))
		oper->iSetJoinFunction(tmp);
	    tmp = operators->GetVal(wxT("restrproc"));
	    if (tmp != wxT("-"))
		oper->iSetRestrictFunction(tmp);
            oper->iSetLeftSortOperator(operators->GetVal(wxT("leftsortop")));
            oper->iSetRightSortOperator(operators->GetVal(wxT("rightsortop")));
            oper->iSetLessOperator(operators->GetVal(wxT("lscmpop")));
            oper->iSetGreaterOperator(operators->GetVal(wxT("gtcmpop")));
            oper->iSetCommutator(operators->GetVal(wxT("compop")));
            oper->iSetNegator(operators->GetVal(wxT("negop")));
            wxString kind=operators->GetVal(wxT("oprkind"));
            oper->iSetKind(kind.IsSameAs(wxT("b")) ? wxT("infix") :
                           kind.IsSameAs(wxT("l")) ? wxT("prefix") :
                           kind.IsSameAs(wxT("r")) ? wxT("postfix") : wxT("unknown"));
            oper->iSetHashJoins(operators->GetBool(wxT("oprcanhash")));

            if (browser)
            {
                collection->AppendBrowserItem(browser, oper);
			    operators->MoveNext();
            }
            else
                break;
        }

		delete operators;
    }
    return oper;
}
