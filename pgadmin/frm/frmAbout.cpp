//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmAbout.cpp - About Box
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/image.h>

// App headers
#include "pgAdmin3.h"
#include "frm/frmAbout.h"

// Copyright text
#include "copyright.h"
#include "version.h"
#include "svnversion.h"

#define VERSION_WITH_DATE_AND_SVN       wxT("Version ") VERSION_STR wxT(" (") __TDATE__ wxT(", rev: ") wxT(VERSION_SVN) wxT(")")


BEGIN_EVENT_TABLE(frmAbout, wxFrame)
	EVT_PAINT(frmAbout::OnPaint)

	EVT_KEY_UP(frmAbout::OnKeyUp)
	EVT_LEFT_DOWN(frmAbout::OnLeftDown)
#ifdef __WXGTK__
	EVT_WINDOW_CREATE(frmAbout::OnWindowCreate)
#endif
END_EVENT_TABLE()

frmAbout::frmAbout(wxFrame *parent)
	: wxFrame(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(100, 100), 0 | wxFRAME_SHAPED | wxSIMPLE_BORDER | wxFRAME_NO_TASKBAR | wxSTAY_ON_TOP)
{
	appearanceFactory->SetIcons(this);
	about = appearanceFactory->GetSplashImage();

	SetClientSize(about.GetWidth(), about.GetHeight());

#ifndef __WXGTK__
	SetWindowShape();
#endif

	CenterOnParent();
}

void frmAbout::OnLeftDown(wxMouseEvent &WXUNUSED(evt))
{
	this->Close();
}

void frmAbout::OnKeyUp(wxKeyEvent &evt)
{
	if (evt.GetKeyCode() == WXK_ESCAPE)
		this->Close();
}


void frmAbout::SetWindowShape()
{
	wxRegion region(about);
	SetShape(region);
}

void frmAbout::OnPaint(wxPaintEvent &WXUNUSED(event))
{
	wxPoint pos = appearanceFactory->GetSplashTextPos();

	wxPaintDC dc(this);
	dc.DrawBitmap(about, 0, 0, true);
	dc.SetTextForeground(appearanceFactory->GetSplashTextColour());
	dc.SetFont(appearanceFactory->GetSplashTextFont());

	if (appearanceFactory->IsBranded())
	{
		dc.DrawText(_("This program is based on pgAdmin III"), pos);
		pos.y += appearanceFactory->GetSplashTextOffset();
	}
	dc.DrawText(VERSION_WITH_DATE_AND_SVN, pos);
	pos.y += appearanceFactory->GetSplashTextOffset();
	dc.DrawText(COPYRIGHT, pos);
	pos.y += appearanceFactory->GetSplashTextOffset();
	dc.DrawText(LICENSE, pos);
}

void frmAbout::OnWindowCreate(wxWindowCreateEvent &WXUNUSED(evt))
{
	SetWindowShape();
}

aboutFactory::aboutFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : actionFactory(list)
{
	mnu->Append(id, _("&About"), _("Show about dialog."));
}


wxWindow *aboutFactory::StartDialog(frmMain *form, pgObject *obj)
{
	frmAbout *frm = new frmAbout((wxFrame *)form);
	frm->Show();
	return 0;
}
