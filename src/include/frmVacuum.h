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
    frmVacuum(frmMain *form, pgObject *_object);
    ~frmVacuum();

    void Go();
    
private:
    void OnOK(wxCommandEvent& ev);
    void OnCancel(wxCommandEvent& ev);
    void OnClose(wxCloseEvent& event);
    void Abort();
    pgObject *object;
    pgQueryThread *thread;
    DECLARE_EVENT_TABLE()
};

#endif
