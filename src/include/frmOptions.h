//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
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

class frmMain;

// Class declarations
class frmOptions : public wxDialog
{
public:
    frmOptions(frmMain *parent);
    ~frmOptions();
    
private:
    frmMain *mainForm;
    wxFont currentFont;

    void OnBrowseLogFile(wxCommandEvent &ev);
    void OnFontSelect(wxCommandEvent &ev);
    void OnOK(wxCommandEvent &ev);
    void OnCancel(wxCommandEvent &ev);
    void OnHelp(wxCommandEvent &ev);
    DECLARE_EVENT_TABLE()
};

#endif
