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
    wxWindowBase::SetFont(settings->GetSystemFont());
    wxXmlResource::Get()->LoadDialog(this, form, wxT("frmStatus")); 
    SetTitle(_title);
    SetIcon(wxIcon(pgAdmin3_xpm));

    mainForm=form;
    timer=0;
    connection=conn;
    backend_pid=conn->GetBackendPID();

    statusList->InsertColumn(0, _("PID"), wxLIST_FORMAT_LEFT, 55);
    statusList->InsertColumn(1, _("Database"), wxLIST_FORMAT_LEFT, 100);
    statusList->InsertColumn(2, _("User"), wxLIST_FORMAT_LEFT, 100);
    statusList->InsertColumn(3, _("Query"), wxLIST_FORMAT_LEFT, 850);
    if (connection->BackendMinimumVersion(7, 4))
        statusList->InsertColumn(4, _("Start"), wxLIST_FORMAT_LEFT, 70);


    long rate;
    settings->Read(wxT("frmStatus/Refreshrate"), &rate, 1);
    spnRefreshRate->SetValue(rate);
    timer=new wxTimer(this, TIMER_ID);
}


frmStatus::~frmStatus()
{
    wxLogInfo(wxT("Destroying server status box"));
    mainForm->RemoveFrame(this);

    settings->Write(wxT("frmStatus/Width"), GetSize().x);
    settings->Write(wxT("frmStatus/Height"), GetSize().y);
    settings->Write(wxT("frmStatus/Left"), GetPosition().x);
    settings->Write(wxT("frmStatus/Top"), GetPosition().y);
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
                wxString qry=dataSet->GetVal(wxT("current_query"));
                statusList->SetItem(row, 3, qry.Left(250));

                if (connection->BackendMinimumVersion(7, 4))
                {
                    if (qry.IsEmpty())
                        statusList->SetItem(row, 4, wxT(""));
                    else
                        statusList->SetItem(row, 4, dataSet->GetVal(wxT("query_start")));
                }

                row++;
            }
            dataSet->MoveNext();
        }
    }
}
