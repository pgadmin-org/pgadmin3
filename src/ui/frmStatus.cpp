//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmStatus.cpp - Status Screen
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/xrc/xmlres.h>
#include <wx/image.h>
#include <wx/textbuf.h>

// App headers
#include "frmStatus.h"
#include "pgfeatures.h"
// Icons
#include "images/pgAdmin3.xpm"


#define TIMER_ID 333
BEGIN_EVENT_TABLE(frmStatus, pgDialog)
    EVT_BUTTON(XRCID("btnRefresh"),					frmStatus::OnRefresh)
    EVT_BUTTON(XRCID("btnClose"),					frmStatus::OnCloseBtn)
    EVT_BUTTON(XRCID("btnCancelSt"),				frmStatus::OnCancelBtn)
    EVT_BUTTON(XRCID("btnTerminateSt"),				frmStatus::OnTerminateBtn)
    EVT_BUTTON(XRCID("btnCancelLk"),				frmStatus::OnCancelBtn)
    EVT_BUTTON(XRCID("btnTerminateLk"),				frmStatus::OnTerminateBtn)
    EVT_CLOSE(										frmStatus::OnClose)
    EVT_SPINCTRL(XRCID("spnRefreshRate"),			frmStatus::OnRateChangeSpin)
    EVT_TEXT(XRCID("spnRefreshRate"),				frmStatus::OnRateChange)
	EVT_NOTEBOOK_PAGE_CHANGING(XRCID("nbStatus"),	frmStatus::OnNotebookPageChanged)
    EVT_TIMER(TIMER_ID,								frmStatus::OnRefreshTimer)
	EVT_LIST_ITEM_SELECTED(XRCID("lstStatus"),		frmStatus::OnSelStatusItem)
	EVT_LIST_ITEM_DESELECTED(XRCID("lstStatus"),	frmStatus::OnSelStatusItem)
	EVT_LIST_ITEM_SELECTED(XRCID("lstLocks"),		frmStatus::OnSelLockItem)
	EVT_LIST_ITEM_DESELECTED(XRCID("lstLocks"),		frmStatus::OnSelLockItem)
    EVT_COMBOBOX(XRCID("cbLogfiles"),               frmStatus::OnLoadLogfile)
    EVT_BUTTON(XRCID("btnRotateLog"),               frmStatus::OnRotateLogfile)
END_EVENT_TABLE();


#define statusList      CTRL_LISTVIEW("lstStatus")
#define lockList        CTRL_LISTVIEW("lstLocks")
#define logList         CTRL_LISTVIEW("lstLog")
#define spnRefreshRate  CTRL_SPIN("spnRefreshRate")
#define nbStatus		CTRL_NOTEBOOK("nbStatus")
#define btnCancelSt		CTRL_BUTTON("btnCancelSt")
#define btnTerminateSt	CTRL_BUTTON("btnTerminateSt")
#define btnCancelLk		CTRL_BUTTON("btnCancelLk")
#define btnTerminateLk	CTRL_BUTTON("btnTerminateLk")
#define cbLogfiles      CTRL_COMBOBOX("cbLogfiles")
#define btnRotateLog    CTRL_BUTTON("btnRotateLog")


void frmStatus::OnCloseBtn(wxCommandEvent &event)
{
    Destroy();
}

void frmStatus::OnClose(wxCloseEvent &event)
{
    Destroy();
}

frmStatus::frmStatus(frmMain *form, const wxString& _title, pgConn *conn)
{
    wxLogInfo(wxT("Creating server status box"));
	loaded = FALSE;


    wxWindowBase::SetFont(settings->GetSystemFont());
    LoadResource(form, wxT("frmStatus")); 

    RestorePosition(-1, -1, 400, 240, 200, 150);
    SetTitle(_title);
    SetIcon(wxIcon(pgAdmin3_xpm));
	statusBar = new  wxStatusBar(this, -1, wxST_SIZEGRIP);
	wxXmlResource::Get()->AttachUnknownControl(wxT("unkStatusBar"), statusBar);

    mainForm=form;
    timer=0;
    connection=conn;
    logHasTimestamp = false;
    logFormatKnown = false;

    logfileLength = 0;
    backend_pid=conn->GetBackendPID();

    statusList->AddColumn(_("PID"), 35);
    statusList->AddColumn(_("Database"), 70);
    statusList->AddColumn(_("User"), 70);
    if (connection->BackendMinimumVersion(7, 4)) {
        statusList->AddColumn(_("Start"), 50);
        statusList->AddColumn(_("Query"), 500);
    } else {
        statusList->AddColumn(_("Query"), 500);
    }

    lockList->AddColumn(_("PID"), 50);
    lockList->AddColumn(_("Database"), 50);
    lockList->AddColumn(_("Relation"), 50);
    lockList->AddColumn(_("User"), 50);
    lockList->AddColumn(_("TX"), 50);
    lockList->AddColumn(_("Mode"), 50);
    lockList->AddColumn(_("Granted"), 50);
    if (connection->BackendMinimumVersion(7, 4)) {
        lockList->AddColumn(_("Start"), 50);
        lockList->AddColumn(_("Query"), 500);
    } else {
        lockList->AddColumn(_("Query"), 500);
    }

    if (connection->BackendMinimumVersion(7, 5) && connection->HasFeature(FEATURE_FILEREAD))
    {
        logFormat = connection->ExecuteScalar(wxT("SHOW log_line_prefix"));
        if (logFormat == wxT("unset"))
            logFormat = wxEmptyString;
        logFmtPos=logFormat.Find('%', true);

        if (logFmtPos < 0)
            logFormatKnown = true;
        else if (!logFmtPos && logFormat.Mid(logFmtPos, 2) == wxT("%t") && logFormat.Length() > 2)
        {
            logFormatKnown = true;
            logHasTimestamp = true;
            logList->AddColumn(_("Timestamp"), 100);
        }

        if (logFormatKnown)
            logList->AddColumn(_("Level"), 35);

        logList->AddColumn(_("Log entry"), 800);
    }
    else
        nbStatus->DeletePage(2);

    long rate;
    settings->Read(wxT("frmStatus/Refreshrate"), &rate, 1);
    spnRefreshRate->SetValue(rate);
    timer=new wxTimer(this, TIMER_ID);

	btnCancelSt->Enable(false);
	btnTerminateSt->Enable(false);
	btnCancelLk->Enable(false);
	btnTerminateLk->Enable(false);

	loaded = TRUE;
}


frmStatus::~frmStatus()
{
    wxLogInfo(wxT("Destroying server status box"));
    mainForm->RemoveFrame(this);

    SavePosition();
    settings->Write(wxT("frmStatus/Refreshrate"), spnRefreshRate->GetValue());

    delete timer;
    if (connection)
        delete connection;

    emptyLogfileCombo();
}


void frmStatus::Go()
{
    Show(true);
    wxCommandEvent nullEvent;

    long rate=spnRefreshRate->GetValue();
    if (rate)
        timer->Start(rate*1000L);

    OnRefresh(nullEvent);
}


void frmStatus::OnNotebookPageChanged(wxNotebookEvent& event)
{
	if (!loaded) return;
	wxCommandEvent buttonEvent(wxEVT_COMMAND_BUTTON_CLICKED, XRCID("btnRefresh"));
    AddPendingEvent(buttonEvent);
	
    // Disable the buttons. They'll get re-enabled if a suitable item is selected.
	btnCancelSt->Enable(false);
	btnTerminateSt->Enable(false);
	btnCancelLk->Enable(false);
	btnTerminateLk->Enable(false);
}


void frmStatus::OnRateChangeSpin(wxSpinEvent &event)
{
	OnRateChange(*(wxCommandEvent*)&event);
}


void frmStatus::OnRateChange(wxCommandEvent &event)
{
    if (timer)
    {
        timer->Stop();
        long rate=spnRefreshRate->GetValue();
        if (rate)
        {
            timer->Start(rate*1000L);
            OnRefresh(event);
        }
    }
}


void frmStatus::OnRefreshTimer(wxTimerEvent &event)
{
	OnRefresh(*(wxCommandEvent*)&event);
}


void frmStatus::OnRefresh(wxCommandEvent &event)
{
    long pid=0;
	// To avoid hammering the lock manager (and the network for that matter),
	// only query for the required tab.

	wxLogNull nolog;

	if (!connection)
	    return;

	if (nbStatus->GetSelection() == 0)
    {
        // Status
		long row=0;
		pgSet *dataSet1=connection->ExecuteSet(wxT("SELECT * FROM pg_stat_activity ORDER BY procpid"));
		if (dataSet1)
		{
            statusList->Freeze();
			statusBar->SetStatusText(_("Refreshing."));
			while (!dataSet1->Eof())
			{
				pid=dataSet1->GetLong(wxT("procpid"));

				if (pid != backend_pid)
				{
					long itempid=0;

					while (row < statusList->GetItemCount())
					{
						itempid=StrToLong(statusList->GetItemText(row));
						if (itempid && itempid < pid)
							statusList->DeleteItem(row);
						else
							break;
					}

					if (!itempid || itempid > pid)
					{
						statusList->InsertItem(row, NumToStr(pid), 0);
					}
					statusList->SetItem(row, 1, dataSet1->GetVal(wxT("datname")));
					statusList->SetItem(row, 2, dataSet1->GetVal(wxT("usename")));
					wxString qry=dataSet1->GetVal(wxT("current_query"));

					if (connection->BackendMinimumVersion(7, 4))
					{
						if (qry.IsEmpty()) {
							statusList->SetItem(row, 3, wxEmptyString);
							statusList->SetItem(row, 4, wxEmptyString);
						} else {
							statusList->SetItem(row, 3, dataSet1->GetVal(wxT("query_start")));
							statusList->SetItem(row, 4, qry.Left(250));
						} 
					} else {
						statusList->SetItem(row, 3, qry.Left(250));
					}

					row++;
				}
				dataSet1->MoveNext();
			}
            delete dataSet1;
			
            while (row < statusList->GetItemCount())
				statusList->DeleteItem(row);

            statusList->Thaw();
			statusBar->SetStatusText(_("Done."));
		}
        else
		    checkConnection();

        row=0;
		while (row < statusList->GetItemCount())
		{
			long itempid=StrToLong(statusList->GetItemText(row));
			if (itempid && itempid > pid)
				statusList->DeleteItem(row);
            else
                row++;
		}
	}
	else if (nbStatus->GetSelection() == 1)
    {
		// Locks
		long row=0;
		wxString sql;
		if (connection->BackendMinimumVersion(7, 4)) {
			sql = wxT("SELECT ")
				  wxT("(SELECT datname FROM pg_database WHERE oid = database) AS dbname, ")
				  wxT("relation::regclass AS class, ")
				  wxT("pg_get_userbyid(pg_stat_get_backend_userid(pid)::int4) as user, ")
				  wxT("transaction, pid, mode, granted, ")
				  wxT("pg_stat_get_backend_activity(pid) AS current_query, ")
				  wxT("pg_stat_get_backend_activity_start(pid) AS query_start ")
				  wxT("FROM pg_locks");
		} else {
			sql = wxT("SELECT ")
				  wxT("(SELECT datname FROM pg_database WHERE oid = database) AS dbname, ")
				  wxT("relation::regclass AS class, ")
				  wxT("pg_get_userbyid(pg_stat_get_backend_userid(pid)::int4) as user, ")
				  wxT("transaction, pid, mode, granted, ")
				  wxT("pg_stat_get_backend_activity(pid) AS current_query ")
				  wxT("FROM pg_locks ");
		}

		pgSet *dataSet2=connection->ExecuteSet(sql);
		if (dataSet2)
		{
			statusBar->SetStatusText(_("Refreshing."));
            lockList->Freeze();

			while (!dataSet2->Eof())
			{
				pid=dataSet2->GetLong(wxT("pid"));

				if (pid != backend_pid)
				{
					long itempid=0;

					while (row < lockList->GetItemCount())
					{
						itempid=StrToLong(lockList->GetItemText(row));
						if (itempid && itempid < pid)
							lockList->DeleteItem(row);
						else
							break;
					}

					if (!itempid || itempid > pid)
					{
						lockList->InsertItem(row, NumToStr(pid), 0);
					}
					lockList->SetItem(row, 1, dataSet2->GetVal(wxT("dbname")));
					lockList->SetItem(row, 2, dataSet2->GetVal(wxT("class")));
					lockList->SetItem(row, 3, dataSet2->GetVal(wxT("user")));
					lockList->SetItem(row, 4, dataSet2->GetVal(wxT("transaction")));
					lockList->SetItem(row, 5, dataSet2->GetVal(wxT("mode")));
					if (dataSet2->GetVal(wxT("granted")) == wxT("t")) {
						lockList->SetItem(row, 6, __("Yes"));
					} else {
						lockList->SetItem(row, 6, __("No"));
					}
					wxString qry=dataSet2->GetVal(wxT("current_query"));

					if (connection->BackendMinimumVersion(7, 4))
					{
						if (qry.IsEmpty()) {
							lockList->SetItem(row, 7, wxEmptyString);
							lockList->SetItem(row, 8, wxEmptyString);
						} else {
							lockList->SetItem(row, 7, dataSet2->GetVal(wxT("query_start")));
							lockList->SetItem(row, 8, qry.Left(250));
						}
					} else {
						lockList->SetItem(row, 7, qry.Left(250));
					}

					row++;
				}
				dataSet2->MoveNext();
			}

            delete dataSet2;
			
            while (row < lockList->GetItemCount())
				lockList->DeleteItem(row);

            lockList->Thaw();
			statusBar->SetStatusText(_("Done."));
		}
        else
            checkConnection();

        row=0;
		while (row < lockList->GetItemCount())
		{
			long itempid=StrToLong(lockList->GetItemText(row));
			if (itempid && itempid > pid)
				lockList->DeleteItem(row);
            else
                row++;
		}
	}
    else
    {
        long newlen=0;

        if (logDirectory.IsEmpty())
        {
            // freshly started
            logDirectory = connection->ExecuteScalar(wxT("SHOW log_directory"));
            if (fillLogfileCombo())
            {
                cbLogfiles->SetSelection(0);
                wxCommandEvent ev;
                OnLoadLogfile(ev);
                return;
            }
            else 
            {
                logDirectory = wxT("-");
                logList->AppendItem(-1, _("file logging not enabled"));
                cbLogfiles->Disable();
                btnRotateLog->Disable();
            }
        }
        
        if (logDirectory == wxT("-"))
            return;


        if (isCurrent)
        {
            // check if the current logfile changed
		    pgSet *set = connection->ExecuteSet(wxT("SELECT pg_file_length(") + qtString(logfileName) + wxT(") AS len"));
		    if (set)
			{
				newlen = set->GetLong(wxT("len"));
				delete set;
		    }
			else
			{
		        checkConnection();
				return;
			}
            if (newlen > logfileLength)
            {
			    statusBar->SetStatusText(_("Refreshing."));
                addLogFile(logfileName, logfileTimestamp, logfilePid, newlen, logfileLength, false);
				statusBar->SetStatusText(_("Done."));

                // as long as there was new data, the logfile is probably the current
                // one so we don't need to check for rotation
                return;
            }
        }        

        // 
        wxString newDirectory = connection->ExecuteScalar(wxT("SHOW log_directory"));

        int newfiles=0;
        if (newDirectory != logDirectory)
            cbLogfiles->Clear();

        newfiles = fillLogfileCombo();

        if (newfiles)
        {
            if (!showCurrent)
                isCurrent=false;
        
            if (isCurrent)
            {
                int pos = cbLogfiles->GetCount() - newfiles;
                bool skipFirst = true;

                while (newfiles--)
                {
                    addLogLine(_("pgadmin:Logfile rotated."), false);
                    wxDateTime *ts=(wxDateTime*)cbLogfiles->GetClientData(pos++);
                    wxASSERT(ts != 0);

                    
                    addLogFile(ts, skipFirst);
                    skipFirst = false;

                    pos++;
                }
            }
        }
    }
}


void frmStatus::checkConnection()
{
    if (!connection->IsAlive())
	{
	    delete connection;
		connection=0;
		statusBar->SetStatusText(_("Connection broken."));
	}
}


void frmStatus::addLogFile(wxDateTime *dt, bool skipFirst)
{
    pgSet *set=connection->ExecuteSet(
        wxT("SELECT filetime, pid, filename, pg_file_length(filename) AS len\n")
        wxT("  FROM pg_logdir_ls\n")
        wxT(" WHERE filetime = '") + DateToAnsiStr(*dt) + wxT("'::timestamp"));
    if (set)
    {
        logfileName = set->GetVal(wxT("filename"));
        logfileTimestamp = set->GetDateTime(wxT("filetime"));
        logfilePid = set->GetLong(wxT("pid"));
        long len=set->GetLong(wxT("len"));

        logfileLength = 0;
        addLogFile(logfileName, logfileTimestamp, logfilePid, len, logfileLength, skipFirst);

        delete set;
    }
}

void frmStatus::addLogFile(const wxString &filename, const wxDateTime timestamp, int pid, long len, long &read, bool skipFirst)
{
    wxString line;

    if (skipFirst)
    {
        long maxServerLogSize = settings->GetMaxServerLogSize();

        if (!logfileLength && maxServerLogSize && logfileLength > maxServerLogSize)
        {
            long maxServerLogSize=settings->GetMaxServerLogSize();
            len = read-maxServerLogSize;
        }
        else
            skipFirst=false;
    }


    while (len > read)
    {
        pgSet *set=connection->ExecuteSet(wxT("SELECT pg_file_read(") + 
            qtString(filename) + wxT(", ") + NumToStr(read) + wxT(", 50000)"));
        if (!set)
        {
            connection->IsAlive();
            return;
        }
        char *raw=set->GetCharPtr(0);

        if (!raw || !*raw)
        {
            delete set;
            break;
        }
        read += strlen(raw);


        wxString str = line + wxTextBuffer::Translate(wxString(raw, set->GetConversion()), wxTextFileType_Unix);
        delete set;

        bool hasCr = (str.Right(1) == wxT("\n"));

        wxStringTokenizer tk(str, wxT("\n"));

        logList->Freeze();
        while (tk.HasMoreTokens())
        {
            str = tk.GetNextToken();
            if (skipFirst)
            {
                // could be truncated
                skipFirst = false;
                continue;
            }
            if (tk.HasMoreTokens() || hasCr)
                addLogLine(str);
            else
                line = str;
        }
        logList->Thaw();
    }
    if (!line.IsEmpty())
        addLogLine(line);
}



void frmStatus::addLogLine(const wxString &str, bool formatted)
{
    int row=logList->GetItemCount();
    if (!logFormatKnown)
        logList->AppendItem(-1, str);
    else
    {
        if (str.Find(':') < 0)
        {
            logList->InsertItem(row, wxEmptyString, -1);
            logList->SetItem(row, (logHasTimestamp ? 2 : 1), str);
        }
        else
        {
            wxString rest;

            if (logHasTimestamp)
            {
                if (formatted)
                {
                    wxString ts=str.Mid(logFmtPos);
                    int pos = ts.Mid(22).Find(logFormat.c_str()[logFmtPos+2]);
                    logList->InsertItem(row, ts.Left(22+pos), -1);
                    rest = ts.Mid(22+pos + logFormat.Length() - logFmtPos-2);
                }
                else
                {
                    rest = str;
                    logList->InsertItem(row, wxEmptyString, -1);
                }
                logList->SetItem(row, 1, rest.BeforeFirst(':'));
                logList->SetItem(row, 2, rest.AfterFirst(':'));
            }
            else
            {
                if (formatted)
                    rest = str.Mid(logFormat.Length());
                else
                    rest = str;

                int pos = rest.Find(':');

                if (pos < 0)
                    logList->InsertItem(row, rest, -1);
                else
                {
                    logList->InsertItem(row, rest.BeforeFirst(':'), -1);
                    logList->SetItem(row, 1, rest.AfterFirst(':'));
                }
            }
        }
    }
}


void frmStatus::emptyLogfileCombo()
{
    while (cbLogfiles->GetCount())
    {
        wxDateTime *dt=(wxDateTime*)cbLogfiles->GetClientData(0);
        if (dt)
            delete dt;
        cbLogfiles->Delete(0);
    }
}


int frmStatus::fillLogfileCombo()
{
    int count=cbLogfiles->GetCount();
    if (!count)
        cbLogfiles->Append(_(" current"));
    else
        count--;

    pgSet *set=connection->ExecuteSet(
        wxT("SELECT ts, pid, fn FROM pg_logfiles_ls() AS (ts timestamp, pid int4, fn text)\n")
        wxT(" ORDER BY ts ASC"));
    if (set)
    {
        if (set->NumRows() <= count)
            return 0;

        set->Locate(count+1);
        count=0;

        while (!set->Eof())
        {
            count++;
            wxString fn= set->GetVal(wxT("fn"));
            long pid = set->GetLong(wxT("pid"));
            wxDateTime ts=set->GetDateTime(wxT("ts"));

            cbLogfiles->Append(DateToStr(ts) + wxT(" (") + NumToStr(pid) + wxT(")"), new wxDateTime(ts));

            set->MoveNext();
        }

        delete set;
    }

    return count;
}


void frmStatus::OnLoadLogfile(wxCommandEvent &event)
{
    int pos=cbLogfiles->GetSelection();
    int lastPos = cbLogfiles->GetCount()-1;
    if (pos >= 0)
    {
        showCurrent = (pos == 0);
        isCurrent = showCurrent || (pos == lastPos);

        wxDateTime *ts=(wxDateTime*)cbLogfiles->GetClientData(showCurrent ? lastPos : pos);
        wxASSERT(ts != 0);

        if (!logfileTimestamp.IsValid() || *ts != logfileTimestamp)
        {
            logList->DeleteAllItems();
            addLogFile(ts, true);
        }
    }
}


void frmStatus::OnRotateLogfile(wxCommandEvent &event)
{
    if (wxMessageBox(_("Are you sure the logfile should be rotated?"), _("Logfile rotation"), wxYES_NO|wxNO_DEFAULT | wxICON_QUESTION) == wxYES)
        connection->ExecuteVoid(wxT("select pg_logfile_rotate()"));
}


void frmStatus::OnCancelBtn(wxCommandEvent &event)
{
    ctlListView *lv;
	
	if (nbStatus->GetSelection() == 0)
		 lv = statusList;
	else if (nbStatus->GetSelection() == 1)
		 lv = lockList;
	else 
	{
		wxLogError(wxT("Couldn't determine the source listview for a cancel backend operation!"));
		return;
	}

	if (wxMessageBox(_("Are you sure you wish to cancel the selected query(s)?"), _("Cancel query?"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION) == wxNO)
		return;

    long item = -1;
	wxString pid;

    for ( ;; )
    {
        item = lv->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if ( item == -1 )
            break;

		pid = lv->GetItemText(item);
		wxString sql = wxT("SELECT pg_cancel_backend(") + pid + wxT(");");
		connection->ExecuteScalar(sql);
	}

	wxMessageBox(_("A cancel signal was sent to the selected server process(es)."), _("Cancel query"), wxOK | wxICON_INFORMATION);
	OnRefresh(*(wxCommandEvent*)&event);
}


void frmStatus::OnTerminateBtn(wxCommandEvent &event)
{
    ctlListView *lv;
	
	if (nbStatus->GetSelection() == 0)
		 lv = statusList;
	else if (nbStatus->GetSelection() == 1)
		 lv = lockList;
	else 
	{
		wxLogError(wxT("Couldn't determine the source listview for a terminate backend operation!"));
		return;
	}

	if (wxMessageBox(_("Are you sure you wish to terminate the selected server process(es)?"), _("Terminate process?"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION) == wxNO)
		return;

    long item = -1;
	wxString pid;

    for ( ;; )
    {
        item = lv->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if ( item == -1 )
            break;

		pid = lv->GetItemText(item);
		wxString sql = wxT("SELECT pg_terminate_backend(") + pid + wxT(");");
		connection->ExecuteScalar(sql);

	}

	wxMessageBox(_("A terminate signal was sent to the selected server process(es)."), _("Terminate process"), wxOK | wxICON_INFORMATION);
	OnRefresh(*(wxCommandEvent*)&event);
}


void frmStatus::OnSelStatusItem(wxListEvent &event)
{
	if (connection->BackendMinimumVersion(7, 5))
	{
		if(statusList->GetSelectedItemCount() >= 0) 
		{
			btnCancelSt->Enable(true);
			btnTerminateSt->Enable(true);
		} 
		else 
		{
			btnCancelSt->Enable(false);
			btnTerminateSt->Enable(false);
		}
	}
}

void frmStatus::OnSelLockItem(wxListEvent &event)
{
	if (connection->BackendMinimumVersion(7, 5))
	{
		if(lockList->GetSelectedItemCount() >= 0) 
		{
			btnCancelLk->Enable(true);
			btnTerminateLk->Enable(true);
		} 
		else 
		{
			btnCancelLk->Enable(false);
			btnTerminateLk->Enable(false);
		}
	}
}
