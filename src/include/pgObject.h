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
#include "misc.h"

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
    ~pgObject() { wxLogInfo(wxT("Destroying a pgDatabase object")); }

    virtual void ShowProperties() const {};
    int GetType() const { return type; }
    wxString GetTypeName() const { return typeName; }
    void iSetName(const wxString& newVal) { name = newVal; }
    wxString GetName() const { return name; }
    virtual wxString GetIdentifier() const { return name; }
    virtual wxString GetQuotedIdentifier() const { return qtIdent(name); }
    double GetOid() const { return oid; }
    void iSetOid(double newVal) { oid = newVal; } 
    wxString GetOwner() const { return owner; }
    void iSetOwner(const wxString& newVal) { owner = newVal; }
    wxString GetComment() const { return comment; }
    void iSetComment(const wxString& newVal) { comment = newVal; }
    wxString GetAcl() const { return acl; }
    void iSetAcl(const wxString& newVal) { acl = newVal; }
    virtual bool GetSystemObject() const { return FALSE; }

private:
    wxString typeName, name, owner, comment, acl;
    int type;
    double oid;
};

#endif

