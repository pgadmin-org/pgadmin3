//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgColumn.h - Column property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_COLUMNPROP
#define __DLG_COLUMNPROP

#include "dlgProperty.h"

class pgColumn;
class pgTable;

class dlgColumn : public dlgTypeProperty
{
public:
    dlgColumn(frmMain *frame, pgColumn *column, pgTable *parentNode);

    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();
    wxString GetDefinition();
    wxString GetPreviousDefinition() { return previousDefinition; }

    int Go(bool modal);


private:
    pgColumn *column;
    pgTable *table;
    void OnChange(wxNotifyEvent &ev);
    void OnSelChangeTyp(wxNotifyEvent &ev);
 
    wxString previousDefinition;

    DECLARE_EVENT_TABLE();
};


#endif
