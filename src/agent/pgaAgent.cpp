//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
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
#include "pgaJob.h"


pgaAgent::pgaAgent(const wxString& newName)
: pgDatabaseObject(PGA_AGENT, newName)
{
    wxLogInfo(wxT("Creating a pgaAgent object"));
}

pgaAgent::~pgaAgent()
{
    wxLogInfo(wxT("Destroying a pgaAgent object"));
}



wxMenu *pgaAgent::GetNewMenu()
{
    wxMenu *menu=new wxMenu();
    if (1) // check priv.
    {
        AppendMenu(menu, PGA_JOB);
    }
    return menu;
}

void pgaAgent::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlListView *statistics, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids=true;

        RemoveDummyChild(browser);
        // Log
        wxLogInfo(wxT("Adding child object to agent."));

        pgaJob::ReadObjects(this, browser);
    }

    if (properties)
    {
        properties->ClearAll();
        properties->AddColumn(_("Instance"), 90);
        properties->AddColumn(_("PID"), 35);
        properties->AddColumn(_("Login time"), 75);
        properties->AddColumn(_("Current job"), 250);

        pgSet *props = GetDatabase()->ExecuteSet(
            wxT("SELECT station, jagpid, logintime, jobname\n")
            wxT("  FROM pga_jobagent\n")
            wxT("  JOIN pga_job ON jobagentid=jagpid\n")
            wxT(" ORDER BY jagpid"));
    
        if (props)
        {
            while (!props->Eof())
            {
                long pos=0;
                properties->InsertItem(pos, props->GetVal(wxT("station")), PGAICON_AGENT);
                properties->SetItem(pos, 1, props->GetVal(wxT("jagpid")));
                properties->SetItem(pos, 2, DateToStr(props->GetDateTime(wxT("logintime"))));
                properties->SetItem(pos, 3, props->GetVal(wxT("jobname")));

                props->MoveNext();
            }
            delete props;
        }
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
       wxT("SELECT oid FROM pg_class where relname='pga_job'"));

    if (!exists.IsNull())
    {
        agent = new pgaAgent(_("pgAgent"));
        agent->iSetDatabase(database);

        if (browser)
            database->AppendBrowserItem(browser, agent);
    }
    return agent;
}
