//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
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
#include "dlgClasses.h"

// Class declarations
class frmMaintenance : public ExecutionDialog
{
public:
    frmMaintenance(frmMain *form, pgObject *_object);
    ~frmMaintenance();
    wxString GetSql();

    void Go();
    
private:
    wxString GetHelpPage() const;
    void OnAction(wxCommandEvent& ev);

    DECLARE_EVENT_TABLE()
};

#endif
