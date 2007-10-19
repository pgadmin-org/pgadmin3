//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmBackupServer.h - Backup server dialogue
//
//////////////////////////////////////////////////////////////////////////


#ifndef FRMBACKUPSERVER_H
#define FRMBACKUPSERVER_H

#include "dlg/dlgClasses.h"
#include "utils/factory.h"

class frmMain;

class frmBackupServer : public ExternProcessDialog
{
public:
    frmBackupServer(frmMain *form, pgObject *_object);
    ~frmBackupServer();

    void Go();
    wxString GetDisplayCmd(int step);
    wxString GetCmd(int step);
    
private:
    wxString GetHelpPage() const;
    void OnChange(wxCommandEvent &ev);
    void OnSelectFilename(wxCommandEvent &ev);
    wxString getCmdPart1();
    wxString getCmdPart2();
    void OnOK(wxCommandEvent &ev);

    pgObject *object;
    wxString processedFile;

    DECLARE_EVENT_TABLE()
};


class backupServerFactory : public contextActionFactory
{
public:
    backupServerFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};

#endif
