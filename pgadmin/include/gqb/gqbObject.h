//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: frmQuery.cpp 7381 2008-08-11 11:33:42Z dpage $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// gqbObject.h - Main basic object used by GQB
//
//////////////////////////////////////////////////////////////////////////

#ifndef GQBOBJECT_H
#define GQBOBJECT_H

enum type_gqbObject
{
    _gqbDatabase,
    _gqbSchema,
    _gqbTable,
    _gqbColumn,
    _gqbQueryObj,
    _gqbQuery,
    _gqbJoin,
    _gqbRestriction
};

// Create Array Objects used as base for gqbCollections
class gqbObject : public wxTreeItemData
{
public:
    gqbObject(wxString name, type_gqbObject type);
    virtual ~gqbObject();
    void setName(wxString name);
    const wxString& getName();
    void setOwner(wxTreeItemData *owner);
    const wxTreeItemData& getOwner();
    void setType(type_gqbObject name);
    const type_gqbObject getType();

private:
    wxString Name;
    wxTreeItemData *Owner;
    type_gqbObject Type;
};
#endif
