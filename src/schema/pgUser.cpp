//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgUser.cpp - PostgreSQL User
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "pgUser.h"
#include "pgCollection.h"
#include "frmMain.h"


pgUser::pgUser(const wxString& newName)
: pgObject(PG_USER, newName)
{
    wxLogInfo(wxT("Creating a pgUser object"));
}

pgUser::~pgUser()
{
    wxLogInfo(wxT("Destroying a pgUser object"));
}



wxString pgUser::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- User: \"") + GetName() + wxT("\"\n")
            + wxT("CREATE User ") + GetQuotedIdentifier()
            + wxT("\n  WITH SYSID ") + NumToStr(userId);
        AppendIfFilled(sql, wxT("\n  PASSWORD ENCRYPTED "), GetPassword());
        sql += wxT("\n ");
        if (GetCreateDatabase())    sql += wxT(" CREATEDB");
        else                        sql += wxT(" NOCREATEDB");
        if (GetUpdateCatalog())     sql += wxT(" CREATEUSER");
        else                        sql += wxT(" NOCREATEUSER");
        AppendIfFilled(sql, wxT(" VALID UNTIL "), GetAccountExpires());
        sql +=wxT(";\n");
    }
    return sql;
}


void pgUser::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (properties)
    {
        wxLogInfo(wxT("Displaying properties for User ") + GetIdentifier());

        CreateListColumns(properties);
        int pos=0;

        InsertListItem(properties, pos++, wxT("Name"), GetName());
        InsertListItem(properties, pos++, wxT("User ID"), GetUserId());
        InsertListItem(properties, pos++, wxT("Account Expires"), GetAccountExpires());
        InsertListItem(properties, pos++, wxT("Superuser?"), BoolToYesNo(GetSuperuser()));
        InsertListItem(properties, pos++, wxT("Create Databases?"), BoolToYesNo(GetCreateDatabase()));
        InsertListItem(properties, pos++, wxT("Update Catalogs?"), BoolToYesNo(GetUpdateCatalog()));
        /*
        session default vars here?
        */
    }
}



pgObject *pgUser::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *user=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgObject *obj=(pgObject*)browser->GetItemData(parentItem);
        if (obj->GetType() == PG_USERS)
            user = ReadObjects((pgCollection*)obj, 0, wxT("\n WHERE usesysid=") + NumToStr(GetUserId()));
    }
    return user;
}



pgObject *pgUser::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgUser *user;

    pgSet *users= collection->GetServer()->ExecuteSet(wxT(
        "SELECT * FROM pg_shadow") + restriction);

    if (users)
    {
        while (!users->Eof())
        {

            user = new pgUser(users->GetVal(wxT("usename")));
            user->iSetServer(collection->GetServer());
            user->iSetUserId(users->GetLong(wxT("usesysid")));
            user->iSetCreateDatabase(StrToBool(users->GetVal(wxT("usecreatedb"))));
            user->iSetSuperuser(users->GetBool(wxT("usesuper")));
            user->iSetUpdateCatalog(users->GetBool(wxT("usecatupd")));
            user->iSetAccountExpires(users->GetVal(wxT("valuntil")));
            user->iSetPassword(users->GetVal(wxT("passwd")));

            if (browser)
            {
                browser->AppendItem(collection->GetId(), user->GetIdentifier(), PGICON_USER, -1, user);
				users->MoveNext();
            }
            else
                break;
        }

		delete users;
    }
    return user;
}



void pgUser::ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (browser->GetChildrenCount(collection->GetId(), FALSE) == 0)
    {

        // Log
        wxLogInfo(wxT("Adding Users to database"));


        // Get the Users
        ReadObjects(collection, browser);
    }
}

