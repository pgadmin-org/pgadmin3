//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgCollection.cpp - Simple object for use with 'collection' nodes
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgCollection.h"
#include "pgServer.h"


pgCollection::pgCollection(int newType, const wxString& newName)
: pgObject(newType, newName)
{ 
    wxLogInfo(wxT("Creating a pgCollection object")); 
}

pgCollection::~pgCollection()
{
    wxLogInfo(wxT("Destroying a pgCollection object"));
}
