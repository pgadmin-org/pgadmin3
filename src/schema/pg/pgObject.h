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

// Class declarations
class pgObject : public wxTreeItemData
{
public:
    pgObject();
    ~pgObject();
    virtual wxString GetType();
    virtual wxString GetIdentifier();
    virtual wxString GetName();
    virtual wxString GetServer();
    virtual wxString GetDatabase();
    virtual wxString GetNamespace();
    virtual wxString GetTable();
    virtual unsigned long GetPort();
    virtual unsigned long GetOid();

private:
    wxString szIdentifier, szName, szServer, szDatabase, szNamespace, szTable;
    unsigned long lPort, lOid;
};

#endif

