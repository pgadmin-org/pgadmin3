//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmStatus.cpp - Status Screen
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/xrc/xmlres.h>
#include <wx/image.h>

// App headers
#include "frmStatus.h"

// Icons
#include "images/pgAdmin3.xpm"


#define TIMER_ID 333
BEGIN_EVENT_TABLE(frmStatus, wxDialog)
    EVT_BUTTON(XRCID("btnRefresh"),         frmStatus::OnRefresh)
    EVT_BUTTON (XRCID("btnClose"),          frmStatus::OnClose)
    EVT_SPINCTRL(XRCID("spnRefreshRate"),   frmStatus::OnRateChangeSpin)
    EVT_TEXT(XRCID("spnRefreshRate"),       frmStatus::OnRateChange)
	EVT_NOTEBOOK_PAGE_CHANGING(XRCID("nbStatus"),       frmStatus::OnNotebookPageChanged)
    EVT_TIMER(TIMER_ID,                     frmStatus::OnRefreshTimer)
END_EVENT_TABLE();


#define statusList      CTRL_LISTVIEW("lstStatus")
#define lockList        CTRL_LISTVIEW("lstLocks")
#define spnRefreshRate  CTRL_SPIN("spnRefreshRate")
#define nbStatus		CTRL_NOTEBOOK("nbStatus")

void frmStatus::OnClose(wxCommandEvent &event)
{
    Destroy();
}

frmStatus::frmStatus(frmMain *form, const wxString& _title, pgConn *conn, const wxPoint& pos, const wxSize& size)
{
    wxLogInfo(wxT("Creating server status box"));
	loaded = FALSE;
    wxWindowBase::SetFont(settings->GetSystemFont());
    wxXmlResource::Get()->LoadDialog(this, form, wxT("frmStatus")); 
    SetTitle(_title);
    SetIcon(wxIcon(pgAdmin3_xpm));

    mainForm=form;
    timer=0;
    connection=conn;
    backend_pid=conn->GetBackendPID();

    statusList->AddColumn(_("PID"), 35);
    statusList->AddColumn(_("Database"), 70);
    statusList->AddColumn(_("User"), 70);
    if (connection->BackendMinimumVersion(7, 4)) {
        statusList->AddColumn(_("Start"), 50);
        statusList->AddColumn(_("Query"), 500);
    } else {
        statusList->AddColumn(_("Query"), 500);
    }

    lockList->AddColumn(_("PID"), 50);
    lockList->AddColumn(_("Database"), 50);
    lockList->AddColumn(_("Relation"), 50);
    lockList->AddColumn(_("User"), 50);
    lockList->AddColumn(_("TX"), 50);
    lockList->AddColumn(_("Mode"), 50);
    lockList->AddColumn(_("Granted"), 50);
    if (connection->BackendMinimumVersion(7, 4)) {
        lockList->AddColumn(_("Start"), 50);
        lockList->AddColumn(_("Query"), 500);
    } else {
        lockList->AddColumn(_("Query"), 500);
    }

    long rate;
    settings->Read(wxT("frmStatus/Refreshrate"), &rate, 1);
    spnRefreshRate->SetValue(rate);
    timer=new wxTimer(this, TIMER_ID);
	loaded = TRUE;
}


frmStatus::~frmStatus()
{
    wxLogInfo(wxT("Destroying server status box"));
    mainForm->RemoveFrame(this);

    settings->Write(wxT("frmStatus"), GetSize(), GetPosition());
    settings->Write(wxT("frmStatus/Refreshrate"), spnRefreshRate->GetValue());

    delete timer;
    if (connection)
        delete connection;
}


void frmStatus::Go()
{
    Show(true);
    wxCommandEvent nullEvent;

    long rate=spnRefreshRate->GetValue();
    if (rate)
        timer->Start(rate*1000L);

    OnRefresh(nullEvent);
}


void frmStatus::OnNotebookPageChanged(wxNotebookEvent& event)
{
	if (!loaded) return;
	wxCommandEvent nullEvent;
	OnRefresh(nullEvent);
}


void frmStatus::OnRateChangeSpin(wxSpinEvent &event)
{
	OnRateChange(*(wxCommandEvent*)&event);
}


void frmStatus::OnRateChange(wxCommandEvent &event)
{
    if (timer)
    {
        timer->Stop();
        long rate=spnRefreshRate->GetValue();
        if (rate)
        {
            timer->Start(rate*1000L);
            OnRefresh(event);
        }
    }
}


void frmStatus::OnRefreshTimer(wxTimerEvent &event)
{
	OnRefresh(*(wxCommandEvent*)&event);
}


void frmStatus::OnRefresh(wxCommandEvent &event)
{

	// To avoid hammering the lock manager (and the network for that matter),
	// only query for the required tab.

	if (nbStatus->GetSelection() == 0) {

		// Status
		pgSet *dataSet1=connection->ExecuteSet(wxT("SELECT * FROM pg_stat_activity ORDER BY procpid"));
		if (dataSet1)
		{
			long row=0;
			while (!dataSet1->Eof())
			{
				long pid=dataSet1->GetLong(wxT("procpid"));

				if (pid != backend_pid)
				{
					long itempid=0;

					while (row < statusList->GetItemCount())
					{
						itempid=StrToLong(statusList->GetItemText(row));
						if (itempid && itempid < pid)
							statusList->DeleteItem(row);
						else
							break;
					}

					if (!itempid || itempid > pid)
					{
						statusList->InsertItem(row, NumToStr(pid), 0);
					}
					statusList->SetItem(row, 1, dataSet1->GetVal(wxT("datname")));
					statusList->SetItem(row, 2, dataSet1->GetVal(wxT("usename")));
					wxString qry=dataSet1->GetVal(wxT("current_query"));

					if (connection->BackendMinimumVersion(7, 4))
					{
						if (qry.IsEmpty()) {
							statusList->SetItem(row, 3, wxT(""));
							statusList->SetItem(row, 4, wxT(""));
						} else {
							statusList->SetItem(row, 3, dataSet1->GetVal(wxT("query_start")));
							statusList->SetItem(row, 4, qry.Left(250));
						} 
					} else {
						statusList->SetItem(row, 3, qry.Left(250));
					}

					row++;
				}
				dataSet1->MoveNext();
			}
		}

	} else {

		// Locks
		wxString sql;
		if (connection->BackendMinimumVersion(7, 4)) {
			sql = wxT("SELECT ")
				  wxT("(SELECT datname FROM pg_database WHERE oid = database) AS dbname, ")
				  wxT("relation::regclass AS class, ")
				  wxT("pg_get_userbyid(pg_stat_get_backend_userid(pid)::int4) as user, ")
				  wxT("transaction, pid, mode, granted, ")
				  wxT("pg_stat_get_backend_activity(pid) AS current_query, ")
				  wxT("pg_stat_get_backend_activity_start(pid) AS query_start ")
				  wxT("FROM pg_locks");
		} else {
			sql = wxT("SELECT ")
				  wxT("(SELECT datname FROM pg_database WHERE oid = database) AS dbname, ")
				  wxT("relation::regclass AS class, ")
				  wxT("pg_get_userbyid(pg_stat_get_backend_userid(pid)::int4) as user, ")
				  wxT("transaction, pid, mode, granted, ")
				  wxT("pg_stat_get_backend_activity(pid) AS current_query ")
				  wxT("FROM pg_locks ");
		}

		pgSet *dataSet2=connection->ExecuteSet(sql);
		if (dataSet2)
		{
			long row=0;
			while (!dataSet2->Eof())
			{
				long pid=dataSet2->GetLong(wxT("pid"));

				if (pid != backend_pid)
				{
					long itempid=0;

					while (row < lockList->GetItemCount())
					{
						itempid=StrToLong(lockList->GetItemText(row));
						if (itempid && itempid < pid)
							lockList->DeleteItem(row);
						else
							break;
					}

					if (!itempid || itempid > pid)
					{
						lockList->InsertItem(row, NumToStr(pid), 0);
					}
					lockList->SetItem(row, 1, dataSet2->GetVal(wxT("dbname")));
					lockList->SetItem(row, 2, dataSet2->GetVal(wxT("class")));
					lockList->SetItem(row, 3, dataSet2->GetVal(wxT("user")));
					lockList->SetItem(row, 4, dataSet2->GetVal(wxT("transaction")));
					lockList->SetItem(row, 5, dataSet2->GetVal(wxT("mode")));
					if (dataSet2->GetVal(wxT("granted")) == wxT("t")) {
						lockList->SetItem(row, 6, __("Yes"));
					} else {
						lockList->SetItem(row, 6, __("No"));
					}
					wxString qry=dataSet2->GetVal(wxT("current_query"));

					if (connection->BackendMinimumVersion(7, 4))
					{
						if (qry.IsEmpty()) {
							lockList->SetItem(row, 7, wxT(""));
							lockList->SetItem(row, 8, wxT(""));
						} else {
							lockList->SetItem(row, 7, dataSet2->GetVal(wxT("query_start")));
							lockList->SetItem(row, 8, qry.Left(250));
						}
					} else {
						lockList->SetItem(row, 7, qry.Left(250));
					}

					row++;
				}
				dataSet2->MoveNext();
			}
		}
	}
}
