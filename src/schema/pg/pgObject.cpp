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


pgObject::pgObject(int iNewType, wxString& szNewName)
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

wxString pgObject::GetTypeName()
{
    return szTypeName;
}

wxString pgObject::GetIdentifier()
{
    return szIdentifier;
}

wxString pgObject::GetName()
{
    return szName;
}

wxString pgObject::GetServer()
{
    return szServer;
}

wxString pgObject::GetDatabase()
{
    return szDatabase;
}

wxString pgObject::GetNamespace()
{
    return szNamespace;
}

wxString pgObject::GetTable()
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
