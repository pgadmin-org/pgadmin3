//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// slPath.cpp PostgreSQL Slony-I path
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "pgObject.h"
#include "slony/slPath.h"
#include "slony/slNode.h"
#include "slony/slCluster.h"
#include "frm/frmMain.h"


slPath::slPath(slNode *n, const wxString& newName)
: slNodeObject(n, pathFactory, newName)
{
    wxLogInfo(wxT("Creating a slPath object"));
}

slPath::~slPath()
{
    wxLogInfo(wxT("Destroying a slPath object"));
}


bool slPath::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
    return GetDatabase()->ExecuteVoid(
              wxT("SELECT ") + GetCluster()->GetSchemaPrefix() 
            + wxT("droppath(") + NumToStr(GetSlId()) 
            + wxT(", ") + NumToStr(GetNode()->GetSlId())
            + wxT(");\n"));
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
        expandedKids=true;

        browser->RemoveDummyChild(this);
    }


    if (properties)
    {
        wxLogInfo(wxT("Displaying properties for path ") + GetIdentifier());

        CreateListColumns(properties);

        properties->AppendItem(_("Server name"), GetName().BeforeFirst('\n'));
        properties->AppendItem(_("Server ID"), GetSlId());
        properties->AppendItem(_("Connect info"), GetConnInfo());
        properties->AppendItem(_("Retry"), GetConnRetry());
    }
}



pgObject *slPath::Refresh(ctlTree *browser, const wxTreeItemId item)
{
    pgObject *path=0;
    pgCollection *coll=browser->GetParentCollection(item);
    if (coll)
        path = pathFactory.CreateObjects(coll, 0, 
            wxT(" WHERE pa_server=") + NumToStr(GetSlId()) + 
            wxT("   AND pa_client=") + NumToStr(GetNode()->GetSlId()) +
            wxT("\n"));

    return path;
}



pgObject *slPathFactory::CreateObjects(pgCollection *coll, ctlTree *browser, const wxString &restr)
{
    slNodeObjCollection *collection=(slNodeObjCollection*)coll;
    slPath *path=0;
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


///////////////////////////////////////////////////

#include "images/slpath.xpm"
#include "images/slpaths.xpm"

slPathFactory::slPathFactory() 
: slNodeObjFactory(__("Path"), __("New Path"), __("Create a new Path."), slpath_xpm)
{
    metaType = SLM_PATH;
}


slPathFactory pathFactory;
static pgaCollectionFactory cf(&pathFactory, __("Paths"), slpaths_xpm);
