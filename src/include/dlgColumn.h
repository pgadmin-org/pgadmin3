//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// dlgColumn.h - Column property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_COLUMNPROP
#define __DLG_COLUMNPROP

#include "dlgProperty.h"

class pgColumn;
class pgTable;

class dlgColumn : public dlgProperty
{
public:
    dlgColumn(frmMain *frame, pgColumn *column, pgTable *parentNode);

    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();
    wxString GetName();
    wxString GetDefinition();
    wxString GetFullType();
    wxString GetPreviousDefinition() { return previousDefinition; }

    int Go(bool modal);


private:
    bool isVarLen, isVarPrec;
    pgColumn *column;
    pgTable *table;
    void OnChange(wxNotifyEvent &ev);
    wxArrayString typmods;

    wxString previousDefinition;

    DECLARE_EVENT_TABLE();
};


#endif
