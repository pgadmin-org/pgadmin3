//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgObject.cpp - PostgreSQL object base class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/treectrl.h>

// App headers
#include "pgAdmin3.h"
#include "pgObject.h"


pgObject::pgObject(int iNewType, const wxString& szNewName)
: wxTreeItemData()
{

    // Set the typename and type
    iType = iNewType;
    switch(iNewType){
        case PG_NONE:
            szTypeName = wxT("None");
            break;

        case PG_SERVERS:
            szTypeName = wxT("Servers");
            break;

        case PG_ADD_SERVER:
            szTypeName = wxT("Add Server");
            break;

        case PG_SERVER:
            szTypeName = wxT("Server");
            break;

        case PG_DATABASES:
            szTypeName = wxT("Databases");
            break;

        case PG_ADD_DATABASE:
            szTypeName = wxT("Add Database");
            break;

        case PG_DATABASE:
            szTypeName = wxT("Database");
            break;

        case PG_GROUPS:
            szTypeName = wxT("Groups");
            break;

        case PG_ADD_GROUP:
            szTypeName = wxT("Add Group");
            break;

        case PG_GROUP:
            szTypeName = wxT("Group");
            break;

        case PG_USERS:
            szTypeName = wxT("Users");
            break;

        case PG_ADD_USER:
            szTypeName = wxT("Add User");
            break;

        case PG_USER:
            szTypeName = wxT("User");
            break;

        default:
            szTypeName = wxT("None");
            break;
    }

    szName = szNewName;
}

pgObject::~pgObject()
{
}

int pgObject::GetType()
{
    return iType;
}

wxString pgObject::GetTypeName() const
{
    return szTypeName;
}

wxString pgObject::GetIdentifier() const
{
    return szIdentifier;
}

wxString pgObject::GetName() const
{
    return szName;
}

unsigned long pgObject::GetOid()
{
    return lOid;
}
