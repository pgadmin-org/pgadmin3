//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmGrantWizard.h - Grant Wizard Dialogue
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMGRANTWIZARD_H
#define FRMGRANTWIZARD_H

// wxWindows headers
#include <wx/wx.h>


// App headers
#include "pgAdmin3.h"
#include "frmMain.h"


class ctlSecurityPanel;


class frmGrantWizard : public ExecutionDialog
{
public:
    frmGrantWizard(frmMain *form, pgObject *_object);
    ~frmGrantWizard();

    void Go();
    wxString GetSql();
    wxString GetHelpPage() const;
    
private:

    void OnPageSelect(wxNotebookEvent& event);
    void OnCheckAll(wxCommandEvent &event);
    void OnUncheckAll(wxCommandEvent &event);

    void AddObjects(pgCollection *collection);

    wxArrayPtrVoid objectArray;
    ctlSQLBox *sqlPane;
    wxNotebook *nbNotebook;
    ctlSecurityPanel *securityPage;

    DECLARE_EVENT_TABLE()
};

#endif
