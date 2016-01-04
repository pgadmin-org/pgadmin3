//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgGroup.cpp - PostgreSQL group (only used on pre-8.1 versions, pgRole
//              is used on newer versions).
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgGroup.h"


pgGroup::pgGroup(const wxString &newName)
	: pgServerObject(groupFactory, newName)
{
	memberCount = 0;
}

wxString pgGroup::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on group");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing group");
			message += wxT(" ") + GetName();
			break;
		case GRANTWIZARDTITLE:
			message = _("Privileges for group");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop group \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop group \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop group cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop group?");
			break;
		case PROPERTIESREPORT:
			message = _("Group properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Group properties");
			break;
		case DDLREPORT:
			message = _("Group DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Group DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("Group dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Group dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Group dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Group dependents");
			break;
	}

	return message;
}

bool pgGroup::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	return server->ExecuteVoid(wxT("DROP GROUP ") + GetQuotedFullIdentifier());
}

wxString pgGroup::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Group: ") + GetName() + wxT("\n\n")
		      + wxT("DROP GROUP ") + GetQuotedFullIdentifier() + wxT(";")
		      + wxT("\n\nCREATE Group ") + GetQuotedIdentifier()
		      + wxT("\n  WITH SYSID ") + NumToStr(groupId)
		      + wxT("\n  USER ") + quotedMembers
		      + wxT(";\n");
	}
	return sql;
}


void pgGroup::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (!expandedKids)
	{
		expandedKids = true;

		if (!memberIds.IsEmpty())
		{
			wxString ml = memberIds;
			ml.Replace(wxT(" "), wxT(","));
			pgSet *set = server->ExecuteSet(wxT(
			                                    "SELECT usename FROM pg_user WHERE usesysid IN (") + ml + wxT(")"));
			if (set)
			{
				while (!set->Eof())
				{
					wxString user = set->GetVal(0);
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
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("Group ID"), GetGroupId());
		properties->AppendItem(_("Member count"), GetMemberCount());
		properties->AppendItem(_("Members"), GetMembers());
	}
}



pgObject *pgGroup::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *group = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		group = groupFactory.CreateObjects(coll, 0, wxT("\n WHERE grosysid=") + NumToStr(GetGroupId()));

	return group;
}



pgObject *pgGroupFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	pgGroup *group = 0;

	pgSet *groups = collection->GetServer()->ExecuteSet(wxT(
	                    "SELECT * from pg_group") + restriction + wxT(" ORDER BY groname"));

	if (groups)
	{
		while (!groups->Eof())
		{
			group = new pgGroup(groups->GetVal(wxT("groname")));
			group->iSetGroupId(groups->GetLong(wxT("grosysid")));
			group->iSetServer(collection->GetServer());
			wxString mids = groups->GetVal(wxT("grolist"));
			mids = mids.Mid(1, mids.Length() - 2);
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

wxString pgGroupCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on groups");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing groups");
			break;
		case OBJECTSLISTREPORT:
			message = _("Groups list report");
			break;
	}

	return message;
}

#include "images/group.pngc"
#include "images/groups.pngc"

pgGroupFactory::pgGroupFactory()
	: pgServerObjFactory(__("Group"), __("New Group..."), __("Create a new Group."), group_png_img)
{
}


pgGroupFactory groupFactory;
static pgaCollectionFactory cf(&groupFactory, __("Groups"), groups_png_img);
