//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmAbout.h - About Box
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMABOUT_H
#define FRMABOUT_H

// wxWindows headers
#include <wx/wx.h>
#include <wx/image.h>

// App headers
#include "pgAdmin3.h"

// Class declarations
class frmAbout : public wxDialog
{
public:
    frmAbout(wxFrame *parent);
    ~frmAbout();
    void OnPaint(wxPaintEvent&);
    
private:
    wxBitmap about;
    DECLARE_EVENT_TABLE()
};

#endif
