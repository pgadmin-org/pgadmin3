//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// edbResourceGroup.cpp - Resource Group (only used for PPAS 9.4)
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/edbResourceGroup.h"

#include "images/resourcegroup.pngc"
#include "images/resourcegroups.pngc"

edbResourceGroup::edbResourceGroup(const wxString &newName)
	: pgServerObject(resourceGroupFactory, newName), cpuRateLimit(0), dirtyRateLimit(0)
{
}

edbResourceGroup::~edbResourceGroup()
{
}

wxString edbResourceGroup::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on resource group");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing resouce group");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop resource group \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop resource group \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop resource group cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop resource group?");
			break;
		case PROPERTIESREPORT:
			message = _("Resource group properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Resource group properties");
			break;
		case DDLREPORT:
			message = _("Resource group DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Resource group DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("Resource group dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Resource group dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Resource group dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Resource group dependents");
			break;
	}

	return message;
}

bool edbResourceGroup::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	return server->ExecuteVoid(wxT("DROP RESOURCE GROUP ") + GetQuotedFullIdentifier());
}

wxString edbResourceGroup::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- RESOURCE GROUP: ") + GetName() + wxT("\n\n")
		      + wxT("-- DROP RESOURCE GROUP ") + GetQuotedFullIdentifier() + wxT(";")
		      + wxT("\n\nCREATE RESOURCE GROUP ") + GetQuotedIdentifier()  + wxT(";")
		      + wxT("\nALTER RESOURCE GROUP ") + GetQuotedIdentifier() + wxT(" SET cpu_rate_limit = ") +
		      wxString::Format(wxT("%f"), GetCPURateLimit()) + wxT(", dirty_rate_limit = ") +
		      wxString::Format(wxT("%f"), GetDirtyRateLimit()) + wxT(";");
	}

	return sql;
}

void edbResourceGroup::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (properties)
	{
		CreateListColumns(properties);
		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("CPU Rate Limit"), GetCPURateLimit());
		properties->AppendItem(_("Dirty Rate Limit"), GetDirtyRateLimit());
	}
}

pgObject *edbResourceGroup::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *group = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		group = resourceGroupFactory.CreateObjects(coll, 0, wxT("\n WHERE oid=") + GetOidStr());

	return group;
}


edbResourceGroupFactory::edbResourceGroupFactory()
	: pgServerObjFactory(__("Resource Group"), __("New Resource Group..."), __("Create a new Resource Group."), resourcegroup_png_img)
{
}

pgObject *edbResourceGroupFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	edbResourceGroup *resGroup = 0;
	double cpuLimit, dirtyLimit;

	pgSet *groups = collection->GetServer()->ExecuteSet(wxT("SELECT oid, * from edb_resource_group") + restriction);

	if (groups)
	{
		while (!groups->Eof())
		{
			resGroup = new edbResourceGroup(groups->GetVal(wxT("rgrpname")));
			resGroup->iSetServer(collection->GetServer());
			resGroup->iSetOid(groups->GetOid(wxT("oid")));
			groups->GetVal(wxT("rgrpcpuratelimit")).ToDouble(&cpuLimit);
			resGroup->iSetCPURateLimit(cpuLimit);
			groups->GetVal(wxT("rgrpdirtyratelimit")).ToDouble(&dirtyLimit);
			resGroup->iSetDirtyRateLimit(dirtyLimit);

			if (browser)
			{
				browser->AppendObject(collection, resGroup);
				groups->MoveNext();
			}
			else
				break;
		}

		delete groups;
	}
	return resGroup;
}

pgCollection *edbResourceGroupFactory::CreateCollection(pgObject *obj)
{
	return new edbResourceGroupCollection(GetCollectionFactory(), (pgServer *)obj);
}

edbResourceGroupFactory resourceGroupFactory;
static pgaCollectionFactory rgcf(&resourceGroupFactory, __("Resource Groups"), resourcegroups_png_img);

edbResourceGroupCollection::edbResourceGroupCollection(pgaFactory *factory, pgServer *sv)
	: pgServerObjCollection(factory, sv)
{
}

wxString edbResourceGroupCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on resource groups");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing resource groups");
			break;
		case OBJECTSLISTREPORT:
			message = _("Resource groups list report");
			break;
	}

	return message;
}

