//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmMain.h - The main form
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMMAIN_H
#define FRMMAIN_H

// wxWindows headers
#include <wx/wx.h>
#include <wx/treectrl.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>

// App headers
#include "../../pgAdmin3.h"
#include "../controls/ctlSQLBox.h"

// Class declarations
class frmMain : public wxFrame
{
public:
    frmMain(const wxString& title, const wxPoint& pos, const wxSize& size);
    void TipOfTheDay();
    
private:
    wxTreeCtrl* tvBrowser;
    wxListCtrl* lvProperties;
    wxListCtrl* lvStatistics;
    wxNotebook* nbListViews;
    ctlSQLBox* txtSQLPane;
    wxMenuBar *mnuBar;
    wxStatusBar *stBar;
    wxToolBar *tlBar;
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnSize(wxSizeEvent &sizForm);
    void OnMove(wxMoveEvent &posForm);
    DECLARE_EVENT_TABLE()
};

// Menu options
enum
{
    MNU_CONNECT = 101,
    MNU_PASSWORD = 102,
    MNU_SAVEDEFINITION = 103,
    MNU_SAVESCHEMA = 104,
    MNU_EXIT = 105,
    MNU_UPGRADEWIZARD = 106,
    MNU_OPTIONS = 107,
    MNU_SYSTEMOBJECTS = 108,
    MNU_CONTENTS = 109,
    MNU_TIPOFTHEDAY = 110,
    MNU_ABOUT = 111
};

// Toolbar buttons
enum
{
    BTN_CONNECT = 101,
    BTN_REFRESH = 102,
    BTN_CREATE = 103,
    BTN_DROP = 104,
    BTN_PROPERTIES = 105,
    BTN_SQL = 106,
    BTN_VIEWDATA = 107,
    BTN_VACUUM = 108,
    BTN_RECORD = 109,
    BTN_STOP = 110,
};

#endif