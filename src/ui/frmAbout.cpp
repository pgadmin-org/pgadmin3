//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmAbout.cpp - About Box
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/image.h>

// App headers
#include "pgAdmin3.h"
#include "frmAbout.h"

// Icons
#include "images/pgAdmin3.xpm"

// Splash
#include "images/splash.xpm"

// Copyright text
#include "copyright.h"
#include "version.h"


BEGIN_EVENT_TABLE(frmAbout, wxDialog)
EVT_PAINT(frmAbout::OnPaint)
END_EVENT_TABLE()

frmAbout::frmAbout(wxFrame *parent)
: wxDialog(parent, -1, APPNAME_L, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxCAPTION | wxDIALOG_MODAL | wxSYSTEM_MENU | wxSTAY_ON_TOP)
{

    wxLogInfo(wxT("Creating an about box"));

    // Icon
    SetIcon(wxIcon(pgAdmin3_xpm));

    // Image
    about = wxBitmap(splash_xpm);

    SetClientSize(about.GetWidth(), about.GetHeight());

	this->Center();
}


frmAbout::~frmAbout()
{
    wxLogInfo(wxT("Destroying an about box"));
}


void frmAbout::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    int y=SPLASH_Y0;
    wxFont fnt(*wxNORMAL_FONT);
    fnt.SetPointSize(SPLASH_FONTSIZE);

    wxPaintDC dc(this);
	dc.DrawBitmap(about, 0, 0);
    dc.SetTextForeground(wxColour(255, 255, 255));
    dc.SetFont(fnt);

    dc.DrawText(VERSION_WITH_DATE, SPLASH_X0, y);
    y += SPLASH_OFFS;
    dc.DrawText(COPYRIGHT, SPLASH_X0, y);
    y += SPLASH_OFFS;
    dc.DrawText(LICENSE, SPLASH_X0, y);
}
