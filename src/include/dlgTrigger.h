//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgTrigger.h - Trigger property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_TRIGGERPROP
#define __DLG_TRIGGERPROP

#include "dlgProperty.h"

class pgTrigger;
class pgTable;

class dlgTrigger : public dlgProperty
{
public:
    dlgTrigger(frmMain *frame, pgTrigger *trg, pgTable *sch);
    int Go(bool modal);

    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:
    pgTable *table;
    pgTrigger *trigger;

    void OnChange(wxNotifyEvent &ev);

    DECLARE_EVENT_TABLE();
};


#endif
