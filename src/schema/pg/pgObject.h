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
    PG_ADD_SERVER,
    PG_SERVER,
    PG_DATABASES,
    PG_ADD_DATABASE,
    PG_DATABASE,
    PG_GROUPS,
    PG_ADD_GROUP,
    PG_GROUP,
    PG_USERS,
    PG_ADD_USER,
    PG_USER
};

// Class declarations
class pgObject : public wxTreeItemData
{
public:
    pgObject(int iNewType = 0, const wxString& szNewName = wxString(""));
    ~pgObject();

    // Everything should implement these
    virtual int GetType();
    virtual wxString GetTypeName() const;
    virtual wxString GetIdentifier() const;
    virtual wxString GetName() const;

    // These are optional
    virtual unsigned long GetOid();

private:
    wxString szTypeName, szIdentifier, szName;
    int iType;
    unsigned long lOid;

};

#endif

