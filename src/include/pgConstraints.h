//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgConstraints.h - Constraint collection
//
//////////////////////////////////////////////////////////////////////////


#ifndef __CONSTRAINTS_H
#define __CONSTRAINTS_H

#include "pgCollection.h"

class pgConstraints : public pgCollection
{
public:
    pgConstraints();
    ~pgConstraints();
    int GetIcon() { return PGICON_CONSTRAINT; }
    bool IsCollectionForType(int objType);
    bool CanCreate() { return false; };

    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane);
};


#endif