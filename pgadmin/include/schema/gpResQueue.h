//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gpResQueue.h - Greenplum Resource Queue
//
//////////////////////////////////////////////////////////////////////////

#ifndef gpResQueue_H
#define gpResQueue_H

#include "pgServer.h"



class gpResQueueFactory : public pgServerObjFactory
{
public:
	gpResQueueFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	pgCollection *CreateCollection(pgObject *obj);
};

extern gpResQueueFactory resQueueFactory;




// Class declarations
class gpResQueue : public pgServerObject
{

protected:
	gpResQueue(pgaFactory &factory, const wxString &newName = wxT(""));

public:
	gpResQueue(const wxString &newName = wxT(""));
	wxString GetTranslatedMessage(int kindOfMessage) const;
	int GetIconId();

	double GetCountLimit() const
	{
		return countlimit;
	}
	double GetCostLimit() const
	{
		return costlimit;
	}
	bool GetOvercommit() const
	{
		return overcommit;
	}
	double GetIgnoreCostLimit() const
	{
		return ignorecostlimit;
	}

	void iSetCountLimit(double newVal)
	{
		countlimit = newVal;
	}
	void iSetCostLimit(double newVal)
	{
		costlimit = newVal;
	}
	void iSetOvercommit(bool newVal)
	{
		overcommit = newVal;
	}
	void iSetIgnoreCostLimit(double newVal)
	{
		ignorecostlimit = newVal;
	}

	// Tree object creation
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);

	// virtual methods
	wxString GetSql(ctlTree *browser);
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);
	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);

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
	wxArrayString queuesIn;
	double countlimit, costlimit, ignorecostlimit;
	bool overcommit;

};

class gpResQueueCollection : public pgServerObjCollection
{
public:
	gpResQueueCollection(pgaFactory *factory, pgServer *sv);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

#endif
