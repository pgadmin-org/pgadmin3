//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// slPath.cpp PostgreSQL Slony-I path
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "slPath.h"
#include "slObject.h"
#include "slNode.h"
#include "slCluster.h"
#include "frmMain.h"


slPath::slPath(slNode *n, const wxString& newName)
: slNodeObject(n, SL_PATH, newName)
{
    wxLogInfo(wxT("Creating a slPath object"));
}

slPath::~slPath()
{
    wxLogInfo(wxT("Destroying a slPath object"));
}


bool slPath::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(
              wxT("SELECT ") + GetCluster()->GetSchemaPrefix() 
            + wxT("setdroppath(") + NumToStr(GetSlId()) 
            + wxT(", ") + NumToStr(GetNode()->GetSlId())
            + wxT(");\n"));
}


wxString slPath::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Register path to node ") + GetName() + wxT(".\n\n")
              wxT("SELECT ") + GetCluster()->GetSchemaPrefix() 
                    + wxT("storepath(") + NumToStr(GetSlId()) 
                    + wxT(", ") + NumToStr(GetNode()->GetSlId())
                    + wxT(", ") + qtString(GetConnInfo())
                    + wxT(", ") + NumToStr(GetConnRetry())
                    + wxT(");\n");
    }
    return sql;
}


void slPath::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids=true;

        RemoveDummyChild(browser);
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



pgObject *slPath::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *path=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        slNodeCollection *coll=(slNodeCollection*)browser->GetItemData(parentItem);
        if (coll->GetType() == SL_PATHS)
            path = ReadObjects(coll, 0, 
            wxT(" WHERE pa_server=") + NumToStr(GetSlId()) + 
            wxT("   AND pa_client=") + NumToStr(GetNode()->GetSlId()) +
            wxT("\n"));
    }
    return path;
}



pgObject *slPath::ReadObjects(slNodeCollection *coll, wxTreeCtrl *browser, const wxString &restriction)
{
    slPath *path=0;

    pgSet *paths = coll->GetDatabase()->ExecuteSet(
        wxT("SELECT pa_client, pa_server, pa_conninfo, pa_connretry, no_comment\n")
        wxT("  FROM ") + coll->GetCluster()->GetSchemaPrefix() + wxT("sl_path\n")
        wxT("  JOIN ") + coll->GetCluster()->GetSchemaPrefix() + wxT("sl_node on no_id=pa_server\n")
         + restriction +
        wxT(" ORDER BY pa_server"));

    if (paths)
    {
        while (!paths->Eof())
        {
            path = new slPath(coll->GetNode(), paths->GetVal(wxT("no_comment")).BeforeFirst('\n'));
            path->iSetSlId(paths->GetLong(wxT("pa_server")));
            path->iSetConnInfo(paths->GetVal(wxT("pa_conninfo")));
            path->iSetConnRetry(paths->GetLong(wxT("pa_connretry")));

            if (browser)
            {
                coll->AppendBrowserItem(browser, path);
				paths->MoveNext();
            }
            else
                break;
        }

		delete paths;
    }
    return path;
}


    
pgObject *slPath::ReadObjects(slNodeCollection *coll, wxTreeCtrl *browser)
{
    // Get the paths
    wxString restriction = wxT(" WHERE pa_client = ") + NumToStr(coll->GetSlId());
    return ReadObjects(coll, browser, restriction);
}

