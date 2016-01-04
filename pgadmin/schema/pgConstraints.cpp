//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgConstraint.cpp - Constraint collection
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>


#include "pgAdmin3.h"
#include "schema/pgTable.h"
#include "schema/pgDomain.h"
#include "schema/pgConstraints.h"
#include "schema/pgIndexConstraint.h"
#include "schema/pgCheck.h"
#include "schema/pgForeignKey.h"


pgConstraintCollection::pgConstraintCollection(pgaFactory *factory, pgSchema *schema)
	: pgSchemaObjCollection(factory, schema)
{
}

wxMenu *pgConstraintCollection::GetNewMenu()
{
	if ((table && !table->CanCreate()) || (domain && !domain->CanCreate()))
		return 0;

	wxMenu *menu = new wxMenu();
	if (table)
	{
		if (table->GetPrimaryKey().IsEmpty())
			primaryKeyFactory.AppendMenu(menu);
		foreignKeyFactory.AppendMenu(menu);
		excludeFactory.AppendMenu(menu);
		uniqueFactory.AppendMenu(menu);
	}
	checkFactory.AppendMenu(menu);
	return menu;
}


void pgConstraintCollection::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (browser->GetChildrenCount(GetId(), false) == 0)
	{
		browser->RemoveDummyChild(this);

		wxTreeItemId id = browser->GetItemParent(GetId());
		wxASSERT(id);
		table = NULL;
		domain = NULL;
		if (browser->GetObject(id)->IsCreatedBy(tableFactory))
		{
			table = (pgTable *)browser->GetObject(id);
		}
		if (browser->GetObject(id)->IsCreatedBy(domainFactory))
		{
			domain = (pgDomain *)browser->GetObject(id);
		}

		if (table)
		{
			primaryKeyFactory.CreateObjects(this, browser);
			foreignKeyFactory.CreateObjects(this, browser);
			excludeFactory.CreateObjects(this, browser);
			uniqueFactory.CreateObjects(this, browser);
		}
		checkFactory.CreateObjects(this, browser);
	}
	UpdateChildCount(browser);
	if (properties)
		ShowList(_("Constraints"), browser, properties);
}


wxString pgConstraintCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on constraints");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing constraints");
			break;
		case OBJECTSLISTREPORT:
			message = _("Constraints list report");
			break;
	}

	return message;
}


/////////////////////////////

#include "images/constraints.pngc"

pgConstraintFactory::pgConstraintFactory()
	: pgSchemaObjFactory(__("Constraint"), 0, 0, 0)
{
	metaType = PGM_CONSTRAINT;
}

pgCollection *pgConstraintFactory::CreateCollection(pgObject *obj)
{
	return new pgConstraintCollection(GetCollectionFactory(), (pgSchema *)obj);
}


pgConstraintFactory constraintFactory;
pgaCollectionFactory constraintCollectionFactory(&constraintFactory, __("Constraints"), constraints_png_img);


