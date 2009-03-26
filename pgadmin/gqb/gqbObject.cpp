//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2009, The pgAdmin Development Team
// This software is released under the BSD Licence
//
// gqbObject.cpp - Main basic object used by GQB
//
//////////////////////////////////////////////////////////////////////////

// App headers
#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "gqb/gqbObject.h"

gqbObject::gqbObject(wxString name, type_gqbObject type)
{
    this->Type=type;
    this->Name=name;
}


gqbObject::~gqbObject()
{
}


void gqbObject::setName(wxString name)
{
    this->Name=name;
}


const wxString& gqbObject::getName()
{
    return Name;
}


void gqbObject::setOwner(wxTreeItemData *owner)
{
    this->Owner=owner;
}


const wxTreeItemData& gqbObject::getOwner()
{
    return (*Owner);
}


void gqbObject::setType(type_gqbObject tname)
{
    this->Type=tname;
}


const type_gqbObject gqbObject::getType()
{
    return this->Type;
}
