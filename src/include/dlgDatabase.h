//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgDatabase.h - Database property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_DATABASEPROP
#define __DLG_DATABASEPROP

#include "dlgProperty.h"

class pgDatabase;

class dlgDatabase : public dlgSecurityProperty
{
public:
    dlgDatabase(frmMain *frame, pgDatabase *db);
    int Go(bool modal);

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();
    wxString GetHelpPage() const;

private:
    pgDatabase *database;
    wxArrayString varInfo;

    void OnGroupAdd(wxCommandEvent &ev);
    void OnGroupRemove(wxCommandEvent &ev);

    void OnVarAdd(wxCommandEvent &ev);
    void OnVarRemove(wxCommandEvent &ev);
    void OnVarSelChange(wxListEvent &ev);

    void OnVarnameSelChange(wxCommandEvent &ev);

    DECLARE_EVENT_TABLE();

    friend class pgaDatabaseFactory;
};


#endif
