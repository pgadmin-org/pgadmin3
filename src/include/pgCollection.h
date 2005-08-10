//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgCollection.h - Simple object for use with 'collection' nodes
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGCOLLECTION_H
#define PGCOLLECTION_H

// wxWindows headers
#include <wx/wx.h>

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
    pgCollection(pgaFactory &factory);
    pgCollection(int newType, pgServer *sv);
    pgCollection(int newType, pgDatabase *db);
    pgCollection(int newType, pgSchema *sch);
	pgCollection(int newType, pgaJob *jb);
    ~pgCollection();
    virtual bool IsCollection() const { return true; }
    virtual bool IsCollectionForType(int objType);

    pgServer *GetServer() const { return server; }
    pgDatabase *GetDatabase() const { return database; }
    pgSchema *GetSchema() const { return schema; }
	pgaJob *GetJob() const { return job; }

    int GetIconId();
    pgaFactory *GetItemFactory() { if (factory) return ((pgaCollectionFactory*)factory)->GetItemFactory(); }
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    void ShowList(const wxString& name, wxTreeCtrl *browser, ctlListView *properties);
    void ShowList(wxTreeCtrl *browser, ctlListView *properties);
    void UpdateChildCount(wxTreeCtrl *browser, int substract=0);
    pgObject *FindChild(wxTreeCtrl *browser, const int index);
    bool CanCreate();

protected:
    pgServer *server;
    pgDatabase *database;
    pgSchema *schema;
	pgaJob *job;
};


class pgServers: public pgCollection
{
public:
    pgServers() : pgCollection(PG_SERVERS, (pgServer*)0) {}
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0) {};
};


#endif
