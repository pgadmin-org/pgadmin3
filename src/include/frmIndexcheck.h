//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmIndexcheck.h - Index checker dialogue
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMINDEXCHECK_H
#define FRMINDEXCHECK_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "frmMain.h"


// Class declarations
class frmIndexcheck : public DialogWithHelp
{
public:
    frmIndexcheck(frmMain *form, pgObject *_object);
    ~frmIndexcheck();

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
