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
#include "../../pgAdmin3.h"
#include "frmSplash.h"

BEGIN_EVENT_TABLE(frmSplash, wxFrame)
EVT_PAINT(frmSplash::OnPaint)
END_EVENT_TABLE()

frmSplash::frmSplash(wxFrame *parent)
: wxFrame(parent, -1, "", wxDefaultPosition, wxDefaultSize, wxFRAME_TOOL_WINDOW)
{
  imgSplash = wxBitmap("images/splash.jpg", wxBITMAP_TYPE_JPEG);
  SetClientSize(imgSplash.GetWidth(), imgSplash.GetHeight());
  wxString szVersion = "Version: ";
  szVersion.Append(VERSION);
  wxStaticText *txtVersion = new wxStaticText(this, -1, szVersion, wxPoint(7,156), wxDefaultSize, wxTRANSPARENT_WINDOW);
  Center();
}

void frmSplash::OnPaint(wxPaintEvent& WXUNUSED(event))
{
  wxPaintDC dc(this);
  dc.DrawBitmap(imgSplash, 0, 0);
}
