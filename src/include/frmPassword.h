//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
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
    void SetServer(pgServer *objNewServer);
    
private:
    pgServer *objServer;
    void OnOK();
    void OnCancel();
    DECLARE_EVENT_TABLE()
};

#endif
