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
#include "frmMain.h"
#include "pgConn.h"

#include "menu.h"



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

void pgDialog::LoadResource(const wxChar *name)
{
    if (name)
        dlgName = name;
    wxXmlResource::Get()->LoadDialog(this, GetParent(), dlgName); 
}

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

BEGIN_EVENT_TABLE(DialogWithHelp, wxDialog)
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


