//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
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
#include "frmMain.h"


pgGroup::pgGroup(const wxString& newName)
: pgObject(PG_GROUP, newName)
{
    wxLogInfo(wxT("Creating a pgGroup object"));
    memberCount=0;
}

pgGroup::~pgGroup()
{
    wxLogInfo(wxT("Destroying a pgGroup object"));
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
                if (memberCount)
                {
                    members += wxT(", ");
                    quotedMembers += wxT(", ");
                }
                members += set->GetVal(0);
                quotedMembers += qtIdent(set->GetVal(0));
                memberCount++;
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



void pgGroup::ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    pgGroup *group;

    if (browser->GetChildrenCount(collection->GetId(), FALSE) == 0)
    {

        wxLogInfo(wxT("Adding Groups to database"));

        // Get the Groups
        pgSet *groups= collection->GetServer()->ExecuteSet(wxT("SELECT * from pg_group"));

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

                browser->AppendItem(collection->GetId(), group->GetIdentifier(), PGICON_GROUP, -1, group);
	    
			    groups->MoveNext();
            }

		    delete groups;
        }
    }
}

