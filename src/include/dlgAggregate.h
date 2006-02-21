//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: dlgAggregate.h 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
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
    dlgAggregate(pgaFactory *factory, frmMain *frame, pgAggregate *agg, pgSchema *sch);
    int Go(bool modal);

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:
    pgSchema *schema;
    pgAggregate *aggregate;

    void OnChangeType(wxCommandEvent &ev);
    void OnChangeTypeBase(wxCommandEvent &ev);
    void OnChangeTypeState(wxCommandEvent &ev);

    DECLARE_EVENT_TABLE();
};


#endif
