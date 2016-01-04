//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmConfig.cpp - Configuration tool
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

#ifdef __WXMSW__
#include <io.h>
#include <fcntl.h>
#endif

#include <wx/imaglist.h>

#include "ctl/ctlMenuToolbar.h"
#include "frm/frmConfig.h"
#include "frm/frmHint.h"
#include "frm/frmMainConfig.h"
#include "frm/frmHbaConfig.h"
#include "frm/frmPgpassConfig.h"
#include "frm/frmMain.h"
#include "frm/frmAbout.h"
#include "utils/utffile.h"
#include "db/pgConn.h"
#include "db/pgSet.h"
#include "frm/menu.h"
#include "utils/pgfeatures.h"

#include "images/file_open.pngc"
#include "images/file_save.pngc"
#include "images/edit_undo.pngc"
#include "images/delete.pngc"
#include "images/help.pngc"
#include "images/hint2.pngc"
#include "images/checked.pngc"
#include "images/unchecked.pngc"
#include "images/query_execute.pngc"


BEGIN_EVENT_TABLE(frmConfig, pgFrame)
	EVT_CLOSE(                              frmConfig::OnClose)
	EVT_MENU(MNU_OPEN,                      frmConfig::OnOpen)
	EVT_MENU(MNU_SAVE,                      frmConfig::OnSave)
	EVT_MENU(MNU_SAVEAS,                    frmConfig::OnSaveAs)
	EVT_MENU(MNU_EXECUTE,                   frmConfig::OnExecute)
	EVT_MENU(MNU_HELP,						frmConfig::OnHelp)
	EVT_MENU(MNU_HINT,                      frmConfig::OnHint)
END_EVENT_TABLE()


wxImageList *configImageList = 0;

frmConfig::frmConfig(frmMain *parent, const wxString &title, pgConn *_conn)
	: pgFrame(parent, title)
{
	mainForm = parent;
	conn = _conn;
	SetStatusBarPane(-1);
}


frmConfig::frmConfig(const wxString &title, const wxString &configFile)
	: pgFrame(NULL, title)
{
	mainForm = 0;
	conn = 0;

	lastPath = configFile;
	SetStatusBarPane(-1);
}


frmConfig::~frmConfig()
{
	if (conn)
		delete conn;
	if (mainForm)
		mainForm->RemoveFrame(this);
}


void frmConfig::InitFrame(const wxChar *frameName)
{
	dlgName = frameName;

	if (!configImageList)
	{
		configImageList = new wxImageList(16, 16, true, 2);
		configImageList->Add(*unchecked_png_ico);
		configImageList->Add(*checked_png_ico);
	}

	menuFactories = new menuFactoryList();

	SetFont(settings->GetSystemFont());

	toolBar = new ctlMenuToolbar(this, -1, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
	toolBar->SetToolBitmapSize(wxSize(16, 16));

	fileMenu = new wxMenu();
	editMenu = new wxMenu();
	helpMenu = new wxMenu();
	recentFileMenu = new wxMenu();

	fileMenu->Append(MNU_OPEN, _("&Open...\tCtrl-O"),   _("Open a query file"));
	toolBar->AddTool(MNU_OPEN, wxEmptyString, *file_open_png_bmp, _("Open file"), wxITEM_NORMAL);

	fileMenu->Append(MNU_SAVE, _("&Save\tCtrl-S"),      _("Save current file"));
	toolBar->AddTool(MNU_SAVE, wxEmptyString, *file_save_png_bmp, _("Save file"), wxITEM_NORMAL);

	fileMenu->Append(MNU_SAVEAS, _("Save &as..."),      _("Save file under new name"));

	fileMenu->AppendSeparator();
	toolBar->AddSeparator();

	// File Menu

	fileMenu->Append(MNU_EXECUTE, _("Reload server"),   _("Reload Server to apply configuration changes."));
	toolBar->AddTool(MNU_EXECUTE, wxEmptyString, *query_execute_png_bmp, _("Reload Server to apply configuration changes."), wxITEM_NORMAL);

	fileMenu->AppendSeparator();
	toolBar->AddSeparator();

	fileMenu->Append(MNU_RECENT, _("&Recent files"), recentFileMenu);
	fileMenu->Append(MNU_EXIT, _("E&xit\tCtrl-W"), _("Exit configuration tool"));

	editMenu->Append(MNU_UNDO, _("&Undo\tCtrl-Z"), _("Undo last action"), wxITEM_NORMAL);
	toolBar->AddTool(MNU_UNDO, wxEmptyString, *edit_undo_png_bmp, _("Undo last action"), wxITEM_NORMAL);
	editMenu->AppendSeparator();
	editMenu->Append(MNU_DELETE, _("&Delete\tDEL"), _("Delete current row"), wxITEM_NORMAL);
	toolBar->AddTool(MNU_DELETE, wxEmptyString, *delete_png_bmp, _("Delete current row"), wxITEM_NORMAL);

	toolBar->AddSeparator();


	helpMenu->Append(MNU_CONTENTS, _("&Help"),                 _("Open the helpfile."));

	helpMenu->Append(MNU_HINT, _("Hints"), _("Display helpful hints on current object."));
	toolBar->AddTool(MNU_HINT, wxEmptyString, *hint2_png_bmp,   _("Display helpful hints on current object."));
	helpMenu->Append(MNU_HELP, _("&Configuration Help\tF1"),      _("Display help on configuration options."));
	helpMenu->AppendSeparator();

	new bugReportFactory(menuFactories, helpMenu, 0);
	aboutFactory *af = new aboutFactory(menuFactories, helpMenu, 0);

	menuBar = new wxMenuBar();
	menuBar->Append(fileMenu, _("&File"));
	menuBar->Append(editMenu, _("&Edit"));
	menuBar->Append(helpMenu, _("&Help"));
	SetMenuBar(menuBar);

#ifdef __WXMAC__
	wxApp::s_macAboutMenuItemId = af->GetId();
#else
	(void)af;
#endif

	menuFactories->RegisterMenu(this, wxCommandEventHandler(pgFrame::OnAction));
	menuFactories->CheckMenu(0, menuBar, toolBar);

	wxAcceleratorEntry entries[3];

	entries[0].Set(wxACCEL_CTRL,                (int)'O',      MNU_OPEN);
	entries[1].Set(wxACCEL_CTRL,                (int)'S',      MNU_SAVE);
	entries[2].Set(wxACCEL_NORMAL,              WXK_F1,        MNU_HELP);

	wxAcceleratorTable accel(3, entries);
	SetAcceleratorTable(accel);

	toolBar->AddTool(MNU_HELP, wxEmptyString, *help_png_bmp,      _("Display help on configuration options."), wxITEM_NORMAL);
	toolBar->Realize();

	UpdateRecentFiles();

	fileMenu->Enable(MNU_SAVE, false);
	editMenu->Enable(MNU_UNDO, false);
	toolBar->EnableTool(MNU_SAVE, false);
	toolBar->EnableTool(MNU_UNDO, false);

	if (conn)
	{
		toolBar->EnableTool(MNU_OPEN, false);
		if (!conn->HasFeature(FEATURE_RELOAD_CONF))
		{
			fileMenu->Enable(MNU_EXECUTE, false);
			toolBar->EnableTool(MNU_EXECUTE, false);
		}
	}
	else
	{
		fileMenu->Enable(MNU_EXECUTE, false);
		toolBar->EnableTool(MNU_EXECUTE, false);
	}

	SetToolBar(toolBar);
	statusBar = CreateStatusBar();
}


void frmConfig::Go()
{
	Show();
}

void frmConfig::DoOpen(const wxString &fn)
{
	wxCommandEvent ev;
	OnOpen(ev);
}


void frmConfig::OnClose(wxCloseEvent &event)
{
	if (CheckChanged(event.CanVeto()) && event.CanVeto())
	{
		event.Veto();
		return;
	}
	Destroy();
}

void frmConfig::OnHelp(wxCommandEvent &event)
{
	DisplayHelp(GetHelpPage(), HELP_POSTGRESQL);
}

void frmConfig::OnHint(wxCommandEvent &event)
{
	DisplayHint(true);
}


void frmConfig::OnBugreport(wxCommandEvent &event)
{
	DisplayHelp(wxT("bugreport"), HELP_PGADMIN);
}


void frmConfig::OnExecute(wxCommandEvent &event)
{
	wxMessageDialog dlg(this, _("Are you sure you want trigger the server to reload its configuration?"),
	                    _("Reload server configuration"), wxICON_EXCLAMATION | wxYES_NO | wxNO_DEFAULT);
	if (dlg.ShowModal() == wxID_YES)
	{
		if (conn->ExecuteVoid(wxT("SELECT pg_reload_conf()")))
		{
		}
	}
}


void frmConfig::OnOpen(wxCommandEvent &event)
{
	if (CheckChanged(true))
		return;

#ifdef __WXMSW__
	wxFileDialog dlg(this, _("Open configuration file"), lastDir, wxT(""),
	                 _("Configuration files (*.conf)|*.conf|All files (*.*)|*.*"), wxFD_OPEN);
#else
	wxFileDialog dlg(this, _("Open configuration file"), lastDir, wxT(""),
	                 _("Configuration files (*.conf)|*.conf|All files (*)|*"), wxFD_OPEN);
#endif

	if (dlg.ShowModal() == wxID_OK)
	{
		lastFilename = dlg.GetFilename();
		lastDir = dlg.GetDirectory();
		lastPath = dlg.GetPath();
		OpenLastFile();
		UpdateRecentFiles();
	}
}



void frmConfig::OnSave(wxCommandEvent &event)
{
	if (!fileMenu->IsEnabled(MNU_SAVE))
		return;

	if (conn)
	{
		wxMessageDialog msg(this, _("If a malformed configuration is written to the server, it might show problems when restarting or reloading.\nAre you sure the configuration is correct?"),
		                    _("Writing configuration file to server"), wxICON_EXCLAMATION | wxYES_NO | wxNO_DEFAULT);

		if (msg.ShowModal() != wxID_YES)
			return;
	}
	WriteFile(conn);
}


void frmConfig::OnSaveAs(wxCommandEvent &event)
{
#ifdef __WXMSW__
	wxFileDialog *dlg = new wxFileDialog(this, _("Save configuration file as"), lastDir, lastFilename,
	                                     _("Configuration files (*.conf)|*.conf|All files (*.*)|*.*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
#else
	wxFileDialog *dlg = new wxFileDialog(this, _("Save configuration file as"), lastDir, lastFilename,
	                                     _("Configuration files (*.conf)|*.conf|All files (*)|*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
#endif
	if (dlg->ShowModal() == wxID_OK)
	{
		lastFilename = dlg->GetFilename();
		lastDir = dlg->GetDirectory();
		lastPath = dlg->GetPath();

		WriteFile();
		if (!changed)
			UpdateRecentFiles();
	}
	delete dlg;
}


void frmConfig::OpenLastFile()
{
	wxUtfFile file(lastPath, wxFile::read, wxFONTENCODING_SYSTEM);
	if (file.IsOpened())
	{
		wxString buffer;
		file.Read(buffer);
		file.Close();

		DisplayFile(buffer);

		statusBar->SetStatusText(wxString::Format(_(" Configuration read from %s"), lastPath.c_str()));

		fileMenu->Enable(MNU_SAVE, false);
		editMenu->Enable(MNU_UNDO, false);
		toolBar->EnableTool(MNU_SAVE, false);
		toolBar->EnableTool(MNU_UNDO, false);
	}
}


bool frmConfig::CheckChanged(bool canVeto)
{
	if (changed && settings->GetAskSaveConfirmation())
	{
		wxMessageDialog msg(this, _("The configuration is modified.\nDo you want to save changes?"),
		                    GetTitle(),
		                    wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION |
		                    (canVeto ? wxCANCEL : 0));

		switch (msg.ShowModal())
		{
			case wxID_YES:
				WriteFile(conn);
				return changed;

			case wxID_CANCEL:
				return true;
		}
	}
	return false;
}


bool frmConfig::DoWriteFile(const wxChar *str, pgConn *conn)
{
	bool done = false;

	wxString buffer;
	if (filetype != wxTextFileType_Unix)
	{
		buffer = wxTextBuffer::Translate(str, filetype);
		str = buffer;
	}
	if (conn)
	{
		done = conn->ExecuteVoid(
		           wxT("SELECT pg_file_unlink('") + serverFileName + wxT(".bak');\n")
		           wxT("SELECT pg_file_write('")  + serverFileName + wxT(".tmp', ")
		           + conn->qtDbString(str) + wxT(", false);\n")
		           wxT("SELECT pg_file_rename('") + serverFileName + wxT(".tmp', '")
		           + serverFileName + wxT("', '")
		           + serverFileName + wxT(".bak');"));
		if (!done)
			conn->ExecuteVoid(
			    wxT("SELECT pg_file_unlink('") + serverFileName + wxT(".tmp')"));
	}
	else
	{
		wxUtfFile file(lastPath, wxFile::write, wxFONTENCODING_SYSTEM);
		if (file.IsOpened())
		{
#ifdef __WXMSW__
			_setmode(file.fd(), _O_BINARY);
#endif
			file.Write(str);
			file.Close();
			done = true;
		}
	}

	if (done)
	{
		if (conn)
			statusBar->SetStatusText(wxString::Format(_(" Configuration written to %s"), conn->GetHost().c_str()));
		else
			statusBar->SetStatusText(wxString::Format(_(" Configuration written to %s"), lastPath.c_str()));
	}
	return done;
}


void frmConfig::DisplayHint(bool force)
{
	wxString str = GetHintString();
	if (str.IsEmpty())
	{
		if (!force)
			return;
		str = _("No configuration setting detected that appears doubtful.");
	}

	wxMessageBox(str, _("Backend Configuration Hints"), wxICON_EXCLAMATION | wxOK);
}


frmConfig *frmConfig ::Create(const wxString &title, const wxString &configFile, tryMode mode)
{
	frmConfig *frm = 0;
	if (wxFile::Exists(configFile))
	{
		if (mode == HBAFILE || configFile.Right(11) == wxT("pg_hba.conf"))
			frm = new frmHbaConfig(title, configFile);
		else if (mode == MAINFILE || configFile.Right(15) == wxT("postgresql.conf"))
			frm = new frmMainConfig(title, configFile);
		else if (mode == PGPASSFILE || configFile.Right(11) == wxT("pgpass.conf") || configFile.Right(7) == wxT(".pgpass"))
			frm = new frmPgpassConfig(title, configFile);

		// unknown config file!
	}

	if (frm)
		frm->Go();

	return frm;
}
