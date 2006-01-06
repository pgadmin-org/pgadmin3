//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
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

// Class declarations
class pgCollection : public pgObject
{
public:
    pgCollection(pgaFactory *factory);
    ~pgCollection();
    virtual bool IsCollection() const { return true; }
    bool IsCollectionForType(int objType);
    bool IsCollectionFor(pgObject *obj);

    pgServer *GetServer() const { return server; }
    pgDatabase *GetDatabase() const { return database; }
    pgSchema *GetSchema() const { return schema; }
	pgaJob *GetJob() const { return job; }

    int GetIconId();
    pgaFactory *GetItemFactory() { if (factory) return ((pgaCollectionFactory*)factory)->GetItemFactory(); }
    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    void ShowList(const wxString& name, ctlTree *browser, ctlListView *properties);
    void ShowList(ctlTree *browser, ctlListView *properties);
    void UpdateChildCount(ctlTree *browser, int substract=0);
    pgObject *FindChild(ctlTree *browser, const int index);

protected:
    pgServer *server;
    pgDatabase *database;
    pgSchema *schema;
	pgaJob *job;
};


#endif
