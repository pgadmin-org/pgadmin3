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
#include "pgAdmin3.h"

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
    PG_USER,
    PG_LANGUAGES,
    PG_ADD_LANGUAGE,
    PG_LANGUAGE,
    PG_SCHEMAS,
    PG_ADD_SCHEMA,
    PG_SCHEMA
};

// Class declarations
class pgObject : public wxTreeItemData
{
public:
    pgObject(int newType = PG_NONE, const wxString& newName = wxString(""));
    ~pgObject();

    int GetType();
    wxString GetTypeName() const;
    void iSetName(const wxString& newVal);
    wxString GetName() const;
    virtual wxString GetIdentifier() const;
    virtual wxString GetQuotedIdentifier() const;
    double GetOid();
    void iSetOid(double newVal);
    wxString GetOwner() const;
    void iSetOwner(const wxString& newVal);
    wxString GetComment() const;
    void iSetComment(const wxString& newVal);
    wxString GetAcl() const;
    void iSetAcl(const wxString& newVal);
    virtual bool GetSystemObject();

private:
    wxString typeName, name, owner, comment, acl;
    int type;
    double oid;
};

#endif

