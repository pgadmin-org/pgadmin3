//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmOptions.h - The main options dialogue
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMOPTIONS_H
#define FRMOPTIONS_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "../../pgAdmin3.h"

// Class declarations
class frmOptions : public wxDialog
{
public:
    frmOptions(wxFrame *parent);
    ~frmOptions();
    
private:
    void OK();
    void Cancel();
    void BrowseLogFile();
    DECLARE_EVENT_TABLE()
};

#endif
