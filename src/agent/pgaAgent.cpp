//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgaAgent.h - PostgreSQL Server Agent
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "pgDatabase.h"
#include "pgCollection.h"
#include "pgaAgent.h"


pgaAgent::pgaAgent(const wxString& newName)
: pgDatabaseObject(PGA_AGENT, newName)
{
    wxLogInfo(wxT("Creating a pgaAgent object"));
}

pgaAgent::~pgaAgent()
{
    wxLogInfo(wxT("Destroying a pgaAgent object"));
}



void pgaAgent::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids=true;

        RemoveDummyChild(browser);
        // Log
        wxLogInfo(wxT("Adding child object to agent."));

        pgCollection *collection;

        // Instances
        collection = new pgCollection(PGA_INSTANCES, GetDatabase());
        AppendBrowserItem(browser, collection);

        // Tasks
        collection = new pgCollection(PGA_JOBS, GetDatabase());
        AppendBrowserItem(browser, collection);
    }
}



pgObject *pgaAgent::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *agent=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgObject *obj=(pgObject*)browser->GetItemData(parentItem);
        if (obj->GetType() == PG_DATABASE)
            agent = ReadObjects((pgDatabase*)obj, 0);
    }
    return agent;
}



pgObject *pgaAgent::ReadObjects(pgDatabase *database, wxTreeCtrl *browser)
{
    pgaAgent *agent=0;

    wxString exists = database->ExecuteScalar(
       wxT("SELECT oid FROM pg_class where relname='pg_job'"));

    if (!exists.IsNull())
    {
        agent = new pgaAgent(_("pgAgent"));
        agent->iSetDatabase(database);

        if (browser)
            database->AppendBrowserItem(browser, agent);
    }
    return agent;
}
