//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgIndexConstraint.h PostgreSQL Index Constraint: PK, Unique
//
//////////////////////////////////////////////////////////////////////////


#ifndef __PG_INDEXCONSTRAINT
#define __PG_INDEXCONSTRAINT

#include "pgIndex.h"

class pgIndexConstraint : public pgIndex
{
public:
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0);
    bool DropObject(wxFrame *frame, wxTreeCtrl *browser);
    wxString GetDefinition();
    wxString GetCreate();
    wxString GetSql(wxTreeCtrl *browser);
    wxString GetHelpPage(bool forCreate) const { return wxT("sql-altertable"); }

protected:
    pgIndexConstraint(pgSchema *newSchema, const wxString& newName, int type)
        : pgIndex(newSchema, newName, type) {}
};


class pgPrimaryKey : public pgIndexConstraint
{
public:
    pgPrimaryKey(pgSchema *newSchema, const wxString& newName = wxT(""))
        : pgIndexConstraint(newSchema, newName, PG_PRIMARYKEY) {}

    bool CanCreate() { return false; }
    int GetIcon() { return PGICON_PRIMARYKEY; }
    static pgObject *ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &where=wxT(""));
};

class pgUnique : public pgIndexConstraint
{
public:
    pgUnique(pgSchema *newSchema, const wxString& newName = wxT(""))
        : pgIndexConstraint(newSchema, newName, PG_UNIQUE) {}

    int GetIcon() { return PGICON_UNIQUE; }
    static pgObject *ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &where=wxT(""));
};


#endif
