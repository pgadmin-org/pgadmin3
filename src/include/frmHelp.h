//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmHelp.h - Help Screen
//
//////////////////////////////////////////////////////////////////////////


#ifndef __FRMHELP
#define __FRMHELP

#include "wx/wxhtml.h"


class ctlHelpWindow;
class frmMain;
class frmHelp : public wxFrame
{
public:
    frmHelp(wxWindow *fr);
    ~frmHelp();
    static bool LoadLocalDoc(wxWindow *wnd, const wxString &page);
    static bool LoadSqlDoc(wxWindow *wnd, const wxString &page);

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
    ctlHelpWindow(frmHelp *frm);

private:
    void OnLinkClicked(const wxHtmlLinkInfo& link);
    frmHelp *helpForm;
};

#endif
