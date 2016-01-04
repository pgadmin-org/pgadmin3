//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgAggregate.h - Aggregate class
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGAGGREGATE_H
#define PGAGGREGATE_H

#include "pgSchema.h"

class pgCollection;
class pgAggregateFactory : public pgSchemaObjFactory
{
public:
	pgAggregateFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);
};
extern pgAggregateFactory aggregateFactory;

class pgAggregate : public pgSchemaObject
{
public:
	pgAggregate(pgSchema *newSchema, const wxString &newName = wxT(""));

	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
	wxString GetSql(ctlTree *browser);
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

	wxString GetQuotedFullName();
	wxString GetFullName();

	bool CanDropCascaded()
	{
		return GetSchema()->GetMetaType() != PGM_CATALOG;
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

private:
	wxArrayString inputTypes;
	wxString stateType, finalType, sortOp, quotedSortOp,
	         stateFunction, finalFunction, initialCondition;
};

class pgAggregateCollection : public pgSchemaObjCollection
{
public:
	pgAggregateCollection(pgaFactory *factory, pgSchema *sch);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

#endif
