//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmUpgradeWizard.cpp  - the Upgrade Wizard
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/image.h>
#include <wx/notebook.h>
#include <wx/listctrl.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>

// App headers
#include "../../pgAdmin3.h"
#include "frmUpgradeWizard.h"

// Icons
#include "../../images/pgAdmin3.xpm"

// Splash
#include "../../images/splash.xpm"
BEGIN_EVENT_TABLE(frmUpgradeWizard, wxDialog)
EVT_PAINT(frmUpgradeWizard::OnPaint)
END_EVENT_TABLE()

frmUpgradeWizard::frmUpgradeWizard(wxFrame *parent)
: wxDialog(parent, -1, "Upgrade Wizard", wxDefaultPosition, wxSize(500,500), wxCAPTION | wxRESIZE_BORDER | wxSTAY_ON_TOP)
{

    wxLogDebug(wxT("Creating new instance of the Upgrade Wizard"));

    // Icon
    SetIcon(wxIcon(pgAdmin3_xpm));
    //New Notebook 
    wxNotebook* nbUpgradeWizard = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize, wxNB_BOTTOM);
    //New Panels
    wxPanel *pnlPage1 = new wxPanel(nbUpgradeWizard, -1, wxDefaultPosition, wxDefaultSize);
    wxPanel *pnlPage2 = new wxPanel(nbUpgradeWizard, -1, wxDefaultPosition, wxDefaultSize);
    
    //Stick the Panels Down	
    nbUpgradeWizard->AddPage(pnlPage1, wxT("Step 1"));
    nbUpgradeWizard->AddPage(pnlPage2, wxT("Step 2"));
	

	
    txtLabel = new wxTextCtrl(pnlPage1,-1,wxT("Test"), wxPoint(10,400), wxSize(240, 250));
    (void)new wxStaticText( pnlPage1, -1,
                            "Click on the Next button to see possible updates to pgAdmin3 and it's related\n"
                            "components such as PostgreSQL or psqlODBC. You must be connected to the internet for\n"
                            "the Upgrade Wizard to work correctly.\n"
                            "\n"
                            "You may also alter the Upgrade Wizard's settings on this page\n",
                            wxPoint(10,10), wxSize(500, 110));
			    
    chkAuto = new wxCheckBox(pnlPage1,- 1, wxT("Automatically run the upgrade wizard every "), wxPoint(10,500));
    
    wxComboBox *cboFrequency = new wxComboBox(pnlPage1,-1,"", wxPoint(100,200), wxDefaultSize);
    cboFrequency->Append("Day");
    cboFrequency->Append("Week");
    cboFrequency->Append("Month");
    cboFrequency->Append("Year");
    
    
    lvVersions = new wxListCtrl(pnlPage2, -1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxSIMPLE_BORDER);

	
    //nbUpgradeWizard->AddPage(lvVersions, "Current State");
    //// Image
    //imgAbout = wxBitmap(splash_xpm);

    //    SetClientSize(imgAbout.GetWidth(), imgAbout.GetHeight());
    //  wxString szVersion = wxT("Version: ");
    //szVersion.Append(VERSION);
    //wxStaticText *txtVersion = new wxStaticText(this, -1, szVersion, wxPoint(7,156), wxDefaultSize, wxTRANSPARENT_WINDOW);
    //Center();
}

frmUpgradeWizard::~frmUpgradeWizard()
{
    wxLogDebug(wxT("Destroying the Upgrade Wizard box"));
}

void frmUpgradeWizard::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    dc.DrawBitmap(imgAbout, 0, 0);
}
