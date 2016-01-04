//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgCollection.cpp - Simple object for use with 'collection' nodes
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "frm/menu.h"
#include "utils/misc.h"
#include "agent/pgaJob.h"
#include "schema/pgSchema.h"
#include "schema/pgTable.h"
#include "frm/frmMain.h"
#include "frm/frmReport.h"

pgCollection::pgCollection(pgaFactory *factory)
	: pgObject(*factory)
{
	um = 0;
	fsrv = 0;
	fdw = 0;
	job = 0;
	schema = 0;
	database = 0;
	server = 0;
}

bool pgCollection::IsCollectionFor(pgObject *obj)
{
	if (!obj)
		return false;
	pgaFactory *f = obj->GetFactory();
	if (!f)
		return false;
	return GetFactory() == f->GetCollectionFactory();
}


bool pgCollection::IsCollectionForType(const int type)
{
	if (GetFactory())
	{
		pgaFactory *f = pgaFactory::GetFactoryByMetaType(type);
		return (f && f->GetCollectionFactory() == GetFactory());
	}
	return false;
}


void pgCollection::ShowList(ctlTree *browser, ctlListView *properties)
{
	ShowList(((pgaCollectionFactory *)GetFactory())->GetItemTypeName(), browser, properties);
}

void pgCollection::ShowList(const wxString &name, ctlTree *browser, ctlListView *properties)
{
	if (properties)
	{
		// Display the properties.
		wxCookieType cookie;
		pgObject *data;

		// Setup listview
		CreateList3Columns(properties, wxGetTranslation(name), _("Owner"), _("Comment"));

		wxTreeItemId item = browser->GetFirstChild(GetId(), cookie);
		long pos = 0;
		while (item)
		{
			data = browser->GetObject(item);
			if (IsCollectionFor(data))
			{
				properties->InsertItem(pos, data->GetFullName(), data->GetIconId());
				properties->SetItem(pos, 1, data->GetOwner());
				properties->SetItem(pos, 2, firstLineOnly(data->GetComment()));
				pos++;
			}
			// Get the next item
			item = browser->GetNextChild(GetId(), cookie);
		}
	}
}

void pgCollection::UpdateChildCount(ctlTree *browser, int substract)
{
	wxString label;
	label.Printf(wxString(wxGetTranslation(GetName())) + wxT(" (%d)"), (int)browser->GetChildrenCount(GetId(), false) - substract);
	browser->SetItemText(GetId(), label);
}


int pgCollection::GetIconId()
{
	pgaFactory *objFactory = pgaFactory::GetFactory(GetType());
	if (objFactory)
		return objFactory->GetIconId();
	return 0;
}


pgObject *pgCollection::FindChild(ctlTree *browser, const int index)
{
	wxCookieType cookie;
	pgObject *data;

	wxTreeItemId item = browser->GetFirstChild(GetId(), cookie);
	long pos = 0;
	while (item && index >= 0)
	{
		data = browser->GetObject(item);
		if (data && IsCollectionFor(data))
		{
			if (index == pos)
				return data;

			pos++;
		}
		item = browser->GetNextChild(GetId(), cookie);
	}
	return 0;
}



void pgCollection::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (browser->GetChildrenCount(GetId(), false) == 0)
	{
		if (GetFactory())
			GetFactory()->CreateObjects(this, browser);
	}

	UpdateChildCount(browser);
	if (properties)
	{
		ShowList(browser, properties);
	}
}
