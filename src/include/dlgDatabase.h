//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
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

    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:
    pgDatabase *database;
    void OnChange(wxNotifyEvent &ev);
    void OnGroupAdd(wxNotifyEvent &ev);
    void OnGroupRemove(wxNotifyEvent &ev);

    void OnVarAdd(wxNotifyEvent &ev);
    void OnVarRemove(wxNotifyEvent &ev);
    void OnVarSelChange(wxListEvent &ev);

    DECLARE_EVENT_TABLE();
};


#endif
