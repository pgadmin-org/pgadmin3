//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmConnect.h - Connect to a database
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMCONNECT_H
#define FRMCONNECT_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgConn.h"
#include "pgServer.h"


// Class declarations
class frmConnect : public wxDialog
{
public:
    frmConnect(wxFrame *form, const wxString& server = wxT(""), const wxString& description=wxT(""),
        const wxString& database = wxT(""), const wxString& username = wxT(""), int port = 5432, bool trusted=false, long ssl=0);
    ~frmConnect();


    wxString GetDescription();
    wxString GetServer();
    wxString GetDatabase();
    wxString GetUsername();
    wxString GetPassword();
    bool GetTrusted();
    long GetPort();
    long GetSSL();
    void LockFields();
    int Go();
    
private:
    bool TransferDataFromWindow();
    void OnHelp(wxCommandEvent& ev);
    void OnOK(wxCommandEvent& ev);
    void OnCancel(wxCommandEvent& ev);
    void OnTrustChange(wxCommandEvent& ev);
    DECLARE_EVENT_TABLE()
};

#endif
