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


pgObject::pgObject()
: wxTreeItemData()
{  
}

pgObject::~pgObject()
{
}

wxString pgObject::GetType()
{
    return wxString("");
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

unsigned long pgObject::GetPort()
{
    return lPort;
}

unsigned long pgObject::GetOid()
{
    return lOid;
}
