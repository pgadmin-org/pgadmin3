//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: dlgRule.h 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgRule.h - Rule property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_RULEPROP
#define __DLG_RULEPROP

#include "dlgProperty.h"

class pgTable;
class pgRule;
class ctlSQLBox;

class dlgRule : public dlgProperty
{
public:
    dlgRule(pgaFactory *factory, frmMain *frame, pgRule *r, pgTable *tab);
    int Go(bool modal);

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:
    pgTable *table;
    pgRule *rule;
    wxString oldDefinition;

    bool didChange();

    DECLARE_EVENT_TABLE();
};


#endif
