//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
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
#include "images/backup.pngc"


#define nbNotebook              CTRL_NOTEBOOK("nbNotebook")
#define txtFilename             CTRL_TEXT("txtFilename")
#define btnFilename             CTRL_BUTTON("btnFilename")
#define cbRolename              CTRL_COMBOBOX("cbRolename")
#define chkVerbose              CTRL_CHECKBOX("chkVerbose")
#define chkForceQuoteForIdent   CTRL_CHECKBOX("chkForceQuoteForIdent")


BEGIN_EVENT_TABLE(frmBackupGlobals, ExternProcessDialog)
	EVT_TEXT(XRCID("txtFilename"),          frmBackupGlobals::OnChange)
	EVT_BUTTON(XRCID("btnFilename"),        frmBackupGlobals::OnSelectFilename)
	EVT_BUTTON(wxID_OK,                     frmBackupGlobals::OnOK)
	EVT_CLOSE(                              ExternProcessDialog::OnClose)
END_EVENT_TABLE()



frmBackupGlobals::frmBackupGlobals(frmMain *form, pgObject *obj) : ExternProcessDialog(form)
{
	object = obj;

	SetFont(settings->GetSystemFont());
	LoadResource(form, wxT("frmBackupGlobals"));
	RestorePosition();

	SetTitle(object->GetTranslatedMessage(BACKUPGLOBALS));

	if (object->GetConnection()->EdbMinimumVersion(8, 0))
		backupExecutable = edbBackupAllExecutable;
	else if (object->GetConnection()->GetIsGreenplum())
		backupExecutable = gpBackupAllExecutable;
	else
		backupExecutable = pgBackupAllExecutable;

	wxString val;
	settings->Read(wxT("frmBackupGlobals/LastFile"), &val, wxEmptyString);
	txtFilename->SetValue(val);

	pgServer *server;
	if (object->GetMetaType() == PGM_SERVER)
		server = (pgServer *)object;
	else
		server = object->GetDatabase()->GetServer();

	bool roles_supported = pgAppMinimumVersion(backupExecutable, 8, 4) && server->GetConnection()->BackendMinimumVersion(8, 1);
	cbRolename->Enable(roles_supported);

	if (roles_supported)
	{
		// Collect the available rolenames
		pgSetIterator set(server->GetConnection(),
		                  wxT("SELECT DISTINCT rolname\n")
		                  wxT("FROM pg_roles db\n")
		                  wxT("ORDER BY rolname"));

		cbRolename->Append(wxEmptyString);

		while(set.RowsLeft())
			cbRolename->Append(set.GetVal(wxT("rolname")));

		cbRolename->SetValue(server->GetRolename());
	}

	if (!server->GetPasswordIsStored())
		environment.Add(wxT("PGPASSWORD=") + server->GetPassword());

	// Pass the SSL mode via the environment
	environment.Add(wxT("PGSSLMODE=") + server->GetConnection()->GetSslModeName());

	if (server->GetSSLRootCert() != wxEmptyString)
		environment.Add(wxT("PGSSLROOTCERT=") + server->GetSSLRootCert());

	if (server->GetSSLCert() != wxEmptyString)
		environment.Add(wxT("PGSSLCERT=") + server->GetSSLCert());

	if (server->GetSSLKey() != wxEmptyString)
		environment.Add(wxT("PGSSLKEY=") + server->GetSSLKey());

	if (server->GetSSLCrl() != wxEmptyString)
		environment.Add(wxT("PGSSLCRL=") + server->GetSSLCrl());

	// Icon
	SetIcon(*backup_png_ico);

	txtMessages = CTRL_TEXT("txtMessages");
	// Note that under GTK+, SetMaxLength() function may only be used with single line text controls.
	// (see http://docs.wxwidgets.org/2.8/wx_wxtextctrl.html#wxtextctrlsetmaxlength)
#ifndef __WXGTK__
	txtMessages->SetMaxLength(0L);
#endif
	btnOK->Disable();

	if (!pgAppMinimumVersion(backupExecutable, 9, 1))
	{
		chkForceQuoteForIdent->Disable();
	}

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

	wxString cmd = backupExecutable;

	if (!server->GetName().IsEmpty())
		cmd += wxT(" --host ") + server->GetName();

	cmd +=  wxT(" --port ") + NumToStr((long)server->GetPort())
	        +  wxT(" --username ") + commandLineCleanOption(qtIdent(server->GetUsername()))
	        +  wxT(" --database ") + commandLineCleanOption(qtIdent(server->GetDatabaseName()));

	if (!cbRolename->GetValue().IsEmpty())
		cmd += wxT(" --role ") + commandLineCleanOption(qtIdent(cbRolename->GetValue()));

	if (pgAppMinimumVersion(backupExecutable, 8, 4))
		cmd += wxT(" --no-password ");

	return cmd;
}


wxString frmBackupGlobals::getCmdPart2()
{
	wxString cmd;

	if (settings->GetIgnoreVersion())
		cmd.Append(wxT(" --ignore-version"));
	if (chkVerbose->GetValue())
		cmd.Append(wxT(" --verbose"));
	if (chkForceQuoteForIdent->GetValue())
		cmd.Append(wxT(" --quote-all-identifiers"));

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
			if (wxMessageBox(_("Are you sure you wish to run a backup to this file again?"), _("Repeat backup?"), wxICON_QUESTION | wxYES_NO) != wxYES)
				return;
		}
		else if (wxFile::Exists(txtFilename->GetValue()))
		{
			wxString msg;
			msg.Printf(_("The file: \n\n%s\n\nalready exists. Do you want to overwrite it?"), txtFilename->GetValue().c_str());
			if (wxMessageBox(msg, _("Overwrite file?"), wxICON_WARNING | wxYES_NO) != wxYES)
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
		return obj->CanBackupGlobals() && !edbBackupAllExecutable.IsEmpty() && pgAppMinimumVersion(edbBackupAllExecutable, 8, 3);
	else if (obj->GetConnection() && obj->GetConnection()->GetIsGreenplum())
		return obj->CanBackupGlobals() && !gpBackupAllExecutable.IsEmpty() && pgAppMinimumVersion(gpBackupAllExecutable, 8, 3);
	else
		return obj->CanBackupGlobals() && !pgBackupAllExecutable.IsEmpty() && pgAppMinimumVersion(pgBackupAllExecutable, 8, 3);
}

