//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
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
    frmConnect(wxFrame *form, const wxString& server = wxString(""), const wxString& database = wxString(""), const wxString& username = wxString(""), int port = 5432);
    ~frmConnect();

    wxString GetServer();
    wxString GetDatabase();
    wxString GetUsername();
    wxString GetPassword();
    long GetPort();
    void LockFields();
    int Go();
    
private:
    bool TransferDataFromWindow();
    DECLARE_EVENT_TABLE()
};

#endif
