//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: frmUpdate.h 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmUpdate.h - Online update
//
//////////////////////////////////////////////////////////////////////////


#ifndef __FRMUPDATE
#define __FRMUPDATE

#include "dlgClasses.h"
#include "base/factory.h"

class frmUpdate : public DialogWithHelp
{
public:
    frmUpdate(frmMain *mainForm);
    ~frmUpdate();
    wxString GetHelpPage() const;
    int Go();

    void OnOk(wxCommandEvent &ev);

    static bool HasLanguageUpdate();
    static bool HasNewVersion();
    static bool HasNewBeta();
    DECLARE_EVENT_TABLE()
};


class onlineUpdateFactory : public actionFactory
{
public:
    onlineUpdateFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
};

#endif

