//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgClasses.cpp - Some dialogue base classes 
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>


// App headers
#include "pgAdmin3.h"
#include "wx/process.h"
#include "frmMain.h"
#include "pgConn.h"

#include "menu.h"


BEGIN_EVENT_TABLE(pgDialog, wxDialog)
    EVT_BUTTON (XRCID("btnCancel"),     pgDialog::OnCancel)
    EVT_CLOSE(                          pgDialog::OnClose)
END_EVENT_TABLE()

void pgDialog::RestorePosition(int defaultX, int defaultY, int defaultW, int defaultH, int minW, int minH)
{
    wxPoint pos(settings->Read(dlgName, wxPoint(defaultX, defaultY)));
    wxSize size;
    if (defaultW < 0)
        size = GetSize();
    else
        size = settings->Read(dlgName, wxSize(defaultW, defaultH));

    bool posDefault = (pos.x == -1 && pos.y == -1);

    CheckOnScreen(pos, size, minW, minH);
    SetSize(pos.x, pos.y, size.x, size.y);
    if (posDefault)
        CenterOnParent();
}

void pgDialog::SavePosition()
{
    settings->Write(dlgName, GetSize(), GetPosition());
}

void pgDialog::LoadResource(wxWindow *parent, const wxChar *name)
{
    if (name)
        dlgName = name;
    wxXmlResource::Get()->LoadDialog(this, parent, dlgName); 
}



void pgDialog::OnClose(wxCloseEvent& event)
{
    if (IsModal())
        EndModal(-1);
    else
        Destroy();
}


void pgDialog::OnCancel(wxCommandEvent& ev)
{
    if (IsModal())
        EndModal(-1);
    else
        Destroy();
}


///////////////////////////////////////////////////////////////////////////////////////


void pgFrame::RestorePosition(int defaultX, int defaultY, int defaultW, int defaultH, int minW, int minH)
{
    wxPoint pos(settings->Read(dlgName, wxPoint(defaultX, defaultY)));
    wxSize size;
    if (defaultW < 0)
        size = GetSize();
    else
        size = settings->Read(dlgName, wxSize(defaultW, defaultH));

    bool posDefault = (pos.x == -1 && pos.y == -1);

    CheckOnScreen(pos, size, minW, minH);
    SetSize(pos.x, pos.y, size.x, size.y);
    if (posDefault)
        CenterOnParent();
}


void pgFrame::SavePosition()
{
    settings->Write(dlgName, GetSize(), GetPosition());
}



//////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(DialogWithHelp, pgDialog)
    EVT_MENU(MNU_HELP,                  DialogWithHelp::OnHelp)
    EVT_BUTTON(XRCID("btnHelp"),        DialogWithHelp::OnHelp)
END_EVENT_TABLE();


DialogWithHelp::DialogWithHelp(frmMain *frame) : pgDialog()
{
    mainForm = frame;

    wxAcceleratorEntry entries[2];
    entries[0].Set(wxACCEL_NORMAL, WXK_F1, MNU_HELP);
// this is for GTK because Meta (usually Numlock) is interpreted like Alt
// there are too many controls to reset m_Meta in all of them
    entries[1].Set(wxACCEL_ALT, WXK_F1, MNU_HELP);
    wxAcceleratorTable accel(2, entries);

    SetAcceleratorTable(accel);
}


void DialogWithHelp::OnHelp(wxCommandEvent& ev)
{
    wxString page=GetHelpPage();

    if (!page.IsEmpty())
        DisplaySqlHelp(this, page);
}



////////////////////////////////////////////////////////////////77


BEGIN_EVENT_TABLE(ExecutionDialog, DialogWithHelp)
    EVT_BUTTON (XRCID("btnOK"),         ExecutionDialog::OnOK)
    EVT_BUTTON (XRCID("btnCancel"),     ExecutionDialog::OnCancel)
    EVT_CLOSE(                          ExecutionDialog::OnClose)
END_EVENT_TABLE()


ExecutionDialog::ExecutionDialog(frmMain *frame, pgObject *_object) : DialogWithHelp(frame)
{
    thread=0;
    object = _object;
    txtMessages = 0;
}


void ExecutionDialog::OnClose(wxCloseEvent& event)
{
    Abort();
    if (IsModal())
        EndModal(-1);
    else
        Destroy();
}


void ExecutionDialog::OnCancel(wxCommandEvent& ev)
{
    if (thread)
    {
        btnCancel->Disable();
        Abort();
        btnCancel->Enable();
        btnOK->Enable();
        wxButton *btn=btnApply;
        if (btn)
            btn->Enable();
    }
    else
    {
        if (IsModal())
            EndModal(-1);
        else
            Destroy();
    }
}


void ExecutionDialog::Abort()
{
    if (thread)
    {
        if (thread->IsRunning())
            thread->Delete();
        delete thread;
        thread=0;
    }
}


void ExecutionDialog::OnOK(wxCommandEvent& ev)
{
    if (!thread)
    {
        wxString sql=GetSql();
        if (sql.IsEmpty())
            return;

        btnOK->Disable();

        thread=new pgQueryThread(object->GetConnection(), sql);
        if (thread->Create() != wxTHREAD_NO_ERROR)
        {
            Abort();
            return;
        }

        wxLongLong startTime=wxGetLocalTimeMillis();
        thread->Run();
        wxNotebook *nb=CTRL_NOTEBOOK("nbNotebook");
        if (nb)
            nb->SetSelection(nb->GetPageCount()-1);

        while (thread && thread->IsRunning())
        {
            wxUsleep(10);
            // here could be the animation
            if (txtMessages)
                txtMessages->AppendText(thread->GetMessagesAndClear());
            wxYield();
        }

        if (thread)
        {
            if (txtMessages)
                txtMessages->AppendText(thread->GetMessagesAndClear());

            if (thread->DataSet() != NULL)
                wxLogDebug(wxString::Format(_("%d rows."), thread->DataSet()->NumRows()));

            if (txtMessages)
                txtMessages->AppendText(_("Total query runtime: ") 
                    + (wxGetLocalTimeMillis()-startTime).ToString() + wxT(" ms."));

                btnOK->SetLabel(_("Done"));
            btnCancel->Disable();
        }
        else
            if (txtMessages)
                txtMessages->AppendText(_("\nCancelled.\n"));

        btnOK->Enable();
        wxButton *btn=btnApply;
        if (btn)
            btn->Enable();
    }
    else
    {
        Abort();
        Destroy();
    }
}



#define TIMER_ID 4442

BEGIN_EVENT_TABLE(ExternProcessDialog, DialogWithHelp)
    EVT_BUTTON(XRCID("btnOK"),              ExternProcessDialog::OnOK)
    EVT_BUTTON(XRCID("btnCancel"),          ExternProcessDialog::OnCancel)
    EVT_CLOSE(                              ExternProcessDialog::OnClose)
    EVT_END_PROCESS(-1,                     ExternProcessDialog::OnEndProcess)
    EVT_TIMER(TIMER_ID,                     ExternProcessDialog::OnPollProcess)
END_EVENT_TABLE()



ExternProcessDialog::ExternProcessDialog(frmMain *frame) : DialogWithHelp(frame)
{
    txtMessages = 0;
    process = 0;
    done = false;

    timer=new wxTimer(this, TIMER_ID);
}



ExternProcessDialog::~ExternProcessDialog()
{
    Abort();
    delete timer;
}


void ExternProcessDialog::OnOK(wxCommandEvent& ev)
{
    if (!done)
        Execute(0);
    else
    {
        Abort();
        pgDialog::OnCancel(ev);
    }
}


bool ExternProcessDialog::Execute(int step, bool finalStep)
{
    btnOK->Disable();
    final=finalStep;

    if (txtMessages)
        txtMessages->AppendText(GetDisplayCmd(step) + END_OF_LINE);

    if (process)
        delete process;

    process = new wxProcess(this);
    size_t i;
    for (i=0 ; i < environment.GetCount() ; i++)
    {
        wxString str=environment.Item(i);
        wxSetEnv(str.BeforeFirst('='), str.AfterFirst('='));
    }
    process->Redirect();
    pid = wxExecute(GetCmd(step), wxEXEC_ASYNC, process);
    if (pid)
    {
        wxNotebook *nb=CTRL_NOTEBOOK("nbNotebook");
        if (nb)
            nb->SetSelection(nb->GetPageCount()-1);
        if (txtMessages)
        {
            checkStreams();
            timer->Start(100L);
        }
        return true;
    }
    else
    {
        delete process;
        return false;
    }
}


void ExternProcessDialog::OnCancel(wxCommandEvent &ev)
{
    if (process)
    {
        btnCancel->Disable();
        Abort();
    }
    else
        pgDialog::OnCancel(ev);
}


void ExternProcessDialog::OnClose(wxCloseEvent &ev)
{
    btnCancel->Disable();
    Abort();
    pgDialog::OnClose(ev);
}


void ExternProcessDialog::OnPollProcess(wxTimerEvent& event)
{
    checkStreams();
}


void ExternProcessDialog::checkStreams()
{
    if (txtMessages && process)
    {
        if (process->IsErrorAvailable())
            readStream(process->GetErrorStream());
        if (process->IsInputAvailable())
            readStream(process->GetInputStream());
    }
}


void ExternProcessDialog::readStream(wxInputStream *input)
{
    if (input)
    {
        char buffer[1000+1];
        size_t size=1;
        while (size && !input->Eof())
        {
            input->Read(buffer, sizeof(buffer)-1);
            size=input->LastRead();
            if (size)
            {
                buffer[size]=0;
                txtMessages->AppendText(wxString::FromAscii(buffer));
            }
        }
    }
}


void ExternProcessDialog::OnEndProcess(wxProcessEvent &ev)
{
    if (process)
    {
        if (final)
            btnOK->SetLabel(_("Done"));
        done=true;
    }
    timer->Stop();

    if (txtMessages)
    {
        checkStreams();
        txtMessages->AppendText(END_OF_LINE
                              + wxString::Format(_("Process returned exit code %d."), ev.GetExitCode())
                              + END_OF_LINE);
    }

    if (process)
    {
        wxKill(ev.GetPid(), wxSIGTERM);
        delete process;
        process=0;
        pid=0;
    }
    btnOK->Enable();
    btnCancel->Enable();
}


void ExternProcessDialog::Abort()
{
    timer->Stop();
    if (process)
    {
        done=false;
        wxProcess *tmpProcess=process;
        process=0;
        wxKill(pid, wxSIGTERM);
        pid=0;
        delete tmpProcess;
    }
}
