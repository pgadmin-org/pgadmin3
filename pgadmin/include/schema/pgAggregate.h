//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgAggregate.h PostgreSQL Aggregate
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGAGGREGATE_H
#define PGAGGREGATE_H

// App headers
#include "pgSchema.h"

class pgCollection;
class pgAggregateFactory : public pgaFactory
{
public:
	pgAggregateFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	pgCollection *CreateCollection(pgObject *obj);
};
extern pgAggregateFactory aggregateFactory;

class pgAggregate : public pgSchemaObject
{
public:
	pgAggregate(pgSchema *newSchema, const wxString &newName = wxT(""));
	~pgAggregate();
	bool CanDropCascaded()
	{
		return GetSchema()->GetMetaType() != PGM_CATALOG;
	}

	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	wxString GetFullName();

	wxArrayString &GetInputTypesArray()
	{
		return inputTypes;
	}
	void iAddInputType(const wxString &s)
	{
		inputTypes.Add(s);
	}
	wxString GetInputTypesList();

	wxString GetStateType()
	{
		return stateType;
	}
	void iSetStateType(const wxString &s)
	{
		stateType = s;
	}
	wxString GetFinalType()
	{
		return finalType;
	}
	void iSetFinalType(const wxString &s)
	{
		finalType = s;
	}
	wxString GetStateFunction()
	{
		return stateFunction;
	}
	void iSetStateFunction(const wxString &s)
	{
		stateFunction = s;
	}
	wxString GetFinalFunction()
	{
		return finalFunction;
	}
	void iSetFinalFunction(const wxString &s)
	{
		finalFunction = s;
	}
	wxString GetInitialCondition()
	{
		return initialCondition;
	}
	void iSetInitialCondition(const wxString &s)
	{
		initialCondition = s;
	}
	wxString GetSortOp()
	{
		return sortOp;
	}
	void iSetSortOp(const wxString &s)
	{
		sortOp = s;
	}
	wxString GetQuotedSortOp()
	{
		return quotedSortOp;
	}
	void iSetQuotedSortOp(const wxString &s)
	{
		quotedSortOp = s;
	}

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

	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
	wxString GetSql(ctlTree *browser);
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

private:
	wxArrayString inputTypes;
	wxString stateType, finalType, sortOp, quotedSortOp,
	         stateFunction, finalFunction, initialCondition;
};

#endif
