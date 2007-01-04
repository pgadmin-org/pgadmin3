//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmOptions.h - The main options dialogue
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMOPTIONS_H
#define FRMOPTIONS_H

#include "dlgClasses.h"
#include "base/factory.h"

class frmMain;

// Class declarations
class frmOptions : public pgDialog
{
public:
    frmOptions(frmMain *parent);
    ~frmOptions();
    
private:
    frmMain *mainForm;
    wxFont currentFont, currentSqlFont;

    void OnBrowseLogFile(wxCommandEvent &ev);
    void OnSqlFontSelect(wxCommandEvent &ev);
    void OnSlonyPathSelect(wxCommandEvent &ev);
    void OnFontSelect(wxCommandEvent &ev);
    void OnOK(wxCommandEvent &ev);
    void OnCancel(wxCommandEvent &ev);
    void OnHelp(wxCommandEvent &ev);
    void OnSuppressHints(wxCommandEvent &ev);
    void OnResetHints(wxCommandEvent &ev);
    DECLARE_EVENT_TABLE()
};


class optionsFactory : public actionFactory
{
public:
    optionsFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
};


#endif
