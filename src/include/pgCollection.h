//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
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
#include "pgServer.h"
#include "pgDatabase.h"
#include "pgSchema.h"

// Class declarations
class pgCollection : public pgObject
{
public:
    pgCollection(int newType = PG_NONE);
    ~pgCollection();
    virtual bool IsCollection() const { return true; }

    pgServer *GetServer() const { return server; }
    void SetServer(pgServer *newServer) { server = newServer; }
    pgDatabase *GetDatabase() const { return database; }
    void SetDatabase(pgDatabase *newDatabase) { database = newDatabase; }
    pgSchema *GetSchema() const { return schema; }
    void SetSchema(pgSchema *newSchema) { schema = newSchema; }
    void SetInfo(pgServer *newServer, pgDatabase *newDatabase, pgSchema *newSchema)
        { server = newServer; database = newDatabase; schema = newSchema; }
    void SetInfo(pgCollection *collection) 
        { SetInfo(collection->GetServer(), collection->GetDatabase(), collection->GetSchema()); }
    void SetInfo(pgSchema *schema)
        { SetInfo(schema->GetDatabase()->GetServer(), schema->GetDatabase(), schema); }

    int GetIcon();
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0);
    void ShowList(const wxString& name, wxTreeCtrl *browser, wxListCtrl *properties);
    void UpdateChildCount(wxTreeCtrl *browser, int substract=0);
    void SetSql(wxTreeCtrl *browser, ctlSQLBox *sqlPane, const int index);
    bool CanCreate();

private:
    pgServer *server;
    pgDatabase *database;
    pgSchema *schema;
};


class pgServers: public pgCollection
{
public:
    pgServers() : pgCollection(PG_SERVERS) {}
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0) {};
};


#endif
