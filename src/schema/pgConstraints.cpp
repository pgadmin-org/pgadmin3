//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgConstraint.cpp - Constraint collection
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>


#include "pgAdmin3.h"
#include "pgTable.h"
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


wxMenu *pgConstraints::GetNewMenu()
{
    if (!GetSchema()->GetCreatePrivilege())
        return 0;

    wxMenu *menu=new wxMenu();
    if (table->GetPrimaryKey().IsEmpty())
        AppendMenu(menu, PG_PRIMARYKEY);
    AppendMenu(menu, PG_FOREIGNKEY);
    AppendMenu(menu, PG_UNIQUE);
    AppendMenu(menu, PG_CHECK);
    return menu;
}

bool pgConstraints::IsCollectionForType(int objType)
{
    switch (objType)
    {
        case PG_FOREIGNKEY:
        case PG_PRIMARYKEY:
        case PG_UNIQUE:
        case PG_CHECK:
            return true;
        default:
            return false;
    }
}


void pgConstraints::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (browser->GetChildrenCount(GetId(), FALSE) == 0)
    {
        wxTreeItemId id=browser->GetItemParent(GetId());
        wxASSERT(id);
        table = (pgTable*)browser->GetItemData(id);
        wxASSERT(table && table->GetType() == PG_TABLE);

        pgPrimaryKey::ReadObjects(this, browser);
        pgForeignKey::ReadObjects(this, browser);
        pgUnique::ReadObjects(this, browser);
        pgCheck::ReadObjects(this, browser);
    }
    UpdateChildCount(browser);
    if (properties)
        ShowList(wxT("Constraints"), browser, properties);
}

