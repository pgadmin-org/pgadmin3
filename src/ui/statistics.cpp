//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// statistics.cpp - Statistics functions for the main form
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/treectrl.h>
#include <wx/listctrl.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "frmMain.h"
#include "pgConn.h"
#include "pgDatabase.h"
#include "pgSet.h"
#include "pgServer.h"
#include "pgObject.h"
#include "pgCollection.h"

void frmMain::svServer(pgServer *server)
{
    if(!server->GetConnected()) return;
    
    wxString msg;
    msg.Printf(wxT("Displaying statistics for server %s"), server->GetIdentifier().c_str());
    wxLogInfo(msg);

    // Add the statistics view columns
    statistics->ClearAll();
    statistics->InsertColumn(0, wxT("Database"), wxLIST_FORMAT_LEFT, 100);
    statistics->InsertColumn(1, wxT("PID"), wxLIST_FORMAT_LEFT, 50);
    statistics->InsertColumn(2, wxT("User"), wxLIST_FORMAT_LEFT, 100);
    statistics->InsertColumn(3, wxT("Current Query"), wxLIST_FORMAT_LEFT, 400);

    pgSet stats = server->ExecuteSet(wxT("SELECT datname, procpid, usename, current_query FROM pg_stat_activity"));

    while (!stats.Eof()) {
        statistics->InsertItem(stats.CurrentPos() - 1, stats.GetVal(wxT("datname")), 0);
        statistics->SetItem(stats.CurrentPos() - 1, 1, stats.GetVal(wxT("procpid")));
        statistics->SetItem(stats.CurrentPos() - 1, 2, stats.GetVal(wxT("usename")));
        statistics->SetItem(stats.CurrentPos() - 1, 3, stats.GetVal(wxT("current_query")));
        stats.MoveNext();
    }


}

void frmMain::svDatabases(pgCollection *collection)
{
    
    wxString msg;
    msg.Printf(wxT("Displaying statistics for databases on %s"), collection->GetServer()->GetIdentifier().c_str());
    wxLogInfo(msg);

    // Add the statistics view columns
    statistics->ClearAll();
    statistics->InsertColumn(0, wxT("Database"), wxLIST_FORMAT_LEFT, 100);
    statistics->InsertColumn(1, wxT("Backends"), wxLIST_FORMAT_LEFT, 75);
    statistics->InsertColumn(2, wxT("Xact Committed"), wxLIST_FORMAT_LEFT, 100);
    statistics->InsertColumn(3, wxT("Xact Rolled Back"), wxLIST_FORMAT_LEFT, 100);
    statistics->InsertColumn(4, wxT("Blocks Read"), wxLIST_FORMAT_LEFT, 100);
    statistics->InsertColumn(5, wxT("Blocks Hit"), wxLIST_FORMAT_LEFT, 100);

    pgSet stats = collection->GetServer()->ExecuteSet(wxT("SELECT datname, numbackends, xact_commit, xact_rollback, blks_read, blks_hit FROM pg_stat_database ORDER BY datname"));

    while (!stats.Eof()) {
        statistics->InsertItem(stats.CurrentPos() - 1, stats.GetVal(wxT("datname")), 0);
        statistics->SetItem(stats.CurrentPos() - 1, 1, stats.GetVal(wxT("numbackends")));
        statistics->SetItem(stats.CurrentPos() - 1, 2, stats.GetVal(wxT("xact_commit")));
        statistics->SetItem(stats.CurrentPos() - 1, 3, stats.GetVal(wxT("xact_rollback")));
        statistics->SetItem(stats.CurrentPos() - 1, 4, stats.GetVal(wxT("blks_read")));
        statistics->SetItem(stats.CurrentPos() - 1, 5, stats.GetVal(wxT("blks_hit")));
        stats.MoveNext();
    }
}

