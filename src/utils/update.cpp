//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: update.cpp 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// update.cpp - Online update check
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"

#include "wx/url.h"
#include "update.h"
#include "menu.h"
#include "frmMain.h"



bool CheckUpdates()
{
    wxString url=settings->Read(wxT("Updates/Site"), wxT("http://www.pgadmin.org/pgadmin3/updates/update.php"));
    url += wxT("?PgsqlVersions=") + settings->Read(wxT("Updates/pgsql-Versions"), wxEmptyString)
        +  wxT("&UseSSL=") + settings->Read(wxT("Updates/UseSSL"), wxEmptyString)
        +  wxT("&OS=" + wxGetOsDescription());

    wxURL site(url);
    site.SetProxy(settings->GetProxy());

    wxInputStream *input=site.GetInputStream();

    if (input)
    {
        size_t lastRead;
        wxString response;

        do
        {
            char buffer[1025];
            input->Read(buffer, 1024);
            lastRead = input->LastRead();
            if (lastRead > 0)
            {
                buffer[lastRead]= 0;
                response += wxString::FromAscii(buffer);
            }
        }
        while (lastRead > 0);

        delete input;

        settings->Write(wxT("Updates/Current/Beta"), wxEmptyString);

        wxStringTokenizer tokens(response, wxT(";"));
        wxString str;

        while (tokens.HasMoreTokens())
        {
            str=tokens.GetNextToken();
            wxString tag=str.BeforeFirst(':').Strip(wxString::both);
            if (tag.IsEmpty())
                continue;
            wxString value=str.AfterFirst(':').Strip(wxString::both);
            settings->Write(wxT("Updates/Current/") + tag, value);
        }

        str=DateToAnsiStr(wxDateTime::Now());
//        settings->Write(wxT("Updates/LastRun"), str);

        return true;
    }

    return false;
}


class UpdateThread : public wxThread
{
public:
    UpdateThread(frmMain *_mainForm) : wxThread(wxTHREAD_JOINABLE) { mainForm = _mainForm; }
    void *Entry();
    frmMain *mainForm;

};


void *UpdateThread::Entry()
{
    bool done=CheckUpdates();

    if (!done)
    {
        wxCommandEvent ev(wxEVT_COMMAND_MENU_SELECTED, MNU_ONLINEUPDATE_NEWDATA);

        mainForm->AddPendingEvent(ev);
    }

    return NULL;
}


wxThread *BackgroundCheckUpdates(frmMain *mainForm)
{
    long updatePeriod = settings->Read(wxT("Updates/UpdateInterval"), 30);
    if (updatePeriod < 1)
        return 0;

    wxDateTime lastRun = StrToDateTime(settings->Read(wxT("Updates/LastRun"), wxEmptyString));

    if (lastRun.IsValid())
    {
        wxTimeSpan ts=wxDateTime::Now().Subtract(lastRun);

        if (ts.GetDays() < updatePeriod)
            return 0;
    }

    // we need to check for updates

    wxThread *thread=new UpdateThread(mainForm);
    if (thread->Create() != wxTHREAD_NO_ERROR)
    {
        thread->Delete();
        delete thread;
        return 0;
    }

    thread->Run();
    return thread;
}
