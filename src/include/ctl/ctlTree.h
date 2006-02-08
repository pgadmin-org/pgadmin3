//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id:  $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// ctlTree.h - wxTreeCtrl containing pgObjects
//
//////////////////////////////////////////////////////////////////////////

#ifndef CTLTREE_H
#define CTLTREE_H

// wxWindows headers
#include <wx/wx.h>
#include <wx/treectrl.h>

class pgObject;
class pgCollection;
class pgaFactory;



class ctlTree : public wxTreeCtrl
{
public:
    ctlTree(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTR_HAS_BUTTONS);
    wxTreeItemId AppendObject(pgObject *parent, pgObject *object);
    void RemoveDummyChild(pgObject *obj);
    pgCollection *AppendCollection(pgObject *parent, pgaFactory &factory);
    pgObject *GetObject(wxTreeItemId id);
    pgObject *GetParentObject(wxTreeItemId id) { return GetObject(GetItemParent(id)); }
    pgCollection *GetParentCollection(wxTreeItemId id);
    pgObject *FindObject(pgaFactory &factory, wxTreeItemId parent);
    pgCollection *FindCollection(pgaFactory &factory, wxTreeItemId parent);
};


class treeObjectIterator
{
public:
    treeObjectIterator(ctlTree *browser, pgObject *obj);
    pgObject *GetNextObject();

private:
    wxTreeItemId lastItem;
    ctlTree *browser;
    pgObject *object;
    wxCookieType cookie;
};



#endif
