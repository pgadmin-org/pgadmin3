//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// slSet.cpp PostgreSQL Slony-I Set
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgObject.h"
#include "slony/dlgRepSet.h"
#include "slony/slSet.h"
#include "slony/slCluster.h"
#include "slony/slNode.h"
#include "slony/slSequence.h"
#include "slony/slTable.h"
#include "slony/slSubscription.h"
#include "frm/frmMain.h"


slSet::slSet(slCluster *cl, const wxString &newName)
	: slObject(cl, setFactory, newName)
{
}

int slSet::GetIconId()
{
	if (GetOriginId() == GetCluster()->GetLocalNodeID())
		return setFactory.GetExportedIconId();
	else
		return setFactory.GetIconId();
}


wxMenu *slSet::GetNewMenu()
{
	wxMenu *menu = pgObject::GetNewMenu();

	if (GetOriginId() == GetCluster()->GetLocalNodeID())
	{
		slSequenceFactory.AppendMenu(menu);
		slTableFactory.AppendMenu(menu);
	}

	subscriptionFactory.AppendMenu(menu);

	return menu;
}


bool slSet::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	return GetDatabase()->ExecuteVoid(
	           wxT("SELECT ") + GetCluster()->GetSchemaPrefix()
	           + wxT("dropset(") + NumToStr(GetSlId()) + wxT(");\n"));
}


wxString slSet::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on Slony set");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing Slony set");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop Slony set \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop Slony set \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop Slony set cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop Slony set?");
			break;
		case PROPERTIESREPORT:
			message = _("Slony set properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Slony set properties");
			break;
		case DDLREPORT:
			message = _("Slony set DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Slony set DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("Slony set dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Slony set dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Slony set dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Slony set dependents");
			break;
	}

	return message;
}

wxString slSet::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Create replication set ") + GetName() + wxT(".\n\n")
		      wxT("SELECT ") + GetCluster()->GetSchemaPrefix() + wxT("storeset(")
		      + NumToStr(GetSlId()) + wxT(", ")
		      + qtDbString(GetComment()) + wxT(");\n");
	}
	return sql;
}

void slSet::ShowDependencies(frmMain *form, ctlListView *list, const wxString &wh)
{
	list->ClearAll();
	list->AddColumn(_("Type"), 60);
	list->AddColumn(_("Name"), 100);
	list->AddColumn(_("Comment"), 200);

	wxString where;
	if (wh.IsEmpty())
		where = wxT(" WHERE set_id=") + NumToStr(GetSlId());
	else
		where = wh;

	pgSet *set = GetDatabase()->ExecuteSet(
	                 wxT("SELECT relkind, nspname, relname, id, comment\n")
	                 wxT("  FROM pg_class cl\n")
	                 wxT("  JOIN pg_namespace nsp ON nsp.oid=cl.relnamespace\n")
	                 wxT("  JOIN (\n")
	                 wxT("       SELECT tab_id AS id, tab_reloid AS oid, tab_altered AS altered, tab_comment AS comment\n")
	                 wxT("         FROM ") + GetCluster()->GetSchemaPrefix() + wxT("sl_table t\n")
	                 wxT("         JOIN ") + GetCluster()->GetSchemaPrefix() + wxT("sl_set s ON tab_set=set_id ") + where + wxT("\n")
	                 wxT("       UNION\n")
	                 wxT("       SELECT seq_id, seq_reloid, NULL, seq_comment\n")
	                 wxT("         FROM ") + GetCluster()->GetSchemaPrefix() + wxT("sl_sequence t\n")
	                 wxT("         JOIN ") + GetCluster()->GetSchemaPrefix() + wxT("sl_set s ON seq_set=set_id ") + where + wxT("\n")
	                 wxT("       ) AS set ON set.oid=cl.oid"));

	if (set)
	{
		while (!set->Eof())
		{
			wxString name = set->GetVal(wxT("nspname")) + wxT(".") + set->GetVal(wxT("relname"));
			wxString typestr = set->GetVal(wxT("relkind"));
			wxString comment = set->GetVal(wxT("comment"));
			wxString typname;
			int icon = -1;
			switch ( (wxChar)typestr.c_str()[0])
			{
				case 'S':
					typname = _("Sequence");
					icon = -1;
					break;
				case 'r':
					typname = _("Table");
					icon = -1;
					break;
			}

			list->AppendItem(icon, typname, name, comment);

			set->MoveNext();
		}
		delete set;
	}
}


wxString slSet::GetLockXXID()
{
	return GetConnection()->ExecuteScalar(
	           wxT("SELECT set_locked FROM ")
	           + GetCluster()->GetSchemaPrefix() + wxT("sl_set\n")
	           wxT(" WHERE set_id=") + NumToStr(GetSlId()));
}


bool slSet::Lock()
{
	return GetConnection()->ExecuteVoid(wxT("SELECT ") + GetCluster()->GetSchemaPrefix()
	                                    + wxT("lockSet(") + NumToStr(GetSlId()) + wxT(");"));
}


bool slSet::Unlock()
{
	return GetConnection()->ExecuteVoid(wxT("SELECT ") + GetCluster()->GetSchemaPrefix()
	                                    + wxT("unlockSet(") + NumToStr(GetSlId()) + wxT(");"));
}


bool slSet::CanDrop()
{
	if (GetMetaType() != SLM_SUBSCRIPTION)
		return !GetSubscriptionCount() && GetOriginId() == GetCluster()->GetLocalNodeID();
	else
		return GetOriginId() != GetCluster()->GetLocalNodeID();
}


void slSet::ShowDependents(frmMain *form, ctlListView *referencedBy, const wxString &wh)
{
}


void slSet::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (!expandedKids)
	{
		expandedKids = true;

		browser->RemoveDummyChild(this);
		// Log
		wxLogInfo(wxT("Adding child object to set %s"), GetIdentifier().c_str());

		if (GetOriginId() == GetCluster()->GetLocalNodeID())
		{
			browser->AppendCollection(this, slSequenceFactory);
			browser->AppendCollection(this, slTableFactory);
		}
		browser->AppendCollection(this, subscriptionFactory);
	}


	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("ID"), GetSlId());
		properties->AppendItem(_("Origin ID"), GetOriginId());
		properties->AppendItem(_("Origin Node"), GetOriginNode());
		properties->AppendItem(_("Subscriptions"), GetSubscriptionCount());

		wxString lockXXID = GetLockXXID();

		if (!lockXXID.IsEmpty())
			properties->AppendItem(_("Lock XXID"), lockXXID);

		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
	}
}



pgObject *slSet::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *set = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		set = setFactory.CreateObjects(coll, 0, wxT(" WHERE set_id=") + NumToStr(GetSlId()) + wxT("\n"));

	return set;
}



pgObject *slSetFactory::CreateObjects(pgCollection *coll, ctlTree *browser, const wxString &restriction)
{
	slSet *set = 0;
	slObjCollection *collection = (slObjCollection *)coll;
	wxString prefix = collection->GetCluster()->GetSchemaPrefix();

	pgSet *sets = collection->GetDatabase()->ExecuteSet(
	                  wxT("SELECT set_id, set_origin, no_comment, set_comment,\n")
	                  wxT("       (SELECT COUNT(1) FROM ") + prefix + wxT("sl_subscribe where sub_set=set_id) AS subcount\n")
	                  wxT("  FROM ") + prefix + wxT("sl_set\n")
	                  wxT("  JOIN ") + prefix + wxT("sl_node ON set_origin=no_id\n")
	                  + restriction +
	                  wxT(" ORDER BY set_id"));

	if (sets)
	{
		while (!sets->Eof())
		{
			set = new slSet(collection->GetCluster(), sets->GetVal(wxT("set_comment")).BeforeFirst('\n'));
			set->iSetSlId(sets->GetLong(wxT("set_id")));
			set->iSetOriginId(sets->GetLong(wxT("set_origin")));
			set->iSetOriginNode(sets->GetVal(wxT("no_comment")));
			set->iSetSubscriptionCount(sets->GetLong(wxT("subcount")));
			set->iSetComment(sets->GetVal(wxT("set_comment")));

			if (browser)
			{
				browser->AppendObject(coll, set);
				sets->MoveNext();
			}
			else
				break;
		}

		delete sets;
	}
	return set;
}


wxString slSetCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on Slony sets");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing Slony sets");
			break;
		case OBJECTSLISTREPORT:
			message = _("Slony sets list report");
			break;
	}

	return message;
}


//////////////////////////////////////////////////

#include "images/slset.pngc"
#include "images/slset2.pngc"
#include "images/slsets.pngc"

slSetFactory::slSetFactory()
	: slObjFactory(__("Set"), __("New Replication Set"), __("Create a new Replication Set."), slset2_png_img)
{
	exportedIconId = addIcon(slset_png_img);
	metaType = SLM_SET;
}


pgCollection *slSetFactory::CreateCollection(pgObject *obj)
{
	return new slSetCollection(GetCollectionFactory(), (slCluster *)obj);
}


slSetObject::slSetObject(slSet *s, pgaFactory &factory, const wxString &newName)
	: slObject(s->GetCluster(), factory, newName)
{
	set = s;
}

bool slSetObject::CanDrop()
{
	if (GetMetaType() != SLM_SUBSCRIPTION)
		return !set->GetSubscriptionCount() && set->GetOriginId() == GetCluster()->GetLocalNodeID();
	else
	{
		if (GetCluster()->ClusterMinimumVersion(1, 1))
			return (set->GetOriginId() == GetCluster()->GetLocalNodeID() || ((slSubscription *)this)->GetForward());
		else
			return set->GetOriginId() != GetCluster()->GetLocalNodeID();
	}
}


bool slSetObject::CanCreate()
{
	if (GetMetaType() != SLM_SUBSCRIPTION)
		return !set->GetSubscriptionCount() && set->GetOriginId() == GetCluster()->GetLocalNodeID();
	else
	{
		if (GetCluster()->ClusterMinimumVersion(1, 1))
			return (set->GetOriginId() == GetCluster()->GetLocalNodeID() || ((slSubscription *)this)->GetForward());
		else
			return set->GetOriginId() != GetCluster()->GetLocalNodeID();
	}
}


slSetObjCollection::slSetObjCollection(pgaFactory *factory, slSet *_set)
	: slObjCollection(factory, _set->GetCluster())
{
	set = _set;
	subscription = 0;
	iSetSlId(set->GetSlId());
}


bool slSetObjCollection::CanCreate()
{
	switch (GetMetaType())
	{
		case SLM_SUBSCRIPTION:
			if (GetCluster()->ClusterMinimumVersion(1, 1))
				return (set->GetOriginId() == GetCluster()->GetLocalNodeID() || ((slSubscription *)this)->GetForward());
			else
				return set->GetOriginId() != GetCluster()->GetLocalNodeID();

		case SLM_TABLE:
		case SLM_SEQUENCE:
			if (set->GetSubscriptionCount())
				return false;
			else
				return set->GetOriginId() == GetCluster()->GetLocalNodeID();
		default:
			return false;
	}
}


pgCollection *slSetObjFactory::CreateCollection(pgObject *obj)
{
	return new slSetObjCollection(GetCollectionFactory(), (slSet *)obj);
}

slSetFactory setFactory;
static pgaCollectionFactory cf(&setFactory, __("Replication Sets"), slsets_png_img);

////////////////////////////////////////////////////////////

slonyMergeSetFactory::slonyMergeSetFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("Merge Set"), _("Merge two replication sets."));
}


wxWindow *slonyMergeSetFactory::StartDialog(frmMain *form, pgObject *obj)
{
	dlgProperty *dlg = new dlgRepSetMerge(&setFactory, form, (slSet *)obj);
	dlg->InitDialog(form, obj);
	dlg->CreateAdditionalPages();
	dlg->Go(false);
	dlg->CheckChange();
	return 0;
}


bool slonyMergeSetFactory::CheckEnable(pgObject *obj)
{
	if (!obj || !obj->IsCreatedBy(setFactory))
		return false;

	slSet *set = (slSet *)obj;

	return set->GetOriginId() == set->GetCluster()->GetLocalNodeID();
}


/////////////////////////////

slonyLockSetFactory::slonyLockSetFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("Lock Set"), _("Lock a replication set against updates."));
}


wxWindow *slonyLockSetFactory::StartDialog(frmMain *form, pgObject *obj)
{
	slSet *set = (slSet *)obj;

	if (set->GetCluster()->GetLocalNode(form->GetBrowser())->CheckAcksAndContinue(form))
	{
		if (set->Lock())
			form->Refresh(set);
	}
	return 0;
}


bool slonyLockSetFactory::CheckEnable(pgObject *obj)
{
	if (!obj || !obj->IsCreatedBy(setFactory))
		return false;

	slSet *set = (slSet *)obj;

	return set->GetOriginId() == set->GetCluster()->GetLocalNodeID() && set->GetLockXXID().IsEmpty();
}


/////////////////////////////

slonyUnlockSetFactory::slonyUnlockSetFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("Unlock Set"), _("Unlock a replication set and re-allow updates."));
}


wxWindow *slonyUnlockSetFactory::StartDialog(frmMain *form, pgObject *obj)
{
	slSet *set = (slSet *)obj;

	if (set->GetCluster()->GetLocalNode(form->GetBrowser())->CheckAcksAndContinue(form))
	{
		if (set->Unlock())
			form->Refresh(set);
	}
	return 0;
}


bool slonyUnlockSetFactory::CheckEnable(pgObject *obj)
{
	if (!obj || !obj->IsCreatedBy(setFactory))
		return false;

	slSet *set = (slSet *)obj;

	return set->GetOriginId() == set->GetCluster()->GetLocalNodeID() && !set->GetLockXXID().IsEmpty();
}


////////////////////////////////


slonyMoveSetFactory::slonyMoveSetFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("Move Set"), _("Move replication set to different node"));
}


wxWindow *slonyMoveSetFactory::StartDialog(frmMain *form, pgObject *obj)
{
	dlgProperty *dlg = new dlgRepSetMove(&setFactory, form, (slSet *)obj);
	dlg->InitDialog(form, obj);
	dlg->CreateAdditionalPages();
	dlg->Go(false);
	dlg->CheckChange();
	return 0;
}


bool slonyMoveSetFactory::CheckEnable(pgObject *obj)
{
	if (!obj || ! obj->IsCreatedBy(setFactory))
		return false;

	slSet *set = (slSet *)obj;

	return set->GetOriginId() == set->GetCluster()->GetLocalNodeID()  && !set->GetLockXXID().IsEmpty();
}



