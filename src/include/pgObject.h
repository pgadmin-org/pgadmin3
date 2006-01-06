//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgObject.h - PostgreSQL base object class
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGOBJECT_H
#define PGOBJECT_H

class ctlTree;
class ctlSQLBox;
class ctlListView;
class frmMain;
class pgDatabase;
class pgSchema;
class pgCollection;
class pgConn;
class pgSet;
class pgServer;


class pgTypes
{
public:
    wxChar *typName;
    long    typeIcon;
    wxChar *newString;
    wxChar *newLongString;
};


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


#endif

