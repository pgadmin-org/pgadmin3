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
    pgObject(int iNewType = PG_NONE, const wxString& szNewName = wxString(""));
    ~pgObject();

    int GetType();
    wxString GetTypeName() const;
    void iSetName(const wxString& szNewVal);
    wxString GetName() const;
    virtual wxString GetIdentifier() const;
    virtual wxString GetQuotedIdentifier() const;
    double GetOid();
    void iSetOid(double lNewVal);
    wxString GetOwner() const;
    void iSetOwner(const wxString& szNewVal);
    wxString GetComment() const;
    void iSetComment(const wxString& szNewVal);
    wxString GetAcl() const;
    void iSetAcl(const wxString& szNewVal);
    virtual bool GetSystemObject();

protected:
    void vCtor(int iNewType = PG_NONE, const wxString& szNewName = wxString(""));

private:
    wxString szTypeName, szName, szOwner, szComment, szAcl;
    int iType;
    double dOid;
};

#endif

