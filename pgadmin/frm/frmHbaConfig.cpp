//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmHbaConfig.cpp - Backend access configuration tool
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"

#ifdef __WXMSW__
#include <io.h>
#include <fcntl.h>
#endif

#include <wx/imaglist.h>

#include "ctl/ctlMenuToolbar.h"
#include "frm/frmHbaConfig.h"
#include "dlg/dlgHbaConfig.h"
#include "frm/frmMain.h"
#include "utils/utffile.h"
#include "schema/pgServer.h"
#include "frm/menu.h"
#include "utils/pgfeatures.h"

#define CTL_CFGVIEW 345

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(pgHbaConfigLineArray);


BEGIN_EVENT_TABLE(frmHbaConfig, frmConfig)
	EVT_MENU(MNU_UNDO,                      frmHbaConfig::OnUndo)
	EVT_MENU(MNU_DELETE,					frmHbaConfig::OnDelete)
	EVT_MENU(MNU_CONTENTS,                  frmHbaConfig::OnContents)
	EVT_LIST_ITEM_ACTIVATED(CTL_CFGVIEW,    frmHbaConfig::OnEditSetting)
	EVT_LIST_ITEM_SELECTED(CTL_CFGVIEW,     frmHbaConfig::OnSelectSetting)
END_EVENT_TABLE()

#define BACE_TITLE _("Backend Access Configuration Editor")


frmHbaConfig::frmHbaConfig(frmMain *parent, pgServer *server)
	: frmConfig(parent, BACE_TITLE, 0)
{
	wxString applicationname = appearanceFactory->GetLongAppName() + _(" - Configuration Editor");
	if (server)
		conn = server->CreateConn(wxEmptyString, 0, applicationname);
	Init();

	if (conn)
	{
		serverFileName = conn->ExecuteScalar(wxT("SHOW hba_file"));
		if (serverFileName == wxT("unset") || serverFileName.IsEmpty())
			serverFileName = wxT("pg_hba.conf");

		wxString txt;
		txt.Printf(_(" - %s on %s (%s:%d)"),
		           serverFileName.c_str(), server->GetDescription().c_str(),
		           server->GetName().c_str(), server->GetPort());
		SetTitle(BACE_TITLE + txt);

		wxString str;
		str = conn->ExecuteScalar(wxT("SELECT pg_file_read('") + serverFileName + wxT("', 0, ")
		                          wxT("pg_file_length('") + serverFileName + wxT("'))"));

		DisplayFile(str);

		statusBar->SetStatusText(wxString::Format(_(" Configuration read from %s"), conn->GetHost().c_str()));
	}
}


frmHbaConfig::frmHbaConfig(const wxString &title, const wxString &configFile)
	: frmConfig(title + wxT(" - ") + _("Backend Access Configuration Editor"), configFile)
{

	Init();

	OpenLastFile();
}


frmHbaConfig::~frmHbaConfig()
{
}


void frmHbaConfig::Init()
{
	appearanceFactory->SetIcons(this);

	InitFrame(wxT("frmHbaConfig"));
	RestorePosition(150, 150, 650, 300, 300, 200);


	listEdit = new ctlListView(this, CTL_CFGVIEW, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER);
	listEdit->SetImageList(configImageList, wxIMAGE_LIST_SMALL);

	listEdit->AddColumn(_("Type"), 40);
	listEdit->AddColumn(_("Database"), 80);
	listEdit->AddColumn(_("User"), 80);
	listEdit->AddColumn(_("IP-Address"), 100);
	listEdit->AddColumn(_("Method"), 40);
	listEdit->AddColumn(_("Option"), 100);

	helpMenu->Enable(MNU_HINT, false);
	toolBar->EnableTool(MNU_HINT, false);

	editMenu->Enable(MNU_DELETE, false);
	toolBar->EnableTool(MNU_DELETE, false);
}


void frmHbaConfig::OnSelectSetting(wxListEvent &event)
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


void frmHbaConfig::DisplayFile(const wxString &str)
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
		pgHbaConfigLine *line = new pgHbaConfigLine(strtok.GetNextToken());
		lines.Add(line);
	}

	listEdit->DeleteAllItems();

	size_t i = lines.GetCount();

	// make sure the last line is empty

	for (i = 0 ; i < lines.GetCount() ; i++)
	{
		pgHbaConfigLine &line = lines.Item(i);
		const wxChar *connTypeStr = line.GetConnectType();
		if ((line.isValid || (!line.isValid && !line.isComment)) && !line.GetText().IsEmpty())
		{
			int imgIndex = 0;
			if (!line.isComment)
				imgIndex = 1;
			long pos = listEdit->AppendItem(imgIndex, connTypeStr, line.database, line.user);
			listEdit->SetItem(pos, 3, line.ipaddress);
			listEdit->SetItem(pos, 4, line.GetMethod());
			listEdit->SetItem(pos, 5, line.option);

			line.item = pos;
		}
	}
	if (!i || !lines.Item(i - 1).text.IsEmpty())
	{
		pgHbaConfigLine *line = new pgHbaConfigLine();
		lines.Add(line);
		line->item = listEdit->AppendItem(0, wxString(wxEmptyString));
	}
}


void frmHbaConfig::WriteFile(pgConn *conn)
{
	wxString str;
	size_t i;
	for (i = 0 ; i < lines.GetCount() - 1 ; i++)
		str.Append(lines.Item(i).GetText() + wxT("\n"));

	if (DoWriteFile(str, conn))
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


wxString frmHbaConfig::GetHintString()
{
	wxString hint;
	return hint;
}


wxString frmHbaConfig::GetHelpPage() const
{
	wxString page = wxT("client-authentication");
	return page;
}


void frmHbaConfig::OnContents(wxCommandEvent &event)
{
	DisplayHelp(wxT("index"), HELP_PGADMIN);
}


void frmHbaConfig::OnUndo(wxCommandEvent &ev)
{
	int pos = listEdit->GetSelection();
	if (pos >= 0)
	{
		size_t i;
		for (i = 0 ; i < lines.GetCount() ; i++)
		{
			pgHbaConfigLine &line = lines.Item(i);

			if (line.item == pos)
			{
				line.Init(line.text);
				UpdateDisplay(line);
				break;
			}
		}
	}
}

void frmHbaConfig::OnDelete(wxCommandEvent &event)
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


void frmHbaConfig::UpdateDisplay(pgHbaConfigLine &line)
{
	long pos = line.item;
	listEdit->SetItemImage(pos, (line.isComment ? 0 : 1));
	listEdit->SetItem(pos, 0, line.GetConnectType());
	listEdit->SetItem(pos, 1, line.database);
	listEdit->SetItem(pos, 2, line.user);
	listEdit->SetItem(pos, 3, line.ipaddress);
	listEdit->SetItem(pos, 4, line.GetMethod());
	listEdit->SetItem(pos, 5, line.option);
}


void frmHbaConfig::OnEditSetting(wxListEvent &event)
{
	long pos = event.GetIndex();
	if (pos < 0)
		return;

	size_t i;

	for (i = 0 ; i < lines.GetCount() ; i++)
	{
		if (lines.Item(i).item == pos)
		{
			pgHbaConfigLine &line = lines.Item(i);
			bool isLastLine = (i == lines.GetCount() - 1 && line.isComment && !line.GetConnectType());

			dlgHbaConfig dlg(this, &line, conn);
			if (dlg.Go() == wxID_OK)
			{
				UpdateDisplay(line);

				if (isLastLine)
				{
					long pos = listEdit->AppendItem(0, wxString(wxEmptyString));
					pgHbaConfigLine *line = new pgHbaConfigLine();
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


hbaConfigFactory::hbaConfigFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : actionFactory(list)
{
	mnu->Append(id, wxT("pg_hba.conf"), _("Edit server access configuration file."));
}


wxWindow *hbaConfigFactory::StartDialog(frmMain *form, pgObject *obj)
{
	pgServer *server = obj->GetServer();
	if (server)
	{
		frmHbaConfig *frm = new frmHbaConfig(form, server);
		frm->Go();
		return frm;
	}
	return 0;
}


bool hbaConfigFactory::CheckEnable(pgObject *obj)
{
	if (obj)
	{
		pgServer *server = obj->GetServer();
		if (server)
		{
			pgConn *conn = server->GetConnection();
			return conn && server->GetSuperUser() &&  conn->HasFeature(FEATURE_FILEREAD);
		}
	}
	return false;
}


hbaConfigFileFactory::hbaConfigFileFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : actionFactory(list)
{
	mnu->Append(id, _("Open pg_hba.conf..."), _("Open configuration editor with pg_hba.conf."));
}


wxWindow *hbaConfigFileFactory::StartDialog(frmMain *form, pgObject *obj)
{
	frmConfig *dlg = new frmHbaConfig(form);
	dlg->Go();
	dlg->DoOpen();
	return dlg;
}
