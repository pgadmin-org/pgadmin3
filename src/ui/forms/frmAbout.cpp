//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmAbout.cpp - About Box
//
//////////////////////////////////////////////////////////////////////////

// wxWindows Headers
#include <wx/wx.h>
#include <wx/image.h>

// App headers
#include "../pgAdmin3.h"
#include "frmAbout.h"

BEGIN_EVENT_TABLE(frmAbout, wxDialog)
EVT_PAINT(frmAbout::OnPaint)
END_EVENT_TABLE()

frmAbout::frmAbout()
: wxDialog((wxFrame *)NULL, -1, "About pgAdmin III", wxDefaultPosition, wxDefaultSize, wxCAPTION | wxDIALOG_MODAL | wxSYSTEM_MENU | wxSTAY_ON_TOP)
{
  imgAbout = wxBitmap("images/splash.jpg", wxBITMAP_TYPE_JPEG);
  SetClientSize(imgAbout.GetWidth(), imgAbout.GetHeight());
  wxString szVersion = "Version: ";
  szVersion.Append(VERSION);
  wxStaticText *txtVersion = new wxStaticText(this, -1, szVersion, wxPoint(7,156), wxDefaultSize, wxTRANSPARENT_WINDOW);
  Center();
}

void frmAbout::OnPaint(wxPaintEvent& WXUNUSED(event))
{
  wxPaintDC dc(this);
  dc.DrawBitmap(imgAbout, 0, 0);
}
