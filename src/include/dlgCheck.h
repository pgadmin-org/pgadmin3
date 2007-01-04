//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
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
    dlgCheck(pgaFactory *factory, frmMain *frame, pgCheck *node=0, pgTable *parentNode=0);

    void CheckChange();
    wxString GetSql();
    wxString GetDefinition();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();
    wxString GetHelpPage() const { return wxT("pg/sql-altertable"); }

    int Go(bool modal);

private:
    pgCheck *check;
    pgTable *table;
    void OnCheckDeferrable(wxCommandEvent &ev);

    DECLARE_EVENT_TABLE()
};


#endif
