//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmPgpassConfig.cpp - Client access configuration tool
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

#ifdef __WXMSW__
#include <io.h>
#include <fcntl.h>
#endif

#include <wx/imaglist.h>

#include "ctl/ctlMenuToolbar.h"
#include "frm/frmPgpassConfig.h"
#include "dlg/dlgPgpassConfig.h"
#include "frm/frmMain.h"
#include "utils/utffile.h"
#include "schema/pgServer.h"
#include "frm/menu.h"
#include "utils/pgfeatures.h"

#define CTL_CFGVIEW 345

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(pgPassConfigLineArray);


BEGIN_EVENT_TABLE(frmPgpassConfig, frmConfig)
	EVT_MENU(MNU_UNDO,                      frmPgpassConfig::OnUndo)
	EVT_MENU(MNU_DELETE,					frmPgpassConfig::OnDelete)
	EVT_MENU(MNU_CONTENTS,                  frmPgpassConfig::OnContents)
	EVT_LIST_ITEM_ACTIVATED(CTL_CFGVIEW,     frmPgpassConfig::OnEditSetting)
	EVT_LIST_ITEM_SELECTED(CTL_CFGVIEW,     frmPgpassConfig::OnSelectSetting)
END_EVENT_TABLE()

#define CACE_TITLE _("Client Access Configuration Editor")

frmPgpassConfig::frmPgpassConfig(const wxString &title, const wxString &configFile)
	: frmConfig(title + wxT(" - ") + _("Client Access Configuration Editor"), configFile)
{
	Init();

	OpenLastFile();

	helpMenu->Enable(MNU_HINT, false);
	toolBar->EnableTool(MNU_HINT, false);
}

frmPgpassConfig::frmPgpassConfig(frmMain *parent)
	: frmConfig(parent, CACE_TITLE, 0)
{
	Init();

	lastPath = sysSettings::GetConfigFile(sysSettings::PGPASS);
	wxFile f;
	if (f.Exists(lastPath))
	{
		OpenLastFile();
	}

	helpMenu->Enable(MNU_HINT, false);
	toolBar->EnableTool(MNU_HINT, false);
}

frmPgpassConfig::~frmPgpassConfig()
{
}


void frmPgpassConfig::Init()
{
	appearanceFactory->SetIcons(this);

	InitFrame(wxT("frmPgpassConfig"));
	RestorePosition(150, 150, 650, 300, 300, 200);


	listEdit = new ctlListView(this, CTL_CFGVIEW, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER);
	listEdit->SetImageList(configImageList, wxIMAGE_LIST_SMALL);

	listEdit->AddColumn(_("Host"), 50);
	listEdit->AddColumn(_("Port"), 30);
	listEdit->AddColumn(_("Database"), 80);
	listEdit->AddColumn(_("Username"), 80);
	listEdit->AddColumn(_("Password"), 80);

	editMenu->Enable(MNU_DELETE, false);
	toolBar->EnableTool(MNU_DELETE, false);
}

void frmPgpassConfig::OnSelectSetting(wxListEvent &event)
{
	// Enable delete because an item has been selected
	if (event.GetIndex() != listEdit->GetItemCount() - 1)
	{
		editMenu->Enable(MNU_DELETE, true);
		toolBar->EnableTool(MNU_DELETE, true);
	}
	else
	{
		editMenu->Enable(MNU_DELETE, false);
		toolBar->EnableTool(MNU_DELETE, false);
	}

	// Disable undo because we don't want to undo the wrong line.
	editMenu->Enable(MNU_UNDO, false);
	toolBar->EnableTool(MNU_UNDO, false);
}

void frmPgpassConfig::DisplayFile(const wxString &str)
{
	lines.Empty();

	filetype = wxTextFileType_Unix;
	wxStringTokenizer strtok;

	if (str.Find('\r') >= 0)
	{
		if (str.Find(wxT("\n\r")) >= 0 || str.Find(wxT("\r\n")))
			filetype = wxTextFileType_Dos;
		else
			filetype = wxTextFileType_Mac;

		strtok.SetString(wxTextBuffer::Translate(str, wxTextFileType_Unix), wxT("\n"), wxTOKEN_RET_EMPTY);
	}
	else
		strtok.SetString(str, wxT("\n"), wxTOKEN_RET_EMPTY);

	while (strtok.HasMoreTokens())
	{
		pgPassConfigLine *line = new pgPassConfigLine(strtok.GetNextToken());
		lines.Add(line);
	}

	listEdit->DeleteAllItems();

	size_t i;

	for (i = 0 ; i < lines.GetCount() ; i++)
	{
		pgPassConfigLine &line = lines.Item(i);
		int imgIndex = 0;
		if (!line.isComment)
			imgIndex = 1;
		long pos = listEdit->AppendItem(imgIndex, line.hostname);

		listEdit->SetItem(pos, 1, line.port);
		listEdit->SetItem(pos, 2, line.database);
		listEdit->SetItem(pos, 3, line.username);
		listEdit->SetItem(pos, 4, line.password.IsEmpty() ? wxT("") : wxT("*********"));
		line.item = pos;
	}
	if (!i || !lines.Item(i - 1).text.IsEmpty())
	{
		pgPassConfigLine *line = new pgPassConfigLine();
		lines.Add(line);
		line->item = listEdit->AppendItem(0, wxString(wxEmptyString));
	}
}


void frmPgpassConfig::WriteFile(pgConn *conn)
{
	wxString str;
	size_t i;
	for (i = 0 ; i < lines.GetCount() - 1 ; i++)
	{
		// Before writing it into the file we need to escape "\" and ":"
		pgPassConfigLine line = lines.Item(i);
		line.hostname.Replace(wxT("\\"), wxT("\\\\"));
		line.hostname.Replace(wxT(":") , wxT("\\:"));
		line.port.Replace(wxT("\\"), wxT("\\\\"));
		line.port.Replace(wxT(":") , wxT("\\:"));
		line.database.Replace(wxT("\\"), wxT("\\\\"));
		line.database.Replace(wxT(":") , wxT("\\:"));
		line.username.Replace(wxT("\\"), wxT("\\\\"));
		line.username.Replace(wxT(":") , wxT("\\:"));
		line.password.Replace(wxT("\\"), wxT("\\\\"));
		line.password.Replace(wxT(":") , wxT("\\:"));

		wxString strLine = line.hostname + wxT(":") + line.port + wxT(":") +
		                   line.database + wxT(":") + line.username + wxT(":") + line.password + wxT("\n");

		str.Append(strLine);
	}

	if (DoWriteFile(str, NULL))
	{
		changed = false;
		fileMenu->Enable(MNU_SAVE, false);
		editMenu->Enable(MNU_UNDO, false);
		toolBar->EnableTool(MNU_SAVE, false);
		toolBar->EnableTool(MNU_UNDO, false);

		// make intermediate change current
		for (i = 0 ; i < lines.GetCount() ; i++)
			lines.Item(i).Init(lines.Item(i).GetText());
	}
}


wxString frmPgpassConfig::GetHintString()
{
	wxString hint;
	return hint;
}


wxString frmPgpassConfig::GetHelpPage() const
{
	wxString page = wxT("libpq-pgpass");
	;
	return page;
}


void frmPgpassConfig::OnContents(wxCommandEvent &event)
{
	DisplayHelp(wxT("index"), HELP_PGADMIN);
}


void frmPgpassConfig::OnUndo(wxCommandEvent &ev)
{
	int pos = listEdit->GetSelection();
	if (pos >= 0)
	{
		size_t i;
		for (i = 0 ; i < lines.GetCount() ; i++)
		{
			pgPassConfigLine &line = lines.Item(i);

			if (line.item == pos)
			{
				line.Init(line.text);
				UpdateDisplay(line);
				break;
			}
		}
	}
}

void frmPgpassConfig::OnDelete(wxCommandEvent &event)
{
	bool found = false;
	int pos = listEdit->GetSelection();
	if (pos >= 0)
	{
		listEdit->DeleteCurrentItem();
		size_t i;
		for (i = 0; i < lines.GetCount(); i++)
		{
			if (lines.Item(i).item == pos)
			{
				lines.RemoveAt(i);
				changed = true;
				fileMenu->Enable(MNU_SAVE, true);
				editMenu->Enable(MNU_UNDO, false);
				editMenu->Enable(MNU_DELETE, false);
				toolBar->EnableTool(MNU_SAVE, true);
				toolBar->EnableTool(MNU_UNDO, false);
				toolBar->EnableTool(MNU_DELETE, false);
				found = true;
				break;
			}
		}
		if (found)
		{
			/* Renumber all positions */
			for (i = 0; i < lines.GetCount(); i++)
			{
				if (lines.Item(i).item > pos)
					lines.Item(i).item--;
			}
		}
	}
}

void frmPgpassConfig::UpdateDisplay(pgPassConfigLine &line)
{
	long pos = line.item;
	listEdit->SetItemImage(pos, (line.isComment ? 0 : 1));
	listEdit->SetItem(pos, 0, line.hostname);
	listEdit->SetItem(pos, 1, line.port);
	listEdit->SetItem(pos, 2, line.database);
	listEdit->SetItem(pos, 3, line.username);
	listEdit->SetItem(pos, 4, line.password.IsEmpty() ? wxT("") : wxT("*********"));
}


void frmPgpassConfig::OnEditSetting(wxListEvent &event)
{
	long pos = event.GetIndex();
	if (pos < 0)
		return;

	size_t i;

	for (i = 0 ; i < lines.GetCount() ; i++)
	{
		if (lines.Item(i).item == pos)
		{
			pgPassConfigLine &line = lines.Item(i);
			bool isLastLine = (i == lines.GetCount() - 1 && line.isComment && line.text.IsEmpty());

			dlgPgpassConfig dlg(this, &line);
			if (dlg.Go() == wxID_OK)
			{
				UpdateDisplay(line);

				if (isLastLine)
				{
					long pos = listEdit->AppendItem(0, wxString(wxEmptyString));
					pgPassConfigLine *line = new pgPassConfigLine();
					line->item = pos;
					lines.Add(line);
				}
				changed = true;
				fileMenu->Enable(MNU_SAVE, true);
				editMenu->Enable(MNU_UNDO, true);
				toolBar->EnableTool(MNU_SAVE, true);
				toolBar->EnableTool(MNU_UNDO, true);
			}
			break;
		}
	}
}


pgpassConfigFileFactory::pgpassConfigFileFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : actionFactory(list)
{
#ifdef WIN32
	mnu->Append(id, _("Open pgpass.conf"), _("Open configuration editor with pgpass.conf."));
#else
	mnu->Append(id, _("Open .pgpass"), _("Open configuration editor with .pgpass"));
#endif
}


wxWindow *pgpassConfigFileFactory::StartDialog(frmMain *form, pgObject *obj)
{
	frmConfig *dlg = new frmPgpassConfig(form);
	dlg->Go();
	return dlg;
}
