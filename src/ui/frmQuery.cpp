//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmQuery.cpp - SQL Query Box
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/file.h>

// App headers
#include "pgAdmin3.h"
#include "frmMain.h"
#include "frmQuery.h"
#include "frmSqlHelp.h"

// Icons
#include "images/sql.xpm"
// Bitmaps
#include "images/file_open.xpm"
#include "images/file_save.xpm"
#include "images/clip_cut.xpm"
#include "images/clip_copy.xpm"
#include "images/clip_paste.xpm"
#include "images/edit_clear.xpm"
//#include "images/edit_find.xpm"
#include "images/edit_undo.xpm"
#include "images/edit_redo.xpm"
#include "images/query_execute.xpm"
#include "images/query_explain.xpm"
#include "images/query_cancel.xpm"



BEGIN_EVENT_TABLE(frmQuery, wxFrame)
    EVT_CLOSE(                      frmQuery::OnClose)
    EVT_MENU(MNU_OPEN,              frmQuery::OnOpen)
    EVT_MENU(MNU_SAVE,              frmQuery::OnSave)
    EVT_MENU(MNU_SAVEAS,            frmQuery::OnSaveAs)
    EVT_MENU(MNU_RECENT+1,          frmQuery::OnRecent)
    EVT_MENU(MNU_RECENT+2,          frmQuery::OnRecent)
    EVT_MENU(MNU_RECENT+3,          frmQuery::OnRecent)
    EVT_MENU(MNU_RECENT+4,          frmQuery::OnRecent)
    EVT_MENU(MNU_RECENT+5,          frmQuery::OnRecent)
    EVT_MENU(MNU_RECENT+6,          frmQuery::OnRecent)
    EVT_MENU(MNU_RECENT+7,          frmQuery::OnRecent)
    EVT_MENU(MNU_RECENT+8,          frmQuery::OnRecent)
    EVT_MENU(MNU_RECENT+9,          frmQuery::OnRecent)
    EVT_MENU(MNU_EXIT,              frmQuery::OnExit)
    EVT_MENU(MNU_CUT,               frmQuery::OnCut)
    EVT_MENU(MNU_COPY,              frmQuery::OnCopy)
    EVT_MENU(MNU_PASTE,             frmQuery::OnPaste)
    EVT_MENU(MNU_CLEAR,             frmQuery::OnClear)
    EVT_MENU(MNU_FIND,              frmQuery::OnFind)
    EVT_MENU(MNU_UNDO,              frmQuery::OnUndo)
    EVT_MENU(MNU_REDO,              frmQuery::OnRedo)
    EVT_MENU(MNU_EXECUTE,           frmQuery::OnExecute)
    EVT_MENU(MNU_EXPLAIN,           frmQuery::OnExplain)
    EVT_MENU(MNU_CANCEL,            frmQuery::OnCancel)
    EVT_MENU(MNU_HELP,              frmQuery::OnHelp)
#ifdef __wxGTK__
    EVT_KEY_DOWN(                   frmQuery::OnKeyDown)
#endif
    EVT_STC_MODIFIED(CTL_SQLQUERY,    frmQuery::OnChange)
END_EVENT_TABLE()




frmQuery::frmQuery(frmMain *form, const wxString& _title, pgConn *_conn, const wxPoint& pos, const wxSize& size, const wxString& query)
: wxFrame(NULL, -1, _title, pos, size)
{
    mainForm=form;
    title = _title;
    conn=_conn;

    SetIcon(wxIcon(sql_xpm));
    wxLogInfo(wxT("Creating SQL Query box"));
    menuBar = new wxMenuBar();

    fileMenu = new wxMenu();
    recentFileMenu = new wxMenu();
    fileMenu->Append(MNU_OPEN, wxT("&Open..."), wxT("Open a query file"));
    fileMenu->Append(MNU_SAVE, wxT("&Save"), wxT("Save current file"));
    fileMenu->Append(MNU_SAVEAS, wxT("Save &as..."), wxT("Save file under new name"));
    fileMenu->Append(MNU_RECENT, wxT("&Recent files"), recentFileMenu);
    fileMenu->Append(MNU_EXIT, wxT("Exit"), wxT("Exit query window"));

    menuBar->Append(fileMenu, wxT("&File"));

    editMenu = new wxMenu();
    editMenu->Append(MNU_CUT, wxT("Cu&t"), wxT("Cut selected text to clipboard"), wxITEM_NORMAL);
    editMenu->Append(MNU_COPY, wxT("&Copy"), wxT("Copy selected text to clipboard"), wxITEM_NORMAL);
    editMenu->Append(MNU_PASTE, wxT("&Paste"), wxT("Paste selected text from clipboard"), wxITEM_NORMAL);
    editMenu->Append(MNU_CLEAR, wxT("C&lear window"), wxT("Clear edit window"), wxITEM_NORMAL);
    editMenu->AppendSeparator();
    editMenu->Append(MNU_UNDO, wxT("&Undo"), wxT("Undo last action"), wxITEM_NORMAL);
    editMenu->Append(MNU_REDO, wxT("&Redo"), wxT("Redo last action"), wxITEM_NORMAL);

    queryMenu = new wxMenu();
    queryMenu->Append(MNU_EXECUTE, wxT("&Execute"), wxT("Execute query"));
    queryMenu->Append(MNU_EXPLAIN, wxT("E&xplain"), wxT("Explain query"));
    queryMenu->Append(MNU_CANCEL, wxT("&Cancel"), wxT("Cancel query"));
    menuBar->Append(queryMenu, wxT("&Query"));
    SetMenuBar(menuBar);

    updateRecentFiles();

    wxAcceleratorEntry entries[7];

    entries[0].Set(wxACCEL_ALT,                 (int)'E',      MNU_EXECUTE);
    entries[1].Set(wxACCEL_ALT,                 (int)'X',      MNU_EXPLAIN);
    entries[2].Set(wxACCEL_CTRL,                (int)'O',      MNU_OPEN);
    entries[3].Set(wxACCEL_CTRL,                (int)'S',      MNU_SAVE);
    entries[4].Set(wxACCEL_NORMAL,              WXK_F5,        MNU_EXECUTE);
    entries[5].Set(wxACCEL_ALT,                 WXK_PAUSE,     MNU_CANCEL);
    entries[6].Set(wxACCEL_NORMAL,              WXK_F1,        MNU_HELP);

    wxAcceleratorTable accel(7, entries);
    SetAcceleratorTable(accel);

    queryMenu->Enable(MNU_CANCEL, false);

    int iWidths[4] = {0, -1, 110, 110};
    CreateStatusBar(4);
    SetStatusWidths(4, iWidths);
    SetStatusText(wxT("ready"), STATUSPOS_MSGS);

    CreateToolBar();
    statusBar = GetStatusBar();
    toolBar = GetToolBar();

    toolBar->SetToolBitmapSize(wxSize(16, 16));

    toolBar->AddTool(MNU_OPEN, wxT("Open"), wxBitmap(file_open_xpm), wxT("Open file"), wxITEM_NORMAL);
    toolBar->AddTool(MNU_SAVE, wxT("Save"), wxBitmap(file_save_xpm), wxT("Save file"), wxITEM_NORMAL);
    toolBar->AddSeparator();
    toolBar->AddTool(MNU_CUT, wxT("Cut"), wxBitmap(clip_cut_xpm), wxT("Cut selected text to clipboard"), wxITEM_NORMAL);
    toolBar->AddTool(MNU_COPY, wxT("Copy"), wxBitmap(clip_copy_xpm), wxT("Copy selected text to clipboard"), wxITEM_NORMAL);
    toolBar->AddTool(MNU_PASTE, wxT("Paste"), wxBitmap(clip_paste_xpm), wxT("Paste selected text from clipboard"), wxITEM_NORMAL);
    toolBar->AddTool(MNU_CLEAR, wxT("Clear window"), wxBitmap(edit_clear_xpm), wxT("Clear edit window"), wxITEM_NORMAL);
    toolBar->AddSeparator();
    toolBar->AddTool(MNU_UNDO, wxT("Undo"), wxBitmap(edit_undo_xpm), wxT("Undo last action"), wxITEM_NORMAL);
    toolBar->AddTool(MNU_REDO, wxT("Redo"), wxBitmap(edit_redo_xpm), wxT("Redo last action"), wxITEM_NORMAL);
    toolBar->AddSeparator();
    toolBar->AddTool(MNU_EXECUTE, wxT("Execute"), wxBitmap(query_execute_xpm), wxT("Execute query"), wxITEM_NORMAL);
    toolBar->AddTool(MNU_EXPLAIN, wxT("Explain"), wxBitmap(query_explain_xpm), wxT("Explain query"), wxITEM_NORMAL);
    toolBar->AddTool(MNU_CANCEL, wxT("Cancel"), wxBitmap(query_cancel_xpm), wxT("Cancel query"), wxITEM_NORMAL);

    toolBar->Realize();
    setTools(false);

    horizontal = new wxSplitterWindow(this, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_LIVE_UPDATE | wxCLIP_CHILDREN);
    horizontal->SetMinimumPaneSize(50);

    sqlQuery = new ctlSQLBox(horizontal, CTL_SQLQUERY, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxSIMPLE_BORDER | wxTE_RICH2);

    output = new wxNotebook(horizontal, -1, wxDefaultPosition, wxDefaultSize, wxNB_BOTTOM);
    sqlResult = new ctlSQLResult(output, conn, CTL_SQLRESULT, wxDefaultPosition, wxDefaultSize);
    msgResult = new wxTextCtrl(output, CTL_MSGRESULT, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_DONTWRAP);

    output->AddPage(sqlResult, wxT("Data Output"));
    output->AddPage(msgResult, wxT("Messages"));

    int splitpos=settings->Read(wxT("frmQuery/Split"), 250);
    if (splitpos < 50)
        splitpos = 50;
    if (splitpos > GetSize().y-50)
        splitpos = GetSize().y-50;
    horizontal->SplitHorizontally(sqlQuery, output, splitpos);


    sqlQuery->SetText(query);
    changed = !query.IsNull();
    if (changed)
        setExtendedTitle();
    updateMenu();
}


frmQuery::~frmQuery()
{
    wxLogInfo(wxT("Destroying SQL Query box"));
    mainForm->RemoveFrame(this);
    settings->Write(wxT("frmQuery/Width"), GetSize().x);
    settings->Write(wxT("frmQuery/Height"), GetSize().y);
    settings->Write(wxT("frmQuery/Left"), GetPosition().x);
    settings->Write(wxT("frmQuery/Top"), GetPosition().y);
    settings->Write(wxT("frmQuery/Split"), horizontal->GetSashPosition());
    if (conn)
        delete conn;
}



void frmQuery::updateRecentFiles()
{
    wxString lastFiles[10]; // 0 will be unused for convenience
    int i, maxFiles=9;
    int recentIndex=maxFiles;

    for (i=1 ; i <= maxFiles ; i++)
    {
        lastFiles[i] = settings->Read(wxT("RecentFiles/") + wxString((char)('0'+i)), wxT(""));
        if (!lastPath.IsNull() && lastPath.IsSameAs(lastFiles[i], wxARE_FILENAMES_CASE_SENSITIVE))
            recentIndex=i;
    }
    while (i <= maxFiles)
        lastFiles[i++] = wxT("");

    if (recentIndex > 1 && !lastPath.IsNull())
    {
        for (i=recentIndex ; i > 1 ; i--)
            lastFiles[i] = lastFiles[i-1];
        lastFiles[1] = lastPath;
    }

    i=recentFileMenu->GetMenuItemCount();
    while (i)
    {
        wxMenuItem *item = recentFileMenu->Remove(MNU_RECENT+i);
        if (item)
            delete item;
        i--;
    }

    for (i=1 ; i <= maxFiles ; i++)
    {
        settings->Write(wxT("RecentFiles/") + wxString((char)('0'+i)), lastFiles[i]);


        if (!lastFiles[i].IsNull())
            recentFileMenu->Append(MNU_RECENT+i, wxT("&") + wxString((char)('0'+i)) + wxT("  ") + lastFiles[i]);
    }
}

void frmQuery::OnRecent(wxCommandEvent& event)
{
    int fileNo=event.GetId() - MNU_RECENT;
    lastPath = settings->Read(wxT("RecentFiles/") + wxString((char)('0'+fileNo)), wxT(""));

    if (!lastPath.IsNull())
    {
        int dirsep;
        dirsep = lastPath.Find(wxFILE_SEP_PATH, true);
        lastDir = lastPath.Mid(0, dirsep);
        lastFilename = lastPath.Mid(dirsep+1);
        openLastFile();
    }
}



void frmQuery::OnKeyDown(wxKeyEvent& event)
{
    event.m_metaDown=false;
    event.Skip();
}


void frmQuery::Go()
{
    Show(TRUE);
    sqlQuery->SetFocus();
}


void frmQuery::OnExit(wxCommandEvent& event)
{
    Close();
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
    { wxT("PREPARE"), 0, 0},
    { wxT("REINDEX"), 0, 0},
    { wxT("RESET"), 0, 0},
    { wxT("REVOKE"), 0, 0},
    { wxT("ROLLBACK"), 0, 0},
    { wxT("SELECT"), 0, 0},
    { wxT("SET"), 0, 0},
    { wxT("SHOW"), 0, 0},
    { wxT("START"), wxT("sql-start-transaction"), 0},
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
    { wxT("RULE"), 0, 11},
    { wxT("SCHEMA"), 0, 11},
    { wxT("SEQUENCE"), 0, 11},
    { wxT("TABLE"), 0, 12},
    { wxT("TRIGGER"), 0, 12},
    { wxT("TYPE"), 0, 11},
    { wxT("USER"), 0, 12},
    { wxT("VIEW"), 0, 11},
    { 0, 0 }
};



void frmQuery::OnHelp(wxCommandEvent& event)
{
    wxString helpSite=settings->GetHelpSite();
    wxString page;
    wxString query=sqlQuery->GetSelectedText();
    if (query.IsNull())
        query = sqlQuery->GetText();
    if (!query.IsEmpty())
    {
	wxStringTokenizer tokens(query);
	query=tokens.GetNextToken();

	SqlTokenHelp *sth=sqlTokenHelp;
	while (sth->token)
	{
	    if (sth->type < 10 && query.IsSameAs(sth->token, false))
	    {
		if (sth->page)
		    page = sth->page;
		else
		    page = wxT("sql-") + query.Lower();
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
			page=wxT("sql-commands");
		}
		page += wxT(".html");
		break;
	    }
	    sth++;
	}
    }

    if (page.IsEmpty())
	page=wxT("sql-commands.html");

    frmSqlHelp *h=new frmSqlHelp(mainForm);
    h->Show(true);
    if (!h->Load(helpSite + page))
        h->Destroy();
}

void frmQuery::OnCut(wxCommandEvent& ev)
{
    sqlQuery->Cut();
}

void frmQuery::OnCopy(wxCommandEvent& ev)
{
    sqlQuery->Copy();
}

void frmQuery::OnPaste(wxCommandEvent& ev)
{
    sqlQuery->Paste();
}

void frmQuery::OnClear(wxCommandEvent& ev)
{
    sqlQuery->ClearAll();
}

void frmQuery::OnFind(wxCommandEvent& ev)
{
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


void frmQuery::updateMenu()
{
    bool canUndo=sqlQuery->CanUndo();
    toolBar->EnableTool(MNU_UNDO, canUndo);
    editMenu->Enable(MNU_UNDO, canUndo);

    bool canRedo=sqlQuery->CanRedo();
    toolBar->EnableTool(MNU_REDO, canRedo);
    editMenu->Enable(MNU_REDO, canRedo);

    bool canPaste=sqlQuery->CanPaste();
    toolBar->EnableTool(MNU_PASTE, canPaste);
    editMenu->Enable(MNU_PASTE, canPaste);
}


void frmQuery::OnClose(wxCloseEvent& event)
{
    if (changed && settings->GetAskSaveConfirmation())
    {
        wxString fn;
        if (!lastPath.IsNull())
            fn = wxT(" in file ") + lastPath;
        wxMessageDialog msg(this, wxT("The text") + fn + wxT(" has changed.\nDo you want to save changes?"), wxT("pgAdmin III Query"), 
                    wxYES_NO|wxNO_DEFAULT|wxICON_EXCLAMATION|
                    (event.CanVeto() ? wxCANCEL : 0));

    	wxCommandEvent noEvent;
        switch (msg.ShowModal())
        {
            case wxID_YES:
                if (lastPath.IsNull())
                {
                    OnSaveAs(noEvent);
                    if (changed && event.CanVeto())
                        event.Veto();
                }
                else
                    OnSave(noEvent);
                break;
            case wxID_CANCEL:
                event.Veto();
                return;
        }
    }
    Destroy();
}


void frmQuery::OnChange(wxNotifyEvent& event)
{
    if (!changed)
    {
        changed=true;
        setExtendedTitle();
        updateMenu();
    }
}


void frmQuery::openLastFile()
{
    FILE *f=fopen(lastPath.c_str(), "rt");
    if (f)
    {
        fseek(f, 0, SEEK_END);
        int len=ftell(f);
        fseek(f, 0, SEEK_SET);
        wxString buf("\0", len+1);
        memset((char*)buf.c_str(), 0, len+1);
        fread((char*)buf.c_str(), len, 1, f);
        fclose(f);
        sqlQuery->SetText(buf);
        wxYield();  // needed to process sqlQuery modify event
        changed = false;
        setExtendedTitle();
        updateRecentFiles();
    }
}
        
void frmQuery::OnOpen(wxCommandEvent& event)
{
    wxFileDialog dlg(this, wxT("Open query file"), lastDir, wxT(""), 
        wxT("Query files (*.sql)|*.sql|All files (*.*)|*.*"), wxOPEN|wxHIDE_READONLY);
    if (dlg.ShowModal() == wxID_OK)
    {
        lastFilename=dlg.GetFilename();
        lastDir = dlg.GetDirectory();
        lastPath = dlg.GetPath();

        openLastFile();
    }
}

void frmQuery::OnSave(wxCommandEvent& event)
{
    if (lastPath.IsNull())
    {
        OnSaveAs(event);
        return;
    }

    FILE *f=fopen(lastPath.c_str(), "w+t");
    if (f)
    {
        setExtendedTitle();

        wxString buf=sqlQuery->GetText();
        fwrite(buf.c_str(), buf.Length(), 1, f);
        fclose(f);
        changed=false;
        setExtendedTitle();
        updateRecentFiles();
    }
}

void frmQuery::OnSaveAs(wxCommandEvent& event)
{
    wxFileDialog *dlg=new wxFileDialog(this, wxT("Save query file as"), lastDir, lastFilename, 
        wxT("Query files (*.sql)|*.sql|All files (*.*)|*.*"), wxSAVE|wxOVERWRITE_PROMPT);
    if (dlg->ShowModal() == wxID_OK)
    {
        lastFilename=dlg->GetFilename();
        lastDir = dlg->GetDirectory();
        lastPath = dlg->GetPath();

        OnSave(event);
    }
    delete dlg;
}


void frmQuery::OnCancel(wxCommandEvent& event)
{
    toolBar->EnableTool(MNU_CANCEL, FALSE);
    queryMenu->Enable(MNU_CANCEL, FALSE);
    SetStatusText(wxT("Cancelling."), STATUSPOS_MSGS);

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
    execQuery(wxT("EXPLAIN ") + query, true, 8);
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

void frmQuery::setTools(const bool running)
{
    toolBar->EnableTool(MNU_EXECUTE, !running);
    toolBar->EnableTool(MNU_EXPLAIN, !running);
    toolBar->EnableTool(MNU_CANCEL, running);
    queryMenu->Enable(MNU_EXECUTE, !running);
    queryMenu->Enable(MNU_EXPLAIN, !running);
    queryMenu->Enable(MNU_CANCEL, running);
}

void frmQuery::showMessage(const wxString& msg, const wxString &msgShort)
{
    msgResult->AppendText(msg + wxT("\n"));
    wxString str;
    if (msgShort.IsNull())
        str=msg;
    else
        str=msgShort;
    str.Replace("\n", " ");
    SetStatusText(str, STATUSPOS_MSGS);
}

void frmQuery::execQuery(const wxString &query, const bool singleResult, const int queryOffset)
{
    setTools(true);

    bool wasChanged = changed;
    sqlQuery->MarkerDeleteAll(0);
    if (!wasChanged)
    {
        changed=false;
        setExtendedTitle();
    }

    aborted=false;
    
    if (sqlResult->Execute(query) >= 0)
    {
        SetStatusText(wxT(""), STATUSPOS_SECS);
        SetStatusText(wxT("Query is running."), STATUSPOS_MSGS);
        SetStatusText(wxT(""), STATUSPOS_ROWS);
        msgResult->Clear();
        Update();
        wxYield();

        wxLongLong startTimeQuery=wxGetLocalTimeMillis();
        while (sqlResult->RunStatus() == CTLSQL_RUNNING)
        {
            elapsedQuery=wxGetLocalTimeMillis() - startTimeQuery;
            SetStatusText(elapsedQuery.ToString() + wxT(" ms"), STATUSPOS_SECS);
            wxYield();
            wxUsleep(10);
            wxYield();
        }

        msgResult->AppendText(sqlResult->GetMessages());

        elapsedQuery=wxGetLocalTimeMillis() - startTimeQuery;
        SetStatusText(elapsedQuery.ToString() + wxT(" ms"), STATUSPOS_SECS);

    
        if (sqlResult->RunStatus() != PGRES_TUPLES_OK)
        {
            if (sqlResult->RunStatus() == PGRES_COMMAND_OK)
            {
                showMessage(wxT("Query returned successfully with no result."), wxT("OK."));
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
            long rowsReadTotal=0, rowsTotal=sqlResult->NumRows();

            if (singleResult)
            {
                rowsReadTotal=sqlResult->RetrieveOne();
                showMessage(NumToStr(rowsReadTotal) + wxT(" rows retrieved."), wxT("OK."));
            }
            else
            {
                SetStatusText(wxT("Retrieving data: " + NumToStr(rowsTotal) + " rows."), STATUSPOS_MSGS);
                wxYield();

                long maxRows=settings->GetMaxRows();

                if (!maxRows)
                    maxRows = rowsTotal;
                if (rowsTotal > maxRows)
                {
                    wxMessageDialog msg(this, wxT("The maximum of ") + NumToStr(maxRows) + wxT(
                                " Rows is exceeded (total ") +NumToStr(rowsTotal) + wxT("). Retrieve all rows anyway?"), wxT("Limit exceeded"), 
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
			            sqlResult->Freeze();
		            }
                    rowsReadTotal += rowsRead;

                    if (elapsed > elapsedRetrieve +100)
                    {
                        elapsedRetrieve=elapsed;
                        SetStatusText(elapsedQuery.ToString() + "+" + elapsedRetrieve.ToString() + wxT(" ms"), STATUSPOS_SECS);
                        wxYield();
                    }
                }
	        	sqlResult->Thaw();

                elapsedRetrieve=wxGetLocalTimeMillis() - startTimeRetrieve;
                SetStatusText(elapsedQuery.ToString() + "+" + elapsedRetrieve.ToString() + wxT(" ms"), STATUSPOS_SECS);

                msgResult->AppendText(wxT(
                    "Total query runtime: ") + elapsedQuery.ToString() + " ms.\n"
                    "Data retrieval runtime: " + elapsedRetrieve.ToString() + wxT(" ms.\n"));

                if (rowsReadTotal == sqlResult->NumRows())
                    showMessage(NumToStr(rowsReadTotal) + wxT(" rows retrieved."), wxT("OK."));
                else
                    showMessage(wxT("Total ") + NumToStr(rowsTotal) + wxT(" rows.\n")+
                        NumToStr(rowsTotal - rowsReadTotal) + wxT(" rows not retrieved."),
                        NumToStr(rowsTotal - rowsReadTotal) + wxT(" rows not retrieved."));

            }
            if (rowsTotal == rowsReadTotal)
                SetStatusText(NumToStr(rowsTotal) + wxT(" rows"), STATUSPOS_ROWS);
            else
                SetStatusText(NumToStr(rowsReadTotal) + wxT(" of ") + NumToStr(rowsTotal) + wxT(" rows"), STATUSPOS_ROWS);
        }
    }

    setTools(false);
}
