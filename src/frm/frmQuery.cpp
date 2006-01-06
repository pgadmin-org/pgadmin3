//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmQuery.cpp - SQL Query Box
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/splitter.h>

// App headers
#include "pgAdmin3.h"
#include "frmMain.h"
#include "frmQuery.h"
#include "frmHelp.h"
#include "menu.h"
#include "explainCanvas.h"

#include "ctl/ctlSQLResult.h"
#include "pgDatabase.h"
#include "dlgSelectConnection.h"

#include <wx/clipbrd.h>

// Icons
#include "images/sql.xpm"
// Bitmaps
#include "images/file_open.xpm"
#include "images/file_save.xpm"
#include "images/clip_cut.xpm"
#include "images/clip_copy.xpm"
#include "images/clip_paste.xpm"
#include "images/edit_clear.xpm"
#include "images/edit_find.xpm"
#include "images/edit_undo.xpm"
#include "images/edit_redo.xpm"
#include "images/query_execute.xpm"
#include "images/query_execfile.xpm"
#include "images/query_explain.xpm"
#include "images/query_cancel.xpm"
#include "images/help.xpm"



#define CTRLID_CONNECTION       4200

BEGIN_EVENT_TABLE(frmQuery, pgFrame)
    EVT_COMBOBOX(CTRLID_CONNECTION, frmQuery::OnChangeConnection)
    EVT_CLOSE(                      frmQuery::OnClose)
    EVT_SET_FOCUS(                  frmQuery::OnSetFocus)
    EVT_MENU(MNU_OPEN,              frmQuery::OnOpen)
    EVT_MENU(MNU_SAVE,              frmQuery::OnSave)
    EVT_MENU(MNU_SAVEAS,            frmQuery::OnSaveAs)
    EVT_MENU(MNU_EXPORT,            frmQuery::OnExport)
    EVT_MENU(MNU_CUT,               frmQuery::OnCut)
    EVT_MENU(MNU_COPY,              frmQuery::OnCopy)
    EVT_MENU(MNU_PASTE,             frmQuery::OnPaste)
    EVT_MENU(MNU_CLEAR,             frmQuery::OnClear)
    EVT_MENU(MNU_FIND,              frmQuery::OnFind)
    EVT_MENU(MNU_REPLACE,           frmQuery::OnReplace)
    EVT_MENU(MNU_UNDO,              frmQuery::OnUndo)
    EVT_MENU(MNU_REDO,              frmQuery::OnRedo)
    EVT_MENU(MNU_EXECUTE,           frmQuery::OnExecute)
    EVT_MENU(MNU_EXECFILE,          frmQuery::OnExecFile)
    EVT_MENU(MNU_EXPLAIN,           frmQuery::OnExplain)
    EVT_MENU(MNU_CANCEL,            frmQuery::OnCancel)
    EVT_MENU(MNU_CONTENTS,          frmQuery::OnContents)
    EVT_MENU(MNU_HELP,              frmQuery::OnHelp)
    EVT_MENU(MNU_CLEARHISTORY,      frmQuery::OnClearHistory)
    EVT_MENU(MNU_SAVEHISTORY,       frmQuery::OnSaveHistory)
    EVT_ACTIVATE(                   frmQuery::OnActivate)
    EVT_STC_MODIFIED(CTL_SQLQUERY,  frmQuery::OnChangeStc)
    EVT_STC_UPDATEUI(CTL_SQLQUERY,  frmQuery::OnPositionStc)
END_EVENT_TABLE()



enum
{
    RESULT_PAGE=0,
    EXPLAIN_PAGE,
    MSG_PAGE,
    HISTORY_PAGE
};

frmQuery::frmQuery(frmMain *form, const wxString& _title, pgConn *_conn, const wxString& query)
: pgFrame(NULL, _title)
{
    mainForm=form;
    conn=_conn;

    dlgName = wxT("frmQuery");
    recentKey = wxT("RecentFiles");
    RestorePosition(100, 100, 600, 500, 200, 150);

    SetIcon(wxIcon(sql_xpm));
    wxWindowBase::SetFont(settings->GetSystemFont());
    wxLogInfo(wxT("Creating SQL Query box"));
    menuBar = new wxMenuBar();

    fileMenu = new wxMenu();
    recentFileMenu = new wxMenu();
    fileMenu->Append(MNU_OPEN, _("&Open...\tCtrl-O"),   _("Open a query file"));
    fileMenu->Append(MNU_SAVE, _("&Save\tCtrl-S"),      _("Save current file"));
    fileMenu->Append(MNU_SAVEAS, _("Save &as..."),      _("Save file under new name"));
    fileMenu->AppendSeparator();
    fileMenu->Append(MNU_EXPORT, _("&Export"),  _("Export data to file"));
    fileMenu->AppendSeparator();
    fileMenu->Append(MNU_RECENT, _("&Recent files"), recentFileMenu);
    fileMenu->Append(MNU_EXIT, _("E&xit\tAlt-F4"), _("Exit query window"));

    menuBar->Append(fileMenu, _("&File"));

    editMenu = new wxMenu();
    editMenu->Append(MNU_UNDO, _("&Undo\tCtrl-Z"), _("Undo last action"), wxITEM_NORMAL);
    editMenu->Append(MNU_REDO, _("&Redo\tCtrl-Y"), _("Redo last action"), wxITEM_NORMAL);
    editMenu->AppendSeparator();
    editMenu->Append(MNU_CUT, _("Cu&t\tCtrl-X"), _("Cut selected text to clipboard"), wxITEM_NORMAL);
    editMenu->Append(MNU_COPY, _("&Copy\tCtrl-C"), _("Copy selected text to clipboard"), wxITEM_NORMAL);
    editMenu->Append(MNU_PASTE, _("&Paste\tCtrl-V"), _("Paste selected text from clipboard"), wxITEM_NORMAL);
    editMenu->Append(MNU_CLEAR, _("C&lear window"), _("Clear edit window"), wxITEM_NORMAL);
    editMenu->AppendSeparator();
    editMenu->Append(MNU_FIND, _("&Find\tCtrl-F"), _("Find text"), wxITEM_NORMAL);
    editMenu->Append(MNU_REPLACE, _("&Replace\tCtrl-R"), _("Find and Replace text"), wxITEM_NORMAL);
    menuBar->Append(editMenu, _("&Edit"));

    queryMenu = new wxMenu();
    queryMenu->Append(MNU_EXECUTE, _("&Execute\tF5"), _("Execute query"));
    queryMenu->Append(MNU_EXECFILE, _("Execute to file"), _("Execute query, write result to file"));
    queryMenu->Append(MNU_EXPLAIN, _("E&xplain\tF7"), _("Explain query"));

    wxMenu *eo=new wxMenu();
    eo->Append(MNU_VERBOSE, _("Verbose"), _("Explain verbose query"), wxITEM_CHECK);
    eo->Append(MNU_ANALYZE, _("Analyze"), _("Explain analyse query"), wxITEM_CHECK);
    queryMenu->Append(MNU_EXPLAINOPTIONS, _("Explain &options"), eo, _("Options modifying Explain output"));
    queryMenu->AppendSeparator();
    queryMenu->Append(MNU_SAVEHISTORY, _("Save history"), _("Save history of executed commands."));
    queryMenu->Append(MNU_CLEARHISTORY, _("Clear history"), _("Clear history window."));
    queryMenu->AppendSeparator();
    queryMenu->Append(MNU_CANCEL, _("&Cancel\tAlt-Break"), _("Cancel query"));
    menuBar->Append(queryMenu, _("&Query"));

    wxMenu *helpMenu=new wxMenu();
    helpMenu->Append(MNU_CONTENTS, _("&Help..."),                 _("Open the pgAdmin III helpfile."));
    helpMenu->Append(MNU_HELP, _("&SQL Help\tF1"),                _("Display help on SQL commands."));
    menuBar->Append(helpMenu, _("&Help"));


    SetMenuBar(menuBar);

    queryMenu->Check(MNU_VERBOSE, settings->GetExplainVerbose());
    queryMenu->Check(MNU_ANALYZE, settings->GetExplainAnalyze());

    UpdateRecentFiles();

    wxAcceleratorEntry entries[9];

    entries[0].Set(wxACCEL_CTRL,                (int)'E',      MNU_EXECUTE);
    entries[1].Set(wxACCEL_CTRL,                (int)'O',      MNU_OPEN);
    entries[2].Set(wxACCEL_CTRL,                (int)'S',      MNU_SAVE);
    entries[3].Set(wxACCEL_CTRL,                (int)'F',      MNU_FIND);
    entries[4].Set(wxACCEL_CTRL,                (int)'R',      MNU_REPLACE);
    entries[5].Set(wxACCEL_NORMAL,              WXK_F5,        MNU_EXECUTE);
    entries[6].Set(wxACCEL_NORMAL,              WXK_F7,        MNU_EXPLAIN);
    entries[7].Set(wxACCEL_ALT,                 WXK_PAUSE,     MNU_CANCEL);
    entries[8].Set(wxACCEL_NORMAL,              WXK_F1,        MNU_HELP);

    wxAcceleratorTable accel(8, entries);
    SetAcceleratorTable(accel);

    queryMenu->Enable(MNU_CANCEL, false);

    int iWidths[5] = {0, -1, 110, 110, 110};
    statusBar=CreateStatusBar(5);
    SetStatusBarPane(-1);
    SetStatusWidths(5, iWidths);
    SetStatusText(_("ready"), STATUSPOS_MSGS);

    toolBar = CreateToolBar();

    toolBar->SetToolBitmapSize(wxSize(16, 16));

    toolBar->AddTool(MNU_OPEN, _("Open"), wxBitmap(file_open_xpm), _("Open file"), wxITEM_NORMAL);
    toolBar->AddTool(MNU_SAVE, _("Save"), wxBitmap(file_save_xpm), _("Save file"), wxITEM_NORMAL);
    toolBar->AddSeparator();
    toolBar->AddTool(MNU_CUT, _("Cut"), wxBitmap(clip_cut_xpm), _("Cut selected text to clipboard"), wxITEM_NORMAL);
    toolBar->AddTool(MNU_COPY, _("Copy"), wxBitmap(clip_copy_xpm), _("Copy selected text to clipboard"), wxITEM_NORMAL);
    toolBar->AddTool(MNU_PASTE, _("Paste"), wxBitmap(clip_paste_xpm), _("Paste selected text from clipboard"), wxITEM_NORMAL);
    toolBar->AddTool(MNU_CLEAR, _("Clear window"), wxBitmap(edit_clear_xpm), _("Clear edit window"), wxITEM_NORMAL);
    toolBar->AddSeparator();
    toolBar->AddTool(MNU_UNDO, _("Undo"), wxBitmap(edit_undo_xpm), _("Undo last action"), wxITEM_NORMAL);
    toolBar->AddTool(MNU_REDO, _("Redo"), wxBitmap(edit_redo_xpm), _("Redo last action"), wxITEM_NORMAL);
    toolBar->AddSeparator();
    toolBar->AddTool(MNU_FIND, _("Find"), wxBitmap(edit_find_xpm), _("Find text"), wxITEM_NORMAL);
    toolBar->AddSeparator();

    cbConnection = new ctlComboBoxFix(toolBar, CTRLID_CONNECTION, wxDefaultPosition, wxSize(GetCharWidth()*30, -1), wxCB_READONLY|wxCB_DROPDOWN);
    cbConnection->Append(conn->GetName(), (void*)conn);
    cbConnection->Append(_("<new connection>"), (void*)0);
    toolBar->AddControl(cbConnection);
    toolBar->AddTool(MNU_EXECUTE, _("Execute"), wxBitmap(query_execute_xpm), _("Execute query"), wxITEM_NORMAL);
    toolBar->AddTool(MNU_EXECFILE, _("Execute to file"), wxBitmap(query_execfile_xpm), _("Execute query, write result to file"), wxITEM_NORMAL);
    toolBar->AddTool(MNU_EXPLAIN, _("Explain"), wxBitmap(query_explain_xpm), _("Explain query"), wxITEM_NORMAL);
    toolBar->AddTool(MNU_CANCEL, _("Cancel"), wxBitmap(query_cancel_xpm), _("Cancel query"), wxITEM_NORMAL);
    toolBar->AddSeparator();

    toolBar->AddTool(MNU_HELP, _("Help"), wxBitmap(help_xpm), _("Display help on SQL commands."), wxITEM_NORMAL);
    toolBar->Realize();

    horizontal = new wxSplitterWindow(this, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_LIVE_UPDATE | wxCLIP_CHILDREN);
    horizontal->SetMinimumPaneSize(50);

    sqlQuery = new ctlSQLBox(horizontal, CTL_SQLQUERY, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxSIMPLE_BORDER | wxTE_RICH2);
    sqlQuery->SetMarginWidth(1, 16);

    output = new wxNotebook(horizontal, -1, wxDefaultPosition, wxDefaultSize, wxNB_BOTTOM);
    sqlResult = new ctlSQLResult(output, conn, CTL_SQLRESULT, wxDefaultPosition, wxDefaultSize);
    explainCanvas = new ExplainCanvas(output);
    msgResult = new wxTextCtrl(output, CTL_MSGRESULT, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_DONTWRAP);
    msgHistory = new wxTextCtrl(output, CTL_MSGHISTORY, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_DONTWRAP);

    output->AddPage(sqlResult, _("Data Output"));
    output->AddPage(explainCanvas, _("Explain"));
    output->AddPage(msgResult, _("Messages"));
    output->AddPage(msgHistory, _("History"));

    sqlQuery->Connect(wxID_ANY, wxEVT_SET_FOCUS,wxFocusEventHandler(frmQuery::OnFocus));
    sqlResult->Connect(wxID_ANY, wxEVT_SET_FOCUS, wxFocusEventHandler(frmQuery::OnFocus));
    msgResult->Connect(wxID_ANY, wxEVT_SET_FOCUS, wxFocusEventHandler(frmQuery::OnFocus));
    msgHistory->Connect(wxID_ANY, wxEVT_SET_FOCUS, wxFocusEventHandler(frmQuery::OnFocus));

    int splitpos=settings->Read(wxT("frmQuery/Split"), 250);
    if (splitpos < 50)
        splitpos = 50;
    if (splitpos > GetSize().y-50)
        splitpos = GetSize().y-50;
    horizontal->SplitHorizontally(sqlQuery, output, splitpos);

    sqlQuery->SetText(query);
    sqlQuery->Colourise(0, query.Length());

    changed = !query.IsNull() && settings->GetStickySql();
    if (changed)
        setExtendedTitle();
    updateMenu();
    queryMenu->Enable(MNU_SAVEHISTORY, false);
    queryMenu->Enable(MNU_CLEARHISTORY, false);
    setTools(false);
    lastFileFormat = settings->GetUnicodeFile();

    msgResult->SetMaxLength(0L);
    msgHistory->SetMaxLength(0L);
}


frmQuery::~frmQuery()
{
    wxLogInfo(wxT("Destroying SQL Query box"));

    sqlQuery->Disconnect(wxID_ANY, wxEVT_SET_FOCUS,wxFocusEventHandler(frmQuery::OnFocus));
    sqlResult->Disconnect(wxID_ANY, wxEVT_SET_FOCUS, wxFocusEventHandler(frmQuery::OnFocus));
    msgResult->Disconnect(wxID_ANY, wxEVT_SET_FOCUS, wxFocusEventHandler(frmQuery::OnFocus));
    msgHistory->Disconnect(wxID_ANY, wxEVT_SET_FOCUS, wxFocusEventHandler(frmQuery::OnFocus));

    mainForm->RemoveFrame(this);

    settings->Write(wxT("frmQuery/Split"), horizontal->GetSashPosition());
    settings->SetExplainAnalyze(queryMenu->IsChecked(MNU_ANALYZE));
    settings->SetExplainVerbose(queryMenu->IsChecked(MNU_VERBOSE));

    sqlResult->Abort(); // to make sure conn is unused

    while (cbConnection->GetCount() > 1)
    {
        delete (pgConn*)cbConnection->GetClientData(0);
        cbConnection->Delete(0);
    }
}


void frmQuery::OnActivate(wxActivateEvent& event)
{
    if (event.GetActive())
        updateMenu();
	event.Skip();
}


void frmQuery::OnExport(wxCommandEvent &ev)
{
    sqlResult->Export();
}


void frmQuery::Go()
{
    cbConnection->SetSelection(0L);
    wxCommandEvent ev;
    OnChangeConnection(ev);

    Show(TRUE);
    sqlQuery->SetFocus();
}


typedef struct __sqltokenhelp
{
    wxChar *token;
    wxChar *page;
    int type;
} SqlTokenHelp;


SqlTokenHelp sqlTokenHelp[] =
{
    { wxT("ABORT"), 0, 0},
    { wxT("ALTER"), 0, 2},
    { wxT("ANALYZE"), 0, 0},
    { wxT("BEGIN"), 0, 0},
    { wxT("CHECKPOINT"), 0, 0},
    { wxT("CLOSE"), 0, 0},
    { wxT("CLUSTER"), 0, 0},
    { wxT("COMMENT"), 0, 0},
    { wxT("COMMIT"), 0, 0},
    { wxT("COPY"), 0, 0},
    { wxT("CREATE"), 0, 1},
    { wxT("DEALLOCATE"), 0, 0},
    { wxT("DECLARE"), 0, 0},
    { wxT("DELETE"), 0, 0},
    { wxT("DROP"), 0, 1},
    { wxT("END"), 0, 0},
    { wxT("EXECUTE"), 0, 0},
    { wxT("EXPLAIN"), 0, 0},
    { wxT("FETCH"), 0, 0},
    { wxT("GRANT"), 0, 0},
    { wxT("INSERT"), 0, 0},
    { wxT("LISTEN"), 0, 0},
    { wxT("LOAD"), 0, 0},
    { wxT("LOCK"), 0, 0},
    { wxT("MOVE"), 0, 0},
    { wxT("NOTIFY"), 0, 0},
    { wxT("END"), 0, 0},
//    { wxT("PREPARE"), 0, 0},  handled individually
    { wxT("REINDEX"), 0, 0},
    { wxT("RELEASE"), wxT("pg/sql-release-savepoint"), 0},
    { wxT("RESET"), 0, 0},
    { wxT("REVOKE"), 0, 0},
//    { wxT("ROLLBACK"), 0, 0}, handled individually
    { wxT("SAVEPOINT"), 0, 0},
    { wxT("SELECT"), 0, 0},
    { wxT("SET"), 0, 0},
    { wxT("SHOW"), 0, 0},
    { wxT("START"), wxT("pg/sql-start-transaction"), 0},
    { wxT("TRUNCATE"), 0, 0},
    { wxT("UNLISTEN"), 0, 0},
    { wxT("UPDATE"), 0, 0},
    { wxT("VACUUM"), 0, 0},

    { wxT("AGGREGATE"), 0, 11},
    { wxT("CAST"), 0, 11},
    { wxT("CONSTRAINT"), 0, 11},
    { wxT("CONVERSION"), 0, 11},
    { wxT("DATABASE"), 0, 12},
    { wxT("DOMAIN"), 0, 11},
    { wxT("FUNCTION"), 0, 11},
    { wxT("GROUP"), 0, 12},
    { wxT("INDEX"), 0, 11},
    { wxT("LANGUAGE"), 0, 11},
    { wxT("OPERATOR"), 0, 11},
    { wxT("ROLE"), 0, 11},
    { wxT("RULE"), 0, 11},
    { wxT("SCHEMA"), 0, 11},
    { wxT("SEQUENCE"), 0, 11},
    { wxT("TABLE"), 0, 12},
    { wxT("TABLESPACE"), 0, 12},
    { wxT("TRIGGER"), 0, 12},
    { wxT("TYPE"), 0, 11},
    { wxT("USER"), 0, 12},
    { wxT("VIEW"), 0, 11},
    { 0, 0 }
};


void frmQuery::OnContents(wxCommandEvent& event)
{
    DisplayHelp(this, wxT("query"), sql_xpm);
}


void frmQuery::OnChangeConnection(wxCommandEvent &ev)
{
    int sel=cbConnection->GetCurrentSelection();
    if (sel == cbConnection->GetCount()-1)
    {
        // new Connection
        dlgSelectConnection dlg(this, mainForm);
        int rc=dlg.Go(conn, cbConnection);
        if (rc == wxID_OK)
        {
            conn = dlg.GetServer()->CreateConn(dlg.GetDatabase());
            if (conn)
            {
                cbConnection->Insert(conn->GetName(), sel, (void*)conn);
                cbConnection->SetSelection(sel);
				OnChangeConnection(ev);
            }
            else
                rc = wxID_CANCEL;
        }
        if (rc != wxID_OK)
        {
            int i;
            for (i=0 ; i < sel ; i++)
            {
                if (cbConnection->GetClientData(i) == conn)
                {
                    cbConnection->SetSelection(i);
                    break;
                }
            }
        }
    }
    else
    {
        conn = (pgConn*)cbConnection->GetClientData(sel);
        sqlResult->SetConnection(conn);
        title = wxT("pgAdmin III Query - ") + cbConnection->GetValue();
        setExtendedTitle();
    }
}


void frmQuery::OnHelp(wxCommandEvent& event)
{
    wxString page;
    wxString query=sqlQuery->GetSelectedText();
    if (query.IsNull())
        query = sqlQuery->GetText();

    query.Trim(false);

    if (!query.IsEmpty())
    {
	    wxStringTokenizer tokens(query);
	    query=tokens.GetNextToken();

        if (query.IsSameAs(wxT("PREPARE"), false))
        {
            if (tokens.GetNextToken().IsSameAs(wxT("TRANSACTION"), false))
                page = wxT("pg/sql-prepare-transaction");
            else
                page = wxT("pg/sql-prepare");
        }
        else if (query.IsSameAs(wxT("ROLLBACK"), false))
        {
            if (tokens.GetNextToken().IsSameAs(wxT("PREPARED"), false))
                page = wxT("pg/sql-rollback-prepared");
            else
                page = wxT("pg/sql-rollback");
        }
        else
        {
	        SqlTokenHelp *sth=sqlTokenHelp;
	        while (sth->token)
	        {
	            if (sth->type < 10 && query.IsSameAs(sth->token, false))
	            {
		            if (sth->page)
		                page = sth->page;
		            else
		                page = wxT("pg/sql-") + query.Lower();

                    if (sth->type)
		            {
		                int type=sth->type+10;

		                query=tokens.GetNextToken();
		                sth=sqlTokenHelp;
		                while (sth->token)
		                {
			                if (sth->type >= type && query.IsSameAs(sth->token, false))
			                {
			                    if (sth->page)
				                    page += sth->page;
			                    else
				                    page += query.Lower();
			                    break;
			                }
			                sth++;
		                }
		                if (!sth->token)
			                page=wxT("pg/sql-commands");
		            }
		            break;
	            }
	            sth++;
	        }
        }
    }
    if (page.IsEmpty())
    	page=wxT("pg/sql-commands");

    DisplaySqlHelp(this, page);
}


void frmQuery::OnSaveHistory(wxCommandEvent& event)
{
    wxFileDialog *dlg=new wxFileDialog(this, _("Save history"), lastDir, wxEmptyString, 
        _("Log files (*.log)|*.log|All files (*.*)|*.*"), wxSAVE|wxOVERWRITE_PROMPT);
    if (dlg->ShowModal() == wxID_OK)
    {
        FileWrite(dlg->GetPath(), msgHistory->GetValue(), false);
    }
    delete dlg;

}


void frmQuery::OnSetFocus(wxFocusEvent& event)
{
	sqlQuery->SetFocus();
	event.Skip();
}


void frmQuery::OnClearHistory(wxCommandEvent& event)
{
    queryMenu->Enable(MNU_SAVEHISTORY, false);
    queryMenu->Enable(MNU_CLEARHISTORY, false);
    msgHistory->Clear();
}


void frmQuery::OnFocus(wxFocusEvent& ev)
{
    if (wxDynamicCast(this, wxFrame))
        updateMenu(ev.GetEventObject());
    else
    {
        frmQuery *wnd=(frmQuery*)GetParent();
        if (wnd)
            wnd->OnFocus(ev);
    }
    ev.Skip();
}


void frmQuery::OnCut(wxCommandEvent& ev)
{
    if (currentControl() == sqlQuery)
    {
        sqlQuery->Cut();
        updateMenu();
    }
}


wxWindow *frmQuery::currentControl()
{
    wxWindow *wnd=FindFocus();
    if (wnd == output)
    {
        switch (output->GetSelection())
        {
            case 0: wnd = sqlResult;    break;
            case 1: wnd = msgResult;    break;
            case 2: wnd = msgHistory;   break;
        }
    }
    return wnd;
}


void frmQuery::OnCopy(wxCommandEvent& ev)
{
    wxWindow *wnd=currentControl();

    if (wnd == sqlQuery)
        sqlQuery->Copy();
    else if (wnd == msgResult)
        msgResult->Copy();
    else if (wnd == msgHistory)
        msgHistory->Copy();
    else if (wnd == sqlResult && sqlResult->GetSelectedItemCount() > 0)
    {
        wxString str;
        int row=-1;
        while (true)
        {
            row = sqlResult->GetNextItem(row, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
            if (row < 0)
                break;
            
            str.Append(sqlResult->GetExportLine(row));
            if (sqlResult->GetSelectedItemCount() > 1)
                str.Append(END_OF_LINE);
        }

        if (wxTheClipboard->Open())
        {
            wxTheClipboard->SetData(new wxTextDataObject(str));
            wxTheClipboard->Close();
        }

    }
    updateMenu();
}

void frmQuery::OnPaste(wxCommandEvent& ev)
{
	if (currentControl() == sqlQuery)
	  sqlQuery->Paste();
}

void frmQuery::OnClear(wxCommandEvent& ev)
{
    wxWindow *wnd=currentControl();

    if (wnd == sqlQuery)
        sqlQuery->ClearAll();
    else if (wnd == msgResult)
        msgResult->Clear();
    else if (wnd == msgHistory)
        msgHistory->Clear();
}

void frmQuery::OnFind(wxCommandEvent& ev)
{
      sqlQuery->OnFind(ev);
}

void frmQuery::OnReplace(wxCommandEvent& ev)
{
      sqlQuery->OnReplace(ev);
}


void frmQuery::OnUndo(wxCommandEvent& ev)
{
    sqlQuery->Undo();
}

void frmQuery::OnRedo(wxCommandEvent& ev)
{
    sqlQuery->Redo();
}

void frmQuery::setExtendedTitle()
{
    wxString chgStr;
    if (changed)
        chgStr = wxT(" *");

    if (lastPath.IsNull())
        SetTitle(title+chgStr);
    else
    {
        SetTitle(title + wxT(" - [") + lastPath + wxT("]") + chgStr);
    }
}


void frmQuery::updateMenu(wxObject *obj)
{
    bool canCut=false;
    bool canPaste=false;
    bool canUndo=false;
    bool canRedo=false;
    bool canClear=false;
    bool canFind=false;


    if (!obj || obj == sqlQuery)
    {
        canUndo=sqlQuery->CanUndo();
        canRedo=sqlQuery->CanRedo();
        canPaste=sqlQuery->CanPaste();

        canCut = true;
        canClear = true;
        canFind = true;
    }
    else if (obj == msgResult || obj == msgHistory)
    {
        canClear = true;
    }


    toolBar->EnableTool(MNU_UNDO, canUndo);
    editMenu->Enable(MNU_UNDO, canUndo);

    toolBar->EnableTool(MNU_REDO, canRedo);
    editMenu->Enable(MNU_REDO, canRedo);

    toolBar->EnableTool(MNU_PASTE, canPaste);
    editMenu->Enable(MNU_PASTE, canPaste);

    toolBar->EnableTool(MNU_CUT, canCut);
    editMenu->Enable(MNU_CUT, canCut);

    toolBar->EnableTool(MNU_CLEAR, canClear);
    editMenu->Enable(MNU_CLEAR, canClear);

    toolBar->EnableTool(MNU_FIND, canFind);
    editMenu->Enable(MNU_FIND, canFind);
}



bool frmQuery::CheckChanged(bool canVeto)
{
    if (changed && settings->GetAskSaveConfirmation())
    {
        wxString fn;
        if (!lastPath.IsNull())
            fn = wxT(" in file ") + lastPath;
        wxMessageDialog msg(this, wxString::Format(_("The text %s has changed.\nDo you want to save changes?"), fn.c_str()), _("pgAdmin III Query"), 
                    wxYES_NO|wxNO_DEFAULT|wxICON_EXCLAMATION|
                    (canVeto ? wxCANCEL : 0));

    	wxCommandEvent noEvent;
        switch (msg.ShowModal())
        {
            case wxID_YES:
                if (lastPath.IsNull())
                    OnSaveAs(noEvent);
                else
                    OnSave(noEvent);

                return changed;

            case wxID_CANCEL:
                return true;
        }
    }
    return false;
}


void frmQuery::OnClose(wxCloseEvent& event)
{
    if (queryMenu->IsEnabled(MNU_CANCEL))
    {
        if (event.CanVeto())
        {
            wxMessageDialog msg(this, _("A query is running. Do you wish to cancel it?"), _("pgAdmin III Query"), 
                        wxYES_NO|wxNO_DEFAULT|wxICON_EXCLAMATION);

            if (msg.ShowModal() == wxID_NO)
            {
                event.Veto();
                return;
            }
        }

        wxCommandEvent ev;
        OnCancel(ev);
    }

    while (sqlResult->RunStatus() == CTLSQL_RUNNING)
    {
        wxLogInfo(wxT("SQL Query box: Waiting for query to abort"));
        wxSleep(1);
    }

    if (CheckChanged(event.CanVeto()) && event.CanVeto())
    {
        event.Veto();
        return;
    }

    Hide();

    Destroy();
}


void frmQuery::OnChangeStc(wxStyledTextEvent& event)
{
    if (!changed)
    {
        changed=true;
        setExtendedTitle();
        updateMenu();
    }
}

void frmQuery::OnPositionStc(wxStyledTextEvent& event)
{
    wxString pos;
    pos.Printf(_("Ln %d Col %d"), sqlQuery->LineFromPosition(sqlQuery->GetCurrentPos()) + 1, sqlQuery->GetColumn(sqlQuery->GetCurrentPos()) + 1);
    SetStatusText(pos, STATUSPOS_POS);
}


void frmQuery::OpenLastFile()
{
    wxString str=FileRead(lastPath);
    if (!str.IsEmpty())
    {
        sqlQuery->SetText(str);
        sqlQuery->Colourise(0, str.Length());
        wxSafeYield();  // needed to process sqlQuery modify event
        changed = false;
        setExtendedTitle();
        UpdateRecentFiles();
    }
}
        
void frmQuery::OnOpen(wxCommandEvent& event)
{
    if (CheckChanged(false))
        return;

    wxFileDialog dlg(this, _("Open query file"), lastDir, wxT(""), 
        _("Query files (*.sql)|*.sql|All files (*.*)|*.*"), wxOPEN);
    if (dlg.ShowModal() == wxID_OK)
    {
        lastFilename=dlg.GetFilename();
        lastDir = dlg.GetDirectory();
        lastPath = dlg.GetPath();
        OpenLastFile();
    }
}

void frmQuery::OnSave(wxCommandEvent& event)
{
    if (lastPath.IsNull())
    {
        OnSaveAs(event);
        return;
    }

    if (FileWrite(lastPath, sqlQuery->GetText(), -1))
    {
        changed=false;
        setExtendedTitle();
        UpdateRecentFiles();
    }
}


void frmQuery::OnSaveAs(wxCommandEvent& event)
{
    wxFileDialog *dlg=new wxFileDialog(this, _("Save query file as"), lastDir, lastFilename, 
        _("Query files (*.sql)|*.sql|UTF-8 query files (*.sql)|*.sql|All files (*.*)|*.*"), wxSAVE|wxOVERWRITE_PROMPT);
    if (dlg->ShowModal() == wxID_OK)
    {
        lastFilename=dlg->GetFilename();
        lastDir = dlg->GetDirectory();
        lastPath = dlg->GetPath();
        switch (dlg->GetFilterIndex())
        {
            case 0: 
                lastFileFormat = false;
#ifdef __WXMAC__
		if (!lastPath.Contains(wxT(".")))
			lastPath += wxT(".sql");
#endif
                break;
            case 1:
                lastFileFormat = true;
#ifdef __WXMAC__
                if (!lastPath.Contains(wxT(".")))
                        lastPath += wxT(".sql");
#endif
                break;
            default:
                lastFileFormat = settings->GetUnicodeFile();
                break;
        }

        if (FileWrite(lastPath, sqlQuery->GetText(), lastFileFormat ? 1 : 0))
        {
            changed=false;
            setExtendedTitle();
            UpdateRecentFiles();
            fileMenu->Enable(MNU_RECENT, (recentFileMenu->GetMenuItemCount() > 0));
        }
    }
    delete dlg;
}


void frmQuery::OnCancel(wxCommandEvent& event)
{
    toolBar->EnableTool(MNU_CANCEL, false);
    queryMenu->Enable(MNU_CANCEL, false);
    SetStatusText(_("Cancelling."), STATUSPOS_MSGS);

    sqlResult->Abort();
    aborted=true;
}


void frmQuery::OnExplain(wxCommandEvent& event)
{
    wxString query=sqlQuery->GetSelectedText();
    if (query.IsNull())
        query = sqlQuery->GetText();

    if (query.IsNull())
        return;
    wxString sql;
    int resultToRetrieve=1;
    bool verbose=queryMenu->IsChecked(MNU_VERBOSE), analyze=queryMenu->IsChecked(MNU_ANALYZE);

    if (analyze)
    {
        sql += wxT("\nBEGIN;\n");
        resultToRetrieve++;
    }
    sql += wxT("EXPLAIN ");
    if (analyze)
        sql += wxT("ANALYZE ");
    if (verbose)
        sql += wxT("VERBOSE ");
    
    int offset=sql.Length();

    sql += query;

    if (analyze)
        sql += wxT(";\nROLLBACK;");

    if (execQuery(sql, resultToRetrieve, true, offset))
    {
        if (!verbose)
        {
            int i;
            wxString str;
            for (i=0 ; i < sqlResult->NumRows() ; i++)
            {
                if (i)
                    str.Append(wxT("\n"));
                str.Append(sqlResult->GetItemText(i));
            }
            explainCanvas->SetExplainString(str);
            output->SetSelection(EXPLAIN_PAGE);
        }
    }

    sqlQuery->SetFocus();
}


void frmQuery::OnExecute(wxCommandEvent& event)
{
    wxString query=sqlQuery->GetSelectedText();
    if (query.IsNull())
        query = sqlQuery->GetText();

    if (query.IsNull())
        return;
    execQuery(query);
    sqlQuery->SetFocus();
}


void frmQuery::OnExecFile(wxCommandEvent &event)
{
    wxString query=sqlQuery->GetSelectedText();
    if (query.IsNull())
        query = sqlQuery->GetText();

    if (query.IsNull())
        return;
    execQuery(query, 0, false, 0, true);
    sqlQuery->SetFocus();
}


void frmQuery::setTools(const bool running)
{
    toolBar->EnableTool(MNU_EXECUTE, !running);
    toolBar->EnableTool(MNU_EXECFILE, !running);
    toolBar->EnableTool(MNU_EXPLAIN, !running);
    toolBar->EnableTool(MNU_CANCEL, running);
    queryMenu->Enable(MNU_EXECUTE, !running);
    queryMenu->Enable(MNU_EXECFILE, !running);
    queryMenu->Enable(MNU_EXPLAIN, !running);
    queryMenu->Enable(MNU_CANCEL, running);
    fileMenu->Enable(MNU_EXPORT, sqlResult->CanExport());
    fileMenu->Enable(MNU_RECENT, (recentFileMenu->GetMenuItemCount() > 0));
}


void frmQuery::showMessage(const wxString& msg, const wxString &msgShort)
{
    msgResult->AppendText(msg + wxT("\n"));
    msgHistory->AppendText(msg + wxT("\n"));
    wxString str;
    if (msgShort.IsNull())
        str=msg;
    else
        str=msgShort;
    str.Replace(wxT("\n"), wxT(" "));
    SetStatusText(str, STATUSPOS_MSGS);
}

bool frmQuery::execQuery(const wxString &query, int resultToRetrieve, bool singleResult, const int queryOffset, bool toFile)
{
    bool done=false;

    long rowsReadTotal=0;
    setTools(true);
    queryMenu->Enable(MNU_SAVEHISTORY, true);
    queryMenu->Enable(MNU_CLEARHISTORY, true);

    explainCanvas->Clear();

    bool wasChanged = changed;
    sqlQuery->MarkerDeleteAll(0);
    if (!wasChanged)
    {
        changed=false;
        setExtendedTitle();
    }

    aborted=false;
    
    if (sqlResult->Execute(query, resultToRetrieve) >= 0)
    {
        SetStatusText(wxT(""), STATUSPOS_SECS);
        SetStatusText(_("Query is running."), STATUSPOS_MSGS);
        SetStatusText(wxT(""), STATUSPOS_ROWS);
        msgResult->Clear();

        msgHistory->AppendText(_("-- Executing query:\n"));
        msgHistory->AppendText(query);
        msgHistory->AppendText(wxT("\n"));
        Update();
        wxYield();

        wxString str;
        wxLongLong elapsedQuery;
        wxLongLong startTimeQuery=wxGetLocalTimeMillis();
        while (sqlResult->RunStatus() == CTLSQL_RUNNING)
        {
            elapsedQuery=wxGetLocalTimeMillis() - startTimeQuery;
            SetStatusText(elapsedQuery.ToString() + wxT(" ms"), STATUSPOS_SECS);
			wxYield();
            if (elapsedQuery < 200)
                wxMilliSleep(10);
            else
                wxMilliSleep(100);

            str=sqlResult->GetMessagesAndClear();
            if (!str.IsEmpty())
            {
                msgResult->AppendText(str);
                msgHistory->AppendText(str);
            }
            wxYield();
        }

        str=sqlResult->GetMessagesAndClear();
        msgResult->AppendText(str);
        msgHistory->AppendText(str);

        elapsedQuery=wxGetLocalTimeMillis() - startTimeQuery;
        SetStatusText(elapsedQuery.ToString() + wxT(" ms"), STATUSPOS_SECS);

        if (sqlResult->RunStatus() != PGRES_TUPLES_OK)
        {
            output->SetSelection(MSG_PAGE);
            if (sqlResult->RunStatus() == PGRES_COMMAND_OK)
            {
                done = true;

                int insertedCount = sqlResult->InsertedCount();
                OID insertedOid = sqlResult->InsertedOid();
                if (insertedCount < 0)
                {
                    showMessage(wxString::Format(_("Query returned successfully with no result in %s ms."),
                        elapsedQuery.ToString().c_str()), _("OK."));
                }
                else if (insertedCount == 1 && insertedOid)
                {
                    showMessage(wxString::Format(_("Query returned successfully: one row with OID %d inserted, %s ms execution time."),
                        insertedOid, elapsedQuery.ToString().c_str()), 
                        wxString::Format(_("One Row with OID %d inserted."), insertedOid));
                }
                else
                {
                    showMessage(wxString::Format(_("Query returned successfully: %d rows affected, %s ms execution time."),
                        insertedCount, elapsedQuery.ToString().c_str()), 
                        wxString::Format(_("%d rows affected."), insertedCount));
                }
            }
            else
            {
                wxString errMsg = sqlResult->GetErrorMessage();
                showMessage(errMsg);

                wxString atChar=wxT(" at character ");
                int chp=errMsg.Find(atChar);

                if (chp > 0)
                {
                    int selStart=sqlQuery->GetSelectionStart(), selEnd=sqlQuery->GetSelectionEnd();
                    if (selStart == selEnd)
                        selStart=0;

                    long errPos=0;
                    errMsg.Mid(chp+atChar.Length()).ToLong(&errPos);
                    errPos += queryOffset;  // do not count EXPLAIN or similar
                    int line=0, maxLine = sqlQuery->GetLineCount();
                    while (line < maxLine && sqlQuery->GetLineEndPosition(line) < errPos + selStart+1)
                        line++;
                    if (line < maxLine)
                    {
                        wasChanged=changed;
                        sqlQuery->MarkerAdd(line, 0);
                        if (!wasChanged)
                        {
                            changed=false;
                            setExtendedTitle();
                        }
                        sqlQuery->EnsureVisible(line);
                    }
                }
            }
        }
        else
        {
            done = true;
            output->SetSelection(RESULT_PAGE);
            long rowsTotal=sqlResult->NumRows();

            if (toFile)
            {
                SetStatusText(wxString::Format(_("%d rows."), rowsTotal), STATUSPOS_ROWS);

                if (rowsTotal)
                {
                    SetStatusText(_("Writing data."), STATUSPOS_MSGS);

                    toolBar->EnableTool(MNU_CANCEL, false);
                    queryMenu->Enable(MNU_CANCEL, false);
                    SetCursor(*wxHOURGLASS_CURSOR);

                    if (sqlResult->Export())
                        SetStatusText(_("Data written to file."), STATUSPOS_MSGS);
                    else
                        SetStatusText(_("Data export aborted."), STATUSPOS_MSGS);
                    SetCursor(wxNullCursor);
                }
                else
                    SetStatusText(_("No data to export."), STATUSPOS_MSGS);
            }
            else
            {
                if (singleResult)
                {
                    rowsReadTotal=sqlResult->RetrieveOne();
                    showMessage(wxString::Format(_("%d rows retrieved."), rowsReadTotal), _("OK."));
                }
                else
                {
                    SetStatusText(wxString::Format(_("Retrieving data: %d rows."), rowsTotal), STATUSPOS_MSGS);
                    wxYield();

                    long maxRows=settings->GetMaxRows();

                    if (!maxRows)
                        maxRows = rowsTotal;
                    if (rowsTotal > maxRows)

                    {
                        wxMessageDialog msg(this, wxString::Format(
                                _("The maximum of %ld rows is exceeded (total %ld)."), maxRows, rowsTotal) +
                                _("\nRetrieve all rows anyway?"), _("Limit exceeded"), 
                                    wxYES_NO|wxCANCEL|wxNO_DEFAULT|wxICON_EXCLAMATION);
                        switch (msg.ShowModal())
                        {
                            case wxID_YES:
                                maxRows = rowsTotal;
                                break;
                            case wxID_CANCEL:
                                maxRows = 0;
                                break;
                        }
                    }
                    wxLongLong startTimeRetrieve=wxGetLocalTimeMillis();
                    wxLongLong elapsed;
                    elapsedRetrieve=0;
                    bool resultFreezed=false;
                
                    while (!aborted && rowsReadTotal < maxRows)
                    {
                        // Rows will be retrieved in chunks, the first being smaller to have an early screen update
                        // later, screen update is disabled to speed up retrieval
                        long chunk;
                        if (!rowsReadTotal) chunk=20;
                        else                chunk=100;

                        if (chunk > maxRows-rowsReadTotal)
                            chunk = maxRows-rowsReadTotal;

                        long rowsRead=sqlResult->Retrieve(chunk);
                        if (!rowsRead)
                            break;

                        elapsed = wxGetLocalTimeMillis() - startTimeRetrieve;

                        if (!rowsReadTotal)
		                {
                            wxYield();
			                if (rowsRead < maxRows)
    				        {
        					    resultFreezed=true;
	        				    sqlResult->Freeze();
			    	        }
		                }
                        rowsReadTotal += rowsRead;

                        if (elapsed > elapsedRetrieve +100)
                        {
                            elapsedRetrieve=elapsed;
                            SetStatusText(elapsedQuery.ToString() + wxT("+") + elapsedRetrieve.ToString() + wxT(" ms"), STATUSPOS_SECS);
                            wxYield();
                        }
                    }
                    if (resultFreezed)
                        sqlResult->Thaw();

                    elapsedRetrieve=wxGetLocalTimeMillis() - startTimeRetrieve;
                    SetStatusText(elapsedQuery.ToString() + wxT("+") + elapsedRetrieve.ToString() + wxT(" ms"), STATUSPOS_SECS);

                    str= _("Total query runtime: ") + elapsedQuery.ToString() + wxT(" ms.\n") +
                         _("Data retrieval runtime: ") + elapsedRetrieve.ToString() + wxT(" ms.\n");
                    msgResult->AppendText(str);
                    msgHistory->AppendText(str);

                    if (rowsReadTotal == sqlResult->NumRows())
                        showMessage(wxString::Format(_("%ld rows retrieved."), rowsReadTotal), _("OK."));
                    else
                    {
                        wxString nr;
                        nr.Printf(_("%ld  rows not retrieved."), rowsTotal - rowsReadTotal);
                        showMessage(wxString::Format(_("Total %ld rows.\n"), rowsTotal) + nr, nr);
                    }
                }
                if (rowsTotal == rowsReadTotal)
                    SetStatusText(wxString::Format(_("%d rows."), rowsTotal), STATUSPOS_ROWS);
                else
                    SetStatusText(wxString::Format(_("%ld of %ld rows"), rowsReadTotal, rowsTotal), STATUSPOS_ROWS);
            }
        }
    }

    // Display async notifications
    pgNotification *notify;
    int notifies = 0;
    notify = conn->GetNotification();
    while (notify)
    {
        wxString notifyStr;
        notifies++;

        if (notify->data.IsEmpty())
            notifyStr.Printf(_("\nAsynchronous notification of '%s' received from backend pid %d"), notify->name.c_str(), notify->pid);
        else
            notifyStr.Printf(_("\nAsynchronous notification of '%s' received from backend pid %d\n   Data: %s"), notify->name.c_str(), notify->pid, notify->data.c_str());

        msgResult->AppendText(notifyStr);
        msgHistory->AppendText(notifyStr);

        notify = conn->GetNotification();
    }

    if (notifies)
    {
        wxString statusMsg = statusBar->GetStatusText(STATUSPOS_MSGS);
        if (statusMsg.Last() == '.')
            statusMsg = statusMsg.Left(statusMsg.Length() - 1);

        SetStatusText(wxString::Format(_("%s (%d asynchronous notifications received)."), statusMsg.c_str(), notifies), STATUSPOS_MSGS);
    }

    msgResult->AppendText(wxT("\n"));
    msgHistory->AppendText(wxT("\n"));


    setTools(false);
    if (rowsReadTotal)
    {
        fileMenu->Enable(MNU_EXPORT, sqlResult->CanExport());
    }

    if (!this->IsActive())
        this->RequestUserAttention();
    return done;
}


queryToolFactory::queryToolFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar) : actionFactory(list)
{
    mnu->Append(id, _("&Query tool"), _("Execute arbitrary SQL queries."));
    toolbar->AddTool(id, _("Query tool"), wxBitmap(sql_xpm), _("Execute arbitrary SQL queries."), wxITEM_NORMAL);
}


wxWindow *queryToolFactory::StartDialog(frmMain *form, pgObject *obj)
{
    pgDatabase *db=obj->GetDatabase();
    pgConn *conn = db->CreateConn();
    if (conn)
    {
        wxString qry;
        if (settings->GetStickySql()) 
            qry = form->GetSqlPane()->GetText();
        frmQuery *fq= new frmQuery(form, wxEmptyString, conn, qry);
        fq->Go();
        return fq;
    }
    return 0;
}


bool queryToolFactory::CheckEnable(pgObject *obj)
{
    return obj && obj->GetDatabase() && obj->GetDatabase()->GetConnected();
}
