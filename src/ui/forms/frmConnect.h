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
#include "../../schema/pg/pgServer.h"

// Class declarations
class frmConnect : public wxDialog
{
public:
    frmConnect(pgServer *parent, const wxString& szServer = wxString(""), const wxString& szDatabase = wxString(""), const wxString& szUsername = wxString(""), int iPort = 5432);
    ~frmConnect();
    void Init();
    wxString GetServer();
    wxString GetDatabase();
    wxString GetUsername();
    wxString GetPassword();
    long GetPort();
    void LockFields();
    
private:
    pgServer *objParent;
    void OnOK();
    void OnCancel();
    DECLARE_EVENT_TABLE()
};

#endif
