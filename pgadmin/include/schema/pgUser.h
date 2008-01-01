//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgUser.h - PostgreSQL User
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGUSER_H
#define PGUSER_H

#include "pgServer.h"

class pgUserFactory : public pgServerObjFactory
{
public:
    pgUserFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern pgUserFactory userFactory;


// Class declarations
class pgUser : public pgServerObject
{
public:
    pgUser(const wxString& newName = wxT(""));

    // User Specific
    bool GetSystemObject() const { return userId < 100; }
    long GetUserId() const { return userId; }
    void iSetUserId(const long l) { userId=l; }
    wxDateTime GetAccountExpires() const { return accountExpires; }
    void iSetAccountExpires(const wxDateTime & dt) { accountExpires=dt; }
    wxString GetPassword() const { return password; }
    void iSetPassword(const wxString& s) { password=s; }
    bool GetCreateDatabase() const { return createDatabase; }
    void iSetCreateDatabase(const bool b) { createDatabase=b; }
    bool GetSuperuser() const { return superuser; }
    void iSetSuperuser(const bool b) { superuser=b; }
    bool GetUpdateCatalog() const { return updateCatalog; }
    void iSetUpdateCatalog(const bool b) { updateCatalog=b; }
    wxArrayString& GetGroupsIn() { return groupsIn; }
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
    bool createDatabase, superuser, updateCatalog;
    wxArrayString groupsIn;
    wxArrayString configList;
    long userId;
};

#endif
