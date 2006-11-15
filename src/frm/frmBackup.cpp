//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
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
#include "frmMain.h"
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

    canBlob = (obj->GetMetaType() == PGM_DATABASE);
    chkBlobs->SetValue(canBlob);
    chkDisableDollar->Enable(obj->GetConnection()->BackendMinimumVersion(7, 5));

    if (!object->GetDatabase()->GetServer()->GetPasswordIsStored())
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
    page = wxT("pg/app-pgdump");
    return page;
}


void frmBackup::OnSelectFilename(wxCommandEvent &ev)
{
    wxString title, prompt;

    if (rbxFormat->GetSelection() == 2) // plain
    {
        title  = _("Select output file");
        prompt = _("Query files (*.sql)|*.sql|All files (*.*)|*.*");
    }
    else
    {
        title  = _("Select backup filename");
        prompt = _("Backup files (*.backup)|*.backup|All files (*.*)|*.*");
    }

    
    wxFileDialog file(this, title, wxGetHomeDir(), txtFilename->GetValue(), prompt, wxSAVE);

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
            {
                cmd.Append(wxT(" -a"));
                if (chkDisableTrigger->GetValue())
                    cmd.Append(wxT(" --disable-triggers"));
            }
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

    if (object->GetMetaType() == PGM_SCHEMA)
        cmd.Append(wxT(" -n ") + ((pgSchema*)object)->GetQuotedIdentifier());
    else if (object->GetMetaType() == PGM_TABLE) 
    {
        cmd.Append(wxT(" -t ") + ((pgTable*)object)->GetQuotedIdentifier());
        cmd.Append(wxT(" -n ") + ((pgTable*)object)->GetSchema()->GetQuotedIdentifier());
    }

    cmd.Append(wxT(" ") + object->GetDatabase()->GetQuotedIdentifier());

    return cmd;
}


void frmBackup::Go()
{
    txtFilename->SetFocus();
    Show(true);
}


backupFactory::backupFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar) : contextActionFactory(list)
{
    mnu->Append(id, _("&Backup..."), _("Creates a backup of the current database to a local file"));
}


wxWindow *backupFactory::StartDialog(frmMain *form, pgObject *obj)
{
    frmBackup *frm=new frmBackup(form, obj);
    frm->Go();
    return 0;
}


bool backupFactory::CheckEnable(pgObject *obj)
{
    extern wxString backupExecutable;

    return obj && obj->CanBackup() && !backupExecutable.IsEmpty();
}
