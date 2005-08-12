//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgConstraints.h - Constraint collection
//
//////////////////////////////////////////////////////////////////////////


#ifndef __CONSTRAINTS_H
#define __CONSTRAINTS_H

#include "pgCollection.h"

class pgTable;
class pgConstraints : public pgCollection
{
public:
    pgConstraints(pgSchema *sch);
    ~pgConstraints();
    int GetIconId() { return PGICON_CONSTRAINT; }
    bool IsCollectionForType(int objType);
    bool CanCreate() { return true; };
    wxString GetHelpPage(bool forCreate) const { return wxT("pg/sql-altertable"); }
    wxMenu *GetNewMenu();

    void ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane);
    pgTable *table;
};


#endif
