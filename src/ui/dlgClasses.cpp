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
#include "dlgClasses.h"
#include "pgConn.h"

#include "menu.h"


BEGIN_EVENT_TABLE(DialogWithHelp, wxDialog)
    EVT_MENU(MNU_HELP,                  DialogWithHelp::OnHelp)
    EVT_BUTTON(XRCID("btnHelp"),        DialogWithHelp::OnHelp)
END_EVENT_TABLE();


DialogWithHelp::DialogWithHelp(frmMain *frame) : wxDialog()
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
    }
    else
    {
        Abort();
        Destroy();
    }
}


