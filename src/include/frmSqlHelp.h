//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmSqlHelp.h - SQL Help Screen
//
//////////////////////////////////////////////////////////////////////////


#ifndef __FRMHELP
#define __FRMHELP

#include "wx/wxhtml.h"


class ctlHelpWindow;
class frmMain;
class frmSqlHelp : public wxFrame
{
public:
    frmSqlHelp(frmMain *fr);
    ~frmSqlHelp();
    bool Load(const wxString &page);
    void OnLinkClicked(const wxHtmlLinkInfo& link);

private:
    void OnBack(wxCommandEvent &ev);
    void OnRefresh(wxCommandEvent &ev);
    void OnForward(wxCommandEvent &ev);
    void CheckToolBar();

    frmMain *mainForm;
    ctlHelpWindow *htmlWindow;
    wxString currentPage;

    DECLARE_EVENT_TABLE();
};

class ctlHelpWindow : public wxHtmlWindow 
{
public:
    ctlHelpWindow(frmSqlHelp *frm);

private:
    void OnLinkClicked(const wxHtmlLinkInfo& link);
    frmSqlHelp *frmHelp;
};

#endif
