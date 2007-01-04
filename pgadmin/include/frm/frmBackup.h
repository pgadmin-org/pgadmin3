//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmBackup.h - Backup database dialogue
//
//////////////////////////////////////////////////////////////////////////


#ifndef FRMBACKUP_H
#define FRMBACKUP_H

#include "dlg/dlgClasses.h"
#include "utils/factory.h"

class frmMain;

class frmBackup : public ExternProcessDialog
{
public:
    frmBackup(frmMain *form, pgObject *_object);
    ~frmBackup();

    void Go();
    wxString GetDisplayCmd(int step);
    wxString GetCmd(int step);
    
private:
    wxString GetHelpPage() const;
    void OnChange(wxCommandEvent &ev);
    void OnSelectFilename(wxCommandEvent &ev);
    void OnChangePlain(wxCommandEvent &ev);
    wxString getCmdPart1();
    wxString getCmdPart2();
    void OnOK(wxCommandEvent &ev);

    pgObject *object;

    bool canBlob;
    DECLARE_EVENT_TABLE()
};


class backupFactory : public contextActionFactory
{
public:
    backupFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};

#endif
