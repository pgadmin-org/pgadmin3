//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgOperator.h - Operator property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_OPERATORPROP
#define __DLG_OPERATORPROP

#include "dlgProperty.h"

class pgSchema;
class pgOperator;

class dlgOperator : public dlgTypeProperty
{
public:
    dlgOperator(frmMain *frame, pgOperator *op, pgSchema *sch);
    int Go(bool modal);

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:
    void CheckChangeType();
    void OnChangeTypeLeft(wxCommandEvent &ev);
    void OnChangeTypeRight(wxCommandEvent &ev);
    void OnChangeJoin(wxCommandEvent &ev);

    void AppendFilledOperator(wxString &sql, wxChar *txt, wxComboBoxFix *cb);

    pgSchema *schema;
    pgOperator *oper;
    wxArrayString procedures;

    DECLARE_EVENT_TABLE();
};


#endif
