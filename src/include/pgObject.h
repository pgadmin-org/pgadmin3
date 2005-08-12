//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgObject.h - PostgreSQL base object class
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGOBJECT_H
#define PGOBJECT_H

// wxWindows headers
#include <wx/wx.h>
#include <ctl/ctlTree.h>
#include "ctl/ctlSQLBox.h"

// App headers
#include "pgAdmin3.h"



class frmMain;
class pgDatabase;
class pgSchema;
class pgCollection;
class pgConn;
class pgSet;
class pgServer;

// This enum lists the type of objects that may be included in the treeview
// as objects. If changing, update typesList[] as well.
enum PG_OBJTYPE
{
    PG_NONE,
    PG_COLUMNS,         PG_COLUMN,
    PG_INDEXES,         PG_INDEX,
    PG_RULES,           PG_RULE,
    PG_TRIGGERS,        PG_TRIGGER,
    PG_CONSTRAINTS,     PG_PRIMARYKEY, PG_UNIQUE, PG_CHECK, PG_FOREIGNKEY,

    PGA_JOBS,			PGA_JOB,
	PGA_SCHEDULES,		PGA_SCHEDULE,
    PGA_STEPS,			PGA_STEP,

    SL_NODES,           SL_NODE,
    SL_PATHS,           SL_PATH,
    SL_LISTENS,         SL_LISTEN,
    SL_SETS,            SL_SET,
    SL_SEQUENCES,       SL_SEQUENCE,
    SL_TABLES,          SL_TABLE,
    SL_SUBSCRIPTIONS,   SL_SUBSCRIPTION,
    
    PG_UNKNOWN
};

class pgTypes
{
public:
    wxChar *typName;
    long    typeIcon;
    wxChar *newString;
    wxChar *newLongString;
};


extern pgTypes typesList[];

class pgaFactory;

// Class declarations
class pgObject : public wxTreeItemData
{
protected:
    pgObject(int newType, const wxString& newName=wxEmptyString);
    pgObject(pgaFactory &factory, const wxString& newName=wxEmptyString);

public:

    static wxString GetPrivileges(const wxString& allPattern, const wxString& acl, const wxString& grantObject, const wxString& user);
    static int GetTypeId(const wxString &typname);

    pgaFactory *GetFactory() { return factory; }
    bool IsCreatedBy(pgaFactory &f) const { return &f == factory; }
    bool IsCreatedBy(pgaFactory *f) const { return f == factory; }
    int GetType() const;
    int GetMetaType() const;
    wxString GetTypeName() const;
    wxString GetTranslatedTypeName() const;
    virtual int GetIconId();
    bool UpdateIcon(ctlTree *browser);

    virtual void ShowProperties() const {};
    virtual pgDatabase *GetDatabase() const { return 0; }
    virtual pgServer *GetServer() const { return 0; }
    void iSetName(const wxString& newVal) { name = newVal; }
    wxString GetName() const { return name; }
    OID GetOid() const { return oid; }
    wxString GetOidStr() const {return NumToStr(oid) + wxT("::oid"); }
    void iSetOid(const OID newVal) { oid = newVal; } 
    wxString GetOwner() const { return owner; }
    void iSetOwner(const wxString& newVal) { owner = newVal; }
    wxString GetComment() const { return comment; }
    void iSetComment(const wxString& newVal) { comment = newVal; }
    wxString GetAcl() const { return acl; }
    void iSetAcl(const wxString& newVal) { acl = newVal; }
    virtual bool GetSystemObject() const { return false; }
    virtual bool IsCollection() const { return false; }
    virtual void ShowHint(frmMain *form, bool force) {}

    void ShowTree(frmMain *form, ctlTree *browser, ctlListView *properties, ctlSQLBox *sqlPane);

    wxTreeItemId AppendBrowserItem(ctlTree *browser, pgObject *object);
    
    virtual wxString GetHelpPage(bool forCreate) const;
    virtual wxString GetFullName() const { return name; }
    virtual wxString GetIdentifier() const { return name; }
    virtual wxString GetQuotedIdentifier() const { return qtIdent(name); }

    virtual wxMenu *GetNewMenu();
    virtual wxString GetSql(ctlTree *browser) { return wxT(""); }
    wxString GetGrant(const wxString& allPattern, const wxString& grantFor=wxT(""));
    wxString GetCommentSql();
    wxString GetOwnerSql(int major, int minor, wxString objname=wxEmptyString);
    pgConn *GetConnection() const;

    virtual void SetDirty() { sql=wxT(""); expandedKids=false; needReread=true; }
    virtual wxString GetFullIdentifier() const { return GetName(); }
    virtual wxString GetQuotedFullIdentifier() const { return qtIdent(name); }

    virtual void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0)
        =0;
    virtual void ShowStatistics(frmMain *form, ctlListView *statistics);
    virtual void ShowDependsOn(frmMain *form, ctlListView *dependsOn, const wxString &where=wxEmptyString);
    virtual void ShowReferencedBy(frmMain *form, ctlListView *referencedBy, const wxString &where=wxEmptyString);
    virtual pgObject *Refresh(ctlTree *browser, const wxTreeItemId item) {return this; }
    virtual bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded=false) {return false; }
    virtual bool EditObject(wxFrame *frame, ctlTree *browser) {return false; }

    virtual bool NeedCascadedDrop() { return false; }
    virtual bool CanCreate() { return false; }
    virtual bool CanView() { return false; }
    virtual bool CanEdit() { return false; }
    virtual bool CanDrop() { return false; }
    virtual bool CanDropCascaded() { return false; }
    virtual bool CanMaintenance() { return false; }
    virtual bool RequireDropConfirm() { return false; }
    virtual bool WantDummyChild() { return false; }
    virtual bool CanBackup() { return false; }
    virtual bool CanRestore() { return false; }
    virtual bool GetCanHint() { return false; }

protected:
    void CreateListColumns(ctlListView *properties, const wxString &left=_("Property"), const wxString &right=_("Value"));

    void AppendMenu(wxMenu *menu, int type=-1);
    virtual void SetContextInfo(frmMain *form) {}

    bool expandedKids, needReread;
    wxString sql;
    bool hintShown;
    pgaFactory *factory;
    
private:
    static void AppendRight(wxString &rights, const wxString& acl, wxChar c, wxChar *rightName);
    static wxString GetPrivilegeGrant(const wxString& allPattern, const wxString& acl, const wxString& grantObject, const wxString& user);
    void ShowDependency(pgDatabase *db, ctlListView *list, const wxString &query, const wxString &clsOrder);
    wxString name, owner, comment, acl;
    int type;
    OID oid;

    friend class pgaFactory;
};


// Object that lives under a server
class pgServerObject : public pgObject
{
public:
    pgServerObject(pgaFactory &factory, const wxString& newName=wxEmptyString) : pgObject(factory, newName) {}
    pgServerObject(int newType, const wxString& newName) : pgObject(newType, newName) {}

    void iSetServer(pgServer *s) { server=s; }
    pgServer *GetServer() const { return server; }

    void FillOwned(ctlTree *browser, ctlListView *referencedBy, const wxArrayString &dblist, const wxString &query);

    bool CanCreate();
    bool CanDrop();
    bool CanEdit() { return true; }

protected:
    pgServer *server;
};



// Object that lives in a database
class pgDatabaseObject : public pgObject
{
public:
    pgDatabaseObject(pgaFactory &factory, const wxString& newName=wxEmptyString) : pgObject(factory, newName) {}
    pgDatabaseObject(int newType, const wxString& newName) : pgObject(newType, newName) {}

    void iSetDatabase(pgDatabase *newDatabase) { database = newDatabase; }
    pgDatabase *GetDatabase() const { return database; }
    pgServer *GetServer() const;

    void DisplayStatistics(ctlListView *statistics, const wxString& query);

    // compiles a prefix from the schema name with '.', if necessary
    wxString GetSchemaPrefix(const wxString &schemaname) const;
    wxString GetQuotedSchemaPrefix(const wxString &schemaname) const;

    bool CanDrop();
    bool CanEdit() { return true; }
    bool CanCreate();

protected:
    pgDatabase *database;
};



// Object that lives in a schema
class pgSchemaObject : public pgDatabaseObject
{
public:
    pgSchemaObject(pgSchema *newSchema, pgaFactory &factory, const wxString& newName=wxEmptyString) : pgDatabaseObject(factory, newName) 
        { tableOid=0; SetSchema(newSchema); wxLogInfo(wxT("Creating a pg") + GetTypeName() + wxT(" object")); }
    pgSchemaObject(pgSchema *newSchema, int newType, const wxString& newName = wxT("")) : pgDatabaseObject(newType, newName)
        { tableOid=0; SetSchema(newSchema); wxLogInfo(wxT("Creating a pg") + GetTypeName() + wxT(" object")); }

    pgSchemaObject::~pgSchemaObject()
        { wxLogInfo(wxT("Destroying a pg") + GetTypeName() + wxT(" object")); }

    bool GetSystemObject() const;

    bool CanDrop();
    bool CanEdit() { return true; }
    bool CanCreate();

    void SetSchema(pgSchema *newSchema);
    pgSchema *GetSchema() const {return schema; }
    pgSet *ExecuteSet(const wxString& sql);
    wxString ExecuteScalar(const wxString& sql);
    bool ExecuteVoid(const wxString& sql);
    OID GetTableOid() const {return tableOid; }
    void iSetTableOid(const OID d) { tableOid=d; }
    wxString GetTableOidStr() const {return NumToStr(tableOid) + wxT("::oid"); }
    virtual wxString GetFullIdentifier() const;
    virtual wxString GetQuotedFullIdentifier() const;


protected:
    virtual void SetContextInfo(frmMain *form);

    pgSchema *schema;
    OID tableOid;
};


class pgRuleObject : public pgSchemaObject
{
public:
    pgRuleObject(pgSchema *newSchema, pgaFactory &factory, const wxString& newName=wxEmptyString) : pgSchemaObject(newSchema, factory, newName) {}
    pgRuleObject(pgSchema *newSchema, int newType, const wxString& newName = wxT("")) 
        : pgSchemaObject(newSchema, newType, newName) {}

    wxString GetFormattedDefinition();
    wxString GetDefinition() const { return definition; }
    void iSetDefinition(const wxString& s) { definition=s; }

protected:
    wxString definition;
};
#endif

