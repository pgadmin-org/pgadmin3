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
#include "../../pgAdmin3.h"
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

wxString pgObject::GetServer() const
{
    return szServer;
}

wxString pgObject::GetDatabase() const
{
    return szDatabase;
}

wxString pgObject::GetNamespace() const
{
    return szNamespace;
}

wxString pgObject::GetTable() const
{
    return szTable;
}

int pgObject::GetPort()
{
    return iPort;
}

unsigned long pgObject::GetOid()
{
    return lOid;
}
