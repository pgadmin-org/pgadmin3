//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgConstraint.cpp - Constraint collection
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>


#include "pgAdmin3.h"
#include "pgConstraints.h"
#include "pgIndexConstraint.h"
#include "pgCheck.h"
#include "pgForeignKey.h"



pgConstraints::pgConstraints(pgSchema *sch)
: pgCollection(PG_CONSTRAINTS, sch)
{ 
    wxLogInfo(wxT("Creating a pgConstraint object")); 
}

pgConstraints::~pgConstraints()
{
    wxLogInfo(wxT("Destroying a pgConstraint object"));
}


bool pgConstraints::IsCollectionForType(int objType)
{
    switch (objType)
    {
        case PG_CHECK:
        case PG_FOREIGNKEY:
        case PG_PRIMARYKEY:
        case PG_UNIQUE:
            return true;
        default:
            return false;
    }
}


void pgConstraints::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (browser->GetChildrenCount(GetId(), FALSE) == 0)
    {
        pgPrimaryKey::ReadObjects(this, browser);
        pgForeignKey::ReadObjects(this, browser);
        pgUnique::ReadObjects(this, browser);
        pgCheck::ReadObjects(this, browser);
    }
    UpdateChildCount(browser);
    if (properties)
        ShowList(wxT("Constraints"), browser, properties);
}

