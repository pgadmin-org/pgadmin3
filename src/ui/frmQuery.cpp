//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
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

// Icons
#include "images/sql.xpm"
// Bitmaps
#include "images/file_open.xpm"
#include "images/file_save.xpm"
#include "images/query_execute.xpm"
#include "images/query_explain.xpm"
#include "images/query_cancel.xpm"


BEGIN_EVENT_TABLE(frmQuery, wxFrame)
    EVT_CLOSE(                      OnClose)
    EVT_MENU(BTN_OPEN,              OnOpen)
    EVT_MENU(BTN_SAVE,              OnSave)
    EVT_MENU(BTN_EXECUTE,           OnExecute)
    EVT_MENU(BTN_EXPLAIN,           OnExplain)
    EVT_MENU(BTN_CANCEL,            OnCancel)
    EVT_MENU(MNU_OPEN,              OnOpen)
    EVT_MENU(MNU_SAVE,              OnSave)
    EVT_MENU(MNU_SAVEAS,            OnSaveAs)
    EVT_MENU(MNU_EXECUTE,           OnExecute)
    EVT_MENU(MNU_EXPLAIN,           OnExplain)
    EVT_MENU(MNU_CANCEL,            OnCancel)
    EVT_STC_CHANGE(CTL_SQLQUERY,    OnChange)
END_EVENT_TABLE()


frmQuery::frmQuery(frmMain *form, const wxString& _title, pgConn *_conn, const wxPoint& pos, const wxSize& size, const wxString& query)
: wxFrame(NULL, -1, _title, pos, size)
{
    mainForm=form;
    conn=_conn;
    title = _title;
    thread=0;

    SetIcon(wxIcon(sql_xpm));
    wxLogInfo(wxT("Creating SQL Query box"));
    menuBar = new wxMenuBar();

    fileMenu = new wxMenu();
    fileMenu->Append(MNU_OPEN, wxT("&Open..."), wxT("Open a query file"));
    fileMenu->Append(MNU_SAVE, wxT("&Save"), wxT("Save current file"));
    fileMenu->Append(MNU_SAVEAS, wxT("Save &as..."), wxT("Save file under new name"));
    menuBar->Append(fileMenu, wxT("&File"));

    queryMenu = new wxMenu();
    queryMenu->Append(MNU_EXECUTE, wxT("&Execute"), wxT("Execute query"));
    queryMenu->Append(MNU_EXPLAIN, wxT("E&xplain"), wxT("Explain query"));
    queryMenu->Append(MNU_CANCEL, wxT("&Cancel"), wxT("Cancel query"));
    menuBar->Append(queryMenu, wxT("&Query"));
    SetMenuBar(menuBar);
    
    wxAcceleratorEntry entries[6];
    entries[0].Set(wxACCEL_ALT,     (int)'E',      MNU_EXECUTE);
    entries[1].Set(wxACCEL_ALT,     (int)'X',      MNU_EXPLAIN);
    entries[2].Set(wxACCEL_CTRL,    (int)'O',      MNU_OPEN);
    entries[3].Set(wxACCEL_CTRL,    (int)'S',      MNU_SAVE);
    entries[4].Set(wxACCEL_NORMAL,  WXK_F5,        MNU_EXECUTE);
    entries[5].Set(wxACCEL_ALT,     WXK_PAUSE,     MNU_CANCEL);

    wxAcceleratorTable accel(6, entries);
    SetAcceleratorTable(accel);

    fileMenu->Enable(MNU_SAVE, false);
    queryMenu->Enable(MNU_CANCEL, false);

    int iWidths[4] = {0, -1, 110, 110};
    CreateStatusBar(4);
    SetStatusWidths(4, iWidths);
    SetStatusText(wxT("ready"), STATUSPOS_MSGS);

    CreateToolBar();
    statusBar = GetStatusBar();
    toolBar = GetToolBar();

    toolBar->SetToolBitmapSize(wxSize(16, 16));

    toolBar->AddTool(BTN_OPEN, wxT("Open"), wxBitmap(file_open_xpm), wxT("Open file"), wxITEM_NORMAL);
    toolBar->AddTool(BTN_SAVE, wxT("Save"), wxBitmap(file_save_xpm), wxT("Save file"), wxITEM_NORMAL);
    toolBar->AddTool(BTN_EXECUTE, wxT("Execute"), wxBitmap(query_execute_xpm), wxT("Execute query"), wxITEM_NORMAL);
    toolBar->AddTool(BTN_EXPLAIN, wxT("Explain"), wxBitmap(query_explain_xpm), wxT("Explain query"), wxITEM_NORMAL);
    toolBar->AddTool(BTN_CANCEL, wxT("Cancel"), wxBitmap(query_cancel_xpm), wxT("Cancel query"), wxITEM_NORMAL);

    toolBar->Realize();
    setTools(false);
    toolBar->EnableTool(BTN_SAVE, false);

    horizontal = new wxSplitterWindow(this, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_LIVE_UPDATE | wxCLIP_CHILDREN);
    horizontal->SetMinimumPaneSize(50);

    sqlQuery = new ctlSQLBox(horizontal, CTL_SQLQUERY, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxSIMPLE_BORDER | wxTE_RICH2);

    output = new wxNotebook(horizontal, -1, wxDefaultPosition, wxDefaultSize, wxNB_BOTTOM);
    sqlResult = new wxListView(output, CTL_SQLRESULT, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxSIMPLE_BORDER);
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
    sqlQuery->SelectAll();
}


frmQuery::~frmQuery()
{
    wxLogInfo(wxT("Destroying SQL Query box"));
    mainForm->RemoveFrame(this);
    if (thread)
    {
        thread->Delete();
        delete thread;
    }
    settings->Write(wxT("frmQuery/Width"), GetSize().x);
    settings->Write(wxT("frmQuery/Height"), GetSize().y);
    settings->Write(wxT("frmQuery/Left"), GetPosition().x);
    settings->Write(wxT("frmQuery/Top"), GetPosition().y);
    settings->Write(wxT("frmQuery/Split"), horizontal->GetSashPosition());
}


void frmQuery::Go()
{
    Show(TRUE);
    sqlQuery->SetFocus();
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
        fileMenu->Enable(MNU_SAVE, true);
        toolBar->EnableTool(BTN_SAVE, true);
        SetTitle(title + wxT(" - [") + lastPath + wxT("]") + chgStr);
    }
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

        switch (msg.ShowModal())
        {
            case wxID_YES:
                if (lastPath.IsNull())
                {
                    OnSaveAs(wxCommandEvent());
                    if (changed && event.CanVeto())
                        event.Veto();
                }
                else
                    OnSave(wxCommandEvent());
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

        FILE *f=fopen(lastPath.c_str(), "rb");
        if (f)
        {
            fseek(f, 0, SEEK_END);
            int len=ftell(f);
            fseek(f, 0, SEEK_SET);
            wxString buf("", len+1);
            fread((char*)buf.c_str(), len, 1, f);
            fclose(f);
            ((char*)buf.c_str())[len]=0;
            sqlQuery->SetText(buf);
            changed = false;
            setExtendedTitle();
        }
    }
}

void frmQuery::OnSave(wxCommandEvent& event)
{
    FILE *f=fopen(lastPath.c_str(), "w+t");
    if (f)
    {
        setExtendedTitle();

        wxString buf=sqlQuery->GetText();
        fwrite(buf.c_str(), buf.Length(), 1, f);
        fclose(f);
        changed=false;
        setExtendedTitle();
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
    toolBar->EnableTool(BTN_CANCEL, FALSE);
    queryMenu->Enable(MNU_CANCEL, FALSE);
    SetStatusText(wxT("Cancelling."), STATUSPOS_MSGS);

    if (thread)
    {
        if (thread->running > 0)
            thread->Delete();
        else
            thread->running = -1;
    }
}


void frmQuery::OnExplain(wxCommandEvent& event)
{
    wxString query=sqlQuery->GetSelectedText();
    if (query.IsNull())
        query = sqlQuery->GetText();

    if (query.IsNull())
        return;
    execQuery(wxT("EXPLAIN ") + query, true, 8);
}


void frmQuery::OnExecute(wxCommandEvent& event)
{
    wxString query=sqlQuery->GetSelectedText();
    if (query.IsNull())
        query = sqlQuery->GetText();

    if (query.IsNull())
        return;
    execQuery(query);
}

void frmQuery::setTools(const bool running)
{
    toolBar->EnableTool(BTN_EXECUTE, !running);
    toolBar->EnableTool(BTN_EXPLAIN, !running);
    toolBar->EnableTool(BTN_CANCEL, running);
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
    sqlResult->ClearAll();

    setTools(true);

    thread = new queryThread(conn->connection(), query);

    sqlQuery->MarkerDeleteAll(0);
    nRows=0;

    if (thread->Create() == wxTHREAD_NO_ERROR)
    {
        SetStatusText(wxT(""), STATUSPOS_SECS);
        SetStatusText(wxT("Query is running."), STATUSPOS_MSGS);
        SetStatusText(wxT(""), STATUSPOS_ROWS);
        msgResult->Clear();
        Update();
        wxYield();
        long row=0;

        wxLongLong startTime=wxGetLocalTimeMillis();
        thread->Run();
        while (thread->IsRunning())
        {
            elapsedQuery=wxGetLocalTimeMillis() - startTime;
            SetStatusText(elapsedQuery.ToString() + wxT(" ms"), STATUSPOS_SECS);
            wxYield();
            wxUsleep(10);
            wxYield();
        }
        msgResult->AppendText(thread->messages);

        elapsedQuery=wxGetLocalTimeMillis() - startTime;
        SetStatusText(elapsedQuery.ToString() + wxT(" ms"), STATUSPOS_SECS);

        int rc=PQresultStatus(thread->result);
        if (rc != PGRES_TUPLES_OK)
        {
            if (rc == PGRES_COMMAND_OK)
            {
                showMessage(wxT("Query returned successfully with no result."), wxT("OK."));
            }
            else
            {
                wxString errMsg = PQerrorMessage(conn->connection());
                showMessage(errMsg);

                wxString atChar=wxT(" at character ");
                int chp=errMsg.Find(atChar);

                if (chp > 0)
                {
                    chp -= queryOffset;  // do not count EXPLAIN or similar
                    int selStart=sqlQuery->GetSelectionStart(), selEnd=sqlQuery->GetSelectionEnd();
                    long errPos=0;
                    errMsg.Mid(chp+atChar.Length()).ToLong(&errPos);
                    int line=0, maxLine = sqlQuery->GetLineCount();
                    while (line < maxLine && sqlQuery->GetLineEndPosition(line) < errPos + selStart+1)
                        line++;
                    if (line < maxLine)
                    {
                        sqlQuery->MarkerAdd(line, 0);
                        sqlQuery->EnsureVisible(line);
                    }
                }
            }
        }
        else
        {
            pgSet *dataSet = new pgSet(thread->result, conn->connection());

            nCols=dataSet->NumCols();
            nRows=dataSet->NumRows();

            if (!nCols)
                showMessage(wxT("No Columns."));
            else
            {
                dataSet->MoveFirst();
                dataSet->GetVal(0);  // Dummy so we read the properties of the last result set

                long col=0;
                wxString colName, colType;

                if (singleResult)
                {
                    int w, h;
                    sqlResult->GetSize(&w, &h);
                    colName = dataSet->ColName(0);
                    sqlResult->InsertColumn(0, colName, wxLIST_FORMAT_LEFT, w);
                }
                else
                {
                    sqlResult->InsertColumn(0, wxT(""), wxLIST_FORMAT_RIGHT, 30);

                    for (col=0 ; col < nCols ; col++)
                    {
                        colName = dataSet->ColName(col);
                        colType = dataSet->ColType(col);
                        sqlResult->InsertColumn(col+1, colName +wxT(" (")+ colType +wxT(")"), wxLIST_FORMAT_LEFT, -1);
                    }
                }
                if (nRows)
                {
                    SetStatusText(wxT("Retrieving data: " + NumToStr(nRows) + " rows."), STATUSPOS_MSGS);
                    wxYield();

                    long maxRows=settings->GetMaxRows();

                    if (!maxRows)
                        maxRows = nRows;
                    if (nRows > maxRows)
                    {
                        wxMessageDialog msg(this, wxT("The maximum of ") + NumToStr(maxRows) + wxT(
                                    " Rows is exceeded (total ") +NumToStr(nRows) + wxT("). Retrieve all rows anyway?"), wxT("Limit exceeded"), 
                                    wxYES_NO|wxCANCEL|wxNO_DEFAULT|wxICON_EXCLAMATION);
                        switch (msg.ShowModal())
                        {
                            case wxID_YES:
                                maxRows = nRows;
                                break;
                            case wxID_CANCEL:
                                maxRows = 0;
                                break;
                        }
                    }

                    wxString value;
                    wxLongLong elapsed;

                    while (row < maxRows && !thread->running && !dataSet->Eof())
                    {
                        if (row == 20)
                            sqlResult->Freeze();

                        sqlResult->InsertItem(row, NumToStr(row+1L));

                        if (singleResult)
                            sqlResult->SetItem(row, 0, dataSet->GetVal(0));
                        else
                        {
                            for (col=0 ; col < nCols ; col++)
                            {
                                value = dataSet->GetVal(col);
                                sqlResult->SetItem(row, col+1, value);
                            }
                        }
                        row++;
                        
                        dataSet->MoveNext();
                        elapsed = wxGetLocalTimeMillis() - startTime - elapsedQuery;
                        if (elapsed > elapsedRetrieve +100)
                        {
                            elapsedRetrieve=elapsed;
                            SetStatusText(elapsedQuery.ToString() + "+" + elapsedRetrieve.ToString() + wxT(" ms"), STATUSPOS_SECS);
                            wxYield();
                        }
                    }
                    elapsedRetrieve=wxGetLocalTimeMillis() - startTime - elapsedQuery;
                    SetStatusText(elapsedQuery.ToString() + "+" + elapsedRetrieve.ToString() + wxT(" ms"), STATUSPOS_SECS);
                    msgResult->AppendText(wxT(
                        "Total query runtime: ") + elapsedQuery.ToString() + " ms.\n"
                        "Data retrieval runtime: " + elapsedRetrieve.ToString() + wxT(" ms.\n"));

                    sqlResult->Thaw();
                    if (!thread->running)
                    {
                        if (row == nRows)
                            showMessage(NumToStr(row) + wxT(" rows retrieved."), wxT("OK."));
                        else
                            showMessage(wxT("Total ") + NumToStr(nRows) + wxT(" rows.\n")+
                                NumToStr(nRows-row) + wxT(" rows not retrieved."),
                                NumToStr(nRows-row) + wxT(" rows not retrieved."));
                    }
                    else
                    {
                        dataSet->MoveLast();
                        dataSet->GetVal(0); // Dummy
                        showMessage(wxT("Cancelled while retrieving data: " + 
                            NumToStr(nRows-row) + " not retrieved."));
                    }
                }
                else
                    SetStatusText(wxT("OK."), STATUSPOS_MSGS);
            }
            delete dataSet;
        }
        if (row == nRows)
            SetStatusText(NumToStr(nRows) + wxT(" rows"), STATUSPOS_ROWS);
        else
            SetStatusText(NumToStr(row) + wxT(" of ") + NumToStr(nRows) + wxT(" rows"), STATUSPOS_ROWS);
        thread->Delete();

        output->SetSelection(row ? 0 : 1);
    }
    delete thread;
    thread=0;
    setTools(false);
}


queryThread::queryThread(PGconn *_conn, const wxString &qry) : wxThread(wxTHREAD_JOINABLE)
{
    running = 1;

    query = qry;
    conn=_conn;
    result=0;
    PQsetnonblocking(conn, 1);
}


queryThread::~queryThread()
{
}


int queryThread::execute()
{
    wxLongLong startTime=wxGetLocalTimeMillis();

    if (!PQsendQuery(conn, query.c_str()))
        return(0);

    while (running > 0)
    {
        if (TestDestroy())
        {
            if (!PQrequestCancel(conn)) // could not abort; abort failed.
            {
                running=-1;
                return(-1);
            }
        }
        if (!PQconsumeInput(conn))
            return(0);
        if (PQisBusy(conn))
        {
            Yield();
            wxUsleep(10);
            continue;
        }

        // only the last result set will be returned
        // all others are discarded
        PGresult *res=PQgetResult(conn);
        wxLongLong elapsed=wxGetLocalTimeMillis() - startTime;
        startTime = wxGetLocalTimeMillis();
        if (!res)
            break;
        if (result)
        {
            messages += wxT(
                "Query result with ") + NumToStr((long)PQntuples(result)) + wxT(" rows discarded.\n");
            PQclear(result);
        }
        result=res;
    }
    messages += wxT("\n");
    running=0;
    return(1);
}


void *queryThread::Entry()
{
    wxString msg;
    msg.Printf(wxT("Running query %s"), query.c_str());
    wxLogInfo(msg);

    execute();

    return(NULL);
}
