//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgDatabase.cpp - PostgreSQL Database
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgDatabase.h"
#include "pgObject.h"
#include "pgServer.h"


pgDatabase::pgDatabase()
: pgObject()
{  
    wxLogInfo(wxT("Creating a pgDatabase object"));
}

pgDatabase::~pgDatabase()
{
    wxLogInfo(wxT("Destroying a pgDatabase object"));
}

int pgDatabase::GetType()
{
    return PG_DATABASE;
}

wxString pgDatabase::GetTypeName() const
{
    return wxString("Database");
}

pgServer pgDatabase::GetServer()
{
    return objServer;
}

int pgDatabase::Connect() {
    return PGCONN_BAD;
}
