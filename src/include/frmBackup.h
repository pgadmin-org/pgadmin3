//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmBackup.h - Backup database dialogue
//
//////////////////////////////////////////////////////////////////////////


#ifndef FRMBACKUP_H
#define FRMBACKUP_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "frmMain.h"

// Class declarations


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

    pgObject *object;

    bool canBlob;
    DECLARE_EVENT_TABLE()
};


class backupFactory : public actionFactory
{
public:
    backupFactory(wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};

#endif
