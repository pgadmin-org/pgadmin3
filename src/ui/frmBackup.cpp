//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmBackup.cpp - Backup database dialogue
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/settings.h>


// App headers
#include "pgAdmin3.h"
#include "frmBackup.h"
#include "sysLogger.h"
#include "pgSchema.h"
#include "pgTable.h"

// Icons
#include "images/backup.xpm"


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


BEGIN_EVENT_TABLE(frmBackup, ExternProcessDialog)
    EVT_TEXT(XRCID("txtFilename"),          frmBackup::OnChange)
    EVT_BUTTON(XRCID("btnFilename"),        frmBackup::OnSelectFilename)
    EVT_RADIOBOX(XRCID("rbxFormat"),        frmBackup::OnChangePlain)
    EVT_CHECKBOX(XRCID("chkOnlyData"),      frmBackup::OnChangePlain)
    EVT_CHECKBOX(XRCID("chkOnlySchema"),    frmBackup::OnChangePlain)
    EVT_CHECKBOX(XRCID("chkNoOwner"),       frmBackup::OnChangePlain)
    EVT_CLOSE(                              ExternProcessDialog::OnClose)
END_EVENT_TABLE()



frmBackup::frmBackup(frmMain *form, pgObject *obj) : ExternProcessDialog(form)
{
    object=obj;
    wxLogInfo(wxT("Creating a backup dialogue for %s %s"), object->GetTypeName().c_str(), object->GetFullName().c_str());

    wxWindowBase::SetFont(settings->GetSystemFont());
    LoadResource(form, wxT("frmBackup"));
    RestorePosition();

    SetTitle(wxString::Format(_("Backup %s %s"), object->GetTranslatedTypeName().c_str(), object->GetFullIdentifier().c_str()));

    canBlob = (obj->GetType() == PG_DATABASE);
    chkBlobs->SetValue(canBlob);
    chkDisableDollar->Enable(obj->GetConnection()->BackendMinimumVersion(7, 5));

    environment.Add(wxT("PGPASSWORD=") + object->GetDatabase()->GetServer()->GetPassword());

    // Icon
    SetIcon(wxIcon(backup_xpm));

	// fix translation problem
	wxString dollarLabel=wxGetTranslation(_("Disable $$ quoting"));
	dollarLabel.Replace(wxT("$$"), wxT("$"));
	chkDisableDollar->SetLabel(dollarLabel);
	chkDisableDollar->SetSize(chkDisableDollar->GetBestSize());

    txtMessages = CTRL_TEXT("txtMessages");
    txtMessages->SetMaxLength(0L);
    btnOK->Disable();

    wxCommandEvent ev;
    OnChangePlain(ev);
    CenterOnParent();
}


frmBackup::~frmBackup()
{
    wxLogInfo(wxT("Destroying a backup dialogue"));
    SavePosition();
}


wxString frmBackup::GetHelpPage() const
{
    wxString page;
    page = wxT("app-pgdump");
    return page;
}


void frmBackup::OnSelectFilename(wxCommandEvent &ev)
{
    wxFileDialog file(this, _("Select backup filename"), wxGetHomeDir(), txtFilename->GetValue(), 
        _("Backup files (*.backup)|*.backup|All files (*.*)|*.*"));

    if (file.ShowModal() == wxID_OK)
    {
        txtFilename->SetValue(file.GetPath());
        OnChange(ev);
    }
}


void frmBackup::OnChange(wxCommandEvent &ev)
{
    if (!process && !done)
        btnOK->Enable(!txtFilename->GetValue().IsEmpty());
}


void frmBackup::OnChangePlain(wxCommandEvent &ev)
{
    bool isPlain = (rbxFormat->GetSelection() == 2);
    sbxPlainOptions->Enable(isPlain);
    chkBlobs->Enable(canBlob && !isPlain);
    chkOnlyData->Enable(isPlain && !chkOnlySchema->GetValue());
    if (isPlain)
        isPlain = !chkOnlyData->GetValue();

    chkOnlySchema->Enable(isPlain);
    chkNoOwner->Enable(isPlain);
    chkDropDb->Enable(isPlain);
    chkCreateDb->Enable(isPlain);
    chkDisableTrigger->Enable(chkOnlyData->GetValue());
}


wxString frmBackup::GetCmd(int step)
{
    wxString cmd = getCmdPart1();

    return cmd + getCmdPart2();
}


wxString frmBackup::GetDisplayCmd(int step)
{
    wxString cmd = getCmdPart1();

    return cmd + getCmdPart2();
}


wxString frmBackup::getCmdPart1()
{
    extern wxString backupExecutable;

    wxString cmd=backupExecutable;

    pgServer *server=object->GetDatabase()->GetServer();
    cmd +=  wxT(" -i")
            wxT(" -h ") + server->GetName()
         +  wxT(" -p ") + NumToStr((long)server->GetPort())
         +  wxT(" -U ") + server->GetUsername();
    return cmd;
}


wxString frmBackup::getCmdPart2()
{
    wxString cmd;
    // if (server->GetSSL())
    // pg_dump doesn't support ssl

    switch (rbxFormat->GetSelection())
    {
        case 0: // compressed
        {
            cmd.Append(wxT(" -F c"));
            if (chkBlobs->GetValue())
                cmd.Append(wxT(" -b"));
            break;
        }
        case 1: // tar
        {
            cmd.Append(wxT(" -F t"));
            if (chkBlobs->GetValue())
                cmd.Append(wxT(" -b"));
            break;
        }
        case 2:
        {
            cmd.Append(wxT(" -F p"));
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


void frmBackup::Go()
{
    txtFilename->SetFocus();
    Show(true);
}
