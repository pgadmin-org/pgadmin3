//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
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
#include <wx/notebook.h>

#include "dlg/dlgClasses.h"
#include "utils/factory.h"

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
    void OnRateChange(wxScrollEvent &event);
	void OnNotebookPageChanged(wxNotebookEvent& event);
    void OnTerminateBtn(wxCommandEvent &event);
    void OnCancelBtn(wxCommandEvent &event);
	void OnSelStatusItem(wxListEvent &event);
	void OnSelLockItem(wxListEvent &event);
    void OnSelXactItem(wxListEvent &event);
	void OnLoadLogfile(wxCommandEvent &event);
    void OnRotateLogfile(wxCommandEvent &event);
    void OnCommit(wxCommandEvent &event);
    void OnRollback(wxCommandEvent &event);

    int fillLogfileCombo();
    void emptyLogfileCombo();

    void addLogFile(wxDateTime *dt, bool skipFirst);
    void addLogFile(const wxString &filename, const wxDateTime timestamp, long len, long &read, bool skipFirst);
    void addLogLine(const wxString &str, bool formatted=true);

	void checkConnection();
    
    frmMain *mainForm;
    wxButton *btnCancelSt, *btnTerminateSt,*btnCancelLk, *btnTerminateLk, *btnCommit, *btnRollback;

    wxString logFormat;
    bool logHasTimestamp, logFormatKnown;
    int logFmtPos;

    wxDateTime logfileTimestamp, latestTimestamp;
    wxString logDirectory, logfileName;
    bool showCurrent, isCurrent;

    pgConn *connection;
    long backend_pid;
    wxTimer *timer;
	bool loaded;
    long logfileLength;
    int xactPage, logPage;

    DECLARE_EVENT_TABLE()
};


class serverStatusFactory : public actionFactory
{
public:
    serverStatusFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};

#endif
