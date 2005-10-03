//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
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
    dlgTable(pgaFactory *factory, frmMain *frame, pgTable *db, pgSchema *sch);
    int Go(bool modal);

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:
    pgSchema *schema;
    pgTable *table;

    void OnOK(wxCommandEvent &ev);
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

    void OnChangeVacuum(wxCommandEvent &ev);

    void FillConstraint();
    wxString GetItemConstraintType(ctlListView *list, long pos);
    bool hasPK;

    wxArrayString previousColumns, previousConstraints;
    wxArrayString tableOids, inheritedTableOids;
    wxTreeItemId columnsItem, constraintsItem;

    wxString GetNumString(wxTextCtrl *ctl, bool enabled, const wxString &val);
    wxString AppendNum(bool &changed, wxTextCtrl *ctl, long val);
    wxString AppendNum(bool &changed, wxTextCtrl *ctl, double val);

    bool tableVacEnabled, hasVacuum;
    long settingVacBaseThr, settingAnlBaseThr, settingCostDelay, settingCostLimit;
    long tableVacBaseThr, tableAnlBaseThr, tableCostDelay, tableCostLimit;
    double settingVacFactor, settingAnlFactor;
    double tableVacFactor, tableAnlFactor;

    DECLARE_EVENT_TABLE();
};


class countRowsFactory : public contextActionFactory
{
public:
    countRowsFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};

#endif
