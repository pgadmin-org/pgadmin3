//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgGroup.cpp - Group class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "pgGroup.h"
#include "pgCollection.h"


pgGroup::pgGroup(const wxString& newName)
: pgServerObject(PG_GROUP, newName)
{
    wxLogInfo(wxT("Creating a pgGroup object"));
    memberCount=0;
}

pgGroup::~pgGroup()
{
    wxLogInfo(wxT("Destroying a pgGroup object"));
}

bool pgGroup::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return server->ExecuteVoid(wxT("DROP GROUP ") + GetQuotedFullIdentifier());
}

wxString pgGroup::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Group: \"") + GetName() + wxT("\"\n")
            + wxT("CREATE Group ") + GetQuotedIdentifier()
            + wxT("\n  WITH SYSID ") + NumToStr(groupId)
            + wxT("\n  USER ") + quotedMembers
            +wxT(";\n");
    }
    return sql;
}


void pgGroup::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids=true;

        wxString ml=memberIds;
        ml.Replace(" ", ",");
        pgSet *set=server->ExecuteSet(wxT(
            "SELECT usename FROM pg_shadow WHERE usesysid IN (") + ml + wxT(")"));
        if (set)
        {
            while (!set->Eof())
            {
                wxString user=set->GetVal(0);
                if (memberCount)
                {
                    members += wxT(", ");
                    quotedMembers += wxT(", ");
                }
                members += user;
                quotedMembers += qtIdent(user);
                memberCount++;
                usersIn.Add(user);
                set->MoveNext();
            }
            delete set;
        }
    }

    if (properties)
    {
        wxLogInfo(wxT("Displaying properties for Group ") + GetIdentifier());

        CreateListColumns(properties);
        int pos=0;

        InsertListItem(properties, pos++, wxT("Name"), GetName());
        InsertListItem(properties, pos++, wxT("Group ID"), GetGroupId());
        InsertListItem(properties, pos++, wxT("Member Count"), GetMemberCount());
        InsertListItem(properties, pos++, wxT("Members"), GetMembers());
    }
}



pgObject *pgGroup::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *group=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgObject *obj=(pgObject*)browser->GetItemData(parentItem);
        if (obj->GetType() == PG_GROUPS)
            group = ReadObjects((pgCollection*)obj, 0, wxT("\n WHERE grosysid=") + NumToStr(GetGroupId()));
    }
    return group;
}



pgObject *pgGroup::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgGroup *group=0;

    pgSet *groups= collection->GetServer()->ExecuteSet(wxT(
        "SELECT * from pg_group") + restriction);

    if (groups)
    {
        while (!groups->Eof())
        {
            group = new pgGroup(groups->GetVal(wxT("groname")));
            group->iSetGroupId(groups->GetLong(wxT("grosysid")));
            group->iSetServer(collection->GetServer());
            wxString mids=groups->GetVal(wxT("grolist"));
            mids = mids.Mid(1, mids.Length()-2);
            group->iSetMemberIds(mids);

            if (browser)
            {
                collection->AppendBrowserItem(browser, group);
	    		groups->MoveNext();
            }
            else
                break;
        }

		delete groups;
    }
    return group;
}
