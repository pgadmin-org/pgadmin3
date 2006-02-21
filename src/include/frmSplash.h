//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: frmSplash.h 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmSplash.h - Splash Screen
//
//////////////////////////////////////////////////////////////////////////

#ifndef SPLASH_H
#define SPLASH_H

#include "dlgClasses.h"
#include "base/factory.h"

// Class declarations
class frmSplash : public pgFrame
{
public:
    frmSplash(wxFrame *parent);
    ~frmSplash();
    void OnPaint(wxPaintEvent&);
    
private:
    wxBitmap splash;
    DECLARE_EVENT_TABLE()
};

#endif
