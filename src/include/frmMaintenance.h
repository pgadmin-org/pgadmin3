//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmMaintenance.h - Maintenance options selection dialogue
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMMAINTENANCE_H
#define FRMMAINTENANCE_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "frmMain.h"


// Class declarations
class frmMaintenance : public DialogWithHelp
{
public:
    frmMaintenance(frmMain *form, pgObject *_object);
    ~frmMaintenance();

    void Go();
    
private:
    wxString GetHelpPage() const;
    void OnAction(wxCommandEvent& ev);
    void OnOK(wxCommandEvent& ev);
    void OnCancel(wxCommandEvent& ev);
    void OnClose(wxCloseEvent& event);
    void Abort();
    pgObject *object;
    pgQueryThread *thread;
    DECLARE_EVENT_TABLE()
};

#endif
