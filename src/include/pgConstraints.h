//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
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
    int GetIcon() { return PGICON_CONSTRAINT; }
    bool IsCollectionForType(int objType);
    bool CanCreate() { return true; };
    wxString GetHelpPage(bool forCreate) const { return wxT("sql-altertable"); }
    wxMenu *GetNewMenu();

    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane);
    pgTable *table;
};


#endif
