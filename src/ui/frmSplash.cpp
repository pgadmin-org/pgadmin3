//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
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

// Copyright text
#include "copyright.h"
#include "version.h"

BEGIN_EVENT_TABLE(frmSplash, wxFrame)
    EVT_PAINT(frmSplash::OnPaint)
END_EVENT_TABLE()

frmSplash::frmSplash(wxFrame *parent)
: wxFrame(parent, -1, wxT(""), wxDefaultPosition, wxDefaultSize, wxFRAME_TOOL_WINDOW)
{
    
    wxLogInfo(wxT("Creating a splash screen"));

    // Icon
    SetIcon(wxIcon(pgAdmin3_xpm));

    // Image
    splash = wxBitmap(splash_xpm);
    
    SetClientSize(splash.GetWidth(), splash.GetHeight());

    Center();
}


frmSplash::~frmSplash()
{
    wxLogInfo(wxT("Destroying a splash screen"));
}


void frmSplash::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    int y=SPLASH_Y0;
    wxFont fnt(*wxNORMAL_FONT);
    fnt.SetPointSize(SPLASH_FONTSIZE);


    wxPaintDC dc(this);
	dc.DrawBitmap(splash, 0, 0);
    dc.SetTextForeground(wxColour(255, 255, 255));
    dc.SetFont(fnt);

    dc.DrawText(VERSION_WITHOUT_DATE, SPLASH_X0, y);
    y += SPLASH_OFFS;
    dc.DrawText(COPYRIGHT, SPLASH_X0, y);
    y += SPLASH_OFFS;
    dc.DrawText(LICENSE, SPLASH_X0, y);
}
