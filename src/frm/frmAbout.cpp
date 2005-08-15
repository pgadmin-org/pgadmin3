//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
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

    appearanceFactory->SetIcons(this);
    about = wxBitmap(appearanceFactory->GetSplashImage());

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


aboutFactory::aboutFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar) : actionFactory(list)
{
    mnu->Append(id, _("&About..."), _("Show about dialog."));
}


wxWindow *aboutFactory::StartDialog(frmMain *form, pgObject *obj)
{
    frmAbout *frm=new frmAbout((wxFrame*)form);
    frm->Show();
    return frm;
}
