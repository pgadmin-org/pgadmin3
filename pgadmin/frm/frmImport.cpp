//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmImport.cpp - Import database dialogue
//
//////////////////////////////////////////////////////////////////////////


// wxWindows headers
#include <wx/wx.h>
#include <wx/settings.h>
#include <wx/filepicker.h>


// App headers
#include "pgAdmin3.h"
#include "db/pgConn.h"
#include "frm/frmMain.h"
#include "frm/frmImport.h"
#include "utils/sysLogger.h"
#include "schema/pgSchema.h"
#include "schema/pgTable.h"
#include "schema/pgColumn.h"

// Icons
#include "images/reload.pngc"


#define nbNotebook                    CTRL_NOTEBOOK("nbNotebook")
#define pickerImportfile              CTRL_FILEPICKER("pickerImportfile")
#define cbFormat                      CTRL_COMBOBOX("cbFormat")
#define cbEncoding                    CTRL_COMBOBOX("cbEncoding")
#define chkOids                       CTRL_CHECKBOX("chkOids")
#define chkHeader                     CTRL_CHECKBOX("chkHeader")
#define cbDelimiter                   CTRL_COMBOBOX("cbDelimiter")
#define cbQuote                       CTRL_COMBOBOX("cbQuote")
#define cbEscape                      CTRL_COMBOBOX("cbEscape")
#define txtNull                       CTRL_TEXT("txtNull")
#define gauge                         CTRL_GAUGE("gauge")
#define lstColumnsToImport			  CTRL_CHECKLISTBOX("lstColumnsToImport")
#define lstIgnoreForColumns			  CTRL_CHECKLISTBOX("lstIgnoreForColumns")


BEGIN_EVENT_TABLE(frmImport, pgDialog)
	EVT_COMBOBOX(XRCID("cbFormat"),   frmImport::OnChangeFormat)
	EVT_BUTTON(wxID_OK,               frmImport::OnOK)
	EVT_BUTTON (wxID_HELP,            frmImport::OnHelp)
END_EVENT_TABLE()


frmImport::frmImport(frmMain *form, pgObject *_object, pgConn *_conn)
{
	// Initialize variables
	connection = _conn;
	object = _object;
	done = false;

	// Set-up window
	SetFont(settings->GetSystemFont());
	LoadResource(form, wxT("frmImport"));
	RestorePosition();
	SetTitle(wxT("Import data from file into ") + object->GetName());
	SetIcon(*reload_png_ico);

	// Fill the encoding combobox
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

	// Fill the columns checklistboxes
	wxCookieType cookie;
	pgObject *data = 0;
	wxTreeItemId columnsItem;
	wxTreeItemId item = form->GetBrowser()->GetFirstChild(object->GetId(), cookie);
	while (item)
	{
		data = form->GetBrowser()->GetObject(item);
		pgaFactory *factory = data->GetFactory();
		if (factory == columnFactory.GetCollectionFactory())
			columnsItem = item;
		if (data->GetMetaType() == PGM_COLUMN && data->IsCollection())
			columnsItem = item;
		if (columnsItem)
			break;

		item = form->GetBrowser()->GetNextChild(object->GetId(), cookie);
	}

	if (columnsItem)
	{
		pgCollection *coll = (pgCollection *)data;
		coll->ShowTreeDetail(form->GetBrowser());
		item = form->GetBrowser()->GetFirstChild(columnsItem, cookie);

		while (item)
		{
			data = form->GetBrowser()->GetObject(item);
			if (data->IsCreatedBy(columnFactory))
			{
				pgColumn *column = (pgColumn *)data;
				column->ShowTreeDetail(form->GetBrowser());
				if (column->GetColNumber() > 0)
				{
					lstColumnsToImport->Append(column->GetName());
					lstIgnoreForColumns->Append(column->GetName());
				}
			}
			item = form->GetBrowser()->GetNextChild(columnsItem, cookie);
		}

		for (unsigned int x = 0; x < lstColumnsToImport->GetCount(); x++)
			lstColumnsToImport->Check(x, true);
	}

	// Fix some widgets
	wxCommandEvent ev;
	OnChangeFormat(ev);
}


frmImport::~frmImport()
{
	SavePosition();
}


void frmImport::OnHelp(wxCommandEvent &ev)
{
	DisplayHelp(wxT("sql-copy"), HELP_POSTGRESQL);
}

void frmImport::OnChangeFormat(wxCommandEvent &ev)
{
	bool enabled = cbFormat->GetValue() == wxT("csv");
	chkHeader->Enable(enabled);
	cbQuote->Enable(enabled);
	cbEscape->Enable(enabled);
	lstIgnoreForColumns->Enable(enabled);
}


void frmImport::OnOK(wxCommandEvent &ev)
{
	wxString query = wxEmptyString;
	wxString columnsToImport = wxEmptyString;
	wxString columnsToIgnoreForNulls = wxEmptyString;
	bool allColumnsToImport = true;
	bool someColumnsToIgnoreForNulls = false;
	wxFileName fn;
	wxFile *file;
	bool copydone = false;
	bool goterror = false;
	int nCount = 8192;

	if (!done)
	{
		// Check checklistboxes
		for (unsigned int x = 0; x < lstColumnsToImport->GetCount(); x++)
		{
			if (lstColumnsToImport->IsChecked(x))
			{
				if (!columnsToImport.IsEmpty())
				{
					columnsToImport.Append(wxT(","));
				}
				columnsToImport.Append(qtIdent(lstColumnsToImport->GetString(x)));
			}
			else
			{
				allColumnsToImport = false;
			}
		}
		for (unsigned int x = 0; x < lstIgnoreForColumns->GetCount(); x++)
		{
			if (lstIgnoreForColumns->IsChecked(x))
			{
				if (!columnsToIgnoreForNulls.IsEmpty())
				{
					columnsToIgnoreForNulls.Append(wxT(","));
				}
				columnsToIgnoreForNulls.Append(qtIdent(lstIgnoreForColumns->GetString(x)));
				someColumnsToIgnoreForNulls = true;
			}
		}

		// Check at least one column is selected else raise a warning
		if (!allColumnsToImport && columnsToImport.Length() <= 0)
		{
			wxMessageBox(_("Please select at least one column to import."), _("Import"), wxICON_WARNING | wxOK, this);
			return;
		}

		// Build COPY query
		query = wxT("COPY ") + object->GetSchema()->GetQuotedIdentifier() + wxT(".") + object->GetQuotedIdentifier();
		if (!allColumnsToImport)
		{
			query += wxT("(") + columnsToImport + wxT(")");
		}
		query += wxT(" FROM STDIN ");
		if (connection->BackendMinimumVersion(9, 0))
		{
			query += wxT("(");
			query += wxT("FORMAT '") + cbFormat->GetValue() + wxT("'");
			if (chkOids->GetValue())
				query += wxT(", OIDS");
			if (cbDelimiter->GetValue() == wxT("[tab]"))
				query += wxT(", DELIMITER E'\\t'");
			else if (!cbDelimiter->GetValue().IsEmpty())
				query += wxT(", DELIMITER ") + connection->qtDbString(cbDelimiter->GetValue());
			if (!txtNull->GetValue().IsEmpty())
				query += wxT(", NULL ") + connection->qtDbString(txtNull->GetValue());
			if (cbFormat->GetValue() == wxT("csv"))
			{
				if (chkHeader->GetValue())
					query += wxT(", HEADER");
				if (!cbQuote->GetValue().IsEmpty())
					query += wxT(", QUOTE ") + connection->qtDbString(cbQuote->GetValue());
				if (!cbEscape->GetValue().IsEmpty())
					query += wxT(", ESCAPE ") + connection->qtDbString(cbEscape->GetValue());
			}
			if (connection->BackendMinimumVersion(9, 1) && !cbEncoding->GetValue().IsEmpty())
				query += wxT(", ENCODING ") + connection->qtDbString(cbEncoding->GetValue());
			if (cbFormat->GetValue() == wxT("csv"))
			{
				if (someColumnsToIgnoreForNulls)
					query += wxT(", FORCE_NOT_NULL (") + columnsToIgnoreForNulls + wxT(")");
			}
			query += wxT(")");
		}
		else
		{
			query += wxT("WITH ");
			if (cbFormat->GetValue() == wxT("binary"))
				query += wxT("BINARY ");
			if (chkOids->GetValue())
				query += wxT("OIDS ");
			if (!cbDelimiter->GetValue().IsEmpty())
				query += wxT("DELIMITER ") + connection->qtDbString(cbDelimiter->GetValue());
			if (!txtNull->GetValue().IsEmpty())
				query += wxT("NULL ") + connection->qtDbString(txtNull->GetValue());
			if (cbFormat->GetValue() == wxT("csv"))
			{
				query += wxT("CSV ");
				if (connection->BackendMinimumVersion(8, 0) && chkHeader->GetValue())
					query += wxT("HEADER ");
				if (connection->BackendMinimumVersion(8, 0) && !cbQuote->GetValue().IsEmpty())
					query += wxT("QUOTE ") + connection->qtDbString(cbQuote->GetValue());
				if (connection->BackendMinimumVersion(8, 0) && !cbEscape->GetValue().IsEmpty())
					query += wxT("ESCAPE ") + connection->qtDbString(cbEscape->GetValue());
				if (someColumnsToIgnoreForNulls)
					query += wxT("FORCE NOT NULL ") + columnsToIgnoreForNulls + wxT(" ");
			}
		}

		// Check CSV file
		if (!wxFileName::FileExists(pickerImportfile->GetPath()))
		{
			wxString msg;
			msg.Printf(_("The file %s doesn't exist.\nPlease select a valid file."), pickerImportfile->GetPath().c_str());
			wxLogError(msg);
			return;
		}

		// Open CSV file
		fn = wxFileName(pickerImportfile->GetPath());
		file = new wxFile(fn.GetFullPath(), wxFile::read);

		// Set range for gauge
		gauge->SetRange(file->Length());

		// Start COPY
		if (!connection->StartCopy(query))
		{
			goterror = true;
		}

		// Send COPY data
		while (!copydone && !goterror)
		{
			// Read a buffer
			char *buffer = new char[nCount + 4];
			memset(buffer, 0, nCount + 4);
			off_t len = file->Read(buffer, nCount);

			// If we read something, send it to the server
			if (len > 0)
			{
				if (!connection->PutCopyData(buffer, len))
				{
					goterror = true;
				}
				else
				{
					gauge->SetValue(gauge->GetValue() + len);
					Update();
				}
			}

			// If we read nothing, we're done
			copydone = len == 0;

			// Clean the buffer
			delete[] buffer;
		}

		// Close CSV file
		file->Close();

		// End COPY
		if (goterror)
		{
			connection->EndPutCopy(_("Copy failed!"));
		}
		else
		{
			goterror = !connection->EndPutCopy(wxT(""));
		}

		// Get final status of the COPY command
		if (!connection->GetCopyFinalStatus())
		{
			goterror = true;
		}

		if (goterror)
		{
			wxLogError(_("Copy failed!\n") + connection->GetLastError());
		}
		else
		{
			btnOK->SetLabel(wxT("Done"));
			done = true;
		}
	}
	else
	{
		Close();
	}
}

importFactory::importFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("&Import..."), _("Import CSV file into a relation"));
}


wxWindow *importFactory::StartDialog(frmMain *form, pgObject *obj)
{
	pgDatabase *db = obj->GetDatabase();
	wxString applicationname = appearanceFactory->GetLongAppName() + _(" - Import Tool");
	pgConn *conn = db->CreateConn(applicationname);
	if (conn)
	{
		frmImport *frm = new frmImport(form, obj, conn);
		frm->Show();
	}
	return 0;
}

bool importFactory::CheckEnable(pgObject *obj)
{
	return obj && obj->IsCreatedBy(tableFactory);;
}
