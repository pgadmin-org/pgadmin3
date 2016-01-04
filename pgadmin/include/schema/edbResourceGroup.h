//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// edbResourceGroup.h - Resource Group (only used for PPAS 9.4)
//
//////////////////////////////////////////////////////////////////////////

#ifndef EDBRESOURCEGROUP_H
#define EDBRESOURCEGROUP_H

#include "pgServer.h"

class edbResourceGroupFactory : public pgServerObjFactory
{
public:
	edbResourceGroupFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);
};

extern edbResourceGroupFactory resourceGroupFactory;


// Class declarations
class edbResourceGroup : public pgServerObject
{
public:
	edbResourceGroup(const wxString &newName = wxT(""));
	~edbResourceGroup();
	wxString GetTranslatedMessage(int kindOfMessage) const;

	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);

	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
	wxString GetSql(ctlTree *browser);
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

	double GetCPURateLimit() const
	{
		return cpuRateLimit;
	}
	void iSetCPURateLimit(const double b)
	{
		cpuRateLimit = b;
	}
	double GetDirtyRateLimit() const
	{
		return dirtyRateLimit;
	}
	void iSetDirtyRateLimit(const double b)
	{
		dirtyRateLimit = b;
	}

private:
	double cpuRateLimit, dirtyRateLimit;
};

class edbResourceGroupCollection : public pgServerObjCollection
{
public:
	edbResourceGroupCollection(pgaFactory *factory, pgServer *sv);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

#endif
