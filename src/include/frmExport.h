//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmExport.h - The export file dialogue
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMEXPORT_H
#define FRMEXPORT_H


class ctlSQLResult;
class pgSet;

#include "dlgClasses.h"

// Class declarations
class frmExport : public pgDialog
{
public:
    frmExport(wxWindow *parent);
    ~frmExport();

    bool Export(ctlSQLResult *data) { return Export(data, 0); }
    bool Export(pgSet *set) { return Export(0, set); }
    
private:
    void OnChange(wxCommandEvent &ev);
    void OnHelp(wxCommandEvent& ev);
    void OnOK(wxCommandEvent &ev);
    void OnCancel(wxCommandEvent &ev);
    void OnBrowseFile(wxCommandEvent &ev);
    bool Export(ctlSQLResult *data, pgSet *set);

    wxWindow *parent;

    DECLARE_EVENT_TABLE()
};

#endif
