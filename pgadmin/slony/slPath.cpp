//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// slPath.cpp PostgreSQL Slony-I path
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgObject.h"
#include "slony/slPath.h"
#include "slony/slNode.h"
#include "slony/slCluster.h"
#include "frm/frmMain.h"


slPath::slPath(slNode *n, const wxString &newName)
	: slNodeObject(n, pathFactory, newName)
{
}

bool slPath::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	return GetDatabase()->ExecuteVoid(
	           wxT("SELECT ") + GetCluster()->GetSchemaPrefix()
	           + wxT("droppath(") + NumToStr(GetSlId())
	           + wxT(", ") + NumToStr(GetNode()->GetSlId())
	           + wxT(");\n"));
}


wxString slPath::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on Slony path");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing Slony path");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop Slony path \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop Slony path \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop Slony path cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop Slony path?");
			break;
		case PROPERTIESREPORT:
			message = _("Slony path properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Slony path properties");
			break;
		case DDLREPORT:
			message = _("Slony path DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Slony path DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("Slony path dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Slony path dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Slony path dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Slony path dependents");
			break;
	}

	return message;
}

wxString slPath::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Register path to node ") + GetName() + wxT(".\n\n")
		      wxT("SELECT ") + GetCluster()->GetSchemaPrefix()
		      + wxT("storepath(") + NumToStr(GetSlId())
		      + wxT(", ") + NumToStr(GetNode()->GetSlId())
		      + wxT(", ") + qtDbString(GetConnInfo())
		      + wxT(", ") + NumToStr(GetConnRetry())
		      + wxT(");\n");
	}
	return sql;
}


void slPath::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (!expandedKids)
	{
		expandedKids = true;

		browser->RemoveDummyChild(this);
	}


	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Server name"), GetName().BeforeFirst('\n'));
		properties->AppendItem(_("Server ID"), GetSlId());
		properties->AppendItem(_("Connect info"), GetConnInfo());
		properties->AppendItem(_("Retry"), GetConnRetry());
	}
}



pgObject *slPath::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *path = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		path = pathFactory.CreateObjects(coll, 0,
		                                 wxT(" WHERE pa_server=") + NumToStr(GetSlId()) +
		                                 wxT("   AND pa_client=") + NumToStr(GetNode()->GetSlId()) +
		                                 wxT("\n"));

	return path;
}



pgObject *slPathFactory::CreateObjects(pgCollection *coll, ctlTree *browser, const wxString &restr)
{
	slNodeObjCollection *collection = (slNodeObjCollection *)coll;
	slPath *path = 0;
	wxString restriction;
	if (restr.IsEmpty())
		restriction = wxT(" WHERE pa_client = ") + NumToStr(collection->GetSlId());
	else
		restriction = restr;

	pgSet *paths = collection->GetDatabase()->ExecuteSet(
	                   wxT("SELECT pa_client, pa_server, pa_conninfo, pa_connretry, no_comment\n")
	                   wxT("  FROM ") + collection->GetCluster()->GetSchemaPrefix() + wxT("sl_path\n")
	                   wxT("  JOIN ") + collection->GetCluster()->GetSchemaPrefix() + wxT("sl_node on no_id=pa_server\n")
	                   + restriction +
	                   wxT(" ORDER BY pa_server"));

	if (paths)
	{
		while (!paths->Eof())
		{
			path = new slPath(collection->GetNode(), paths->GetVal(wxT("no_comment")).BeforeFirst('\n'));
			path->iSetSlId(paths->GetLong(wxT("pa_server")));
			path->iSetConnInfo(paths->GetVal(wxT("pa_conninfo")));
			path->iSetConnRetry(paths->GetLong(wxT("pa_connretry")));

			if (browser)
			{
				browser->AppendObject(collection, path);
				paths->MoveNext();
			}
			else
				break;
		}

		delete paths;
	}
	return path;
}


wxString slPathCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on Slony paths");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing Slony paths");
			break;
		case OBJECTSLISTREPORT:
			message = _("Slony paths list report");
			break;
	}

	return message;
}


///////////////////////////////////////////////////

#include "images/slpath.pngc"
#include "images/slpaths.pngc"

slPathFactory::slPathFactory()
	: slNodeObjFactory(__("Path"), __("New Path"), __("Create a new Path."), slpath_png_img)
{
	metaType = SLM_PATH;
}


pgCollection *slPathFactory::CreateCollection(pgObject *obj)
{
	return new slPathCollection(GetCollectionFactory(), (slNode *)obj);
}


slPathFactory pathFactory;
static pgaCollectionFactory cf(&pathFactory, __("Paths"), slpaths_png_img);
