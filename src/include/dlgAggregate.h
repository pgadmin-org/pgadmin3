//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgAggregate.h - Aggregate property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_AGGREGATEPROP
#define __DLG_AGGREGATEPROP

#include "dlgProperty.h"

class pgSchema;
class pgAggregate;

class dlgAggregate : public dlgTypeProperty
{
public:
    dlgAggregate(frmMain *frame, pgAggregate *agg, pgSchema *sch);
    int Go(bool modal);

    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:
    pgSchema *schema;
    pgAggregate *aggregate;

    void OnChange(wxNotifyEvent &ev);
    void OnChangeType(wxNotifyEvent &ev);
    
    wxArrayString procedures;

    DECLARE_EVENT_TABLE();
};


#endif
