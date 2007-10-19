//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgCheck.h PostgreSQL Check
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGCHECK_H
#define PGCHECK_H

// App headers
#include "pgTable.h"
#include "pgConstraints.h"



class pgCheckFactory : public pgTableObjFactory
{
public:
    pgCheckFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern pgCheckFactory checkFactory;


class pgCheck : public pgTableObject
{
public:
    pgCheck(pgTable *newTable, const wxString& newName = wxT(""));
    ~pgCheck();

    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);

    wxString GetFkTable() const { return fkTable; }
    void iSetFkTable(const wxString& s) { fkTable=s; }
    wxString GetFkSchema() const { return fkSchema; }
    void iSetFkSchema(const wxString& s) { fkSchema=s; }
    wxString GetDefinition() const { return definition; }
    void iSetDefinition(const wxString& s) { definition=s; }

    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
    wxString GetConstraint();
    wxString GetSql(ctlTree *browser);
    wxString GetHelpPage(bool forCreate) const { return wxT("pg/sql-altertable"); }
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

    bool HasStats() { return false; }
    bool HasDepends() { return true; }
    bool HasReferences() { return true; }

private:
    wxString definition, fkTable, fkSchema;
};

#endif
