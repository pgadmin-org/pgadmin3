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


pgCollection::pgCollection(int iNewType, const wxString& szNewName)
: pgObject()
{

    wxLogInfo(wxT("Creating a pgCollection object"));

    // Call the 'virtual' ctor
    vCtor(iNewType, szNewName);
}

pgCollection::~pgCollection()
{
    wxLogInfo(wxT("Destroying a pgCollection object"));
}

// Parent objects
pgServer *pgCollection::GetServer() {
    return objServer;
}

void pgCollection::SetServer(pgServer *objNewServer) {
    objServer = objNewServer;
}