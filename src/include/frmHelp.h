//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmHelp.h - Help Screen
//
//////////////////////////////////////////////////////////////////////////


#ifndef __FRMHELP
#define __FRMHELP

#include "wx/wxhtml.h"

#include "dlg/dlgClasses.h"
#include "utils/factory.h"

class ctlHelpWindow;
class frmMain;
class frmHelp : public pgFrame
{
public:
    frmHelp(wxWindow *fr);
    ~frmHelp();
    static bool LoadLocalDoc(wxWindow *wnd, const wxString &page, char **icon=0);
    static bool LoadSqlDoc(wxWindow *wnd, const wxString &page, char **icon=0);

    bool Load(const wxString &page, char **icon=0);
    void OnLinkClicked(const wxHtmlLinkInfo& link);

private:
    void OnBack(wxCommandEvent &ev);
    void OnRefresh(wxCommandEvent &ev);
    void OnForward(wxCommandEvent &ev);
    void CheckToolBar();

    frmMain *mainForm;
    ctlHelpWindow *htmlWindow;
    wxString currentPage;

    DECLARE_EVENT_TABLE()
};

class ctlHelpWindow : public wxHtmlWindow 
{
public:
    ctlHelpWindow(frmHelp *frm);

private:
    void OnLinkClicked(const wxHtmlLinkInfo& link);
    frmHelp *helpForm;
};


class contentsFactory : public actionFactory
{
public:
    contentsFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
};


class pgsqlHelpFactory : public actionFactory
{
public:
    pgsqlHelpFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar, bool bigTool);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
};


class faqFactory : public actionFactory
{
public:
    faqFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
};


class bugReportFactory : public actionFactory
{
public:
    bugReportFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
};

#endif
