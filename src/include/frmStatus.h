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
	void OnSelStatusItem(wxCommandEvent &event);


    void addLog(const wxString &str);
    
    frmMain *mainForm;
    wxString logFormat;
    bool logHasTimestamp, logFormatKnown;
    int logFmtPos;

    pgConn *connection;
    long backend_pid;
    wxTimer *timer;
	bool loaded;
    long logFileLength;

    DECLARE_EVENT_TABLE();
};

#endif
