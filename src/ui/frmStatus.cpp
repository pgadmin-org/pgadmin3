//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmStatus.cpp - Status Screen
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/xrc/xmlres.h>
#include <wx/image.h>
#include <wx/spinctrl.h>

// App headers
#include "pgAdmin3.h"
#include "frmStatus.h"

// Icons
#include "images/pgAdmin3.xpm"


#define TIMER_ID 333
BEGIN_EVENT_TABLE(frmStatus, wxDialog)
    EVT_BUTTON(XRCID("btnRefresh"),         frmStatus::OnRefresh)
    EVT_BUTTON (XRCID("btnClose"),          frmStatus::OnClose)
    EVT_SPINCTRL(XRCID("spnRefreshRate"),   frmStatus::OnRateChange)
    EVT_TEXT(XRCID("spnRefreshRate"),       frmStatus::OnRateChange)
    EVT_TIMER(TIMER_ID,                     frmStatus::OnRefresh)
END_EVENT_TABLE();


#define statusList      CTRL("lstStatus", wxListCtrl)
#define spnRefreshRate  CTRL("spnRefreshRate", wxSpinCtrl)

void frmStatus::OnClose(wxCommandEvent &event)
{
    Destroy();
}

frmStatus::frmStatus(frmMain *form, const wxString& _title, pgConn *conn, const wxPoint& pos, const wxSize& size)
{
    wxLogInfo(wxT("Creating server status box"));
    wxXmlResource::Get()->LoadDialog(this, form, wxT("frmStatus")); 
    SetTitle(_title);
    SetIcon(wxIcon(pgAdmin3_xpm));

    mainForm=form;
    connection=conn;
    backend_pid=conn->GetBackendPID();

    statusList->InsertColumn(0, _("PID"), wxLIST_FORMAT_LEFT, 55);
    statusList->InsertColumn(1, _("Database"), wxLIST_FORMAT_LEFT, 100);
    statusList->InsertColumn(2, _("User"), wxLIST_FORMAT_LEFT, 100);
    statusList->InsertColumn(3, _("Query"), wxLIST_FORMAT_LEFT, 850);

    timer=new wxTimer(this, TIMER_ID);
    timer->Start(1000);
}


frmStatus::~frmStatus()
{
    wxLogInfo(wxT("Destroying server status box"));
    mainForm->RemoveFrame(this);

    settings->Write(wxT("frmStatus/Width"), GetSize().x);
    settings->Write(wxT("frmStatus/Height"), GetSize().y);
    settings->Write(wxT("frmStatus/Left"), GetPosition().x);
    settings->Write(wxT("frmStatus/Top"), GetPosition().y);

    delete timer;
    if (connection)
        delete connection;
}


void frmStatus::Go()
{
    Show(true);
    wxCommandEvent nullEvent;
    OnRefresh(nullEvent);
}


void frmStatus::OnRateChange(wxCommandEvent &event)
{
    timer->Stop();
    long rate=spnRefreshRate->GetValue();
    if (rate)
    {
        timer->Start(rate*1000L);
        wxCommandEvent nullEvent;
        OnRefresh(nullEvent);
    }
}



void frmStatus::OnRefresh(wxCommandEvent &event)
{
    pgSet *dataSet=connection->ExecuteSet(wxT("SELECT * FROM pg_stat_activity ORDER BY procpid"));
    if (dataSet)
    {
        long row=0;
        while (!dataSet->Eof())
        {
            long pid=dataSet->GetLong(wxT("procpid"));

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
                statusList->SetItem(row, 1, dataSet->GetVal(wxT("datname")));
                statusList->SetItem(row, 2, dataSet->GetVal(wxT("usename")));
                statusList->SetItem(row, 3, dataSet->GetVal(wxT("current_query")));
                wxString query=dataSet->GetVal(wxT("query_start"));
                statusList->SetItem(row, 4, query.Left(250));

                row++;
            }
            dataSet->MoveNext();
        }
    }
}
