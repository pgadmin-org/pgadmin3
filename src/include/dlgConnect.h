//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgConnect.h - Connect to a database
//
//////////////////////////////////////////////////////////////////////////

#ifndef DLGCONNECT_H
#define DLGCONNECT_H

#include "dlgClasses.h"

// Class declarations
class dlgConnect : public DialogWithHelp
{
public:
    dlgConnect(frmMain *form, const wxString& description, bool needPwd);
    ~dlgConnect();
    wxString GetHelpPage() const;


    wxString GetPassword();
    bool GetStorePwd();
    int Go();
    
private:
    void OnOK(wxCommandEvent& ev);
    void OnCancel(wxCommandEvent& ev);
    DECLARE_EVENT_TABLE()
};

#endif
