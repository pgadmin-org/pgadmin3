//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmSplash.cpp - Splash Screen
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/image.h>

// App headers
#include "pgAdmin3.h"
#include "frmSplash.h"

// Icons
#include "images/pgAdmin3.xpm"

// Splash
#include "images/splash.xpm"

BEGIN_EVENT_TABLE(frmSplash, wxFrame)
    EVT_PAINT(frmSplash::OnPaint)
END_EVENT_TABLE()

frmSplash::frmSplash(wxFrame *parent)
: wxFrame(parent, -1, "", wxDefaultPosition, wxDefaultSize, wxFRAME_TOOL_WINDOW)
{
    
    wxLogInfo(wxT("Creating a splash screen"));

    // Icon
    SetIcon(wxIcon(pgAdmin3_xpm));

    // Image
    splash = wxBitmap(splash_xpm);
    
    SetClientSize(splash.GetWidth(), splash.GetHeight());
    wxString version = wxT(" "VERSION" ("__DATE__") ");

    (void)new wxPanel(this, -1, wxPoint(7,95), wxSize(0,0));
	(void)new wxStaticText(this, -1, version, wxPoint(7,95));
    Center();
}

frmSplash::~frmSplash()
{
    wxLogInfo(wxT("Destroying a splash screen"));
}

void frmSplash::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    dc.DrawBitmap(splash, 0, 0);
}
