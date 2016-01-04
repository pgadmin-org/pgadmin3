//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgCollection.h - Simple object for use with 'collection' nodes
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGCOLLECTION_H
#define PGCOLLECTION_H

// App headers
#include "pgAdmin3.h"
#include "pgObject.h"

class pgServer;
class pgDatabase;
class pgaJob;
class pgSchema;
class pgForeignDataWrapper;
class pgForeignServer;
class pgUserMapping;

// Class declarations
class pgCollection : public pgObject
{
public:
	pgCollection(pgaFactory *factory);

	virtual bool IsCollection() const
	{
		return true;
	}
	bool IsCollectionForType(const int type);
	bool IsCollectionFor(pgObject *obj);

	pgServer *GetServer() const
	{
		return server;
	}
	pgDatabase *GetDatabase() const
	{
		return database;
	}
	pgSchema *GetSchema() const
	{
		return schema;
	}
	pgForeignDataWrapper *GetForeignDataWrapper() const
	{
		return fdw;
	}
	pgForeignServer *GetForeignServer() const
	{
		return fsrv;
	}
	pgUserMapping *GetUserMapping() const
	{
		return um;
	}
	pgaJob *GetJob() const
	{
		return job;
	}

	int GetIconId();
	pgaFactory *GetItemFactory()
	{
		if (factory) return ((pgaCollectionFactory *)factory)->GetItemFactory();
		else return NULL;
	}
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	void ShowList(const wxString &name, ctlTree *browser, ctlListView *properties);
	void ShowList(ctlTree *browser, ctlListView *properties);
	void UpdateChildCount(ctlTree *browser, int substract = 0);
	pgObject *FindChild(ctlTree *browser, const int index);

	bool HasStats()
	{
		return false;
	}
	bool HasDepends()
	{
		return false;
	}
	bool HasReferences()
	{
		return false;
	}

protected:
	pgServer *server;
	pgDatabase *database;
	pgSchema *schema;
	pgaJob *job;
	pgForeignDataWrapper *fdw;
	pgForeignServer *fsrv;
	pgUserMapping *um;
};


#endif
