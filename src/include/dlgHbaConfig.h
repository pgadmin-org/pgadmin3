//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgHbaConfig.h - Configure setting
//
//////////////////////////////////////////////////////////////////////////

#ifndef __DLGHBACONFIG_H
#define __DLGHBACONFIG_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"

#include "pgConfig.h"

// Class declarations
class dlgHbaConfig : public DialogWithHelp
{
public:
    dlgHbaConfig(pgFrame *parent, pgHbaConfigLine *line, pgConn *conn);
    ~dlgHbaConfig();
    wxString GetHelpPage() const;

    int Go();
    
private:
    pgHbaConfigLine *line;

    void OnOK(wxCommandEvent& ev);
    void OnCancel(wxCommandEvent& ev);
    void OnChange(wxCommandEvent& ev);
    void OnAddDatabase(wxCommandEvent& ev);
    void OnAddUser(wxCommandEvent& ev);
    void OnAddValue(wxCommandEvent& ev);

    wxString database, user;

    bool databaseAdding, userAdding;

    DECLARE_EVENT_TABLE()
};

#endif
