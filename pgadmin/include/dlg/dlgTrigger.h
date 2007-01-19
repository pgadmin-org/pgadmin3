//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgTrigger.h - Trigger property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_TRIGGERPROP
#define __DLG_TRIGGERPROP

#include "dlg/dlgProperty.h"

class pgTrigger;
class pgTable;

class dlgTrigger : public dlgProperty
{
public:
    dlgTrigger(pgaFactory *factory, frmMain *frame, pgTrigger *trg, pgTable *sch);
    int Go(bool modal);

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:
    pgTable *table;
    pgTrigger *trigger;

    void OnChangeFunc(wxCommandEvent &ev);

    DECLARE_EVENT_TABLE()
};

#endif
