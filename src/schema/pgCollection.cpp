//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgCollection.cpp - Simple object for use with 'collection' nodes
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgSchema.h"


pgCollection::pgCollection(pgaFactory *factory)
: pgObject(*factory)
{ 
    wxLogInfo(wxT("Creating a pgCollection object")); 
	job=0;
    schema=0;
    database=0;
    server= 0;
}


pgCollection::~pgCollection()
{
    wxLogInfo(wxT("Destroying a pgCollection object"));
}


bool pgCollection::IsCollectionFor(pgObject *obj)
{
    if (!obj)
        return false;
    pgaFactory *f=obj->GetFactory();
    if (!f)
        return false;
    return GetFactory() == f->GetCollectionFactory();
}


bool pgCollection::IsCollectionForType(int objType)
{
    if (GetFactory())
    {
        pgaFactory *f=pgaFactory::GetFactory(objType);
        return (f && f->GetCollectionFactory() == GetFactory());
    }
    return (GetType() == objType-1);
}


void pgCollection::ShowList(ctlTree *browser, ctlListView *properties)
{
    ShowList(((pgaCollectionFactory*)GetFactory())->GetItemTypeName(), browser, properties);
}


void pgCollection::ShowList(const wxString& name, ctlTree *browser, ctlListView *properties)
{
    if (properties)
    {
        // Display the properties.
        wxCookieType cookie;
        pgObject *data;

        // Setup listview
        CreateListColumns(properties, wxGetTranslation(name), _("Comment"));

        wxTreeItemId item = browser->GetFirstChild(GetId(), cookie);
        long pos=0;
        while (item)
        {
            data = browser->GetObject(item);
            if (IsCollectionFor(data))
            {
                properties->InsertItem(pos, data->GetFullName(), data->GetIconId());
                properties->SetItem(pos, 1, data->GetComment());
            }
            // Get the next item
            item = browser->GetNextChild(GetId(), cookie);
            pos++;
        }
    }
}



void pgCollection::UpdateChildCount(ctlTree *browser, int substract)
{
    wxString label;
    label.Printf(wxString(wxGetTranslation(GetName())) + wxT(" (%d)"), browser->GetChildrenCount(GetId(), false) -substract);
    browser->SetItemText(GetId(), label);
}


int pgCollection::GetIconId()
{
    pgaFactory *objFactory=pgaFactory::GetFactory(GetType());
    if (objFactory)
        return objFactory->GetIconId();
    return 0;
}


pgObject *pgCollection::FindChild(ctlTree *browser, int index)
{
    wxCookieType cookie;
    pgObject *data;

    wxTreeItemId item = browser->GetFirstChild(GetId(), cookie);
    long pos=0;
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
