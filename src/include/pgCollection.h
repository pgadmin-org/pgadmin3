//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
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
#include "pgSchema.h"

class pgServer;
class pgDatabase;

// Class declarations
class pgCollection : public pgObject
{
public:
    pgCollection(int newType, pgServer *sv);
    pgCollection(int newType, pgDatabase *db);
    pgCollection(int newType, pgSchema *sch);
    ~pgCollection();
    virtual bool IsCollection() const { return true; }
    virtual bool IsCollectionForType(int objType) { return GetType() == objType-1; }

    pgServer *GetServer() const { return server; }
    pgDatabase *GetDatabase() const { return database; }
    pgSchema *GetSchema() const { return schema; }

    int GetIcon();
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    void ShowStatistics(ctlListView *statistics);
    void ShowList(const wxString& name, wxTreeCtrl *browser, ctlListView *properties);
    void UpdateChildCount(wxTreeCtrl *browser, int substract=0);
    pgObject *FindChild(wxTreeCtrl *browser, const int index);
    bool CanCreate();

private:
    pgServer *server;
    pgDatabase *database;
    pgSchema *schema;
};


class pgServers: public pgCollection
{
public:
    pgServers() : pgCollection(PG_SERVERS, (pgServer*)0) {}
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0) {};
};


#endif
