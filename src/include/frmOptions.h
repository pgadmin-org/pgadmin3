//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmOptions.h - The main options dialogue
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMOPTIONS_H
#define FRMOPTIONS_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"

// Class declarations
class frmOptions : public wxDialog
{
public:
    frmOptions(wxFrame *parent);
    ~frmOptions();
    
private:
    void OnOK(wxCommandEvent &ev);
    void OnCancel(wxCommandEvent &ev);
    void OnBrowseLogFile(wxCommandEvent &ev);
    DECLARE_EVENT_TABLE()
};

#endif
