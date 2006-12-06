//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmIndexcheck.h - Index checker dialogue
//
//////////////////////////////////////////////////////////////////////////

#ifndef __FRMINDEXCHECK_H
#define __FRMINDEXCHECK_H

#include "dlgClasses.h"
#include "utils/factory.h"


// Class declarations
class frmIndexcheck : public ExecutionDialog
{
public:
    frmIndexcheck(frmMain *form, pgObject *_object);
    ~frmIndexcheck();

    wxString GetHelpPage() const;
    wxString GetSql();
    void Go();
    
private:
    void OnPageSelect(wxNotebookEvent& event);
    void OnCheckAll(wxCommandEvent &event);
    void OnUncheckAll(wxCommandEvent &event);

    void AddObjects(const wxString &where);
    
    ctlSQLBox *sqlPane;
    wxNotebook *nbNotebook;

    DECLARE_EVENT_TABLE()
};


class indexCheckFactory : public contextActionFactory
{
public:
    indexCheckFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};

#endif
