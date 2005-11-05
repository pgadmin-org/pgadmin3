//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
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



pgConstraintCollection::pgConstraintCollection(pgaFactory *factory, pgTable *table)
: pgTableObjCollection(factory, table)
{ 
    wxLogInfo(wxT("Creating a pgConstraintCollection object")); 
}

pgConstraintCollection::~pgConstraintCollection()
{
    wxLogInfo(wxT("Destroying a pgConstraintCollection object"));
}


wxMenu *pgConstraintCollection::GetNewMenu()
{
    if (!GetSchema()->GetCreatePrivilege())
        return 0;

    wxMenu *menu=new wxMenu();
    if (table->GetPrimaryKey().IsEmpty())
        primaryKeyFactory.AppendMenu(menu);
    foreignKeyFactory.AppendMenu(menu);
    uniqueFactory.AppendMenu(menu);
    checkFactory.AppendMenu(menu);
    return menu;
}


void pgConstraintCollection::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (browser->GetChildrenCount(GetId(), false) == 0)
    {
        wxTreeItemId id=browser->GetItemParent(GetId());
        wxASSERT(id);
        table = (pgTable*)browser->GetObject(id);
        wxASSERT(table && table->GetMetaType() == PGM_TABLE);

        primaryKeyFactory.CreateObjects(this, browser);
        foreignKeyFactory.CreateObjects(this, browser);
        uniqueFactory.CreateObjects(this, browser);
        checkFactory.CreateObjects(this, browser);
    }
    UpdateChildCount(browser);
    if (properties)
        ShowList(_("Constraints"), browser, properties);
}



/////////////////////////////

#include "images/constraints.xpm"

pgConstraintFactory::pgConstraintFactory() 
: pgTableObjFactory(__("Constraint"), 0, 0, 0)
{
}

pgCollection *pgConstraintFactory::CreateCollection(pgObject *obj)
{
    return new pgConstraintCollection(GetCollectionFactory(), (pgTable*)obj);
}


pgConstraintFactory constraintFactory;
pgaCollectionFactory constraintCollectionFactory(&constraintFactory, __("Constraints"), constraints_xpm);


