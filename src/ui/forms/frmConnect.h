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
#include "../../pgAdmin3.h"
#include "../../db/pg/pgConn.h"

// Class declarations
class frmConnect : public wxDialog
{
public:
    frmConnect(wxFrame *parent);
    ~frmConnect();
    wxString GetServer();
    wxString GetDatabase();
    wxString GetUsername();
    wxString GetPassword();
    long GetPort();
    
private:
    void OK();
    void Cancel();
    DECLARE_EVENT_TABLE()
};

#endif
