//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgObject.h - PostgreSQL base object class
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGOBJECT_H
#define PGOBJECT_H

// wxWindows headers
#include <wx/wx.h>
#include <wx/treectrl.h>

// App headers
#include "../../pgAdmin3.h"

// This enum lists the type of objects that may be included in the treeview
// as objects.
enum PG_OBJTYPE
{
    PG_NONE,
    PG_SERVERS,
    PG_SERVER,
    PG_ADD_SERVER
};

// Class declarations
class pgObject : public wxTreeItemData
{
public:
    pgObject(int iNewType = 0, wxString& szNewName = wxString(""));
    ~pgObject();
    virtual int GetType();
    virtual wxString GetTypeName();
    virtual wxString GetIdentifier();
    virtual wxString GetName();
    virtual wxString GetServer();
    virtual wxString GetDatabase();
    virtual wxString GetNamespace();
    virtual wxString GetTable();
    virtual int GetPort();
    virtual unsigned long GetOid();

private:
    wxString szTypeName, szIdentifier, szName, szServer, szDatabase, szNamespace, szTable;
    int iType, iPort;
    unsigned long lOid;
};

#endif

