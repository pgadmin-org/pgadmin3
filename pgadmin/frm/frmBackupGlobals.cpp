//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmBackupGlobals.cpp - Backup globals dialogue
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/settings.h>


// App headers
#include "pgAdmin3.h"
#include "frm/frmMain.h"
#include "frm/frmBackupGlobals.h"
#include "utils/sysLogger.h"
#include "schema/pgSchema.h"
#include "schema/pgTable.h"

// Icons
#include "images/backup.xpm"


#define nbNotebook              CTRL_NOTEBOOK("nbNotebook")
#define txtFilename             CTRL_TEXT("txtFilename")
#define btnFilename             CTRL_BUTTON("btnFilename")
#define chkVerbose              CTRL_CHECKBOX("chkVerbose")


BEGIN_EVENT_TABLE(frmBackupGlobals, ExternProcessDialog)
    EVT_TEXT(XRCID("txtFilename"),          frmBackupGlobals::OnChange)
    EVT_BUTTON(XRCID("btnFilename"),        frmBackupGlobals::OnSelectFilename)
    EVT_BUTTON(wxID_OK,                     frmBackupGlobals::OnOK)
    EVT_CLOSE(                              ExternProcessDialog::OnClose)
END_EVENT_TABLE()



frmBackupGlobals::frmBackupGlobals(frmMain *form, pgObject *obj) : ExternProcessDialog(form)
{
    object=obj;

    wxWindowBase::SetFont(settings->GetSystemFont());
    LoadResource(form, wxT("frmBackupGlobals"));
    RestorePosition();

    SetTitle(wxString::Format(_("Backup globals %s %s"), object->GetTranslatedTypeName().c_str(), object->GetFullIdentifier().c_str()));

    wxString val;
    settings->Read(wxT("frmBackupGlobals/LastFile"), &val, wxEmptyString);
    txtFilename->SetValue(val);

	if (object->GetMetaType() == PGM_SERVER)
	{
		if (!((pgServer *)object)->GetPasswordIsStored())
			environment.Add(wxT("PGPASSWORD=") + ((pgServer *)object)->GetPassword());
	}
	else
	{
		if (!object->GetDatabase()->GetServer()->GetPasswordIsStored())
			environment.Add(wxT("PGPASSWORD=") + object->GetDatabase()->GetServer()->GetPassword());
	}

    // Icon
    SetIcon(wxIcon(backup_xpm));

    txtMessages = CTRL_TEXT("txtMessages");
    txtMessages->SetMaxLength(0L);
    btnOK->Disable();

    wxCommandEvent ev;
    OnChange(ev);
}


frmBackupGlobals::~frmBackupGlobals()
{
    SavePosition();
}


wxString frmBackupGlobals::GetHelpPage() const
{
    wxString page;
    page = wxT("pg/app-pgdumpall");
    return page;
}


void frmBackupGlobals::OnSelectFilename(wxCommandEvent &ev)
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


void frmBackupGlobals::OnChange(wxCommandEvent &ev)
{
    if (!process && !done)
        btnOK->Enable(!txtFilename->GetValue().IsEmpty());
}

wxString frmBackupGlobals::GetCmd(int step)
{
    wxString cmd = getCmdPart1();

    return cmd + getCmdPart2();
}


wxString frmBackupGlobals::GetDisplayCmd(int step)
{
    wxString cmd = getCmdPart1();

    return cmd + getCmdPart2();
}


wxString frmBackupGlobals::getCmdPart1()
{
    extern wxString pgBackupAllExecutable;
    extern wxString edbBackupAllExecutable;

	pgServer * server;
	if (object->GetMetaType() == PGM_SERVER)
		server = (pgServer *)object;
	else
        server=object->GetDatabase()->GetServer();

    wxString cmd;
    if (server->GetConnection()->EdbMinimumVersion(8,0))
        cmd=edbBackupAllExecutable;
    else
        cmd=pgBackupAllExecutable;

    cmd +=  wxT(" -i")
            wxT(" -h ") + server->GetName()
         +  wxT(" -p ") + NumToStr((long)server->GetPort())
         +  wxT(" -U ") + server->GetUsername();
    return cmd;
}


wxString frmBackupGlobals::getCmdPart2()
{
    wxString cmd;
    // if (server->GetSSL())
    // pg_dump doesn't support ssl

    if (chkVerbose->GetValue())
        cmd.Append(wxT(" -v"));

    cmd.Append(wxT(" -f \"") + txtFilename->GetValue() + wxT("\""));

    cmd.Append(wxT(" --globals-only"));

    return cmd;
}


void frmBackupGlobals::Go()
{
    txtFilename->SetFocus();
    Show(true);
}

void frmBackupGlobals::OnOK(wxCommandEvent &ev)
{
    settings->Write(wxT("frmBackupGlobals/LastFile"), txtFilename->GetValue());
    ExternProcessDialog::OnOK(ev);
}

backupGlobalsFactory::backupGlobalsFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar) : contextActionFactory(list)
{
    mnu->Append(id, _("&Backup globals..."), _("Creates a backup of the global database objects"));
}


wxWindow *backupGlobalsFactory::StartDialog(frmMain *form, pgObject *obj)
{
    frmBackupGlobals *frm=new frmBackupGlobals(form, obj);
    frm->Go();
    return 0;
}


bool backupGlobalsFactory::CheckEnable(pgObject *obj)
{
    extern wxString pgBackupExecutable;
    extern wxString edbBackupExecutable;

	if (!obj)
		return false;

	if (obj->GetMetaType() == PGM_SERVER)
		if (!((pgServer *)obj)->GetConnected())
			return false;

    if (obj->GetConnection()->EdbMinimumVersion(8, 0))
        return obj->CanBackupGlobals() && !edbBackupExecutable.IsEmpty() && pgAppMinimumVersion(edbBackupExecutable, 8, 3);
    else
        return obj->CanBackupGlobals() && !pgBackupExecutable.IsEmpty() && pgAppMinimumVersion(pgBackupExecutable, 8, 3);
}
