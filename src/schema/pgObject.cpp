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
#include "misc.h"
#include "pgObject.h"
#include "pgServer.h"


pgObject::pgObject(int newType, const wxString& newName)
: wxTreeItemData()
{
    wxLogInfo(wxT("Creating a pgObject object"));

    // Set the typename and type
    type = newType;
    switch(type){
        case PG_NONE:
            typeName = wxT("None");
            break;

        case PG_SERVERS:
            typeName = wxT("Servers");
            break;

        case PG_ADD_SERVER:
            typeName = wxT("Add Server");
            break;

        case PG_SERVER:
            typeName = wxT("Server");
            break;

        case PG_DATABASES:
            typeName = wxT("Databases");
            break;

        case PG_ADD_DATABASE:
            typeName = wxT("Add Database");
            break;

        case PG_DATABASE:
            typeName = wxT("Database");
            break;

        case PG_GROUPS:
            typeName = wxT("Groups");
            break;

        case PG_ADD_GROUP:
            typeName = wxT("Add Group");
            break;

        case PG_GROUP:
            typeName = wxT("Group");
            break;

        case PG_USERS:
            typeName = wxT("Users");
            break;

        case PG_ADD_USER:
            typeName = wxT("Add User");
            break;

        case PG_USER:
            typeName = wxT("User");
            break;

        case PG_LANGUAGES:
            typeName = wxT("Languages");
            break;

        case PG_ADD_LANGUAGE:
            typeName = wxT("Add Language");
            break;

        case PG_LANGUAGE:
            typeName = wxT("Language");
            break;

        case PG_SCHEMAS:
            typeName = wxT("Schemas");
            break;

        case PG_ADD_SCHEMA:
            typeName = wxT("Add Schema");
            break;

        case PG_SCHEMA:
            typeName = wxT("Schema");
            break;

        default:
            typeName = wxT("None");
            break;


    }

    name = newName;
}

pgObject::~pgObject()
{
    wxLogInfo(wxT("Destroying a pgDatabase object"));
}

int pgObject::GetType()
{
    return type;
}

wxString pgObject::GetTypeName() const
{
    return typeName;
}

wxString pgObject::GetName() const
{
    return name;
}
void pgObject::iSetName(const wxString& newVal)
{
    name = newVal;
}


wxString pgObject::GetIdentifier() const
{
    return name;
}

wxString pgObject::GetQuotedIdentifier() const
{
    return qtIdent(name);
}

double pgObject::GetOid()
{
    return oid;
}
void pgObject::iSetOid(double newVal)
{
    oid = newVal;
}

wxString pgObject::GetOwner() const
{
    return owner;
}
void pgObject::iSetOwner(const wxString& newVal)
{
    owner = newVal;
}

wxString pgObject::GetComment() const
{
    return comment;
}
void pgObject::iSetComment(const wxString& newVal)
{
    comment = newVal;
}

wxString pgObject::GetAcl() const
{
    return acl;
}
void pgObject::iSetAcl(const wxString& newVal)
{
    acl = newVal;
}

bool pgObject::GetSystemObject()
{
    return FALSE;
}
