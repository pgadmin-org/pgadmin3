//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmPassword.h - Change password
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMPASSWORD_H
#define FRMPASSWORD_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgServer.h"

// Class declarations
class frmPassword : public wxDialog
{
public:
    frmPassword(wxFrame *parent);
    ~frmPassword();
    void SetServer(pgServer *newServer);
    
private:
    pgServer *server;
    void OnHelp(wxCommandEvent& ev);
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    DECLARE_EVENT_TABLE()
};

#endif
