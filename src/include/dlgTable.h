//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgTable.h - Table property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_TABLEPROP
#define __DLG_TABLEPROP

#include "dlgProperty.h"

class pgSchema;
class pgTable;
class pgColumn;

class dlgTable : public dlgSecurityProperty
{
public:
    dlgTable(frmMain *frame, pgTable *db, pgSchema *sch);
    int Go(bool modal);

    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:
    pgSchema *schema;
    pgTable *table;

    void OnChange(wxCommandEvent &ev);
    void OnChangeOwner(wxCommandEvent &ev);
    void OnChangeTable(wxCommandEvent &ev);

    void OnAddTable(wxCommandEvent &ev);
    void OnRemoveTable(wxCommandEvent &ev);
    void OnSelChangeTable(wxCommandEvent &ev);

    void OnAddCol(wxCommandEvent &ev);
    void OnChangeCol(wxCommandEvent &ev);
    void OnRemoveCol(wxCommandEvent &ev);
    void OnSelChangeCol(wxListEvent &ev);

    void OnAddConstr(wxCommandEvent &ev);
    void OnRemoveConstr(wxCommandEvent &ev);
    void OnSelChangeConstr(wxListEvent &ev);

    void FillConstraint();
    wxString GetItemConstraintType(ctlListView *list, long pos);
    bool hasPK;

    wxArrayString previousColumns, previousConstraints;
    wxArrayString tableOids, inheritedTableOids;
    wxTreeItemId columnsItem, constraintsItem;

    DECLARE_EVENT_TABLE();
};


#endif
