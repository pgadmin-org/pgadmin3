//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmVacuum.h - Vacuum options selection dialogue
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMVACUUM_H
#define FRMVACUUM_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "frmMain.h"


// Class declarations
class frmVacuum : public wxDialog
{
public:
    frmVacuum(frmMain *form, wxString& info);
    ~frmVacuum();

    int Go();
    
private:
    void OnOK(wxCommandEvent& ev);
    void OnCancel(wxCommandEvent& ev);
    DECLARE_EVENT_TABLE()
};

#endif
