//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmBackup.cpp - Backup database dialogue
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/settings.h>


// App headers
#include "pgAdmin3.h"
#include "frm/frmMain.h"
#include "frm/frmBackup.h"
#include "utils/sysLogger.h"
#include "schema/pgSchema.h"
#include "schema/pgTable.h"
#include "ctl/ctlCheckTreeView.h"

// Icons
#include "images/backup.pngc"


#define nbNotebook              CTRL_NOTEBOOK("nbNotebook")
#define txtFilename             CTRL_TEXT("txtFilename")
#define btnFilename             CTRL_BUTTON("btnFilename")
#define txtCompressRatio        CTRL_TEXT("txtCompressRatio")
#define cbEncoding              CTRL_COMBOBOX("cbEncoding")
#define cbFormat                CTRL_COMBOBOX("cbFormat")
#define txtNumberOfJobs          CTRL_TEXT("txtNumberOfJobs")
#define cbRolename              CTRL_COMBOBOX("cbRolename")
#define chkBlobs                CTRL_CHECKBOX("chkBlobs")
#define chkOid                  CTRL_CHECKBOX("chkOid")
#define chkInsert               CTRL_CHECKBOX("chkInsert")
#define chkDisableDollar        CTRL_CHECKBOX("chkDisableDollar")
#define chkOnlyData             CTRL_CHECKBOX("chkOnlyData")
#define chkOnlySchema           CTRL_CHECKBOX("chkOnlySchema")
#define chkNoOwner              CTRL_CHECKBOX("chkNoOwner")
#define chkCreateDb             CTRL_CHECKBOX("chkCreateDb")
#define chkDropDb               CTRL_CHECKBOX("chkDropDb")
#define chkDisableTrigger       CTRL_CHECKBOX("chkDisableTrigger")
#define chkVerbose              CTRL_CHECKBOX("chkVerbose")
#define chkColumnInserts        CTRL_CHECKBOX("chkColumnInserts")
#define chkNoPrivileges         CTRL_CHECKBOX("chkNoPrivileges")
#define chkNoTablespaces        CTRL_CHECKBOX("chkNoTablespaces")
#define chkUseSetSession        CTRL_CHECKBOX("chkUseSetSession")
#define chkForceQuoteForIdent   CTRL_CHECKBOX("chkForceQuoteForIdent")
#define ctvObjects              CTRL_CHECKTREEVIEW("ctvObjects")
#define chkNoUnloggedTableData  CTRL_CHECKBOX("chkNoUnloggedTableData")
#define chkSectionPreData       CTRL_CHECKBOX("chkSectionPreData")
#define chkSectionData          CTRL_CHECKBOX("chkSectionData")
#define chkSectionPostData      CTRL_CHECKBOX("chkSectionPostData")


BEGIN_EVENT_TABLE(frmBackup, ExternProcessDialog)
	EVT_TEXT(XRCID("txtFilename"),                frmBackup::OnChange)
	EVT_BUTTON(XRCID("btnFilename"),              frmBackup::OnSelectFilename)
	EVT_BUTTON(wxID_OK,                           frmBackup::OnOK)
	EVT_COMBOBOX(XRCID("cbFormat"),               frmBackup::OnChangePlain)
	EVT_CHECKBOX(XRCID("chkOnlyData"),            frmBackup::OnChangePlain)
	EVT_CHECKBOX(XRCID("chkOnlySchema"),          frmBackup::OnChangePlain)
	EVT_CHECKBOX(XRCID("chkNoUnloggedTableData"), frmBackup::OnChangePlain)
	EVT_CHECKBOX(XRCID("chkSectionPreData"),      frmBackup::OnChangePlain)
	EVT_CHECKBOX(XRCID("chkSectionData"),         frmBackup::OnChangePlain)
	EVT_CHECKBOX(XRCID("chkSectionPostData"),     frmBackup::OnChangePlain)
	EVT_CLOSE(                                    ExternProcessDialog::OnClose)
END_EVENT_TABLE()



frmBackup::frmBackup(frmMain *form, pgObject *obj) : ExternProcessDialog(form)
{
	object = obj;

	SetFont(settings->GetSystemFont());
	LoadResource(form, wxT("frmBackup"));
	RestorePosition();

	SetTitle(object->GetTranslatedMessage(BACKUPTITLE));

	if (object->GetConnection()->EdbMinimumVersion(8, 0))
		backupExecutable = edbBackupExecutable;
	else if (object->GetConnection()->GetIsGreenplum())
		backupExecutable = gpBackupExecutable;
	else
		backupExecutable = pgBackupExecutable;

	canBlob = (obj->GetMetaType() == PGM_DATABASE);
	chkBlobs->SetValue(canBlob);
	chkDisableDollar->Enable(obj->GetConnection()->BackendMinimumVersion(7, 5));

	wxString val;
	settings->Read(wxT("frmBackup/LastFile"), &val, wxEmptyString);
	txtFilename->SetValue(val);

	pgServer *server = object->GetServer();

	if (!server->GetPasswordIsStored())
		environment.Add(wxT("PGPASSWORD=") + object->GetServer()->GetPassword());

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

	// fix translation problem
	wxString dollarLabel = wxGetTranslation(_("$$ quoting"));
	dollarLabel.Replace(wxT("$$"), wxT("$"));
	chkDisableDollar->SetLabel(dollarLabel);
	chkDisableDollar->SetSize(chkDisableDollar->GetBestSize());

	txtMessages = CTRL_TEXT("txtMessages");
	// Note that under GTK+, SetMaxLength() function may only be used with single line text controls.
	// (see http://docs.wxwidgets.org/2.8/wx_wxtextctrl.html#wxtextctrlsetmaxlength)
#ifndef __WXGTK__
	txtMessages->SetMaxLength(0L);
#endif
	btnOK->Disable();

	long encNo = 0;
	wxString encStr;
	cbEncoding->Append(wxT(""));
	do
	{
		encStr = object->GetConnection()->ExecuteScalar(
		             wxT("SELECT pg_encoding_to_char(") + NumToStr(encNo) + wxT(")"));
		if (pgConn::IsValidServerEncoding(encNo) && !encStr.IsEmpty())
			cbEncoding->Append(encStr);

		encNo++;
	}
	while (!encStr.IsEmpty());

	cbEncoding->SetSelection(0);

	wxString i18ndb = _("Database");
	wxTreeItemId db = ctvObjects->AddRoot(i18ndb + wxT(" ") + object->GetDatabase()->GetName(), 1);
	bool checked;

	wxString query = wxT("SELECT nspname, relname ")
	                 wxT("FROM pg_namespace n ")
	                 wxT("LEFT JOIN pg_class c ON n.oid=c.relnamespace AND relkind='r' ")
	                 wxT("WHERE nspname NOT LIKE E'pg\\\\_%' AND nspname <> 'information_schema' ");
	if (!object->GetDatabase()->GetSchemaRestriction().IsEmpty())
		query += wxT("AND nspname IN (") + object->GetDatabase()->GetSchemaRestriction() + wxT(")");
	query += wxT("ORDER BY nspname, relname");

	pgSet *objects = object->GetDatabase()->ExecuteSet(query);

	if (objects)
	{
		wxString currentSchema = wxT("");
		wxTreeItemId currentSchemaNode;
		while (!objects->Eof())
		{
			if (currentSchema != objects->GetVal(wxT("nspname")))
			{
				currentSchema = objects->GetVal(wxT("nspname"));
				if (object->GetMetaType() == PGM_SCHEMA)
				{
					checked = ((pgSchema *)object)->GetIdentifier() == currentSchema;
				}
				else
				{
					if (object->GetMetaType() == PGM_TABLE || object->GetMetaType() == GP_PARTITION)
					{
						checked = ((pgTable *)object)->GetSchema()->GetIdentifier() == currentSchema
						          || (object->GetMetaType() != PGM_TABLE && object->GetMetaType() != GP_PARTITION);
					}
					else
					{
						checked = object->GetMetaType() != PGM_TABLE && object->GetMetaType() != GP_PARTITION;
					}
				}
				currentSchemaNode = ctvObjects->AppendItem(db, currentSchema, checked ? 1 : 0);
			}
			if (!objects->GetVal(wxT("relname")).IsNull())
			{
				if (object->GetMetaType() == PGM_TABLE || object->GetMetaType() == GP_PARTITION)
				{
					checked = ((pgTable *)object)->GetSchema()->GetIdentifier() == currentSchema
					          && ((pgTable *)object)->GetIdentifier() == objects->GetVal(wxT("relname"));
				}
				else
				{
					if (object->GetMetaType() == PGM_SCHEMA)
					{
						checked = ((pgSchema *)object)->GetIdentifier() == currentSchema;
					}
					else
					{
						if (object->GetMetaType() == PGM_TABLE || object->GetMetaType() == GP_PARTITION)
						{
							checked = ((pgTable *)object)->GetSchema()->GetIdentifier() == currentSchema
							          || (object->GetMetaType() != PGM_TABLE && object->GetMetaType() != GP_PARTITION);
						}
						else
						{
							checked = object->GetMetaType() != PGM_TABLE && object->GetMetaType() != GP_PARTITION;
						}
					}
				}
				ctvObjects->AppendItem(currentSchemaNode, objects->GetVal(wxT("relname")), checked ? 1 : 0);
			}
			objects->MoveNext();
		}
		ctvObjects->ExpandAll();

		delete objects;
	}

	if (!pgAppMinimumVersion(backupExecutable, 9, 3))
	{
		txtNumberOfJobs->Disable();
	}
	if (!pgAppMinimumVersion(backupExecutable, 9, 2))
	{
		chkSectionPreData->Disable();
		chkSectionData->Disable();
		chkSectionPostData->Disable();
	}
	if (!pgAppMinimumVersion(backupExecutable, 9, 1))
	{
		chkForceQuoteForIdent->Disable();
		chkNoUnloggedTableData->Disable();
	}
	if (!pgAppMinimumVersion(backupExecutable, 8, 4))
	{
		chkNoTablespaces->Disable();
		cbRolename->Disable();
	}
	else
	{
		// Available rolenames
		if (object->GetServer()->GetConnection()->BackendMinimumVersion(8, 1))
		{
			pgSetIterator set(object->GetServer()->GetConnection(),
			                  wxT("SELECT DISTINCT rolname\n")
			                  wxT("FROM pg_roles db\n")
			                  wxT("ORDER BY rolname"));

			cbRolename->Append(wxEmptyString);

			while(set.RowsLeft())
				cbRolename->Append(set.GetVal(wxT("rolname")));

			cbRolename->SetValue(object->GetServer()->GetRolename());
			cbRolename->Enable(true);
		}
		else
			cbRolename->Disable();
	}
	if (!pgAppMinimumVersion(backupExecutable, 8, 1))
	{
		cbEncoding->Disable();
	}

	cbFormat->Append(_("Custom"));
	cbFormat->Append(_("Tar"));
	cbFormat->Append(_("Plain"));
	cbFormat->SetSelection(0);

	if (pgAppMinimumVersion(backupExecutable, 9, 1))
		cbFormat->Append(_("Directory"));

	wxCommandEvent ev;
	OnChangePlain(ev);
}


frmBackup::~frmBackup()
{
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
	wxString title, prompt, FilenameOnly;

	if (cbFormat->GetSelection() == 2) // plain
	{
		title  = _("Select output file");
#ifdef __WXMSW__
		prompt = _("Query files (*.sql)|*.sql|All files (*.*)|*.*");
#else
		prompt = _("Query files (*.sql)|*.sql|All files (*)|*");
#endif
	}
	else
	{
		title  = _("Select backup filename");
#ifdef __WXMSW__
		prompt = _("Backup files (*.backup)|*.backup|All files (*.*)|*.*");
#else
		prompt = _("Backup files (*.backup)|*.backup|All files (*)|*");
#endif
	}

	wxFileName::SplitPath(txtFilename->GetValue(), NULL, NULL, &FilenameOnly, NULL);
	wxFileDialog file(this, title, ::wxPathOnly(txtFilename->GetValue()), FilenameOnly, prompt, wxFD_SAVE);

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
	bool isDirectory = (cbFormat->GetSelection() == 3);
	bool isSection = chkSectionPreData->GetValue() || chkSectionData->GetValue() || chkSectionPostData->GetValue();

	chkOnlyData->Enable(!chkOnlySchema->GetValue() && !isSection);
	chkOnlySchema->Enable(!chkOnlyData->GetValue() && !isSection);

	chkSectionPreData->Enable(!chkOnlyData->GetValue() && !chkOnlySchema->GetValue());
	chkSectionData->Enable(!chkOnlyData->GetValue() && !chkOnlySchema->GetValue());
	chkSectionPostData->Enable(!chkOnlyData->GetValue() && !chkOnlySchema->GetValue());

	chkDisableTrigger->Enable(chkOnlyData->GetValue());

	btnFilename->Enable(!isDirectory);

	txtNumberOfJobs->Enable(isDirectory && pgAppMinimumVersion(backupExecutable, 9, 3));
	if (!txtNumberOfJobs->IsEnabled())
		txtNumberOfJobs->SetValue(wxEmptyString);

	wxCommandEvent nullEvent;
	OnChange(nullEvent);
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
	pgServer *server = object->GetDatabase()->GetServer();

	wxString cmd = backupExecutable;

	if (!server->GetName().IsEmpty())
		cmd += wxT(" --host ") + server->GetName();

	cmd +=  wxT(" --port ") + NumToStr((long)server->GetPort())
	        +  wxT(" --username ") + commandLineCleanOption(qtIdent(server->GetUsername()));

	if (!cbRolename->GetValue().IsEmpty())
		cmd += wxT(" --role ") + commandLineCleanOption(qtIdent(cbRolename->GetValue()));

	if (pgAppMinimumVersion(backupExecutable, 8, 4))
		cmd += wxT(" --no-password ");

	if (object->GetConnection()->GetIsGreenplum())
		cmd += wxT(" --gp-syntax ");
	return cmd;
}


wxString frmBackup::getCmdPart2()
{
	wxString cmd;

	switch (cbFormat->GetSelection())
	{
		case 0: // compressed
		{
			cmd.Append(wxT(" --format custom"));
			if (chkBlobs->GetValue())
				cmd.Append(wxT(" --blobs"));
			if (!txtCompressRatio->GetValue().IsEmpty())
				cmd.Append(wxT(" --compress ") + txtCompressRatio->GetValue());
			break;
		}
		case 1: // tar
		{
			cmd.Append(wxT(" --format tar"));
			if (chkBlobs->GetValue())
				cmd.Append(wxT(" --blobs"));
			break;
		}
		case 2: // plain
		{
			cmd.Append(wxT(" --format plain"));
			if (chkOnlyData->GetValue())
			{
				cmd.Append(wxT(" --data-only"));
				if (chkDisableTrigger->GetValue())
					cmd.Append(wxT(" --disable-triggers"));
			}
			else
			{
				if (chkOnlySchema->GetValue())
					cmd.Append(wxT(" --schema-only"));
				if (chkNoOwner->GetValue())
					cmd.Append(wxT(" --no-owner"));
				if (chkCreateDb->GetValue())
					cmd.Append(wxT(" --create"));
				if (chkDropDb->GetValue())
					cmd.Append(wxT(" --clean"));
			}
			break;
		}
		case 3: // directory
		{
			cmd.Append(wxT(" --format directory"));
			break;
		}
	}

	// Section
	if (pgAppMinimumVersion(backupExecutable, 9, 2))
	{
		if (chkSectionPreData->GetValue())
			cmd.Append(wxT(" --section pre-data"));
		if (chkSectionData->GetValue())
			cmd.Append(wxT(" --section data"));
		if (chkSectionPostData->GetValue())
			cmd.Append(wxT(" --section post-data"));
	}

	if (!cbEncoding->GetValue().IsEmpty())
		cmd.Append(wxT(" --encoding ") + cbEncoding->GetValue());
	if (chkOid->GetValue())
		cmd.Append(wxT(" --oids"));
	if (chkInsert->GetValue())
		cmd.Append(wxT(" --inserts"));
	if (chkColumnInserts->GetValue())
		cmd.Append(wxT(" --column-inserts"));
	if (chkNoPrivileges->GetValue())
		cmd.Append(wxT(" --no-privileges"));
	if (chkNoTablespaces->GetValue())
		cmd.Append(wxT(" --no-tablespaces"));
	if (chkUseSetSession->GetValue())
		cmd.Append(wxT(" --use-set-session-authorization"));
	if (chkDisableDollar->GetValue())
		cmd.Append(wxT(" --disable-dollar-quoting"));
	if (settings->GetIgnoreVersion())
		cmd.Append(wxT(" --ignore-version"));
	if (chkVerbose->GetValue())
		cmd.Append(wxT(" --verbose"));
	if (chkForceQuoteForIdent->GetValue())
		cmd.Append(wxT(" --quote-all-identifiers"));
	if (chkNoUnloggedTableData->GetValue())
		cmd.Append(wxT(" --no-unlogged-table-data"));

	cmd.Append(wxT(" --file \"") + txtFilename->GetValue() + wxT("\""));

	if (!txtNumberOfJobs->GetValue().IsEmpty())
		cmd.Append(wxT(" --jobs ") + txtNumberOfJobs->GetValue());

	// Process selected items
	wxTreeItemId root, schema, table;
	wxTreeItemIdValue schemaData, tableData;
	wxString cmdSchemas, cmdTables, tmpTables;
	bool partialDump;
	bool partialSchema;

	root = ctvObjects->GetRootItem();
	schema = ctvObjects->GetFirstChild(root, schemaData);
	cmdSchemas = wxT("");
	cmdTables = wxT("");
	partialDump = false;
	while (schema.IsOk())
	{
		if (ctvObjects->IsChecked(schema))
		{
			partialSchema = false;
			tmpTables = wxT("");
			table = ctvObjects->GetFirstChild(schema, tableData);
			while (table.IsOk())
			{
				if (ctvObjects->IsChecked(table))
				{
					// The syntax changed in 8.2 :-(
					if (pgAppMinimumVersion(backupExecutable, 8, 2))
					{
						tmpTables.Append(wxT(" --table \"") +
						                 commandLineCleanOption(qtIdent(ctvObjects->GetItemText(schema)), true) +
						                 wxT(".") +
						                 commandLineCleanOption(qtIdent(ctvObjects->GetItemText(table)), true) +
						                 wxT("\""));
					}
					else
					{
						tmpTables.Append(wxT(" --table \"") + commandLineCleanOption(qtIdent(ctvObjects->GetItemText(table)), true) + wxT("\""));
						tmpTables.Append(wxT(" --schema \"") + commandLineCleanOption(qtIdent(ctvObjects->GetItemText(schema)), true) + wxT("\""));
					}
				}
				else
				{
					partialDump = true;
					partialSchema = true;
				}
				table = ctvObjects->GetNextChild(schema, tableData);
			}

			if (partialSchema)
			{
				cmdTables += tmpTables;
			}
			else
			{
				cmdSchemas.Append(wxT(" --schema \"") + commandLineCleanOption(qtIdent(ctvObjects->GetItemText(schema)), true) + wxT("\""));
			}
		}
		else
		{
			partialDump = true;
		}

		schema = ctvObjects->GetNextChild(root, schemaData);
	}

	if (partialDump)
	{
		if (!cmdTables.IsEmpty())
			cmd.Append(cmdTables);
		if (!cmdSchemas.IsEmpty())
			cmd.Append(cmdSchemas);
	}

	cmd.Append(wxT(" ") + commandLineCleanOption(object->GetDatabase()->GetQuotedIdentifier()));

	return cmd;
}


void frmBackup::Go()
{
	txtFilename->SetFocus();
	Show(true);
}

void frmBackup::OnOK(wxCommandEvent &ev)
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

	settings->Write(wxT("frmBackup/LastFile"), txtFilename->GetValue());
	ExternProcessDialog::OnOK(ev);
}

backupFactory::backupFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("&Backup..."), _("Creates a backup of the current database to a local file"));
}


wxWindow *backupFactory::StartDialog(frmMain *form, pgObject *obj)
{
	frmBackup *frm = new frmBackup(form, obj);
	frm->Go();
	return 0;
}


bool backupFactory::CheckEnable(pgObject *obj)
{
	if (!obj)
		return false;

	if (obj->GetConnection() && obj->GetConnection()->EdbMinimumVersion(8, 0))
		return obj->CanBackup() && !edbBackupExecutable.IsEmpty();
	else if (obj->GetConnection() && obj->GetConnection()->GetIsGreenplum())
		return obj->CanBackup() && !gpBackupExecutable.IsEmpty();
	else
		return obj->CanBackup() && !pgBackupExecutable.IsEmpty();

}


