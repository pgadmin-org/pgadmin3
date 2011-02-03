//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
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
	object = obj;

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

		// Pass the SSL mode via the environment
		environment.Add(wxT("PGSSLMODE=") + ((pgServer *)object)->GetConnection()->GetSslModeName());
	}
	else
	{
		if (!object->GetDatabase()->GetServer()->GetPasswordIsStored())
			environment.Add(wxT("PGPASSWORD=") + object->GetServer()->GetPassword());

		// Pass the SSL mode via the environment
		environment.Add(wxT("PGSSLMODE=") + object->GetServer()->GetConnection()->GetSslModeName());
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
	page = wxT("pg/app-pg-dumpall");
	return page;
}


void frmBackupGlobals::OnSelectFilename(wxCommandEvent &ev)
{
	wxString title, prompt, FilenameOnly;

	title  = _("Select output file");
#ifdef __WXMSW__
	prompt = _("Query files (*.sql)|*.sql|All files (*.*)|*.*");
#else
	prompt = _("Query files (*.sql)|*.sql|All files (*)|*");
#endif

	wxFileName::SplitPath(txtFilename->GetValue(), NULL, NULL, &FilenameOnly, NULL);
	wxFileDialog file(this, title, ::wxPathOnly(txtFilename->GetValue()), FilenameOnly, prompt, wxFD_SAVE);

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
	pgServer *server;
	if (object->GetMetaType() == PGM_SERVER)
		server = (pgServer *)object;
	else
		server = object->GetDatabase()->GetServer();

	wxString cmd;
	if (server->GetConnection()->EdbMinimumVersion(8, 0))
		cmd = edbBackupAllExecutable;
	else if (server->GetConnection()->GetIsGreenplum())
		cmd = gpBackupAllExecutable;
	else
		cmd = pgBackupAllExecutable;

	if (!server->GetName().IsEmpty())
		cmd += wxT(" --host ") + server->GetName();

	cmd +=  wxT(" --port ") + NumToStr((long)server->GetPort())
	        +  wxT(" --username ") + commandLineCleanOption(qtIdent(server->GetUsername()));
	return cmd;
}


wxString frmBackupGlobals::getCmdPart2()
{
	wxString cmd;

	if (settings->GetIgnoreVersion())
		cmd.Append(wxT(" --ignore-version"));
	if (chkVerbose->GetValue())
		cmd.Append(wxT(" --verbose"));

	cmd.Append(wxT(" --file \"") + txtFilename->GetValue() + wxT("\""));

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
	if (!done)
	{
		if (processedFile == txtFilename->GetValue())
		{
			if (wxMessageBox(_("Are you sure you wish to run a backup to this file again?"), _("Repeat backup?"), wxICON_QUESTION | wxYES_NO) == wxNO)
				return;
		}
		else if (wxFile::Exists(txtFilename->GetValue()))
		{
			wxString msg;
			msg.Printf(_("The file: \n\n%s\n\nalready exists. Do you want to overwrite it?"), txtFilename->GetValue().c_str());
			if (wxMessageBox(msg, _("Overwrite file?"), wxICON_WARNING | wxYES_NO) == wxNO)
				return;
		}

		processedFile = txtFilename->GetValue();
	}

	settings->Write(wxT("frmBackupGlobals/LastFile"), txtFilename->GetValue());
	ExternProcessDialog::OnOK(ev);
}

backupGlobalsFactory::backupGlobalsFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("&Backup globals..."), _("Creates a backup of the global database objects"));
}


wxWindow *backupGlobalsFactory::StartDialog(frmMain *form, pgObject *obj)
{
	frmBackupGlobals *frm = new frmBackupGlobals(form, obj);
	frm->Go();
	return 0;
}


bool backupGlobalsFactory::CheckEnable(pgObject *obj)
{
	if (!obj)
		return false;

	if (obj->GetMetaType() == PGM_SERVER)
		if (!((pgServer *)obj)->GetConnected())
			return false;

	if (obj->GetConnection() && obj->GetConnection()->EdbMinimumVersion(8, 0))
		return obj->CanBackupGlobals() && !edbBackupExecutable.IsEmpty() && pgAppMinimumVersion(edbBackupExecutable, 8, 3);
	else if (obj->GetConnection() && obj->GetConnection()->GetIsGreenplum())
		return obj->CanBackupGlobals() && !gpBackupExecutable.IsEmpty() && pgAppMinimumVersion(gpBackupExecutable, 8, 3);
	else
		return obj->CanBackupGlobals() && !pgBackupExecutable.IsEmpty() && pgAppMinimumVersion(pgBackupExecutable, 8, 3);
}

