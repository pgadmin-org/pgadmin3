//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmMain.h - The main form
//
//////////////////////////////////////////////////////////////////////////

#include <wx/treectrl.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include <wx/stc/stc.h>

// Class declarations
class frmMain : public wxFrame
{
public:
  frmMain(const wxString& title, const wxPoint& pos, const wxSize& size);
  void OnExit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  wxMenuBar *mnuBar;
  wxStatusBar *stBar;
  wxToolBar *tlBar;

private:
  wxTreeCtrl* tvBrowser;
  wxListCtrl* lvProperties;
  wxListCtrl* lvStatistics;
  wxNotebook* nbListViews;
  wxStyledTextCtrl* txtSQLPane;
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
