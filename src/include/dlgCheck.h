//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
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
    wxString GetHelpPage() const { return wxT("sql-altertable"); }

    int Go(bool modal);

private:
    pgCheck *check;
    pgTable *table;
    void OnChange(wxNotifyEvent &ev);
    void OnCheckDeferrable(wxNotifyEvent &ev);

    DECLARE_EVENT_TABLE();
};


#endif
