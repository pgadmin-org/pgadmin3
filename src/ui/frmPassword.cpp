//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmPassword.cpp - Change password
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/xrc/xmlres.h>


// App headers
#include "pgAdmin3.h"
#include "frmPassword.h"

// Icons
#include "images/pgAdmin3.xpm"

BEGIN_EVENT_TABLE(frmPassword, wxDialog)
  EVT_BUTTON (XRCID("btnOK"), frmPassword::OnOK)
  EVT_BUTTON (XRCID("btnCancel"), frmPassword::OnCancel)
END_EVENT_TABLE()

frmPassword::frmPassword(wxFrame *parent)
{

    wxLogInfo(wxT("Creating a change password dialogue"));

    wxXmlResource::Get()->LoadDialog(this, parent, "frmPassword"); 

    // Icon
    SetIcon(wxIcon(pgAdmin3_xpm));
    Center();
}

frmPassword::~frmPassword()
{
    wxLogInfo(wxT("Destroying a change password dialogue"));
}

void frmPassword::OnOK()
{
    this->Destroy();
}

void frmPassword::OnCancel()
{
    this->Destroy();
}
