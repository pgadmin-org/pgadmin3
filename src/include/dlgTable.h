//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// dlgTable.h - Table property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_TABLEPROP
#define __DLG_TABLEPROP

#include "dlgProperty.h"

class pgTable;
class pgColumn;

class dlgTable : public dlgSecurityProperty
{
public:
    dlgTable(frmMain *frame, pgTable *db);
    int Go(bool modal);

    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:
    pgTable *table;
    pgColumn *column;
    void OnChange(wxNotifyEvent &ev);

    void OnAddCol(wxNotifyEvent &ev);
    void OnRemoveCol(wxNotifyEvent &ev);
    void OnSelChangeCol(wxListEvent &ev);

    void OnAddConstr(wxNotifyEvent &ev);
    void OnRemoveConstr(wxNotifyEvent &ev);
    void OnSelChangeConstr(wxListEvent &ev);

    void FillConstraint();
    wxString GetItemConstraintType(wxListCtrl *list, long pos);
    bool hasPK;

    wxArrayString previousColumns, previousConstraints;
    wxTreeItemId columnsItem, constraintsItem;

    DECLARE_EVENT_TABLE();
};


#endif
