//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2009, The pgAdmin Development Team
// This software is released under the BSD Licence
//
// ctlTree.cpp - wxTreeCtrl containing pgObjects
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "ctl/ctlTree.h"

#include "schema/pgObject.h"
#include "schema/pgCollection.h"
#include "schema/pgServer.h"

BEGIN_EVENT_TABLE(ctlTree, wxTreeCtrl)
    EVT_CHAR(ctlTree::OnChar)
END_EVENT_TABLE()

int level = 0;

wxTreeItemId ctlTree::FindItem(const wxTreeItemId& item, const wxString& str)
{
    wxTreeItemId resultItem;
    wxTreeItemId currItem = item;
    if (!currItem.IsOk())
        return currItem;
    
    wxString val = GetItemText(currItem);

    // Ignore Dummy Nodes
    if (!(val == wxT("Dummy") && GetItemData(item) == NULL) && val.Lower().StartsWith(str))
    {
        return currItem;
    }
    
    if (HasChildren(currItem))
    {
        wxTreeItemIdValue cookie;
        wxTreeItemId childItem = GetFirstChild(currItem, cookie);
        level++;
        resultItem = FindItem(childItem, str);
        if (resultItem.IsOk())
        {
            level--;
            return resultItem;
        }
        level--;
    }
    
    currItem = GetNextSibling(currItem);
    
    if (currItem.IsOk())
        resultItem = FindItem(currItem, str);

    return resultItem;
}

void ctlTree::OnChar(wxKeyEvent& event)
{
    int keyCode = event.GetKeyCode();
    if ( !event.HasModifiers() &&
         ((keyCode >= '0' && keyCode <= '9') ||
          (keyCode >= 'a' && keyCode <= 'z') ||
          (keyCode >= 'A' && keyCode <= 'Z')))
    {
        wxChar ch = (wxChar)keyCode;
        wxTreeItemId currItem = GetSelection();
        if (!currItem.IsOk())
            return;

        wxTreeItemId matchItem = FindItem(currItem, wxString(ch).Lower());
        if (matchItem.IsOk())
        {
            EnsureVisible(matchItem);
            SelectItem(matchItem);
        }
    }
    else
    {
        event.Skip(true);
    }
}

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


pgObject *ctlTree::GetObject(wxTreeItemId id)
{
    if (id)
        return (pgObject*)GetItemData(id);
    return 0;
}


pgCollection *ctlTree::GetParentCollection(wxTreeItemId id)
{
    pgCollection *coll=(pgCollection*)GetParentObject(id);
    if (coll && coll->IsCollection())
        return coll;
    return 0;
}


void ctlTree::SetItemImage(const wxTreeItemId& item, int image, wxTreeItemIcon which)
{
    wxTreeCtrl::SetItemImage(item, image, which);
    
    wxTreeItemData *data = GetItemData(item);

    // Set the item colour
    if (data)
    {
       if (((pgObject *)data)->GetMetaType() == PGM_SERVER)
       {
           if (!((pgServer *)data)->GetColour().IsEmpty())
               SetItemBackgroundColour(item, wxColour(((pgServer *)data)->GetColour()));
       }
       else if (((pgObject *)data)->GetServer())
       {
           if (!((pgObject *)data)->GetServer()->GetColour().IsEmpty())
               SetItemBackgroundColour(item, wxColour(((pgObject *)data)->GetServer()->GetColour()));
       }
    }
}

wxTreeItemId ctlTree::AppendItem(const wxTreeItemId& parent, const wxString& text, int image, int selImage, wxTreeItemData* data)
{
    wxTreeItemId itm = wxTreeCtrl::AppendItem(parent, text, image, selImage, data); 

    // Set the item colour
    if (data)
    {
        if (((pgObject *)data)->GetMetaType() == PGM_SERVER)
        {
            if (!((pgServer *)data)->GetColour().IsEmpty())
                SetItemBackgroundColour(itm, wxColour(((pgServer *)data)->GetColour()));
        }
        else if (((pgObject *)data)->GetServer())
        {
            if (!((pgObject *)data)->GetServer()->GetColour().IsEmpty())
                SetItemBackgroundColour(itm, wxColour(((pgObject *)data)->GetServer()->GetColour()));
        }
    }

    return itm;
}

wxTreeItemId ctlTree::AppendObject(pgObject *parent, pgObject *object)
{
    wxString label;
    wxTreeItemId item;

    if (object->IsCollection())
        label = object->GetTypeName();
    else
        label = object->GetDisplayName();
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


pgObject *ctlTree::FindObject(pgaFactory &factory, wxTreeItemId parent)
{
    wxCookieType cookie;
    wxTreeItemId item = GetFirstChild(parent, cookie);
    while (item)
    {
        pgObject *obj=(pgObject*)GetItemData(item);
        if (obj && obj->IsCreatedBy(factory))
            return obj;
        item = GetNextChild(parent, cookie);
    }
    return 0;
}


pgCollection *ctlTree::FindCollection(pgaFactory &factory, wxTreeItemId parent)
{
    pgaFactory *cf=factory.GetCollectionFactory();
    if (!cf)
        return 0;

    pgCollection *collection=(pgCollection*)FindObject(*cf, parent);

    if (!collection || !collection->IsCollection())
        return 0;
    return collection;
}


//////////////////////

treeObjectIterator::treeObjectIterator(ctlTree *brow, pgObject *obj)
{
    browser=brow;
    object=obj;
}


pgObject *treeObjectIterator::GetNextObject()
{
    if (!object ||!browser)
        return 0;

    if (!lastItem)
        lastItem = browser->GetFirstChild(object->GetId(), cookie);
    else
        lastItem = browser->GetNextChild(object->GetId(), cookie);

    if (lastItem)
        return browser->GetObject(lastItem);
    else
        object=0;

    return 0;
}
