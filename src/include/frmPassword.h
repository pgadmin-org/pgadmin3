//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
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
class frmPassword : public pgDialog
{
public:
    frmPassword(wxFrame *parent, pgObject *obj);
    ~frmPassword();
    
private:
    pgServer *server;
    void OnHelp(wxCommandEvent& ev);
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    DECLARE_EVENT_TABLE()
};


class passwordFactory : public actionFactory
{
public:
    passwordFactory(wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(pgFrame *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};

#endif
