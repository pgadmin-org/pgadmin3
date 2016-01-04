//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
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

gqbObject::gqbObject(wxString name, wxTreeItemData *owner, pgConn *connection, OID oid)
{
	Name = name;
	Owner = owner;
	conn = connection;
	Oid = oid;
}


gqbObject::~gqbObject()
{
}
