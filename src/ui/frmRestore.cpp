//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmRestore.cpp - Restore database dialogue
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/settings.h>


// App headers
#include "pgAdmin3.h"
#include "frmRestore.h"
#include "sysLogger.h"
#include "pgSchema.h"
#include "pgTable.h"

// Icons
#include "images/restore.xpm"


#define nbNotebook              CTRL_NOTEBOOK("nbNotebook")
#define txtFilename             CTRL_TEXT("txtFilename")
#define btnFilename             CTRL_BUTTON("btnFilename")
#define rbxFormat               CTRL_RADIOBOX("rbxFormat")
#define chkBlobs                CTRL_CHECKBOX("chkBlobs")
#define chkOid                  CTRL_CHECKBOX("chkOid")
#define chkInsert               CTRL_CHECKBOX("chkInsert")
#define chkDisableDollar        CTRL_CHECKBOX("chkDisableDollar")
#define sbxPlainOptions         CTRL_STATICBOX("sbxPlainOptions")
#define chkOnlyData             CTRL_CHECKBOX("chkOnlyData")
#define chkOnlySchema           CTRL_CHECKBOX("chkOnlySchema")
#define chkNoOwner              CTRL_CHECKBOX("chkNoOwner")
#define chkCreateDb             CTRL_CHECKBOX("chkCreateDb")
#define chkDropDb               CTRL_CHECKBOX("chkDropDb")
#define chkDisableTrigger       CTRL_CHECKBOX("chkDisableTrigger")
#define chkVerbose              CTRL_CHECKBOX("chkVerbose")


BEGIN_EVENT_TABLE(frmRestore, ExternProcessDialog)
    EVT_TEXT(XRCID("txtFilename"),          frmRestore::OnChangeName)
    EVT_BUTTON(XRCID("btnFilename"),        frmRestore::OnSelectFilename)
    EVT_RADIOBOX(XRCID("rbxFormat"),        frmRestore::OnChangePlain)
    EVT_CHECKBOX(XRCID("chkOnlyData"),      frmRestore::OnChangePlain)
    EVT_CHECKBOX(XRCID("chkOnlySchema"),    frmRestore::OnChangePlain)
    EVT_CHECKBOX(XRCID("chkNoOwner"),       frmRestore::OnChangePlain)
END_EVENT_TABLE()



frmRestore::frmRestore(frmMain *form, pgObject *obj) : ExternProcessDialog(form)
{
    object=obj;
    wxLogInfo(wxT("Creating a restore dialogue for %s %s"), object->GetTypeName().c_str(), object->GetFullName().c_str());

    wxWindowBase::SetFont(settings->GetSystemFont());
    LoadResource(wxT("frmRestore"));
    RestorePosition();

    SetTitle(wxString::Format(_("Restore %s %s"), object->GetTypeName().c_str(), object->GetFullIdentifier().c_str()));


    // Icon
    SetIcon(wxIcon(restore_xpm));

    txtMessages = CTRL_TEXT("txtMessages");
    txtMessages->SetMaxLength(0L);
    btnOK->Disable();

    wxCommandEvent ev;
    OnChangePlain(ev);
    CenterOnParent();
}


frmRestore::~frmRestore()
{
    wxLogInfo(wxT("Destroying a restore dialogue"));
    SavePosition();
}


wxString frmRestore::GetHelpPage() const
{
    wxString page;
    page = wxT("app-pgdump");
    return page;
}


void frmRestore::OnSelectFilename(wxCommandEvent &ev)
{
    wxFileDialog file(this, _("Select backup filename"), wxGetHomeDir(), txtFilename->GetValue(), 
        _("Backup files (*.backup)|*.backup|All files (*.*)|*.*"));

    if (file.ShowModal() == wxID_OK)
    {
        txtFilename->SetValue(file.GetPath());
        OnChangeName(ev);
    }
}


void frmRestore::OnChangeName(wxCommandEvent &ev)
{
    btnOK->Enable(!txtFilename->GetValue().IsEmpty());
}


void frmRestore::OnChangePlain(wxCommandEvent &ev)
{
    bool isPlain = (rbxFormat->GetSelection() == 2);
    sbxPlainOptions->Enable(isPlain);
    chkBlobs->Enable(!isPlain);
    chkOnlyData->Enable(isPlain && !chkOnlySchema->GetValue());
    if (isPlain)
        isPlain = !chkOnlyData->GetValue();

    chkOnlySchema->Enable(isPlain);
    chkNoOwner->Enable(isPlain);
    chkDropDb->Enable(isPlain);
    chkCreateDb->Enable(isPlain);
    chkDisableTrigger->Enable(isPlain);
}


wxString frmRestore::GetCmd()
{
    wxString cmd = getCmdPart1();
    pgServer *server=object->GetDatabase()->GetServer();

    if (!server->GetTrusted())
        cmd += wxT(" -W ") + server->GetPassword();

    return cmd + getCmdPart2();
}


wxString frmRestore::GetDisplayCmd()
{
    wxString cmd = getCmdPart1();
    pgServer *server=object->GetDatabase()->GetServer();

    if (!server->GetTrusted())
        cmd += wxT(" -W ****");

    return cmd + getCmdPart2();
}


wxString frmRestore::getCmdPart1()
{
    extern wxString restoreExecutable;

    wxString cmd=restoreExecutable;

    pgServer *server=object->GetDatabase()->GetServer();
    cmd += wxT(" -h ") + server->GetName()
         +  wxT(" -p ") + NumToStr((long)server->GetPort())
         +  wxT(" -U ") + server->GetUsername();
    return cmd;
}


wxString frmRestore::getCmdPart2()
{
    wxString cmd;
    // if (server->GetSSL())
    // pg_dump doesn't support ssl

    switch (rbxFormat->GetSelection())
    {
        case 0: // compressed
        case 1: // tar
        {
            if (chkBlobs->GetValue())
                cmd.Append(wxT(" -b"));
            break;
        }
        case 2:
        {
            if (chkOnlyData->GetValue())
                cmd.Append(wxT(" -a"));
            else
            {
                if (chkOnlySchema->GetValue())
                    cmd.Append(wxT(" -s"));
                if (chkOnlySchema->GetValue())
                    cmd.Append(wxT(" -s"));
                if (chkNoOwner->GetValue())
                    cmd.Append(wxT(" -O"));
                if (chkCreateDb->GetValue())
                    cmd.Append(wxT(" -C"));
                if (chkDropDb->GetValue())
                    cmd.Append(wxT(" -c"));
                if (chkDisableTrigger->GetValue())
                    cmd.Append(wxT(" --disable-triggers"));
            }
            break;
        }
    }

    if (chkOid->GetValue())
        cmd.Append(wxT(" -o"));
    if (chkInsert->GetValue())
        cmd.Append(wxT(" -D"));
    if (chkDisableDollar->GetValue())
        cmd.Append(wxT(" --disable-dollar-quoting"));
    if (chkVerbose->GetValue())
        cmd.Append(wxT(" -v"));

    cmd.Append(wxT(" -f \"") + txtFilename->GetValue() + wxT("\""));

    pgSchema *schema=0;
    if (object->GetType() == PG_SCHEMA)
        schema =(pgSchema*)object;
    else if (object->GetType() == PG_TABLE)
        cmd.Append(wxT(" -t ") + ((pgTable*)object)->GetQuotedIdentifier());

    if (schema)
        cmd.Append(wxT(" -n ") + schema->GetQuotedIdentifier());

    cmd.Append(wxT(" ") + object->GetDatabase()->GetQuotedIdentifier());

    return cmd;
}


void frmRestore::Go()
{
    txtFilename->SetFocus();
    Show(true);
}
