//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
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
#include "frm/frmAbout.h"

// Copyright text
#include "copyright.h"
#include "version.h"
#include "svnversion.h"

#define VERSION_WITH_DATE_AND_SVN       wxT("Version ") VERSION_STR wxT(" (") __TDATE__ wxT(", rev: ") wxT(VERSION_SVN) wxT(")")


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
    wxPoint pos=appearanceFactory->GetSplashTextPos();

    wxPaintDC dc(this);
	dc.DrawBitmap(about, 0, 0);
    dc.SetTextForeground(appearanceFactory->GetSplashTextColour());
    dc.SetFont(appearanceFactory->GetSplashTextFont());

    dc.DrawText(VERSION_WITH_DATE_AND_SVN, pos);
    pos.y += appearanceFactory->GetSplashTextOffset();
    dc.DrawText(COPYRIGHT, pos);
    pos.y += appearanceFactory->GetSplashTextOffset();
    dc.DrawText(LICENSE, pos);
}


aboutFactory::aboutFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar) : actionFactory(list)
{
    mnu->Append(id, _("&About"), _("Show about dialog."));
}


wxWindow *aboutFactory::StartDialog(frmMain *form, pgObject *obj)
{
    frmAbout *frm=new frmAbout((wxFrame*)form);
    frm->Show();
    return 0;
}
