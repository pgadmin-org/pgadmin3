//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgPgpassConfig.h - Configure setting
//
//////////////////////////////////////////////////////////////////////////

#ifndef __DLGPGPASSCONFIG_H
#define __DLGPGPASSCONFIG_H

#include "dlgClasses.h"
#include "utils/pgconfig.h"

// Class declarations
class dlgPgpassConfig : public DialogWithHelp
{
public:
    dlgPgpassConfig(pgFrame *parent, pgPassConfigLine *line);
    ~dlgPgpassConfig();
    wxString GetHelpPage() const;

    int Go();
    
private:
    pgPassConfigLine *line;

    void OnOK(wxCommandEvent& ev);
    void OnCancel(wxCommandEvent& ev);
    void OnChange(wxCommandEvent& ev);

    wxString database, user;

    bool databaseAdding, userAdding;

    DECLARE_EVENT_TABLE()
};

#endif
