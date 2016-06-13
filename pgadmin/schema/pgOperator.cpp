//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgOperator.cpp - Operator class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgOperator.h"


pgOperator::pgOperator(pgSchema *newSchema, const wxString &newName)
	: pgSchemaObject(newSchema, operatorFactory, newName)
{
}

pgOperator::~pgOperator()
{
}

wxString pgOperator::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on operator");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing operator");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop operator \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop operator \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop operator cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop operator?");
			break;
		case PROPERTIESREPORT:
			message = _("Operator properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Operator properties");
			break;
		case DDLREPORT:
			message = _("Operator DDL report");
			message += wxT(" - ") + GetName();;
			break;
		case DDL:
			message = _("Operator DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("Operator dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Operator dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Operator dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Operator dependents");
			break;
	}

	return message;
}

bool pgOperator::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("DROP OPERATOR ") + this->GetSchema()->GetQuotedIdentifier() + wxT(".") + this->GetIdentifier();

	if (GetLeftType().Length() > 0)
		sql += wxT(" (") + qtTypeIdent(GetLeftType());
	else
		sql += wxT(") (NONE");

	if (GetRightType().Length() > 0)
		sql += wxT(", ") + qtTypeIdent(GetLeftType()) + wxT(")");
	else
		sql += wxT(", NONE)");

	if (cascaded)
		sql += wxT(" CASCADE");

	return GetDatabase()->ExecuteVoid(sql);
}


wxString pgOperator::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Operator: ") + GetQuotedFullIdentifier() + wxT("(") + GetOperands() + wxT(")\n\n")
		      + wxT("-- DROP OPERATOR ") + GetQuotedFullIdentifier()
		      + wxT("(") + GetOperands() + wxT(");\n\n")
		      wxT("CREATE OPERATOR ") + GetQuotedFullIdentifier()
		      + wxT("(\n  PROCEDURE = ") + GetOperatorFunction();
		AppendIfFilled(sql, wxT(",\n  LEFTARG = "), qtTypeIdent(GetLeftType()));
		AppendIfFilled(sql, wxT(",\n  RIGHTARG = "), qtTypeIdent(GetRightType()));
		AppendIfFilled(sql, wxT(",\n  COMMUTATOR = "), GetCommutator());
		AppendIfFilled(sql, wxT(",\n  NEGATOR = "), GetNegator());
		AppendIfFilled(sql, wxT(",\n  RESTRICT = "), GetRestrictFunction());
		AppendIfFilled(sql, wxT(",\n  JOIN = "), GetJoinFunction());
		if (GetHashJoins()) sql += wxT(",\n  HASHES");
		if (GetMergeJoins()) sql += wxT(",\n  MERGES");

		if (!GetDatabase()->BackendMinimumVersion(8, 3))
		{
			AppendIfFilled(sql, wxT(",\n  SORT1 = "), GetLeftSortOperator());
			AppendIfFilled(sql, wxT(",\n  SORT2 = "), GetRightSortOperator());
			AppendIfFilled(sql, wxT(",\n  LTCMP = "), GetLessOperator());
			AppendIfFilled(sql, wxT(",\n  GTCMP = "), GetGreaterOperator());
		}

		sql += wxT(");\n");

		if (!GetComment().IsNull())
			sql += wxT("COMMENT ON OPERATOR ") + GetQuotedFullIdentifier()
			       + wxT("(") + GetOperands() + wxT(") IS ")
			       + qtDbString(GetComment()) + wxT(";\n");
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
wxString pgOperator::GetFullName()
{
	if (leftType.IsEmpty() || rightType.IsEmpty())
		return GetName() + wxT(" (") + leftType + rightType + wxT(")");
	else
		return GetName() + wxT(" (") + leftType + wxT(", ") + rightType + wxT(")");
}


void pgOperator::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
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

		if (!GetDatabase()->BackendMinimumVersion(8, 3))
		{
			properties->AppendItem(_("Left Sort operator"), GetLeftSortOperator());
			properties->AppendItem(_("Right Sort operator"), GetRightSortOperator());
			properties->AppendItem(_("Less Than operator"), GetLessOperator());
			properties->AppendItem(_("Greater than operator"), GetGreaterOperator());
		}

		properties->AppendYesNoItem(_("Supports hash?"), GetHashJoins());
		properties->AppendYesNoItem(_("Supports merge?"), GetMergeJoins());
		properties->AppendYesNoItem(_("System operator?"), GetSystemObject());
		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
	}
}



pgObject *pgOperator::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *oper = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		oper = operatorFactory.CreateObjects(coll, 0, wxT("\n   AND op.oid=") + GetOidStr());

	return oper;
}


//////////////////////////////////////////////////////


pgOperatorCollection::pgOperatorCollection(pgaFactory *factory, pgSchema *sch)
	: pgSchemaObjCollection(factory, sch)
{
}


wxString pgOperatorCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on operators");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing operators");
			break;
		case OBJECTSLISTREPORT:
			message = _("Operators list report");
			break;
	}

	return message;
}


//////////////////////////////////////////////////////


pgObject *pgOperatorFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	pgOperator *oper = 0;

	pgSet *operators;
	if (collection->GetDatabase()->BackendMinimumVersion(8, 3))
	{
		operators = collection->GetDatabase()->ExecuteSet(
		                wxT("SELECT op.oid, op.oprname, pg_get_userbyid(op.oprowner) as opowner, op.oprkind, op.oprcanhash, op.oprcanmerge,\n")
		                wxT("       op.oprleft, op.oprright, lt.typname as lefttype, rt.typname as righttype, et.typname as resulttype,\n")
		                wxT("       co.oprname as compop, ne.oprname as negop,\n")
		                wxT("       op.oprcode as operproc, op.oprjoin as joinproc, op.oprrest as restrproc, description\n")
		                wxT("  FROM pg_operator op\n")
		                wxT("  LEFT OUTER JOIN pg_type lt ON lt.oid=op.oprleft\n")
		                wxT("  LEFT OUTER JOIN pg_type rt ON rt.oid=op.oprright\n")
		                wxT("  JOIN pg_type et on et.oid=op.oprresult\n")
		                wxT("  LEFT OUTER JOIN pg_operator co ON co.oid=op.oprcom\n")
		                wxT("  LEFT OUTER JOIN pg_operator ne ON ne.oid=op.oprnegate\n")
		                wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=op.oid AND des.classoid='pg_operator'::regclass)\n")
		                wxT(" WHERE op.oprnamespace = ") + collection->GetSchema()->GetOidStr()
		                + restriction + wxT("\n")
		                wxT(" ORDER BY op.oprname"));
	}
	else
	{
		operators = collection->GetDatabase()->ExecuteSet(
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
		                wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=op.oid AND des.classoid='pg_operator'::regclass)\n")
		                wxT(" WHERE op.oprnamespace = ") + collection->GetSchema()->GetOidStr()
		                + restriction + wxT("\n")
		                wxT(" ORDER BY op.oprname"));
	}

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

			wxString tmp = operators->GetVal(wxT("joinproc"));
			if (tmp != wxT("-"))
				oper->iSetJoinFunction(tmp);
			tmp = operators->GetVal(wxT("restrproc"));
			if (tmp != wxT("-"))
				oper->iSetRestrictFunction(tmp);

			if (!collection->GetDatabase()->BackendMinimumVersion(8, 3))
			{
				oper->iSetLeftSortOperator(operators->GetVal(wxT("leftsortop")));
				oper->iSetRightSortOperator(operators->GetVal(wxT("rightsortop")));
				oper->iSetLessOperator(operators->GetVal(wxT("lscmpop")));
				oper->iSetGreaterOperator(operators->GetVal(wxT("gtcmpop")));
			}

			oper->iSetCommutator(operators->GetVal(wxT("compop")));
			oper->iSetNegator(operators->GetVal(wxT("negop")));
			wxString kind = operators->GetVal(wxT("oprkind"));
			oper->iSetKind(kind.IsSameAs(wxT("b")) ? wxT("infix") :
			               kind.IsSameAs(wxT("l")) ? wxT("prefix") :
			               kind.IsSameAs(wxT("r")) ? wxT("postfix") : wxT("unknown"));
			oper->iSetHashJoins(operators->GetBool(wxT("oprcanhash")));

			if (!collection->GetDatabase()->BackendMinimumVersion(8, 3))
				oper->iSetMergeJoins(!oper->GetLeftSortOperator().IsNull() || !oper->GetRightSortOperator().IsNull() ||
				                     !oper->GetLessOperator().IsNull() || !oper->GetGreaterOperator().IsNull());
			else
				oper->iSetMergeJoins(operators->GetBool(wxT("oprcanmerge")));


			if (browser)
			{
				browser->AppendObject(collection, oper);
				operators->MoveNext();
			}
			else
				break;
		}

		delete operators;
	}
	return oper;
}


#include "images/operator.pngc"
#include "images/operators.pngc"

pgOperatorFactory::pgOperatorFactory()
	: pgSchemaObjFactory(__("Operator"), __("New Operator..."), __("Create a new Operator."), operator_png_img)
{
}


pgCollection *pgOperatorFactory::CreateCollection(pgObject *obj)
{
	return new pgOperatorCollection(GetCollectionFactory(), (pgSchema *)obj);
}

pgOperatorFactory operatorFactory;
static pgaCollectionFactory cf(&operatorFactory, __("Operators"), operators_png_img);
