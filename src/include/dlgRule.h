//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
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
    dlgRule(frmMain *frame, pgRule *r, pgTable *tab);
    int Go(bool modal);

    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:
    pgTable *table;
    pgRule *rule;
    wxString oldDefinition;

    void OnChange(wxCommandEvent &ev);
    void OnChangeStc(wxStyledTextEvent& event);

    bool didChange();

    DECLARE_EVENT_TABLE();
};


#endif
