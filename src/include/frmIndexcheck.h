//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
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
class frmIndexcheck : public ExecutionDialog
{
public:
    frmIndexcheck(frmMain *form, pgObject *_object);
    ~frmIndexcheck();

    wxString GetHelpPage() const;
    wxString GetSql();
    void Go();
    
private:
    void OnPageSelect(wxNotebookEvent& event);
    void OnCheckAll(wxCommandEvent &event);
    void OnUncheckAll(wxCommandEvent &event);

    void AddObjects(const wxString &where);
    
    ctlSQLBox *sqlPane;
    wxNotebook *nbNotebook;

    DECLARE_EVENT_TABLE()
};

#endif
