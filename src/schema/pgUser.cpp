//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgUser.cpp - PostgreSQL User
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgUser.h"
#include "pgObject.h"
#include "pgServer.h"


pgUser::pgUser()
: pgObject()
{  
    wxLogInfo(wxT("Creating a pgUser object"));
}

pgUser::~pgUser()
{
    wxLogInfo(wxT("Destroying a pgUser object"));
}

int pgUser::GetType()
{
    return PG_USER;
}

wxString pgUser::GetTypeName() const
{
    return wxString("User");
}

pgServer pgUser::GetServer()
{
    return objServer;
}

int pgUser::GetUserID() {
    return iUserID;
}
