//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
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

#include "frmConfig.h"
#include "frmHint.h"
#include "frmMainConfig.h"
#include "frmHbaConfig.h"
#include "frmMain.h"
#include "frmAbout.h"
#include "utffile.h"
#include "pgConn.h"
#include "pgSet.h"
#include "menu.h"
#include "pgfeatures.h"

#include "images/pgAdmin3.xpm"
#include "images/elephant32.xpm"
#include "images/file_open.xpm"
#include "images/file_save.xpm"
#include "images/edit_undo.xpm"
#include "images/help.xpm"
#include "images/checked.xpm"
#include "images/unchecked.xpm"
#include "images/query_execute.xpm"


BEGIN_EVENT_TABLE(frmConfig, pgFrame)
    EVT_CLOSE(                              frmConfig::OnClose)
    EVT_MENU(MNU_OPEN,                      frmConfig::OnOpen)
    EVT_MENU(MNU_SAVE,                      frmConfig::OnSave)
    EVT_MENU(MNU_SAVEAS,                    frmConfig::OnSaveAs)
    EVT_MENU(MNU_EXECUTE,                   frmConfig::OnExecute)
END_EVENT_TABLE()


wxImageList *configImageList=0;

frmConfig::frmConfig(frmMain *parent, const wxString& title, pgConn *_conn)
: pgFrame(parent, title)
{
    mainForm = parent;
    conn=_conn;
    SetStatusBarPane(-1);
}


frmConfig::frmConfig(const wxString& title, const wxString &configFile)
: pgFrame(NULL, title)
{
    mainForm = 0;
    conn=0;
    
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
        configImageList->Add(wxIcon(unchecked_xpm));
        configImageList->Add(wxIcon(checked_xpm));
    }

    wxWindowBase::SetFont(settings->GetSystemFont());

    toolBar=CreateToolBar();
    toolBar->SetToolBitmapSize(wxSize(16, 16));

    fileMenu = new wxMenu();
    editMenu = new wxMenu();
    helpMenu=new wxMenu();
    recentFileMenu = new wxMenu();

    fileMenu->Append(MNU_OPEN, _("&Open...\tCtrl-O"),   _("Open a query file"));
    toolBar->AddTool(MNU_OPEN, _("Open"), wxBitmap(file_open_xpm), _("Open file"), wxITEM_NORMAL);

    fileMenu->Append(MNU_SAVE, _("&Save\tCtrl-S"),      _("Save current file"));
    toolBar->AddTool(MNU_SAVE, _("Save"), wxBitmap(file_save_xpm), _("Save file"), wxITEM_NORMAL);

    fileMenu->Append(MNU_SAVEAS, _("Save &as..."),      _("Save file under new name"));

    fileMenu->AppendSeparator();
    toolBar->AddSeparator();

        // File Menu

    fileMenu->Append(MNU_EXECUTE, _("Reload server"),   _("Reload Server to apply configuration changes."));
    toolBar->AddTool(MNU_EXECUTE, _("Reload server"), wxBitmap(query_execute_xpm), _("Reload Server to apply configuration changes."), wxITEM_NORMAL);

    fileMenu->AppendSeparator();
    toolBar->AddSeparator();

    fileMenu->Append(MNU_RECENT, _("&Recent files"), recentFileMenu);
    fileMenu->Append(MNU_EXIT, _("E&xit\tAlt-F4"), _("Exit configuration tool"));

    editMenu->Append(MNU_UNDO, _("&Undo\tCtrl-Z"), _("Undo last action"), wxITEM_NORMAL);
    toolBar->AddTool(MNU_UNDO, _("Undo"), wxBitmap(edit_undo_xpm), _("Undo last action"), wxITEM_NORMAL);

    toolBar->AddSeparator();


    helpMenu->Append(MNU_CONTENTS, _("&Help..."),                 _("Open the pgAdmin III helpfile."));

    menuFactories.Add(new hintFactory(helpMenu, toolBar, false));
    helpMenu->Append(MNU_HELP, _("&Configuration Help\tF1"),      _("Display help on configuration options."));
    helpMenu->AppendSeparator();

    helpMenu->Append(MNU_BUGREPORT, _("&Bugreport"),              _("How to send a bugreport to the pgAdmin Development Team."));

    aboutFactory *af=new aboutFactory(helpMenu, 0);
    menuFactories.Add(af);


    menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, _("&File"));
    menuBar->Append(editMenu, _("&Edit"));
    menuBar->Append(helpMenu, _("&Help"));
    SetMenuBar(menuBar);

#ifdef __WXMAC__
    wxApp::s_macAboutMenuItemId = af->GetId();
#endif 

    wxAcceleratorEntry entries[3];

    entries[0].Set(wxACCEL_CTRL,                (int)'O',      MNU_OPEN);
    entries[1].Set(wxACCEL_CTRL,                (int)'S',      MNU_SAVE);
    entries[2].Set(wxACCEL_NORMAL,              WXK_F1,        MNU_HELP);

    wxAcceleratorTable accel(3, entries);
    SetAcceleratorTable(accel);

    toolBar->AddTool(MNU_HELP, _("Help"), wxBitmap(help_xpm),      _("Display help on configuration options."), wxITEM_NORMAL);
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

    statusBar = CreateStatusBar();
}


void frmConfig::Go()
{
    Show();
    DisplayHint(false);
}


void frmConfig::DoOpen(const wxString &fn)
{
    wxCommandEvent ev;
    OnOpen(ev);
}


void frmConfig::OnClose(wxCloseEvent& event)
{
    if (CheckChanged(event.CanVeto()) && event.CanVeto())
    {
        event.Veto();
        return;
    }
    Destroy();
}


void frmConfig::OnHint(wxCommandEvent& event)
{
    DisplayHint(true);
}


void frmConfig::OnBugreport(wxCommandEvent& event)
{
    DisplayHelp(this, wxT("bugreport"));
}


void frmConfig::OnExecute(wxCommandEvent& event)
{
    wxMessageDialog dlg(this, _("Are you sure you want trigger the server to reload its configuration?"),
                _("Reload server configuration"), wxICON_EXCLAMATION | wxYES_NO |wxNO_DEFAULT);
    if (dlg.ShowModal() == wxID_YES)
    {
        if (conn->ExecuteVoid(wxT("SELECT pg_reload_conf()")))
        {
        }
    }
}


void frmConfig::OnOpen(wxCommandEvent& event)
{
    if (CheckChanged(false))
        return;

    wxFileDialog dlg(this, _("Open configuration file"), lastDir, wxT(""), 
        _("Configuration files (*.conf)|*.conf|All files (*.*)|*.*"), wxOPEN);
    if (dlg.ShowModal() == wxID_OK)
    {
        lastFilename=dlg.GetFilename();
        lastDir = dlg.GetDirectory();
        lastPath = dlg.GetPath();
        OpenLastFile();
    }
}



void frmConfig::OnSave(wxCommandEvent& event)
{
    if (!fileMenu->IsEnabled(MNU_SAVE))
        return;

    if (conn)
    {
        wxMessageDialog msg(this, _("If a malformed configuration is written to the server, it might show problems when restarting or reloading.\nAre you sure the configuration is correct?"),
            _("Writing configuration file to server"), wxICON_EXCLAMATION|wxYES_NO|wxNO_DEFAULT);

        if (msg.ShowModal() != wxID_YES)
            return;
    }
    WriteFile(conn);
}


void frmConfig::OnSaveAs(wxCommandEvent& event)
{
    wxFileDialog *dlg=new wxFileDialog(this, _("Save configuration file as"), lastDir, lastFilename, 
        _("Configuration files (*.conf)|*.conf|All files (*.*)|*.*"), wxSAVE|wxOVERWRITE_PROMPT);
    if (dlg->ShowModal() == wxID_OK)
    {
        lastFilename=dlg->GetFilename();
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
    wxFile file(lastPath, wxFile::read);
    if (file.IsOpened())
    {
#ifdef __WXMSW__
        _setmode(file.fd(), _O_BINARY);
#endif
        char *buffer=new char[file.Length()+1];
        int cnt=file.Read(buffer,file.Length());
        buffer[cnt] = 0;
        file.Close();

        DisplayFile(wxString::FromAscii(buffer));

        delete buffer;
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
                    wxYES_NO|wxNO_DEFAULT|wxICON_EXCLAMATION|
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
    bool done=false;

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
                                           + qtString(str) + wxT(", false);\n")
            wxT("SELECT pg_file_rename('") + serverFileName + wxT(".tmp', '") 
                                           + serverFileName + wxT("', '") 
                                           + serverFileName + wxT(".bak');"));
        if (!done)
            conn->ExecuteVoid(
                wxT("SELECT pg_file_unlink('") + serverFileName + wxT(".tmp')"));
    }
    else
    {
        wxFile file(lastPath, wxFile::write);
        if (file.IsOpened())
        {
            file.Write(str);
            file.Close();
            done=true;
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
    wxString str=GetHintString();
    if (str.IsEmpty())
    {
        if (!force)
            return;
        str = _("No configuration setting detected that appears doubtful.");
    }

    wxMessageBox(str, _("pgAdmin III - Backend Configuration Hints"), wxICON_EXCLAMATION);
}


frmConfig *frmConfig ::Create(const wxString &title, const wxString &configFile, tryMode mode)
{
    frmConfig *frm=0;
    if (wxFile::Exists(configFile))
    {
        if (mode == HBAFILE ||configFile.Right(11) == wxT("pg_hba.conf"))
            frm = new frmHbaConfig(title, configFile);
        else if (mode == MAINFILE || configFile.Right(15) == wxT("postgresql.conf"))
            frm = new frmMainConfig(title, configFile);

        // unknown config file!
    }

    if (frm)
        frm->Go();

    return frm;
}
