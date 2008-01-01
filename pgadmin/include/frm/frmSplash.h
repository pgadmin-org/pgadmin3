//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmSplash.h - Splash Screen
//
//////////////////////////////////////////////////////////////////////////

#ifndef SPLASH_H
#define SPLASH_H

// Class declarations
class frmSplash : public wxFrame
{
public:
    frmSplash(wxFrame *parent);
    void OnPaint(wxPaintEvent&);
    
private:
    void SetWindowShape();
    void OnWindowCreate(wxWindowCreateEvent& WXUNUSED(evt));

    wxBitmap splash;
    DECLARE_EVENT_TABLE()
};

#endif
