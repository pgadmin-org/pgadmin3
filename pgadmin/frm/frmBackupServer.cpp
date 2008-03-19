//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmBackupServer.cpp - Backup server dialogue
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/settings.h>


// App headers
#include "pgAdmin3.h"
#include "frm/frmMain.h"
#include "frm/frmBackupServer.h"
#include "utils/sysLogger.h"
#include "schema/pgSchema.h"
#include "schema/pgTable.h"

// Icons
#include "images/backup.xpm"


#define nbNotebook              CTRL_NOTEBOOK("nbNotebook")
#define txtFilename             CTRL_TEXT("txtFilename")
#define btnFilename             CTRL_BUTTON("btnFilename")
#define chkVerbose              CTRL_CHECKBOX("chkVerbose")


BEGIN_EVENT_TABLE(frmBackupServer, ExternProcessDialog)
    EVT_TEXT(XRCID("txtFilename"),          frmBackupServer::OnChange)
    EVT_BUTTON(XRCID("btnFilename"),        frmBackupServer::OnSelectFilename)
    EVT_BUTTON(wxID_OK,                     frmBackupServer::OnOK)
    EVT_CLOSE(                              ExternProcessDialog::OnClose)
END_EVENT_TABLE()



frmBackupServer::frmBackupServer(frmMain *form, pgObject *obj) : ExternProcessDialog(form)
{
    object=obj;

    wxWindowBase::SetFont(settings->GetSystemFont());
    LoadResource(form, wxT("frmBackupServer"));
    RestorePosition();

    SetTitle(wxString::Format(_("Backup globals %s %s"), object->GetTranslatedTypeName().c_str(), object->GetFullIdentifier().c_str()));

    wxString val;
    settings->Read(wxT("frmBackupServer/LastFile"), &val, wxEmptyString);
    txtFilename->SetValue(val);

    if (!((pgServer *)object)->GetPasswordIsStored())
       environment.Add(wxT("PGPASSWORD=") + ((pgServer *)object)->GetPassword());

    // Icon
    SetIcon(wxIcon(backup_xpm));

    txtMessages = CTRL_TEXT("txtMessages");
    txtMessages->SetMaxLength(0L);
    btnOK->Disable();

    wxCommandEvent ev;
    OnChange(ev);
}


frmBackupServer::~frmBackupServer()
{
    SavePosition();
}


wxString frmBackupServer::GetHelpPage() const
{
    wxString page;
    page = wxT("pg/app-pgdumpall");
    return page;
}


void frmBackupServer::OnSelectFilename(wxCommandEvent &ev)
{
    wxString title, prompt;

    title  = _("Select output file");
    prompt = _("Query files (*.sql)|*.sql|All files (*.*)|*.*");
    
    wxFileDialog file(this, title, ::wxPathOnly(txtFilename->GetValue()), txtFilename->GetValue(), prompt, wxFD_SAVE);

    if (file.ShowModal() == wxID_OK)
    {
        txtFilename->SetValue(file.GetPath());
        OnChange(ev);
    }
}


void frmBackupServer::OnChange(wxCommandEvent &ev)
{
    if (!process && !done)
        btnOK->Enable(!txtFilename->GetValue().IsEmpty());
}

wxString frmBackupServer::GetCmd(int step)
{
    wxString cmd = getCmdPart1();

    return cmd + getCmdPart2();
}


wxString frmBackupServer::GetDisplayCmd(int step)
{
    wxString cmd = getCmdPart1();

    return cmd + getCmdPart2();
}


wxString frmBackupServer::getCmdPart1()
{
    extern wxString pgBackupAllExecutable;
    extern wxString edbBackupAllExecutable;

    pgServer *server = (pgServer *)object;

    wxString cmd;
    if (server->GetConnection()->EdbMinimumVersion(8,0))
        cmd=edbBackupAllExecutable;
    else
        cmd=pgBackupAllExecutable;

    if (!server->GetName().IsEmpty())
        cmd += wxT(" -h ") + server->GetName();

    cmd +=  wxT(" -p ") + NumToStr((long)server->GetPort())
         +  wxT(" -U ") + server->GetUsername();
    return cmd;
}


wxString frmBackupServer::getCmdPart2()
{
    wxString cmd;
    // if (server->GetSSL())
    // pg_dump doesn't support ssl

    if (chkVerbose->GetValue())
        cmd.Append(wxT(" -v"));

    cmd.Append(wxT(" -f \"") + txtFilename->GetValue() + wxT("\""));

    return cmd;
}


void frmBackupServer::Go()
{
    txtFilename->SetFocus();
    Show(true);
}

void frmBackupServer::OnOK(wxCommandEvent &ev)
{
    if (!done)
    {
        if (processedFile == txtFilename->GetValue())
        {
            if (wxMessageBox(_("Are you sure you wish to run a backup to this file again?"), _("Repeat backup?"), wxICON_QUESTION | wxYES_NO) == wxNO)
                return;
        }

        processedFile = txtFilename->GetValue();
    }

    settings->Write(wxT("frmBackupServer/LastFile"), txtFilename->GetValue());
    ExternProcessDialog::OnOK(ev);
}

backupServerFactory::backupServerFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar) : contextActionFactory(list)
{
    mnu->Append(id, _("&Backup server..."), _("Creates a backup of the entire server"));
}


wxWindow *backupServerFactory::StartDialog(frmMain *form, pgObject *obj)
{
    frmBackupServer *frm=new frmBackupServer(form, obj);
    frm->Go();
    return 0;
}


bool backupServerFactory::CheckEnable(pgObject *obj)
{
    extern wxString pgBackupExecutable;
    extern wxString edbBackupExecutable;

    if (!obj)
        return false;

    if (!((pgServer *)obj)->GetConnected() || obj->GetMetaType() != PGM_SERVER)
        return false;

    if (obj->GetConnection()->EdbMinimumVersion(8, 0))
        return !edbBackupExecutable.IsEmpty() && pgAppMinimumVersion(edbBackupExecutable, 8, 3);
    else
        return !pgBackupExecutable.IsEmpty() && pgAppMinimumVersion(pgBackupExecutable, 8, 3);
}
