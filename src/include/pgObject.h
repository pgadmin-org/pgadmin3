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
#include <wx/listctrl.h>
#include "ctlSQLbox.h"

// App headers
#include "pgAdmin3.h"
#include "misc.h"


class frmMain;
class pgSchema;
class pgCollection;
class pgSet;


// This enum lists the type of objects that may be included in the treeview
// as objects.
enum PG_OBJTYPE
{
    PG_NONE,
    PG_SERVERS,         PG_SERVER,          PG_ADD_SERVER,
    PG_DATABASES,       PG_DATABASE,        PG_ADD_DATABASE,
    PG_GROUPS,          PG_GROUP,           PG_ADD_GROUP,
    PG_USERS,           PG_USER,            PG_ADD_USER,
    PG_LANGUAGES,       PG_LANGUAGE,        PG_ADD_LANGUAGE,
    PG_SCHEMAS,         PG_SCHEMA,          PG_ADD_SCHEMA,
    PG_AGGREGATES,      PG_AGGREGATE,
    PG_DOMAINS,         PG_DOMAIN,
    PG_FUNCTIONS,       PG_FUNCTION,
    PG_OPERATORS,       PG_OPERATOR,
    PG_SEQUENCES,       PG_SEQUENCE,
    PG_TABLES,          PG_TABLE,
    PG_TYPES,           PG_TYPE,
    PG_VIEWS,           PG_VIEW,
    PG_CHECKS,          PG_CHECK,
    PG_COLUMNS,         PG_COLUMN,
    PG_FOREIGNKEYS,     PG_FOREIGNKEY,
    PG_INDEXES,         PG_INDEX,
    PG_RULES,           PG_RULE,
    PG_TRIGGERS,        PG_TRIGGER,
    
    PG_UNKNOWN
};

extern char *typeNameList[];

// Class declarations
class pgObject : public wxTreeItemData
{
protected:
    pgObject(int newType, const wxString& newName);

public:
    ~pgObject() { wxLogInfo(wxT("Destroying a pgDatabase object")); }

    virtual void ShowProperties() const {};
    int GetType() const { return type; }
    wxString GetTypeName() const { return typeName; }
    void iSetName(const wxString& newVal) { name = newVal; }
    wxString GetName() const { return name; }
    double GetOid() const { return oid; }
    wxString GetOidStr() const {return NumToStr(oid) + wxT("::oid"); }
    void iSetOid(double newVal) { oid = newVal; } 
    wxString GetOwner() const { return owner; }
    void iSetOwner(const wxString& newVal) { owner = newVal; }
    wxString GetComment() const { return comment; }
    void iSetComment(const wxString& newVal) { comment = newVal; }
    wxString GetAcl() const { return acl; }
    void iSetAcl(const wxString& newVal) { acl = newVal; }
    virtual bool GetSystemObject() const { return FALSE; }
    void ShowTree(frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane);


    virtual wxString GetFullName() const { return name; }
    virtual wxString GetIdentifier() const { return name; }
    virtual wxString GetFullIdentifier() const {return GetOwner() + wxT(".")+GetFullName(); }
    virtual wxString GetQuotedFullIdentifier() const {return qtIdent(GetOwner()) + wxT(".")+qtIdent(GetFullName()); }
    virtual wxString GetQuotedIdentifier() const { return qtIdent(name); }

    virtual wxString GetSql(wxTreeCtrl *browser) { return wxT(""); }
    wxString GetGrant(const wxString& grantFor=wxT(""), bool noOwner=false);
    wxString GetCommentSql();

    virtual void SetDirty() { sql=wxT(""); expandedKids=0; }
    virtual void SetSql(wxTreeCtrl *browser, ctlSQLBox *sqlPane, const int index) { return; }

protected:
    static void InsertListItem(wxListCtrl *list, const int pos, const wxString& str1, const wxString& str2);
    virtual void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0)
        =0;

    bool expandedKids;
    wxString sql;
    
private:
    wxString typeName, name, owner, comment, acl;
    int type;
    double oid;
};


class pgSchemaObject : public pgObject
{
public:
    pgSchemaObject(pgSchema *newSchema, int newType, const wxString& newName = wxString("")) : pgObject(newType, newName)
        { tableOid=0; schema = newSchema; wxLogInfo(wxT("Creating a pg") + GetTypeName() + wxT(" object")); }

    pgSchemaObject::~pgSchemaObject()
        { wxLogInfo(wxT("Destroying a pg") + GetTypeName() + wxT(" object")); }

    void SetSchema(pgSchema *newSchema) { schema = newSchema; }
    pgSchema *GetSchema() const {return schema; }
    pgSet *ExecuteSet(const wxString& sql);
    void DisplayStatistics(wxListCtrl *statistics, const wxString& query);
    void SetButtons(frmMain *form, bool canVacuum=false);
    double GetTableOid() const {return tableOid; }
    void iSetTableOid(const double d) { tableOid=d; }
    wxString GetTableOidStr() const {return NumToStr(tableOid) + wxT("::oid"); }

protected:
    pgSchema *schema;
    double tableOid;
};


#endif

