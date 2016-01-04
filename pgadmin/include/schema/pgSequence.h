//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgSequence.h PostgreSQL Sequence
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGSEQUENCE_H
#define PGSEQUENCE_H


#include "pgSchema.h"

class pgSequenceFactory : public pgSchemaObjFactory
{
public:
	pgSequenceFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);
	int GetReplicatedIconId()
	{
		return replicatedIconId;
	}
private:
	int replicatedIconId;
};
extern pgSequenceFactory sequenceFactory;

class pgSequence : public pgSchemaObject
{
public:
	pgSequence(pgSchema *newSchema, const wxString &newName = wxT(""));
	~pgSequence();
	wxString GetTranslatedMessage(int kindOfMessage) const;
	int GetIconId();

	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	void ShowStatistics(frmMain *form, ctlListView *statistics);
	bool CanDropCascaded()
	{
		return !GetSystemObject() && pgSchemaObject::CanDrop();
	}

	void UpdateValues();
	wxLongLong GetLastValue() const
	{
		return lastValue;
	}
	wxLongLong GetNextValue() const
	{
		return nextValue;
	}
	wxLongLong GetMinValue() const
	{
		return minValue;
	}
	wxLongLong GetMaxValue() const
	{
		return maxValue;
	}
	wxLongLong GetCacheValue() const
	{
		return cacheValue;
	}
	wxLongLong GetIncrement() const
	{
		return increment;
	}
	bool GetCycled() const
	{
		return cycled;
	}
	bool GetCalled() const
	{
		return called;
	}

	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
	wxString GetSql(ctlTree *browser);
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

	bool HasStats()
	{
		return true;
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
	wxLongLong lastValue, nextValue, minValue, maxValue, cacheValue, increment;
	bool cycled, called, isReplicated;
};

class pgSequenceCollection : public pgSchemaObjCollection
{
public:
	pgSequenceCollection(pgaFactory *factory, pgSchema *sch);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

#endif
