//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmStatus.h - Status Screen
//
//////////////////////////////////////////////////////////////////////////

#ifndef __FRMSTATUS_H
#define __FRMSTATUS_H

// wxWindows headers
#include <wx/wx.h>
#include <wx/image.h>
#include <wx/listctrl.h>
#include <wx/spinctrl.h>

// App headers
#include "pgAdmin3.h"
#include "frmMain.h"

#define CTL_STATUSLIST 300

// Class declarations

class frmStatus : public pgDialog
{
public:
    frmStatus(frmMain *form, const wxString& _title, pgConn *conn);
    ~frmStatus();
    void Go();
    
private:
    void OnHelp(wxCommandEvent& ev);
    void OnCloseBtn(wxCommandEvent &event);
    void OnClose(wxCloseEvent &event);
    void OnRefresh(wxCommandEvent &event);
    void OnRefreshTimer(wxTimerEvent &event);
    void OnRateChange(wxCommandEvent &event);
    void OnRateChangeSpin(wxSpinEvent &event);
	void OnNotebookPageChanged(wxNotebookEvent& event);
    void OnTerminateBtn(wxCommandEvent &event);
    void OnCancelBtn(wxCommandEvent &event);
	void OnSelStatusItem(wxListEvent &event);
	void OnSelLockItem(wxListEvent &event);
	void OnLoadLogfile(wxCommandEvent &event);
    void OnRotateLogfile(wxCommandEvent &event);

    int fillLogfileCombo();
    void emptyLogfileCombo();

    void addLogFile(wxDateTime *dt, bool skipFirst);
    void addLogFile(const wxString &filename, const wxDateTime timestamp, int pid, long len, long &read, bool skipFirst);
    void addLogLine(const wxString &str, bool formatted=true);

	void checkConnection();
    
    frmMain *mainForm;
	wxStatusBar *statusBar;
    wxString logFormat;
    bool logHasTimestamp, logFormatKnown;
    int logFmtPos;

    wxDateTime logfileTimestamp, latestTimestamp;
    wxString logDirectory, logfileName;
    long logfilePid;
    bool showCurrent, isCurrent;

    pgConn *connection;
    long backend_pid;
    wxTimer *timer;
	bool loaded;
    long logfileLength;

    DECLARE_EVENT_TABLE();
};

#endif
