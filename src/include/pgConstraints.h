//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: pgConstraints.h 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgConstraints.h - Constraint collection
//
//////////////////////////////////////////////////////////////////////////


#ifndef __CONSTRAINTS_H
#define __CONSTRAINTS_H

#include "pgTable.h"


class pgConstraintCollection : public pgTableObjCollection
{
public:
    pgConstraintCollection(pgaFactory *factory, pgTable *table);
    ~pgConstraintCollection();
    wxString GetHelpPage(bool forCreate) const { return wxT("pg/sql-altertable"); }
    bool CanCreate() { return false; }
    wxMenu *GetNewMenu();

    void ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane);
    pgTable *table;
};


class pgConstraintFactory : public pgTableObjFactory
{
public:
    pgConstraintFactory();
    virtual dlgProperty *CreateDialog(class frmMain *,class pgObject *,class pgObject *) { return 0; }
    virtual pgCollection *CreateCollection(pgObject *obj);
};
extern pgConstraintFactory constraintFactory;
extern pgaCollectionFactory constraintCollectionFactory;

#endif
