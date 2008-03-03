//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmOptions.h - The main options dialogue
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMOPTIONS_H
#define FRMOPTIONS_H

#include "dlg/dlgClasses.h"
#include "utils/factory.h"

class frmMain;

class ItemWithString : public wxClientData
{
public:
	wxString data;
};

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
    void OnPostgresqlPathSelect(wxCommandEvent &ev);
    void OnEnterprisedbPathSelect(wxCommandEvent &ev);
    void OnFontSelect(wxCommandEvent &ev);
    void OnOK(wxCommandEvent &ev);
    void OnCancel(wxCommandEvent &ev);
    void OnHelp(wxCommandEvent &ev);
    void OnDefault(wxCommandEvent &ev);
    void OnSuppressHints(wxCommandEvent &ev);
    void OnResetHints(wxCommandEvent &ev);
	void OnChangeCopyQuote(wxCommandEvent &ev);
    DECLARE_EVENT_TABLE()
};


class optionsFactory : public actionFactory
{
public:
    optionsFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
};


#endif
