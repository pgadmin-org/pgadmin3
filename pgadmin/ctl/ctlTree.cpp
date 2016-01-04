//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
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


wxTreeItemId ctlTree::FindItem(const wxTreeItemId &idParent, const wxString &prefixOrig)
{
	// match is case insensitive as this is more convenient to the user: having
	// to press Shift-letter to go to the item starting with a capital letter
	// would be too bothersome
	wxString prefix = prefixOrig.Lower();

	// determine the starting point: we shouldn't take the current item (this
	// allows to switch between two items starting with the same letter just by
	// pressing it) but we shouldn't jump to the next one if the user is
	// continuing to type as otherwise he might easily skip the item he wanted
	wxTreeItemId id = idParent;

	if ( prefix.length() == 1 )
	{
		wxCookieType cookie;
		if (HasChildren(id))
			id = GetFirstChild(id, cookie);
		else
		{
			// Try a sibling of this or ancestor instead
			wxTreeItemId p = id;
			wxTreeItemId toFind;
			do
			{
				toFind = GetNextSibling(p);
				p = GetItemParent(p);
			}
			while (p.IsOk() && !toFind.IsOk());
			id = toFind;
		}
	}

	// look for the item starting with the given prefix after it
	while ( id.IsOk() &&
	        ( ( GetItemText(id) == wxT("Dummy") && !GetItemData(id) ) ||
	          !GetItemText(id).Lower().StartsWith(prefix) ))
	{
		wxCookieType cookie;
		if ( HasChildren(id) )
			id = GetFirstChild(id, cookie);
		else
		{
			// Try a sibling of this or ancestor instead
			wxTreeItemId p = id;
			wxTreeItemId toFind;
			do
			{
				toFind = GetNextSibling(p);
				p = GetItemParent(p);
			}
			while (p.IsOk() && !toFind.IsOk());
			id = toFind;
		}
	}

	// if we haven't found anything...
	if ( !id.IsOk() )
	{
		// ... wrap to the beginning
		id = GetRootItem();
		if ( HasFlag(wxTR_HIDE_ROOT) )
		{
			wxCookieType cookie;
			// can't select virtual root
			if ( HasChildren(id) )
				id = GetFirstChild(id, cookie);
			else
			{
				// Try a sibling of this or ancestor instead
				wxTreeItemId p = id;
				wxTreeItemId toFind;
				do
				{
					toFind = GetNextSibling(p);
					p = GetItemParent(p);
				}
				while (p.IsOk() && !toFind.IsOk());
				id = toFind;
			}
		}

		// and try all the items (stop when we get to the one we started from)
		while ( id.IsOk() && id != idParent &&
		        (( GetItemText(id) == wxT("Dummy") && !GetItemData(id) ) ||
		         !GetItemText(id).Lower().StartsWith(prefix) ))
		{
			wxCookieType cookie;
			if ( HasChildren(id) )
				id = GetFirstChild(id, cookie);
			else
			{
				// Try a sibling of this or ancestor instead
				wxTreeItemId p = id;
				wxTreeItemId toFind;
				do
				{
					toFind = GetNextSibling(p);
					p = GetItemParent(p);
				}
				while (p.IsOk() && !toFind.IsOk());
				id = toFind;
			}
		}
		// If we haven't found the item, id.IsOk() will be false, as per
		// documentation
	}

	return id;
}

void ctlTree::OnChar(wxKeyEvent &event)
{
	int keyCode = event.GetKeyCode();
	if ( !event.HasModifiers() &&
	        ((keyCode >= '0' && keyCode <= '9') ||
	         (keyCode >= 'a' && keyCode <= 'z') ||
	         (keyCode >= 'A' && keyCode <= 'Z')))
	{
		wxTreeItemId currItem = GetSelection();

		if (!currItem.IsOk())
			return;

		wxTreeItemId matchItem = FindItem(currItem, m_findPrefix + (wxChar)keyCode);

		if ( matchItem.IsOk() )
		{
			EnsureVisible(matchItem);
			SelectItem(matchItem);

			m_findPrefix += (wxChar)keyCode;

			// also start the timer to reset the current prefix if the user
			// doesn't press any more alnum keys soon -- we wouldn't want
			// to use this prefix for a new item search
			if ( !m_findTimer )
			{
				m_findTimer = new ctlTreeFindTimer(this);
			}

			m_findTimer->Start(ctlTreeFindTimer::CTLTREE_DELAY, wxTIMER_ONE_SHOT);
			return;
		}
	}
	else
	{
		event.Skip(true);
	}
}

ctlTree::ctlTree(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style)
	: wxTreeCtrl(parent, id, pos, size, style), m_findTimer(NULL)
{
}

ctlTree::~ctlTree()
{
	if ( m_findTimer )
		delete m_findTimer;
	m_findTimer = NULL;
}


void ctlTree::RemoveDummyChild(pgObject *obj)
{
	wxCookieType cookie;
	wxTreeItemId childItem = GetFirstChild(obj->GetId(), cookie);
	if (childItem && !GetItemData(childItem))
	{
		// The child was a dummy item, which will be replaced by the following ShowTreeDetail by true items
		Delete(childItem);
	}
}


pgObject *ctlTree::GetObject(wxTreeItemId id)
{
	if (id)
		return (pgObject *)GetItemData(id);
	return 0;
}


pgCollection *ctlTree::GetParentCollection(wxTreeItemId id)
{
	pgCollection *coll = (pgCollection *)GetParentObject(id);
	if (coll && coll->IsCollection())
		return coll;
	return 0;
}


void ctlTree::SetItemImage(const wxTreeItemId &item, int image, wxTreeItemIcon which)
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

wxTreeItemId ctlTree::AppendItem(const wxTreeItemId &parent, const wxString &text, int image, int selImage, wxTreeItemData *data)
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
	pgCollection *collection = factory.CreateCollection(parent);
	AppendObject(parent, collection);
	return collection;
}


pgObject *ctlTree::FindObject(pgaFactory &factory, wxTreeItemId parent)
{
	wxCookieType cookie;
	wxTreeItemId item = GetFirstChild(parent, cookie);
	while (item)
	{
		pgObject *obj = (pgObject *)GetItemData(item);
		if (obj && obj->IsCreatedBy(factory))
			return obj;
		item = GetNextChild(parent, cookie);
	}
	return 0;
}


pgCollection *ctlTree::FindCollection(pgaFactory &factory, wxTreeItemId parent)
{
	pgaFactory *cf = factory.GetCollectionFactory();
	if (!cf)
		return 0;

	pgCollection *collection = (pgCollection *)FindObject(*cf, parent);

	if (!collection || !collection->IsCollection())
		return 0;
	return collection;
}


void ctlTree::NavigateTree(int keyCode)
{
	switch(keyCode)
	{
		case WXK_LEFT:
		{
			//If tree item has children and is expanded, collapse it, otherwise select it's parent if has one
			wxTreeItemId currItem = GetSelection();

			if (ItemHasChildren(currItem) && IsExpanded(currItem))
			{
				Collapse(currItem);
			}
			else
			{
				wxTreeItemId parent = GetItemParent(currItem);
				if (parent.IsOk())
				{
					SelectItem(currItem, false);
					SelectItem(parent, true);
				}
			}
		}
		break;
		case WXK_RIGHT:
		{
			//If tree item do not have any children ignore it,
			//otherwise  expand it if not expanded, and select first child if already expanded
			wxTreeItemId currItem = GetSelection();

			if(ItemHasChildren(currItem))
			{
				if (!IsExpanded(currItem))
				{
					Expand(currItem);
				}
				else
				{
					wxCookieType cookie;
					wxTreeItemId firstChild = GetFirstChild(currItem, cookie);
					SelectItem(currItem, false);
					SelectItem(firstChild, true);
				}
			}
		}
		break;
		default:
			wxASSERT_MSG(false, _("Currently handles only right and left arrow key, other keys are working"));
			break;
	}
}

//////////////////////

treeObjectIterator::treeObjectIterator(ctlTree *brow, pgObject *obj)
{
	browser = brow;
	object = obj;
}


pgObject *treeObjectIterator::GetNextObject()
{
	if (!object || !browser)
		return 0;

	if (!lastItem)
		lastItem = browser->GetFirstChild(object->GetId(), cookie);
	else
		lastItem = browser->GetNextChild(object->GetId(), cookie);

	if (lastItem)
		return browser->GetObject(lastItem);
	else
		object = 0;

	return 0;
}
