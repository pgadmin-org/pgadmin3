//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
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

// App headers
#include "pgAdmin3.h"
#include "frmMain.h"

#define CTL_STATUSLIST 300

// Class declarations

class frmStatus : public wxDialog
{
public:
    frmStatus(frmMain *form, const wxString& _title, pgConn *conn, const wxPoint& pos, const wxSize& size);
    ~frmStatus();
    void Go();
    
private:
    void OnHelp(wxCommandEvent& ev);
    void OnClose(wxCommandEvent &event);
    void OnRefresh(wxCommandEvent &event);
    void OnRateChange(wxCommandEvent &event);
    frmMain *mainForm;
    pgConn *connection;
    long backend_pid;
    wxTimer *timer;

    DECLARE_EVENT_TABLE();
};

#endif
