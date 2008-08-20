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
    GQB_DATABASE,
    GQB_SCHEMA,
    GQB_TABLE,
	GQB_VIEW,
    GQB_COLUMN,
    GQB_QUERYOBJ,
    GQB_QUERY,
    GQB_JOIN,
    GQB_RESTRICTION
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
