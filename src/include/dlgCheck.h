//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// dlgCheck.h - Check property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_CHECKPROP
#define __DLG_CHECKPROP

#include "dlgProperty.h"


class pgCheck;
class pgTable;

class dlgCheck : public dlgProperty
{
public:
    dlgCheck(frmMain *frame, pgCheck *node=0, pgTable *parentNode=0);
    wxString GetSql();
    wxString GetDefinition();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

    int Go(bool modal);

private:
    pgCheck *check;
    pgTable *table;
    void OnChange(wxNotifyEvent &ev);
    void OnCheckDeferrable(wxNotifyEvent &ev);

    DECLARE_EVENT_TABLE();
};


#endif
