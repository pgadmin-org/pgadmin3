//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgSchema.h PostgreSQL Schema
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGSCHEMA_H
#define PGSCHEMA_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgObject.h"
#include "pgServer.h"
#include "pgDatabase.h"

class pgCollection;

class pgSchema : public pgObject
{
public:
    pgSchema(const wxString& newName = wxString(""));
    ~pgSchema();
    void SetDatabase(pgDatabase *newDatabase) { database = newDatabase; }
    pgDatabase *GetDatabase() const {return database; }

    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0);
    static void ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane);
    wxString GetSql(wxTreeCtrl *browser);
    long GetSchemaTyp() const { return schemaTyp; }
    void iSetSchemaTyp(const long l) { schemaTyp=l; }
    bool GetSystemObject() const { return schemaTyp < 1; }

private:
    pgDatabase *database;
    long schemaTyp; // 0: System 1: temporär 2: normal
};

#endif