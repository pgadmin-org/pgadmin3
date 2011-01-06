//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmRestore.cpp - Restore database dialogue
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/settings.h>
#include <wx/process.h>
#include <wx/textbuf.h>
#include <wx/file.h>
#include <wx/filename.h>


// App headers
#include "pgAdmin3.h"
#include "frm/frmRestore.h"
#include "frm/frmMain.h"
#include "utils/sysLogger.h"
#include "schema/pgTable.h"
#include "schema/pgFunction.h"
#include "schema/pgLanguage.h"
#include "schema/pgConstraints.h"
#include "schema/pgForeignKey.h"
#include "ctl/ctlCheckTreeView.h"

// Icons
#include "images/restore.xpm"


#define nbNotebook               CTRL_NOTEBOOK("nbNotebook")
#define txtFilename              CTRL_TEXT("txtFilename")
#define btnFilename              CTRL_BUTTON("btnFilename")
#define cbRolename               CTRL_COMBOBOX("cbRolename")
#define chkOnlyData              CTRL_CHECKBOX("chkOnlyData")
#define chkOnlySchema            CTRL_CHECKBOX("chkOnlySchema")
#define chkNoOwner               CTRL_CHECKBOX("chkNoOwner")
#define chkNoPrivileges          CTRL_CHECKBOX("chkNoPrivileges")
#define chkNoTablespaces         CTRL_CHECKBOX("chkNoTablespaces")
#define chkCreateDb		         CTRL_CHECKBOX("chkCreateDb")
#define chkClean		         CTRL_CHECKBOX("chkClean")
#define chkSingleXact		     CTRL_CHECKBOX("chkSingleXact")
#define chkDisableTrigger        CTRL_CHECKBOX("chkDisableTrigger")
#define chkNoDataForFailedTables CTRL_CHECKBOX("chkNoDataForFailedTables")
#define chkUseSetSession         CTRL_CHECKBOX("chkUseSetSession")
#define chkExitOnError           CTRL_CHECKBOX("chkExitOnError")
#define txtNumberOfJobs          CTRL_TEXT("txtNumberOfJobs")
#define chkVerbose               CTRL_CHECKBOX("chkVerbose")
#define stSingleObject           CTRL_STATIC("stSingleObject")

#define ctvObjects              CTRL_CHECKTREEVIEW("ctvObjects")
#define btnView                 CTRL_BUTTON("btnView")


BEGIN_EVENT_TABLE(frmRestore, ExternProcessDialog)
	EVT_TEXT(XRCID("txtFilename"),               frmRestore::OnChangeName)
	EVT_CHECKBOX(XRCID("chkOnlyData"),           frmRestore::OnChangeData)
	EVT_CHECKBOX(XRCID("chkOnlySchema"),         frmRestore::OnChangeSchema)
	EVT_BUTTON(XRCID("btnFilename"),             frmRestore::OnSelectFilename)
	EVT_BUTTON(wxID_OK,                          frmRestore::OnOK)
	EVT_BUTTON(XRCID("btnView"),                 frmRestore::OnView)
	EVT_END_PROCESS(-1,                          frmRestore::OnEndProcess)
	EVT_CLOSE(                                   ExternProcessDialog::OnClose)
END_EVENT_TABLE()



frmRestore::frmRestore(frmMain *_form, pgObject *obj) : ExternProcessDialog(form)
{
	object = obj;

	if (object->GetMetaType() == PGM_SERVER)
		server = (pgServer *)object;
	else
		server = object->GetDatabase()->GetServer();

	form = _form;

	wxWindowBase::SetFont(settings->GetSystemFont());
	LoadResource(_form, wxT("frmRestore"));
	RestorePosition();

	SetTitle(object->GetTranslatedMessage(RESTORETITLE));

	if (object->GetConnection()->EdbMinimumVersion(8, 0))
		restoreExecutable = edbRestoreExecutable;
	else if (object->GetConnection()->GetIsGreenplum())
		restoreExecutable = gpRestoreExecutable;
	else
		restoreExecutable = pgRestoreExecutable;

	if (object->GetMetaType() != PGM_DATABASE)
	{
		chkOnlySchema->SetValue(object->GetMetaType() == PGM_FUNCTION
		                        || object->GetMetaType() == PGM_INDEX
		                        || object->GetMetaType() == PGM_TRIGGER);
		chkOnlyData->SetValue(object->GetMetaType() == PGM_TABLE
		                      || object->GetMetaType() == GP_PARTITION);
		if (object->GetMetaType() != PGM_SCHEMA)
		{
			chkOnlyData->Disable();
			chkOnlySchema->Disable();
		}
		if (object->GetMetaType() == PGM_FUNCTION)
		{
			chkClean->SetValue(true);
			chkClean->Disable();
		}
		btnView->Disable();
	}

	wxString val;
	settings->Read(wxT("frmRestore/LastFile"), &val, wxEmptyString);
	txtFilename->SetValue(val);

	// Icon
	SetIcon(wxIcon(restore_xpm));

	txtMessages = CTRL_TEXT("txtMessages");
	txtMessages->SetMaxLength(0L);
	btnOK->Disable();
	filenameValid = false;

	if (!server->GetPasswordIsStored())
		environment.Add(wxT("PGPASSWORD=") + server->GetPassword());

	// Pass the SSL mode via the environment
	environment.Add(wxT("PGSSLMODE=") + server->GetConnection()->GetSslModeName());

	if (!pgAppMinimumVersion(restoreExecutable, 8, 4))
	{
		chkNoTablespaces->Disable();
		chkSingleXact->Disable();
		txtNumberOfJobs->Disable();
		cbRolename->Disable();
	}
	else
	{
		// Available rolenames
		if (server->GetConnection()->BackendMinimumVersion(8, 1))
		{
			pgSetIterator set(server->GetConnection(),
			                  wxT("SELECT DISTINCT rolname\n")
			                  wxT("FROM pg_roles db\n")
			                  wxT("ORDER BY rolname"));

			cbRolename->Append(wxEmptyString);

			while(set.RowsLeft())
				cbRolename->Append(set.GetVal(wxT("rolname")));

			cbRolename->SetValue(server->GetRolename());
			cbRolename->Enable(true);
		}
		else
			cbRolename->Disable();
	}
	if (!pgAppMinimumVersion(restoreExecutable, 8, 2))
	{
		chkNoDataForFailedTables->Disable();
	}
	if (!pgAppMinimumVersion(restoreExecutable, 8, 0))
	{
		chkExitOnError->Disable();
	}

	wxCommandEvent ev;
	OnChangeName(ev);
}


frmRestore::~frmRestore()
{
	SavePosition();
}


wxString frmRestore::GetHelpPage() const
{
	wxString page;
	page = wxT("pg/app-pgrestore");
	return page;
}


void frmRestore::OnSelectFilename(wxCommandEvent &ev)
{

	wxString FilenameOnly;
	wxFileName::SplitPath(txtFilename->GetValue(), NULL, NULL, &FilenameOnly, NULL);

#ifdef __WXMSW__
	wxFileDialog file(this, _("Select backup filename"), ::wxPathOnly(txtFilename->GetValue()), FilenameOnly,
	                  _("Backup files (*.backup)|*.backup|All files (*.*)|*.*"));
#else
	wxFileDialog file(this, _("Select backup filename"), ::wxPathOnly(txtFilename->GetValue()), FilenameOnly,
	                  _("Backup files (*.backup)|*.backup|All files (*)|*"));
#endif

	if (file.ShowModal() == wxID_OK)
	{
		txtFilename->SetValue(file.GetPath());
		OnChange(ev);
	}
}


void frmRestore::OnChangeData(wxCommandEvent &ev)
{
	if (chkOnlyData->GetValue())
	{
		chkOnlySchema->SetValue(false);
		chkOnlySchema->Disable();
	}
	else
		chkOnlySchema->Enable();

	OnChange(ev);
}


void frmRestore::OnChangeSchema(wxCommandEvent &ev)
{
	if (chkOnlySchema->GetValue())
	{
		chkOnlyData->SetValue(false);
		chkOnlyData->Disable();
	}
	else
		chkOnlyData->Enable();

	OnChange(ev);
}


void frmRestore::OnChangeName(wxCommandEvent &ev)
{
	wxString name = txtFilename->GetValue();
	if (name.IsEmpty() || !wxFile::Exists(name))
		filenameValid = false;
	else
	{
		wxFile file(name, wxFile::read);
		if (file.IsOpened())
		{
			char buffer[8];
			off_t size = file.Read(buffer, 8);
			if (size == 8)
			{
				if (memcmp(buffer, "PGDMP", 5) && !memcmp(buffer, "toc.dat", 8))
				{
					// tar format?
					file.Seek(512);
					size = file.Read(buffer, 8);
				}
				if (size == 8 && !memcmp(buffer, "PGDMP", 5))
				{
					// check version here?
					filenameValid = true;
				}
			}
		}
	}
	OnChange(ev);
}


void frmRestore::OnChange(wxCommandEvent &ev)
{
	btnOK->Enable(filenameValid);
	btnView->Enable(filenameValid && object->GetMetaType() == PGM_DATABASE);
}


wxString frmRestore::GetCmd(int step)
{
	wxString cmd = getCmdPart1();

	return cmd + getCmdPart2(step);
}


wxString frmRestore::GetDisplayCmd(int step)
{
	wxString cmd = getCmdPart1();

	return cmd + getCmdPart2(step);
}


wxString frmRestore::getCmdPart1()
{
	wxString cmd;

	cmd = restoreExecutable;

	if (!server->GetName().IsEmpty())
		cmd += wxT(" --host ") + server->GetName();

	cmd += wxT(" --port ") + NumToStr((long)server->GetPort())
	       + wxT(" --username ") + commandLineCleanOption(qtIdent(server->GetUsername()))
	       + wxT(" --dbname ") + commandLineCleanOption(object->GetDatabase()->GetQuotedIdentifier());

	if (!cbRolename->GetValue().IsEmpty())
		cmd += wxT(" --role ") + commandLineCleanOption(qtIdent(cbRolename->GetValue()));

	if (pgAppMinimumVersion(restoreExecutable, 8, 4))
		cmd += wxT(" --no-password ");

	return cmd;
}


wxString frmRestore::getCmdPart2(int step)
{
	wxString cmd;

	wxString restoreExecutable;
	if (object->GetConnection()->EdbMinimumVersion(8, 0))
		restoreExecutable = edbBackupExecutable;
	else if (object->GetConnection()->GetIsGreenplum())
		restoreExecutable = gpBackupExecutable;
	else
		restoreExecutable = pgBackupExecutable;

	if (step)
	{
		cmd.Append(wxT(" --list"));
	}
	else
	{
		if (chkOnlyData->GetValue())
		{
			cmd.Append(wxT(" --data-only"));
		}
		else
		{
			if (chkNoOwner->GetValue())
				cmd.Append(wxT(" --no-owner"));
			if (chkNoPrivileges->GetValue())
				cmd.Append(wxT(" --no-privileges"));
			if (chkNoTablespaces->GetValue())
				cmd.Append(wxT(" --no-tablespaces"));
		}

		if (chkOnlySchema->GetValue())
		{
			cmd.Append(wxT(" --schema-only"));
		}
		else
		{
			if (chkDisableTrigger->GetValue())
				cmd.Append(wxT(" --disable-triggers"));
		}
		if (chkCreateDb->GetValue())
			cmd.Append(wxT(" --create"));
		if (chkClean->GetValue())
			cmd.Append(wxT(" --clean"));
		if (chkSingleXact->GetValue())
			cmd.Append(wxT(" --single-transaction"));
		if (chkNoDataForFailedTables->GetValue())
			cmd.Append(wxT(" --no-data-for-failed-tables"));
		if (chkUseSetSession->GetValue())
			cmd.Append(wxT(" --use-set-session-authorization"));
		if (chkExitOnError->GetValue())
			cmd.Append(wxT(" --exit-on-error"));

		if (!txtNumberOfJobs->GetValue().IsEmpty())
			cmd.Append(wxT(" --jobs ") + txtNumberOfJobs->GetValue());

		// Process selected items
		wxTreeItemId root, firstLevelObject, secondLevelObject;
		wxTreeItemIdValue firstLevelObjectData, secondLevelObjectData;
		bool partialDump = false;

		// Get root object
		root = ctvObjects->GetRootItem();

		if (root && object->GetMetaType() == PGM_DATABASE)
		{
			// Prepare the array
			wxArrayString restoreStrings;
			restoreStrings.Add(wxEmptyString, numberOfTOCItems);
			restoreTreeItemData *data;

			// Loop through first level objects
			firstLevelObject = ctvObjects->GetFirstChild(root, firstLevelObjectData);
			while (firstLevelObject.IsOk())
			{
				if (ctvObjects->IsChecked(firstLevelObject))
				{
					// Write the file
					data = (restoreTreeItemData *)ctvObjects->GetItemData(firstLevelObject);
					restoreStrings[data->GetId()] = data->GetDesc();

					// Loop through second level objects
					secondLevelObject = ctvObjects->GetFirstChild(firstLevelObject, secondLevelObjectData);
					while (secondLevelObject.IsOk())
					{
						if (ctvObjects->IsChecked(secondLevelObject))
						{
							// Write the file
							data = (restoreTreeItemData *)ctvObjects->GetItemData(secondLevelObject);
							restoreStrings[data->GetId()] = data->GetDesc();
						}
						else
							partialDump = true;
						secondLevelObject = ctvObjects->GetNextChild(firstLevelObject, secondLevelObjectData);
					}
				}
				else
					partialDump = true;
				firstLevelObject = ctvObjects->GetNextChild(root, firstLevelObjectData);
			}

			// Open a temporary file to store the TOC
			restoreTOCFilename = wxFileName::CreateTempFileName(wxT("restore"));
			wxFile tocFile;
			tocFile.Open(restoreTOCFilename.c_str(), wxFile::write);

			// Write all selected items in it
			for (int i = 0; i < numberOfTOCItems; i++)
			{
				if (restoreStrings[i] != wxEmptyString)
				{
					if (!tocFile.Write(restoreStrings[i] + wxT("\n")))
						wxLogError(_("Error writing to the temporary file ") + restoreTOCFilename);
				}
			}

			// If some items were not checked and if the file still contains something, we have to use the list
			if (partialDump && tocFile.Length() > 0)
				cmd.Append(wxT(" --use-list \"") + restoreTOCFilename + wxT("\""));
			tocFile.Close();
		}
		else if (object->GetMetaType() != PGM_DATABASE)
		{
			switch (object->GetMetaType())
			{
				case PGM_FUNCTION:
					cmd.Append(wxT(" --function ") + commandLineCleanOption(qtIdent(object->GetFullName()), true));
					break;
				case PGM_INDEX:
					cmd.Append(wxT(" --index ") + commandLineCleanOption(object->GetQuotedIdentifier(), true));
					break;
				case PGM_TABLE:
					cmd.Append(wxT(" --table ") + commandLineCleanOption(object->GetQuotedIdentifier(), true));
					break;
				case PGM_TRIGGER:
					cmd.Append(wxT(" --trigger ") + commandLineCleanOption(object->GetQuotedIdentifier(), true));
					break;
				default:
					break;
			}
			if (object->GetMetaType() == PGM_SCHEMA)
				cmd.Append(wxT(" --schema ") + commandLineCleanOption(object->GetQuotedIdentifier(), true));
			else if (pgAppMinimumVersion(restoreExecutable, 8, 2))
				cmd.Append(wxT(" --schema ") + commandLineCleanOption(object->GetSchema()->GetQuotedIdentifier(), true));
		}

		if (settings->GetIgnoreVersion())
			cmd.Append(wxT(" --ignore-version"));
		if (chkVerbose->GetValue())
			cmd.Append(wxT(" --verbose"));
	}


	cmd.Append(wxT(" \"") + txtFilename->GetValue() + wxT("\""));

	return cmd;
}


void frmRestore::OnView(wxCommandEvent &ev)
{
	btnView->Disable();
	btnOK->Disable();
	viewRunning = true;
	Execute(1, false);
	btnOK->SetLabel(_("OK"));
	done = 0;
}


void frmRestore::OnOK(wxCommandEvent &ev)
{
	if (!done)
	{
		if (processedFile == txtFilename->GetValue())
		{
			if (wxMessageBox(_("Are you sure you wish to run a restore from this file again?"), _("Repeat restore?"), wxICON_QUESTION | wxYES_NO) == wxNO)
				return;
		}

		processedFile = txtFilename->GetValue();
	}

	settings->Write(wxT("frmRestore/LastFile"), txtFilename->GetValue());
	viewRunning = false;
	btnView->Disable();

	ExternProcessDialog::OnOK(ev);

}


void frmRestore::OnEndProcess(wxProcessEvent &ev)
{
	ExternProcessDialog::OnEndProcess(ev);

	if (done && viewRunning && !ev.GetExitCode())
	{
		done = false;

		wxString str = wxTextBuffer::Translate(txtMessages->GetValue(), wxTextFileType_Unix);

		wxStringTokenizer line(str, wxT("\n"));
		line.GetNextToken();

		wxBeginBusyCursor();

		wxString i18nbackup = _("Backup");
		wxTreeItemId root = ctvObjects->AddRoot(i18nbackup + wxT(" ") + txtFilename->GetValue());
		wxString currentSchema = wxT("");
		wxTreeItemId currentSchemaNode;
		wxTreeItemId schemaNode, lastItem;
		wxTreeItemIdValue schemaNodeData;
		numberOfTOCItems = 0;

		while (line.HasMoreTokens())
		{
			// Read the next line
			str = line.GetNextToken();

			// Skip the few lines of comments
			if (str.Left(1) == wxT(";") || str.Left(1) == wxT("P"))
				continue;

			// Split the line according to spaces
			wxStringTokenizer col(str, wxT(" "));

			// Column 1 (dumpId)
			col.GetNextToken();

			// Column 2 (tableOid)
			col.GetNextToken();

			// Column 3 (oid)
			col.GetNextToken();

			// Column 4 (desc)
			// First interesting information: object's type
			wxString type = col.GetNextToken();

			if (type == wxT("PROCEDURAL"))
			{
				// type for a PL is PROCEDURAL LANGUAGE
				// we'll keep the next column for the object's type
				type = col.GetNextToken();
			}
			else if (type == wxT("SHELL"))
			{
				// type for a SHELL is SHELL TYPE
				// we'll keep both columns for the object's type
				type += col.GetNextToken();
			}
			else if (type == wxT("OPERATOR"))
			{
				// type for an operator class is OPERATOR CLASS
				// we'll keep the two columns for the object's type
				wxString tmp = str.Mid(str.Find(type) + type.Length() + 1, 5);
				if (tmp == wxT("CLASS"))
					type += wxT(" ") + col.GetNextToken();
			}
			else if (type == wxT("SEQUENCE"))
			{
				// type for a sequence can be SEQUENCE, SEQUENCE OWNED BY or SEQUENCE SET
				// we'll keep all these columns for the object's type
				wxString tmp = str.Mid(str.Find(type) + type.Length() + 1, 3);
				if (tmp == wxT("OWN") || tmp == wxT("SET"))
				{
					type += wxT(" ") + col.GetNextToken();
					if (type == wxT("SEQUENCE OWNED"))
						type += wxT(" ") + col.GetNextToken();
				}
			}
			else if (type == wxT("FK"))
			{
				// type for a FK is FK CONSTRAINT
				// we'll keep the next column for the object's type
				type = col.GetNextToken();
			}
			else if (type == wxT("TABLE"))
			{
				if (col.CountTokens() == 4)
				{
					// TABLE DATA detected
					type += wxT(" ") + col.GetNextToken();
				}
			}

			// Column 5 (namespace)
			// Second interesting information: object's schema
			wxString schema = col.GetNextToken();

			// Column 6 (tag)
			// Third interesting information: object's qualified name
			//wxString name=col.GetNextToken();
			wxString name = str.Mid(str.Find(schema) + schema.Length() + 1).BeforeLast(' ');

			// Column 7 (owner)
			// Fourth interesting information: object's owner
			wxString owner = str.Mid(str.Find(name) + name.Length() + 1);

			// New method
			if (type == wxT("LANGUAGE"))
			{
				lastItem = ctvObjects->AppendItem(root, wxT("Language ") + name + wxT(" [") + _("owner") + wxT(": ") + owner + wxT("]"), 1);
			}
			else if (type == wxT("ACL") && schema == wxT("-"))
			{
				lastItem = ctvObjects->AppendItem(root, type + wxT(" ") + name, 1);
			}
			else if (type == wxT("CAST"))
			{
				lastItem = ctvObjects->AppendItem(root, name, 1);
			}
			else if (type == wxT("SCHEMA"))
			{
				currentSchema = name;
				lastItem = currentSchemaNode = ctvObjects->AppendItem(root, wxT("Schema ") + name, 1);
			}
			else
			{
				if (schema != currentSchema)
				{
					// Loop through the nodes to find the schema
					schemaNode = ctvObjects->GetFirstChild(root, schemaNodeData);
					bool found = false;
					while (schemaNode.IsOk() && !found)
					{
						if (ctvObjects->GetItemText(schemaNode) == wxT("Schema ") + schema)
							found = true;
						else
							schemaNode = ctvObjects->GetNextChild(root, schemaNodeData);
					}

					// Found it?
					if (schemaNode.IsOk())
					{
						currentSchema = schema;
						currentSchemaNode = schemaNode;
					}
					// if we are treating a comment, we use the schema of its
					// object (ie, the previous line)
					else if (type != wxT("COMMENT"))
						wxLogError(_("Schema node not found for object ") + type + wxT(" ") + name + wxT(" [") + _("owner") + wxT(": ") + owner + wxT("]"));
				}
				lastItem = ctvObjects->AppendItem(currentSchemaNode, type + wxT(" ") + name + wxT(" [") + _("owner") + wxT(": ") + owner + wxT("]"), 1);
			}
			ctvObjects->SetItemData(lastItem, new restoreTreeItemData(numberOfTOCItems, str));
			numberOfTOCItems++;
		}

		wxEndBusyCursor();
		nbNotebook->SetSelection(3);
	}
	else
	{
		wxRemoveFile(restoreTOCFilename);
	}
}


void frmRestore::Go()
{
	txtFilename->SetFocus();
	Show(true);
}



restoreFactory::restoreFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("&Restore..."), _("Restores a backup from a local file"));
}


wxWindow *restoreFactory::StartDialog(frmMain *form, pgObject *obj)
{
	frmRestore *frm = new frmRestore(form, obj);
	frm->Go();
	return 0;
}


bool restoreFactory::CheckEnable(pgObject *obj)
{
	if (!obj)
		return false;

	if (obj->GetConnection() && obj->GetConnection()->EdbMinimumVersion(8, 0))
		return obj->CanCreate() && obj->CanRestore() && !edbRestoreExecutable.IsEmpty();
	else if (obj->GetConnection() && obj->GetConnection()->GetIsGreenplum())
		return obj->CanCreate() && obj->CanRestore() && !gpRestoreExecutable.IsEmpty();
	else
		return obj->CanCreate() && obj->CanRestore() && !pgRestoreExecutable.IsEmpty();
}


restoreTreeItemData::restoreTreeItemData(int id, const wxString &desc)
{
	restoreId = id;
	restoreDesc = desc;
}
