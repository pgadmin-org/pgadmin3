//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmVacuum.cpp - Vacuum options selection dialogue
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>


// App headers
#include "pgAdmin3.h"
#include "frmVacuum.h"
#include "sysLogger.h"

// Icons
#include "images/pgAdmin3.xpm"


BEGIN_EVENT_TABLE(frmVacuum, wxDialog)
    EVT_BUTTON (XRCID("btnOK"),       frmVacuum::OnOK)
    EVT_BUTTON (XRCID("btnCancel"),   frmVacuum::OnCancel)
END_EVENT_TABLE()


frmVacuum::frmVacuum(frmMain *form, wxString& info)
{
    wxLogInfo(wxT("Creating a vacuum dialogue for ") + info);

    wxXmlResource::Get()->LoadDialog(this, form, "frmVacuum");
    SetTitle(wxT("VACUUM ") + info);

    // Icon
    SetIcon(wxIcon(pgAdmin3_xpm));
    CenterOnParent();
}

frmVacuum::~frmVacuum()
{
    wxLogInfo(wxT("Destroying a vacuum dialogue"));
}

void frmVacuum::OnOK(wxCommandEvent& ev)
{
    int res = 0;

    if (XRCCTRL(*this, "chkFull", wxCheckBox)->GetValue()) res = res + 1;
    if (XRCCTRL(*this, "chkFreeze", wxCheckBox)->GetValue()) res = res + 2;
    if (XRCCTRL(*this, "chkAnalyse", wxCheckBox)->GetValue()) res = res + 4;

    EndModal(res);
    Destroy();
}

void frmVacuum::OnCancel(wxCommandEvent& ev)
{
    EndModal(-1);
    Destroy();
}

int frmVacuum::Go()
{
    // Set focus on the Password textbox and show modal
    XRCCTRL(*this, "chkFull", wxCheckBox)->SetFocus();
    return ShowModal();
}
