//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
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
    if (GetUpdateCatalog())
    {
        wxMessageDialog dlg(frame, 
            _("Deleting a superuser might result in unwanted behaviour (e.g. when restoring the database).\nAre you sure?"),
            _("Confirm superuser deletion"),
                     wxICON_EXCLAMATION | wxYES_NO |wxNO_DEFAULT);
        if (dlg.ShowModal() != wxID_YES)
            return false;
    }
    return server->ExecuteVoid(wxT("DROP USER ") + GetQuotedFullIdentifier() + wxT(";"));
}


wxString pgUser::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- User: \"") + GetName() + wxT("\"\n\n")
            + wxT("-- DROP USER ") + GetQuotedFullIdentifier() + wxT(";")
            + wxT("\n\nCREATE USER ") + GetQuotedIdentifier()
            + wxT("\n  WITH SYSID ") + NumToStr(userId);
        if (GetPassword() != wxT("********"))
            AppendIfFilled(sql, wxT("\n  PASSWORD ENCRYPTED "), GetPassword());
        sql += wxT("\n ");
        if (GetCreateDatabase())    sql += wxT(" CREATEDB");
        else                        sql += wxT(" NOCREATEDB");
        if (GetUpdateCatalog())     sql += wxT(" CREATEUSER");
        else                        sql += wxT(" NOCREATEUSER");
        if (GetAccountExpires().IsValid())
        AppendIfFilled(sql, wxT(" VALID UNTIL "), qtString(DateToAnsiStr(GetAccountExpires())));
        sql +=wxT(";\n");

        size_t index;
        for (index=0 ; index < configList.GetCount() ; index++)
        {
            sql += wxT("ALTER USER ") + GetQuotedIdentifier()
                + wxT(" SET ") + configList.Item(index) + wxT(";\n");
        }
        for (index=0 ; index < groupsIn.GetCount() ; index++)
            sql += wxT("ALTER GROUP ") + qtIdent(groupsIn.Item(index))
                +  wxT(" ADD USER ") + GetQuotedIdentifier() + wxT(";\n");

    }
    return sql;
}


void pgUser::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
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
                    wxStringTokenizer ids(str.Mid(1, str.Length()-2), wxT(","));
                    while (ids.HasMoreTokens())
                    {
                        if (StrToLong(ids.GetNextToken()) == GetUserId())
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

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("User ID"), GetUserId());
        properties->AppendItem(_("Account expires"), GetAccountExpires());
        properties->AppendItem(_("Superuser?"), BoolToYesNo(GetSuperuser()));
        properties->AppendItem(_("Create databases?"), BoolToYesNo(GetCreateDatabase()));
        properties->AppendItem(_("Update catalogs?"), BoolToYesNo(GetUpdateCatalog()));

        wxString groupList;

        size_t index;
        for (index=0 ; index < groupsIn.GetCount() ; index++)
        {
            if (!groupList.IsEmpty())
                groupList += wxT(", ");
            groupList += groupsIn.Item(index);
        }
        properties->AppendItem(wxT("Member of"), groupList);

        for (index=0; index < configList.GetCount() ; index++)
        {
            wxString item=configList.Item(index);
            properties->AppendItem(item.BeforeFirst('='), item.AfterFirst('='));
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
    pgUser *user=0;

    wxString tabname;

    if (collection->GetServer()->HasPrivilege(wxT("table"), wxT("pg_shadow"), wxT("SELECT")))
        tabname=wxT("pg_shadow");
    else
        tabname=wxT("pg_user");

    pgSet *users = collection->GetServer()->ExecuteSet(wxT(
        "SELECT * FROM ") + tabname + restriction + wxT(" ORDER BY usename"));

    if (users)
    {
        while (!users->Eof())
        {

            user = new pgUser(users->GetVal(wxT("usename")));
            user->iSetServer(collection->GetServer());
            user->iSetUserId(users->GetLong(wxT("usesysid")));
            user->iSetCreateDatabase(users->GetBool(wxT("usecreatedb")));
            user->iSetSuperuser(users->GetBool(wxT("usesuper")));
            user->iSetUpdateCatalog(users->GetBool(wxT("usecatupd")));
            user->iSetAccountExpires(users->GetDateTime(wxT("valuntil")));
            user->iSetPassword(users->GetVal(wxT("passwd")));

            wxString cfg=users->GetVal(wxT("useconfig"));
            if (!cfg.IsEmpty())
                FillArray(user->GetConfigList(), cfg.Mid(1, cfg.Length()-2));

            if (browser)
            {
                collection->AppendBrowserItem(browser, user);
				users->MoveNext();
            }
            else
                break;
        }

		delete users;
    }
    return user;
}
