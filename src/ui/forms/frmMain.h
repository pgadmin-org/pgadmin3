//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmMain.h - The main form
//
//////////////////////////////////////////////////////////////////////////

// Class declarations
class frmMain : public wxFrame
{
public:
    frmMain(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~frmMain();
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
	wxMenuBar *mnuBar;
	wxStatusBar *stBar;
    wxToolBar *tlBar;
private:
    DECLARE_EVENT_TABLE()
};

// Constants
enum
{
	mnuConnect = 1,
	mnuPassword,
	mnuSaveDefinition,
	mnuSaveSchema,
    mnuExit,
	mnuUpgradeWizard,
	mnuOptions,
	mnuSystemObjects,
	mnuContents,
	mnuTipOfTheDay,
    mnuAbout
};
