//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmUpgradeWizard.h - the Upgrade Wizard
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMUPGRADEWIZARD_H
#define FRMUPGRADEWIZARD_H

// wxWindows headers
#include <wx/wx.h>
#include <wx/image.h>
#include <wx/listctrl.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>


// App headers
#include "../../pgAdmin3.h"

// Class declarations
class frmUpgradeWizard : public wxDialog
{
public:
    frmUpgradeWizard(wxFrame *parent);
    ~frmUpgradeWizard();
    void OnPaint(wxPaintEvent&);
    
private:
    wxBitmap imgAbout;
    wxListCtrl* lvVersions;
    wxTextCtrl* txtpgAdminwebsite;
    wxCheckBox* chkAuto;
    DECLARE_EVENT_TABLE()

};

#endif