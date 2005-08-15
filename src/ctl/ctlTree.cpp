//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id:  $
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// ctlTree.cpp - wxTreeCtrl containing pgObjects
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "ctl/ctlTree.h"

#include "pgObject.h"
#include "pgCollection.h"

ctlTree::ctlTree(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
: wxTreeCtrl(parent, id, pos, size, style)
{
}


void ctlTree::RemoveDummyChild(pgObject *obj)
{
    wxCookieType cookie;
    wxTreeItemId childItem=GetFirstChild(obj->GetId(), cookie);
    if (childItem && !GetItemData(childItem))
    {
        // The child was a dummy item, which will be replaced by the following ShowTreeDetail by true items
        Delete(childItem);
    }
}


wxTreeItemId ctlTree::AppendObject(pgObject *parent, pgObject *object)
{
    wxString label;
    wxTreeItemId item;

    if (object->IsCollection())
        label = object->GetTypeName();
    else
        label = object->GetFullName();
    item = AppendItem(parent->GetId(), label, object->GetIconId(), -1, object);
    if (object->IsCollection())
        object->ShowTreeDetail(this);
    else if (object->WantDummyChild())
        AppendItem(object->GetId(), wxT("Dummy"));

	return item;
}


pgCollection *ctlTree::AppendCollection(pgObject *parent, pgaFactory &factory)
{
    pgCollection *collection=factory.CreateCollection(parent);
    AppendObject(parent, collection);
    return collection;
}
