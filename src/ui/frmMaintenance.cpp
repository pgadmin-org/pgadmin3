//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmMaintenance.cpp - Maintenance options selection dialogue
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>


// App headers
#include "pgAdmin3.h"
#include "frmMaintenance.h"
#include "sysLogger.h"
#include "pgIndex.h"

// Icons
#include "images/vacuum.xpm"


BEGIN_EVENT_TABLE(frmMaintenance, DialogWithHelp)
    EVT_RADIOBOX(XRCID("rbxAction"),    frmMaintenance::OnAction)
    EVT_BUTTON (XRCID("btnOK"),         frmMaintenance::OnOK)
    EVT_BUTTON (XRCID("btnCancel"),     frmMaintenance::OnCancel)
    EVT_CLOSE(                          frmMaintenance::OnClose)
END_EVENT_TABLE()

#define rbxAction               CTRL_RADIOBOX("rbxAction")
#define sbxOptions              CTRL_STATICBOX("sbxOptions")
#define chkFull                 CTRL_CHECKBOX("chkFull")
#define chkFreeze               CTRL_CHECKBOX("chkFreeze")
#define chkAnalyze              CTRL_CHECKBOX("chkAnalyze")
#define sbxReindexOptions       CTRL_STATICBOX("sbxReindexOptions")
#define chkForce                CTRL_CHECKBOX("chkForce")
#define chkRecreate             CTRL_CHECKBOX("chkRecreate")
#define chkVerbose              CTRL_CHECKBOX("chkVerbose")
#define txtMessages             CTRL_TEXT("txtMessages")

#define stBitmap                CTRL("stBitmap", wxStaticBitmap)

#define btnOK                   CTRL_BUTTON("btnOK")
#define btnCancel               CTRL_BUTTON("btnCancel")



frmMaintenance::frmMaintenance(frmMain *form, pgObject *obj) : DialogWithHelp(form)
{
    object=obj;
    thread=0;
    wxLogInfo(wxT("Creating a maintenance dialogue for %s %s"), object->GetTypeName().c_str(), object->GetFullName().c_str());

    wxWindowBase::SetFont(settings->GetSystemFont());
    wxXmlResource::Get()->LoadDialog(this, form, wxT("frmMaintenance"));
    SetTitle(wxString::Format(_("Maintain %s %s"), object->GetTypeName().c_str(), object->GetFullIdentifier().c_str()));

    // Icon
    SetIcon(wxIcon(vacuum_xpm));

    // Bitmap
    stBitmap->SetBitmap(wxBitmap(vacuum_xpm));
    txtMessages->SetMaxLength(0L);

    if (object->GetType() == PG_INDEX)
    {
        rbxAction->SetSelection(2);
        rbxAction->Enable(0, false);
        rbxAction->Enable(1, false);
    }
    wxCommandEvent ev;
    OnAction(ev);
    CenterOnParent();
}


frmMaintenance::~frmMaintenance()
{
    wxLogInfo(wxT("Destroying a maintenance dialogue"));
    Abort();
}


wxString frmMaintenance::GetHelpPage() const
{
    wxString page;
    switch ((XRCCTRL(*(frmMaintenance*)this, "rbxAction", wxRadioBox))->GetSelection())
    {
        case 0: page = wxT("sql-vacuum"); break;
        case 1: page = wxT("sql-analyze"); break;
        case 2: page = wxT("sql-reindex"); break;
    }
    return page;
}


void frmMaintenance::Abort()
{
    if (thread)
    {
        if (thread->IsRunning())
            thread->Delete();
        delete thread;
        thread=0;
    }
}


void frmMaintenance::OnAction(wxCommandEvent& ev)
{
    bool isVacuum = (rbxAction->GetSelection() == 0);
    sbxOptions->Enable(isVacuum);
    chkFull->Enable(isVacuum);
    chkFreeze->Enable(isVacuum);
    chkAnalyze->Enable(isVacuum);

    bool isReindex = (rbxAction->GetSelection() == 2);
    sbxReindexOptions->Enable(isReindex && object->GetType() == PG_DATABASE || object->GetType() == PG_INDEX);
    chkForce->Enable(isReindex && object->GetType() == PG_DATABASE);
    chkRecreate->Enable(isReindex && object->GetType() == PG_INDEX);
}


void frmMaintenance::OnOK(wxCommandEvent& ev)
{
    if (!thread)
    {
        btnOK->Disable();

        wxString sql;

        txtMessages->Clear();

        switch (rbxAction->GetSelection())
        {
            case 0:
            {
                sql=wxT("VACUUM ");

                if (chkFull->GetValue())
                    sql += wxT("FULL ");
                if (chkFreeze->GetValue())
                    sql += wxT("FREEZE ");
                if (chkVerbose->GetValue())
                    sql += wxT("VERBOSE ");
                if (chkAnalyze->GetValue())
                    sql += wxT("ANALYZE ");

                if (object->GetType() != PG_DATABASE)
                    sql += object->GetQuotedFullIdentifier();
                
                break;
            }
            case 1:
            {
                sql = wxT("ANALYZE ");
                if (chkVerbose->GetValue())
                    sql += wxT("VERBOSE ");
                
                if (object->GetType() != PG_DATABASE)
                    sql += object->GetQuotedFullIdentifier();

                break;
            }
            case 2:
            {
                if (chkRecreate->GetValue())
                {
                    sql = wxT("DROP INDEX ") + object->GetQuotedFullIdentifier() + wxT(";\n")
                        + ((pgIndex*)object)->GetCreate()
                        + object->GetCommentSql();
                }
                else
                {
                    sql = wxT("REINDEX ") + object->GetTypeName().Upper()
                        + wxT(" ") + object->GetQuotedFullIdentifier();
                    if (chkForce->GetValue())
                        sql += wxT(" FORCE");
                }
                break;
            }
        }


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
            txtMessages->AppendText(thread->GetMessagesAndClear());
            wxYield();
        }

        if (thread)
        {
            txtMessages->AppendText(thread->GetMessagesAndClear());

            if (thread->DataSet() != NULL)
                wxLogDebug(wxString::Format(_("%d rows."), thread->DataSet()->NumRows()));

            txtMessages->AppendText(_("Total query runtime: ") 
                + (wxGetLocalTimeMillis()-startTime).ToString() + wxT(" ms."));

                btnOK->SetLabel(_("Done"));
            btnCancel->Disable();
        }
        else
            txtMessages->AppendText(_("\nCancelled.\n"));

        btnOK->Enable();
    }
    else
    {
        Abort();
        Destroy();
    }
}


void frmMaintenance::OnClose(wxCloseEvent& event)
{
    Abort();
    Destroy();
}


void frmMaintenance::OnCancel(wxCommandEvent& ev)
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


void frmMaintenance::Go()
{
    chkFull->SetFocus();
    Show(true);
}
