//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
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
#include "frm/frmSplash.h"


// Copyright text
#include "copyright.h"
#include "version.h"

BEGIN_EVENT_TABLE(frmSplash, pgFrame)
    EVT_PAINT(frmSplash::OnPaint)
END_EVENT_TABLE()

frmSplash::frmSplash(wxFrame *parent)
: pgFrame(parent,  wxT(""), wxDefaultPosition, wxDefaultSize, wxNO_BORDER)
{
    
    wxLogInfo(wxT("Creating a splash screen"));

    appearanceFactory->SetIcons(this);
    splash = wxBitmap(appearanceFactory->GetSplashImage());
    
    SetClientSize(splash.GetWidth(), splash.GetHeight());

    Center();
}


frmSplash::~frmSplash()
{
    wxLogInfo(wxT("Destroying a splash screen"));
}


void frmSplash::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPoint pos=appearanceFactory->GetSplashTextPos();

    wxPaintDC dc(this);
	dc.DrawBitmap(splash, 0, 0);

    dc.SetTextForeground(appearanceFactory->GetSplashTextColour());
    dc.SetFont(appearanceFactory->GetSplashTextFont());

    dc.DrawText(VERSION_WITHOUT_DATE, pos);
    pos.y += appearanceFactory->GetSplashTextOffset();
    dc.DrawText(COPYRIGHT, pos);
    pos.y += appearanceFactory->GetSplashTextOffset();
    dc.DrawText(LICENSE, pos);
}
