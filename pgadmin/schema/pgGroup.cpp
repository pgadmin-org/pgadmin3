//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgGroup.cpp - Group class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgGroup.h"


pgGroup::pgGroup(const wxString& newName)
: pgServerObject(groupFactory, newName)
{
    wxLogInfo(wxT("Creating a pgGroup object"));
    memberCount=0;
}

pgGroup::~pgGroup()
{
    wxLogInfo(wxT("Destroying a pgGroup object"));
}

bool pgGroup::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
    return server->ExecuteVoid(wxT("DROP GROUP ") + GetQuotedFullIdentifier());
}

wxString pgGroup::GetSql(ctlTree *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Group: \"") + GetName() + wxT("\"\n\n")
            + wxT("DROP GROUP ") + GetQuotedFullIdentifier() + wxT(";")
            + wxT("\n\nCREATE Group ") + GetQuotedIdentifier()
            + wxT("\n  WITH SYSID ") + NumToStr(groupId)
            + wxT("\n  USER ") + quotedMembers
            +wxT(";\n");
    }
    return sql;
}


void pgGroup::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids=true;

        if (!memberIds.IsEmpty())
        {
            wxString ml=memberIds;
            ml.Replace(wxT(" "), wxT(","));
            pgSet *set=server->ExecuteSet(wxT(
                "SELECT usename FROM pg_user WHERE usesysid IN (") + ml + wxT(")"));
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
    }

    if (properties)
    {
        wxLogInfo(wxT("Displaying properties for Group ") + GetIdentifier());

        CreateListColumns(properties);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("Group ID"), GetGroupId());
        properties->AppendItem(_("Member count"), GetMemberCount());
        properties->AppendItem(_("Members"), GetMembers());
    }
}



pgObject *pgGroup::Refresh(ctlTree *browser, const wxTreeItemId item)
{
    pgObject *group=0;
    pgCollection *coll=browser->GetParentCollection(item);
    if (coll)
        group = groupFactory.CreateObjects(coll, 0, wxT("\n WHERE grosysid=") + NumToStr(GetGroupId()));

    return group;
}



pgObject *pgGroupFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
    pgGroup *group=0;

    pgSet *groups= collection->GetServer()->ExecuteSet(wxT(
        "SELECT * from pg_group") + restriction + wxT(" ORDER BY groname"));

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
                browser->AppendObject(collection, group);
	    		groups->MoveNext();
            }
            else
                break;
        }

		delete groups;
    }
    return group;
}


#include "images/group.xpm"
#include "images/groups.xpm"

pgGroupFactory::pgGroupFactory() 
: pgServerObjFactory(__("Group"), __("New Group..."), __("Create a new Group."), group_xpm)
{
}


pgGroupFactory groupFactory;
static pgaCollectionFactory cf(&groupFactory, __("Groups"), groups_xpm);
