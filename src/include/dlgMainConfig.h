//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgMainConfig.h - Configure setting
//
//////////////////////////////////////////////////////////////////////////

#ifndef __DLGMAINCONFIG_H
#define __DLGMAINCONFIG_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"

#include "pgconfig.h"

// Class declarations
class dlgMainConfig : public DialogWithHelp
{
public:
    dlgMainConfig(pgFrame *parent, pgSettingItem *item);
    ~dlgMainConfig();
    wxString GetHelpPage() const;

    int Go();
    
private:
    pgSettingItem *item;
    wxString GetValue();

    void OnOK(wxCommandEvent& ev);
    void OnCancel(wxCommandEvent& ev);
    void OnChange(wxCommandEvent& ev);
    DECLARE_EVENT_TABLE()
};

#endif
