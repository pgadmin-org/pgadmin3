//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgUser.h - PostgreSQL User
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGUSER_H
#define PGUSER_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgConn.h"
#include "pgObject.h"
#include "pgServer.h"

// Class declarations
class pgUser : public pgObject
{
public:
    pgUser(const wxString& newName = wxString(""));
    ~pgUser();

    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0);
    static void ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane);
    wxString GetSql(wxTreeCtrl *browser);

    // User Specific
    void iSetServer(pgServer *s) { server=s; }
    long GetUserId() const { return userId; }
    void iSetUserId(const long l) { userId=l; }
    wxString GetAccountExpires() const { return accountExpires; }
    void iSetAccountExpires(const wxString& s) { accountExpires=s; }
    wxString GetPassword() const { return password; }
    void iSetPassword(const wxString& s) { password=s; }
    bool GetCreateDatabase() const { return createDatabase; }
    void iSetCreateDatabase(const bool b) { createDatabase=b; }
    bool GetSuperuser() const { return superuser; }
    void iSetSuperuser(const bool b) { superuser=b; }
    bool GetUpdateCatalog() const { return updateCatalog; }
    void iSetUpdateCatalog(const bool b) { updateCatalog=b; }

private:
    wxString accountExpires, password;
    bool createDatabase, superuser, updateCatalog;
    long userId;
    pgServer *server;
};

#endif
