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
: pgServerObject(PG_USER, newName)
{
    wxLogInfo(wxT("Creating a pgUser object"));
}

pgUser::~pgUser()
{
    wxLogInfo(wxT("Destroying a pgUser object"));
}



bool pgUser::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return server->ExecuteVoid(wxT("DROP USER ") + GetQuotedFullIdentifier());
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
        if (!configList.IsEmpty())
        {
            wxStringTokenizer cfgTokens(configList, ',');
            while (cfgTokens.HasMoreTokens())
            {
                sql += wxT("ALTER USER ") + GetQuotedIdentifier()
                    + wxT(" SET ") + cfgTokens.GetNextToken() + wxT(";\n");
            }
        }
        unsigned index;
        for (index=0 ; index < groupsIn.GetCount() ; index++)
            sql += wxT("ALTER GROUP ") + qtIdent(groupsIn.Item(index))
                +  wxT(" ADD USER ") + GetQuotedIdentifier() + wxT(";\n");

    }
    return sql;
}


void pgUser::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids=true;

        pgSet *set=GetServer()->ExecuteSet(wxT("SELECT groname, grolist FROM pg_group ORDER BY groname"));
        if (set)
        {
            while (!set->Eof())
            {
                wxString groupName=set->GetVal(wxT("groname"));
                wxString str=set->GetVal(wxT("grolist"));
                if (!str.IsNull())
                {
                    wxStringTokenizer ids(str.Mid(1, str.Length()-2), ',');
                    while (ids.HasMoreTokens())
                    {
                        if (atoi(ids.GetNextToken()) == GetUserId())
                        {
                            groupsIn.Add(groupName);
                            break;
                        }
                    }
                }
                set->MoveNext();
            }
            delete set;
        }
    }
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

        wxString groupList;

        unsigned index;
        for (index=0 ; index < groupsIn.GetCount() ; index++)
        {
            if (!groupList.IsEmpty())
                groupList += wxT(", ");
            groupList += groupsIn.Item(index);
        }
        InsertListItem(properties, pos, wxT("Member of"), groupList);

        wxStringTokenizer cfgTokens(configList, ',');
        while (cfgTokens.HasMoreTokens())
        {
            wxString token=cfgTokens.GetNextToken();
            wxString varName=token.BeforeFirst('=');
            InsertListItem(properties, pos++, varName, token.Mid(varName.Length()+1));
        }
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
            wxString str=users->GetVal(wxT("useconfig"));
            if (!str.IsEmpty())
                user->iSetConfigList(str.Mid(1, str.Length()-2));

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

