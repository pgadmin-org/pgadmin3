//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gpResQueue.cpp - Greenplum Resource Queue
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/choicdlg.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/gpResQueue.h"
#include "frm/frmMain.h"
#include "utils/pgDefs.h"
#include "schema/pgDatabase.h"


gpResQueue::gpResQueue(pgaFactory &factory, const wxString &newName)
	: pgServerObject(factory, newName)
{
}

gpResQueue::gpResQueue(const wxString &newName)
	: pgServerObject(resQueueFactory, newName)
{
}

wxString gpResQueue::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on resource queue");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing resource queue");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop resource queue \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop resource queue \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop resource queue cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop resource queue?");
			break;
		case PROPERTIESREPORT:
			message = _("Resource queue properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Resource queue properties");
			break;
		case DDLREPORT:
			message = _("Resource queue DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Resource queue DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("Resource queue dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Resource queue dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Resource queue dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Resource queue dependents");
			break;
	}

	return message;
}

int gpResQueue::GetIconId()
{
	return resQueueFactory.GetIconId();
}



bool gpResQueue::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	return server->ExecuteVoid(wxT("DROP RESOURCE QUEUE ") + GetQuotedFullIdentifier() + wxT(";"));
}


wxString gpResQueue::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Resource Queue: \"") + GetName() + wxT("\"\n\n")
		      + wxT("-- DROP RESOURCE QUEUE ") + GetQuotedFullIdentifier() + wxT(";")
		      + wxT("\n\nCREATE RESOURCE QUEUE ") + GetQuotedIdentifier();

		if (GetCountLimit() != -1.0)
		{
			sql += wxT(" ACTIVE THRESHOLD ");
			sql += NumToStr(GetCountLimit());
		}
		if (GetCostLimit() != -1.0)
		{
			sql += wxT(" COST THRESHOLD ");
			sql += NumToStr(GetCostLimit());
		}
		if (GetIgnoreCostLimit() != 0.0)
		{
			sql += wxT(" IGNORE THRESHOLD ");
			sql += NumToStr(GetIgnoreCostLimit());
		}
		if (GetOvercommit())
			sql += wxT(" OVERCOMMIT");
		else
			sql += wxT(" NOOVERCOMMIT");
	}
	return sql;
}






void gpResQueue::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (!expandedKids)
	{
		expandedKids = true;
		wxString queuesquery;

		queuesquery = wxT("SELECT rolname,\n")
		              wxT(" pg_catalog.shobj_description(r.oid, 'pg_authid') AS description\n");

		queuesquery += wxT("  FROM pg_roles r\n")
		               wxT("  JOIN pg_resqueue q ON  rolresqueue=q.oid\n")
		               wxT(" WHERE  rolresqueue=") + GetOidStr() + wxT("\n")
		               wxT(" ORDER BY rolname");

		pgSetIterator queues(GetConnection(), queuesquery);

		while (queues.RowsLeft())
		{
			wxString queue = queues.GetVal(wxT("rolname"));

			queuesIn.Add(queue);
		}
	}
	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetOid());
		properties->AppendItem(_("Active threshold"), GetCountLimit());
		properties->AppendItem(_("Cost threshold"), GetCostLimit());
		properties->AppendItem(_("Ignore threshold"), GetIgnoreCostLimit());
		properties->AppendItem(_("Over commit?"), BoolToYesNo(GetOvercommit()));

		wxString roleList;

		size_t index;
		for (index = 0 ; index < queuesIn.GetCount() ; index++)
		{
			if (!roleList.IsEmpty())
				roleList += wxT(", ");
			roleList += queuesIn.Item(index);
		}
		properties->AppendItem(_("Roles using this"), roleList);
		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));

	}
}




pgObject *gpResQueue::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *queue = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		queue = resQueueFactory.CreateObjects(coll, 0, wxT("\n WHERE oid=") + GetOidStr());

	return queue;
}



pgObject *gpResQueueFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	gpResQueue *queue = 0;
	pgSet *queues = 0;



	queues = collection->GetServer()->ExecuteSet(wxT("SELECT rsqname, rsqcountlimit, rsqcostlimit, rsqovercommit, rsqignorecostlimit, oid FROM pg_resqueue ORDER BY 1"));

	if (queues)
	{
		while (!queues->Eof())
		{
			queue = new gpResQueue(queues->GetVal(wxT("rsqname")));

			queue->iSetServer(collection->GetServer());
			queue->iSetCountLimit(queues->GetDouble(wxT("rsqcountlimit")));
			queue->iSetCostLimit(queues->GetDouble(wxT("rsqcostlimit")));
			queue->iSetOvercommit(queues->GetBool(wxT("rsqovercommit")));
			queue->iSetIgnoreCostLimit(queues->GetDouble(wxT("rsqignorecostlimit")));
			queue->iSetOid(queues->GetOid(wxT("oid")));

			if (browser)
			{
				browser->AppendObject(collection, queue);
				queues->MoveNext();
			}
			else
				break;
		}

		delete queues;
	}
	return queue;
}

/////////////////////////////

gpResQueueCollection::gpResQueueCollection(pgaFactory *factory, pgServer *sv)
	: pgServerObjCollection(factory, sv)
{
}

wxString gpResQueueCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on resource queues");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing resource queues");
			break;
		case GRANTWIZARDTITLE:
			message = _("Privileges for resource queues");
			break;
		case OBJECTSLISTREPORT:
			message = _("Ressource queues list report");
			break;
	}

	return message;
}

/////////////////////////////

#include "images/group.pngc"
#include "images/roles.pngc"


gpResQueueFactory::gpResQueueFactory()
	: pgServerObjFactory(__("Resource Queue"), __("New Resource Queue..."), __("Create a new Resource Queue."), group_png_img)
{
	metaType = GP_RESOURCE_QUEUE;
}


pgCollection *gpResQueueFactory::CreateCollection(pgObject *obj)
{
	return new gpResQueueCollection(GetCollectionFactory(), (pgServer *)obj);
}

gpResQueueFactory resQueueFactory;
static pgaCollectionFactory lcf(&resQueueFactory, __("Resource Queues"), roles_png_img);


dlgProperty *gpResQueueFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return 0;
}

