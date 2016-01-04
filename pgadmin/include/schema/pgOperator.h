//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgOperator.h PostgreSQL Operator
//
//////////////////////////////////////////////////////////////////////////

#ifndef PG_OPERATOR_H
#define PG_OPERATOR_H


#include "pgSchema.h"


class pgOperatorFactory : public pgSchemaObjFactory
{
public:
	pgOperatorFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);
};
extern pgOperatorFactory operatorFactory;

class pgOperator : public pgSchemaObject
{
public:
	pgOperator(pgSchema *newSchema, const wxString &newName = wxT(""));
	~pgOperator();

	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	virtual wxString GetQuotedIdentifier() const
	{
		return GetName();
	}
	bool CanDropCascaded()
	{
		return GetSchema()->GetMetaType() != PGM_CATALOG;
	}

	wxString GetFullName();
	wxString GetOperands() const;
	wxString GetLeftType() const
	{
		return leftType;
	}
	void iSetLeftType(const wxString &s)
	{
		leftType = s;
	}
	wxString GetRightType() const
	{
		return rightType;
	}
	void iSetRightType(const wxString &s)
	{
		rightType = s;
	}
	OID GetLeftTypeOid() const
	{
		return leftTypeOid;
	}
	void iSetLeftTypeOid(const OID o)
	{
		leftTypeOid = o;
	}
	OID GetRightTypeOid() const
	{
		return rightTypeOid;
	}
	void iSetRightTypeOid(const OID o)
	{
		rightTypeOid = o;
	}
	wxString GetResultType()
	{
		return resultType;
	}
	void iSetResultType(const wxString &s)
	{
		resultType = s;
	}
	wxString GetOperatorFunction() const
	{
		return operatorFunction;
	}
	void iSetOperatorFunction(const wxString &s)
	{
		operatorFunction = s;
	}
	wxString GetJoinFunction() const
	{
		return joinFunction;
	}
	void iSetJoinFunction(const wxString &s)
	{
		joinFunction = s;
	}
	wxString GetRestrictFunction() const
	{
		return restrictFunction;
	}
	void iSetRestrictFunction(const wxString &s)
	{
		restrictFunction = s;
	}
	wxString GetCommutator() const
	{
		return commutator;
	}
	void iSetCommutator(const wxString &s)
	{
		commutator = s;
	}
	wxString GetNegator() const
	{
		return negator;
	}
	void iSetNegator(const wxString &s)
	{
		negator = s;
	}
	wxString GetKind() const
	{
		return kind;
	}
	void iSetKind(const wxString &s)
	{
		kind = s;
	}
	wxString GetLeftSortOperator() const
	{
		return leftSortOperator;
	}
	void iSetLeftSortOperator(const wxString &s)
	{
		leftSortOperator = s;
	}
	wxString GetRightSortOperator() const
	{
		return  rightSortOperator;
	}
	void iSetRightSortOperator(const wxString &s)
	{
		rightSortOperator = s;
	}
	wxString GetLessOperator() const
	{
		return lessOperator;
	}
	void iSetLessOperator(const wxString &s)
	{
		lessOperator = s;
	}
	wxString GetGreaterOperator() const
	{
		return  greaterOperator;
	}
	void iSetGreaterOperator(const wxString &s)
	{
		greaterOperator = s;
	}
	bool GetHashJoins() const
	{
		return hashJoins;
	}
	void iSetHashJoins(bool b)
	{
		hashJoins = b;
	}
	bool GetMergeJoins() const
	{
		return mergeJoins;
	}
	void iSetMergeJoins(bool b)
	{
		mergeJoins = b;
	}

	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
	wxString GetSql(ctlTree *browser);
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

	bool HasStats()
	{
		return false;
	}
	bool HasDepends()
	{
		return true;
	}
	bool HasReferences()
	{
		return true;
	}

private:
	wxString leftType, rightType, resultType,
	         operatorFunction, joinFunction, restrictFunction,
	         commutator, negator, kind,
	         leftSortOperator, rightSortOperator, lessOperator, greaterOperator;
	OID leftTypeOid, rightTypeOid;
	bool hashJoins, mergeJoins;
};

class pgOperatorCollection : public pgSchemaObjCollection
{
public:
	pgOperatorCollection(pgaFactory *factory, pgSchema *sch);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

#endif
