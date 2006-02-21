//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: dlgConnect.h 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
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
