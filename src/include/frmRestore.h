//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmRestore.h - Restore database dialogue
//
//////////////////////////////////////////////////////////////////////////


#ifndef FRMRESTORE_H
#define FRMRESTORE_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "frmMain.h"

// Class declarations


class frmRestore : public ExternProcessDialog
{
public:
    frmRestore(frmMain *_form, pgObject *_object);
    ~frmRestore();

    void Go();
    wxString GetDisplayCmd(int step);
    wxString GetCmd(int step);
    
private:
    wxString GetHelpPage() const;
    void OnChange(wxCommandEvent &ev);
    void OnSelectFilename(wxCommandEvent &ev);
    void OnView(wxCommandEvent &ev);
    void OnOK(wxCommandEvent &ev);
    void OnEndProcess(wxProcessEvent& event);

    wxString getCmdPart1();
    wxString getCmdPart2(int step);

    frmMain *form;
    pgObject *object;
    bool viewRunning;

    DECLARE_EVENT_TABLE()
};

#endif
