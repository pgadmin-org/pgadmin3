//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmExport.h - The export file dialogue
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMEXPORT_H
#define FRMEXPORT_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"

class ctlSQLResult;
// Class declarations
class frmExport : public wxDialog
{
public:
    frmExport(ctlSQLResult *parent);
    ~frmExport();
    
private:
    void OnChange(wxCommandEvent &ev);
    void OnHelp(wxCommandEvent& ev);
    void OnOK(wxCommandEvent &ev);
    void OnCancel(wxCommandEvent &ev);
    void OnBrowseFile(wxCommandEvent &ev);

    ctlSQLResult *data;

    DECLARE_EVENT_TABLE()
};

#endif
