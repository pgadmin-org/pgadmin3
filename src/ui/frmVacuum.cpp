//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmVacuum.cpp - Vacuum options selection dialogue
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>


// App headers
#include "pgAdmin3.h"
#include "frmVacuum.h"
#include "sysLogger.h"

// Icons
#include "images/vacuum.xpm"


BEGIN_EVENT_TABLE(frmVacuum, DialogWithHelp)
    EVT_BUTTON (XRCID("btnOK"),         frmVacuum::OnOK)
    EVT_BUTTON (XRCID("btnCancel"),     frmVacuum::OnCancel)
    EVT_CLOSE(                          frmVacuum::OnClose)
END_EVENT_TABLE()

#define chkFull     CTRL("chkFull", wxCheckBox)
#define chkFreeze   CTRL("chkFreeze", wxCheckBox)
#define chkAnalyze  CTRL("chkAnalyze", wxCheckBox)
#define stBitmap    CTRL("stBitmap", wxStaticBitmap)
#define btnOK       CTRL("btnOK", wxButton)
#define btnCancel   CTRL("btnCancel", wxButton)



frmVacuum::frmVacuum(frmMain *form, pgObject *obj) : DialogWithHelp(form)
{
    object=obj;
    thread=0;
    wxLogInfo(wxT("Creating a vacuum dialogue for %s %s"), object->GetTypeName().c_str(), object->GetFullName().c_str());

    wxXmlResource::Get()->LoadDialog(this, form, wxT("frmVacuum"));
    SetTitle(_("Vacuum ") + object->GetTypeName() + wxT(" ") + object->GetFullIdentifier());

    // Icon
    SetIcon(wxIcon(vacuum_xpm));

    // Bitmap
    stBitmap->SetBitmap(wxBitmap(vacuum_xpm));

    CenterOnParent();
}

frmVacuum::~frmVacuum()
{
    wxLogInfo(wxT("Destroying a vacuum dialogue"));
    Abort();
}


wxString frmVacuum::GetHelpPage() const
{
    return wxT("sql-vacuum.html");
}

void frmVacuum::Abort()
{
    if (thread)
    {
        if (thread->IsRunning())
            thread->Delete();
        delete thread;
        thread=0;
    }
}


void frmVacuum::OnOK(wxCommandEvent& ev)
{
    if (!thread)
    {
        btnOK->Disable();

        wxString sql=wxT("VACUUM ");

        if (chkFull->GetValue())
            sql += wxT("FULL ");
        if (chkFreeze->GetValue())
            sql += wxT("FREEZE ");
        if (chkAnalyze->GetValue())
            sql += wxT("ANALYZE ");

        if (object->GetType() != PG_DATABASE)
            sql += object->GetTypeName() + wxT(" ")
                +  object->GetQuotedFullIdentifier();

        thread=new pgQueryThread(object->GetConnection()->connection(), sql);
        if (thread->Create() != wxTHREAD_NO_ERROR)
        {
            Abort();
            return;
        }

        thread->Run();

        while (thread && thread->IsRunning())
        {
            wxYield();
            wxUsleep(10);
            // here could be the animation
        }

        if (thread)
        {
            btnOK->SetLabel(_("Done"));
        }
        btnOK->Enable();
    }
    else
    {
        Abort();
        Destroy();
    }
}


void frmVacuum::OnClose(wxCloseEvent& event)
{
    Abort();
    Destroy();
}


void frmVacuum::OnCancel(wxCommandEvent& ev)
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

void frmVacuum::Go()
{
    chkFull->SetFocus();
    Show(true);
}
