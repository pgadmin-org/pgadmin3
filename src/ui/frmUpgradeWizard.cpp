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
#include <wx/imaglist.h>
#include <wx/url.h>
#include <wx/stream.h>

// App headers
#include "pgAdmin3.h"
#include "frmUpgradeWizard.h"

// Icons
#include "../../images/pgAdmin3.xpm"
#include "../../images/unknown.xpm"
#include "../../images/upgrade.xpm"

BEGIN_EVENT_TABLE(frmUpgradeWizard, wxDialog)

END_EVENT_TABLE()

frmUpgradeWizard::frmUpgradeWizard(wxFrame *parent)
: wxDialog(parent, -1, "Upgrade Wizard", wxDefaultPosition, wxSize(450,255), wxCAPTION | wxDIALOG_MODAL | wxSYSTEM_MENU | wxSTAY_ON_TOP)
{

    wxLogInfo(wxT("Creating new instance of the Upgrade Wizard"));

    // Icon
    SetIcon(wxIcon(pgAdmin3_xpm));
    //New Notebook 
    wxNotebook* nbUpgradeWizard = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize, wxNB_BOTTOM);
    //New Panels
    wxPanel *pnlPage1 = new wxPanel(nbUpgradeWizard, -1, wxDefaultPosition, wxDefaultSize);
    wxPanel *pnlPage2 = new wxPanel(nbUpgradeWizard, -1, wxDefaultPosition, wxDefaultSize);
    
    //Stick the Panels Down	
    nbUpgradeWizard->AddPage(pnlPage1, wxT("Settings"));
    nbUpgradeWizard->AddPage(pnlPage2, wxT("Versions"));

    //Panel 1 (Settings Page)
    (void)new wxStaticText( pnlPage1, -1,
                            "Click on the Versions button to see possible updates to pgAdmin3 and it's\n"
                            "related components such as PostgreSQL or psqlODBC. You must be\n"
                            "connected tothe internet for the Upgrade Wizard to work correctly.\n"
                            "\n"
                            "You may also alter the Upgrade Wizard's settings on this page\n",
                            wxPoint(10,10), wxSize(500, 110));
			    
    chkAuto = new wxCheckBox(pnlPage1,- 1, wxT("Automatically run the upgrade wizard every "), wxPoint(30,110));
    
    wxComboBox *cboFrequency = new wxComboBox(pnlPage1,-1,"", wxPoint(300,110), wxDefaultSize);
    cboFrequency->Append("Day");
    cboFrequency->Append("Week");
    cboFrequency->Append("Month");
    cboFrequency->Append("Year");
    txtpgAdminwebsite = new wxTextCtrl(pnlPage1,-1,wxT(""), wxPoint(175,160), wxSize(250, 20));
    (void)new wxStaticText( pnlPage1, -1, "pgAdmin website address", wxPoint(20,160), wxDefaultSize);

    //Panel 2 (Versions Page)
    (void)new wxStaticText (pnlPage2, -1, wxT("The software packages listed below are the cuurent versions of each.\n"
					      "Double click on a package to open it's associated download site in your\n"
					      "browser. Note that the installed  versions of PostgreSQL and PsqlODBC\n" 
					      "will be listed as 0.0.0 if you are not connected to a database.")
					      , wxPoint(5,10), wxSize(430,80));
    lvVersions = new wxListCtrl(pnlPage2, -1, wxPoint(5,80), wxSize(430, 130), wxLC_REPORT | wxSIMPLE_BORDER);

	//Create a new ImageList for thw UpgradeWizard
	wxImageList * ilUpgradeWizard = new wxImageList(16,16);
	//Associate the list to the listview control
	lvVersions->SetImageList(ilUpgradeWizard, wxIMAGE_LIST_SMALL);
	//Stuff the Image List
	ilUpgradeWizard->Add(wxIcon(unknown_xpm));
	ilUpgradeWizard->Add(wxIcon(upgrade_xpm));

	//Stuff the Listview Control
	lvVersions->InsertColumn(0, wxT("Software"), wxLIST_FORMAT_LEFT, 100);
	lvVersions->InsertColumn(1, wxT("Installed"), wxLIST_FORMAT_LEFT, 80);
	lvVersions->InsertColumn(2, wxT("Available"), wxLIST_FORMAT_LEFT, 80);
	lvVersions->InsertColumn(3, wxT("Download Site"), wxLIST_FORMAT_LEFT, 170);


	
	//Add the list Items. (I'll just add them for effect now but this will need to go check)
	lvVersions->InsertItem(0,wxT("PostgreSQL"),0);
	lvVersions->SetItem(0, 1, wxT("0.0.0"));
	lvVersions->SetItem(0, 2, wxT("7.2.1"));
	lvVersions->SetItem(0, 3, wxT("http://www.postgresql.org"));

	lvVersions->InsertItem(1,wxT("PsqlODBC"),0);
	lvVersions->SetItem(1, 1, wxT("0.0.0"));
	lvVersions->SetItem(1, 2, wxT("7.2.3"));
	lvVersions->SetItem(1, 3, wxT("http://odbc.postgresql.org"));

	lvVersions->InsertItem(2,wxT("pgAdmin III"),1);
	lvVersions->SetItem(2, 1, wxT("0.0.0"));
	lvVersions->SetItem(2, 2, wxT("0.0.0"));
	lvVersions->SetItem(2, 3, wxT("http://pgadmin.postgresql.org"));




	
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
    wxLogInfo(wxT("Destroying the Upgrade Wizard box"));
}

