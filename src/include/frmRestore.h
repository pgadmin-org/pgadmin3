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
    frmRestore(frmMain *form, pgObject *_object);
    ~frmRestore();

    void Go();
    wxString GetDisplayCmd();
    wxString GetCmd();
    
private:
    wxString GetHelpPage() const;
    void OnChangeName(wxCommandEvent &ev);
    void OnSelectFilename(wxCommandEvent &ev);
    void OnChangePlain(wxCommandEvent &ev);
    wxString getCmdPart1();
    wxString getCmdPart2();

    pgObject *object;

    DECLARE_EVENT_TABLE()
};

#endif
