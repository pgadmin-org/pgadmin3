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
#include "pgServer.h"


pgObject::pgObject(int iNewType, const wxString& szNewName)
: wxTreeItemData()
{
    wxLogInfo(wxT("Creating a pgObject object"));

    // Call the 'virtual' ctor
    vCtor(iNewType, szNewName);
}

void pgObject::vCtor(int iNewType, const wxString& szNewName)
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

        case PG_LANGUAGES:
            szTypeName = wxT("Languages");
            break;

        case PG_ADD_LANGUAGE:
            szTypeName = wxT("Add Language");
            break;

        case PG_LANGUAGE:
            szTypeName = wxT("Language");
            break;

        case PG_SCHEMAS:
            szTypeName = wxT("Schemas");
            break;

        case PG_ADD_SCHEMA:
            szTypeName = wxT("Add Schema");
            break;

        case PG_SCHEMA:
            szTypeName = wxT("Schema");
            break;

        default:
            szTypeName = wxT("None");
            break;


    }

    szName = szNewName;
}

pgObject::~pgObject()
{
    wxLogInfo(wxT("Destroying a pgDatabase object"));
}

int pgObject::GetType()
{
    return iType;
}

wxString pgObject::GetTypeName() const
{
    return szTypeName;
}

wxString pgObject::GetName() const
{
    return szName;
}
void pgObject::iSetName(const wxString& szNewVal)
{
    szName = szNewVal;
}


wxString pgObject::GetIdentifier() const
{
    return szName;
}

wxString pgObject::GetQuotedIdentifier() const
{
    return qtIdent(szName);
}

double pgObject::GetOid()
{
    return dOid;
}
void pgObject::iSetOid(double dNewVal)
{
    dOid = dNewVal;
}

wxString pgObject::GetOwner() const
{
    return szOwner;
}
void pgObject::iSetOwner(const wxString& szNewVal)
{
    szOwner = szNewVal;
}

wxString pgObject::GetComment() const
{
    return szComment;
}
void pgObject::iSetComment(const wxString& szNewVal)
{
    szComment = szNewVal;
}

wxString pgObject::GetAcl() const
{
    return szAcl;
}
void pgObject::iSetAcl(const wxString& szNewVal)
{
    szAcl = szNewVal;
}

bool pgObject::GetSystemObject()
{
    return bSystemObject;
}
void pgObject::iSetSystemObject(bool bNewVal)
{
    bSystemObject = bNewVal;
}