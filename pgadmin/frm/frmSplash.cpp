//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
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

BEGIN_EVENT_TABLE(frmSplash, wxFrame)
	EVT_PAINT(frmSplash::OnPaint)

#ifdef __WXGTK__
	EVT_WINDOW_CREATE(frmSplash::OnWindowCreate)
#endif
END_EVENT_TABLE()

frmSplash::frmSplash(wxFrame *parent)
#ifndef __WXDEBUG__
	: wxFrame((wxFrame *)NULL, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(100, 100), 0 | wxFRAME_SHAPED | wxSIMPLE_BORDER | wxFRAME_NO_TASKBAR | wxSTAY_ON_TOP)
#else
	: wxFrame((wxFrame *)NULL, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(100, 100), 0 | wxFRAME_SHAPED | wxSIMPLE_BORDER | wxFRAME_NO_TASKBAR)
#endif
{
	appearanceFactory->SetIcons(this);
	splash = appearanceFactory->GetSplashImage();

	SetClientSize(splash.GetWidth(), splash.GetHeight());

#ifndef __WXGTK__
	SetWindowShape();
#endif

	CenterOnScreen();
}

void frmSplash::SetWindowShape()
{
	wxRegion region(splash);
	SetShape(region);
}

void frmSplash::OnPaint(wxPaintEvent &WXUNUSED(event))
{
	wxPoint pos = appearanceFactory->GetSplashTextPos();

	wxPaintDC dc(this);
	dc.DrawBitmap(splash, 0, 0, true);

	dc.SetTextForeground(appearanceFactory->GetSplashTextColour());
	dc.SetFont(appearanceFactory->GetSplashTextFont());

	if (appearanceFactory->IsBranded())
	{
		dc.DrawText(_("This program is based on pgAdmin III"), pos);
		pos.y += appearanceFactory->GetSplashTextOffset();
	}
	dc.DrawText(VERSION_WITHOUT_DATE, pos);
	pos.y += appearanceFactory->GetSplashTextOffset();
	dc.DrawText(COPYRIGHT, pos);
	pos.y += appearanceFactory->GetSplashTextOffset();
	dc.DrawText(LICENSE, pos);
}

void frmSplash::OnWindowCreate(wxWindowCreateEvent &WXUNUSED(evt))
{
	SetWindowShape();
}
