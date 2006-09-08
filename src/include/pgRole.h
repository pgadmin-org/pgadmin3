//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgRole.h - PostgreSQL Role
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGROLE_H
#define PGROLE_H

#include "pgServer.h"


#define PGROLE_ADMINOPTION      wxT("(*)")
#define PGROLE_ADMINOPTION_LEN  3


class pgRoleBaseFactory : public pgServerObjFactory
{
public:
    pgRoleBaseFactory(const wxChar *tn, const wxChar *ns, const wxChar *nls, char **img);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr);
};

class pgLoginRoleFactory : public pgRoleBaseFactory
{
public:
    pgLoginRoleFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr);
};

class pgGroupRoleFactory : public pgRoleBaseFactory
{
public:
    pgGroupRoleFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr);
};

extern pgLoginRoleFactory loginRoleFactory;
extern pgGroupRoleFactory groupRoleFactory;


// Class declarations
class pgRole : public pgServerObject
{
protected:
    pgRole(pgaFactory &factory, const wxString& newName = wxT(""));

public:
    ~pgRole();
    int GetIconId();


    // Role Specific
    wxDateTime GetAccountExpires() const { return accountExpires; }
    void iSetAccountExpires(const wxDateTime & dt) { accountExpires=dt; }
    wxString GetPassword() const { return password; }
    void iSetPassword(const wxString& s) { password=s; }
    void iSetInherits(const bool b) { inherits = b; }
    bool GetInherits() const { return inherits; }
    void iSetCanLogin(const bool b) { canLogin = b; }
    bool GetCanLogin() const { return canLogin; }
    bool GetCreateDatabase() const { return createDatabase; }
    void iSetCreateDatabase(const bool b) { createDatabase=b; }
    bool GetCreateRole() const { return createRole; }
    void iSetCreateRole(const bool b) { createRole=b; }
    bool GetSuperuser() const { return superuser; }
    void iSetSuperuser(const bool b) { superuser=b; }
    bool GetUpdateCatalog() const { return updateCatalog; }
    void iSetUpdateCatalog(const bool b) { updateCatalog=b; }
    wxArrayString& GetRolesIn() { return rolesIn; }
    wxArrayString& GetConfigList() { return configList; }


    // Tree object creation
    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    void ShowDependents(frmMain *form, ctlListView *referencedBy, const wxString &where);
    
    // virtual methods
    wxString GetSql(ctlTree *browser);
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);
    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);

    bool HasStats() { return false; }
    bool HasDepends() { return true; }
    bool HasReferences() { return true; }
private:
    wxString password;
    wxDateTime accountExpires;
    bool superuser, createDatabase, createRole, updateCatalog, inherits, canLogin;
    wxArrayString rolesIn;
    wxArrayString configList;
};



class pgLoginRole : public pgRole
{
public:
    pgLoginRole(const wxString& newName = wxT(""));
};


class pgGroupRole : public pgRole
{
public:
    pgGroupRole(const wxString& newName = wxT(""));
};



#endif
