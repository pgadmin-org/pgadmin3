//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgRepTable.h - Slony-I table property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_REPTABLEPROP
#define __DLG_REPTABLEPROP

#include "dlgRepProperty.h"

class slSet;
class slTable;

class dlgRepTable : public dlgRepProperty
{
public:
    dlgRepTable(frmMain *frame, slTable *tab, slSet *s);
    int Go(bool modal);

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:

    slTable *table;
    slSet *set;
    long lastTableSelection;

    void LoadTrigger(OID relid);

    void OnChangeTable(wxCommandEvent &ev);
    void OnChangeTableSel(wxCommandEvent &ev);

    DECLARE_EVENT_TABLE();
};


#endif
