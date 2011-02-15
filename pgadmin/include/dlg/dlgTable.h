//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgTable.h - Table property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_TABLEPROP
#define __DLG_TABLEPROP

#include "dlg/dlgProperty.h"

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
	~dlgTable();

private:
	pgSchema *schema;
	pgTable *table;
	dataTypeCache dtCache;

	void OnOK(wxCommandEvent &ev);
	void OnChangeTable(wxCommandEvent &ev);
#ifdef __WXMAC__
	void OnChangeSize(wxSizeEvent &ev);
#endif

	void OnChangeOfType(wxCommandEvent &ev);

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
	void FillAutoVacuumParameters(wxString &setString, wxString &resetStr,
	                              const wxString &parameter, const wxString &val);
	void PopulateDatatypeCache();
	wxString GetItemConstraintType(ctlListView *list, long pos);
	bool hasPK;

	wxArrayString previousColumns, previousConstraints, previousTables;
	wxArrayString tableOids, inheritedTableOids;
	wxTreeItemId columnsItem, constraintsItem;

	wxString GetNumString(wxTextCtrl *ctl, bool enabled, const wxString &val);
	wxString AppendNum(bool &changed, wxTextCtrl *ctl, long val);
	wxString AppendNum(bool &changed, wxTextCtrl *ctl, double val);

	bool tableVacEnabled, hasVacuum, settingAutoVacuum;
	long settingVacBaseThr, settingAnlBaseThr, settingCostDelay, settingCostLimit,
	     settingFreezeMinAge, settingFreezeMaxAge, settingFreezeTableAge;
	long tableVacBaseThr, tableAnlBaseThr, tableCostDelay, tableCostLimit,
	     tableFreezeMinAge, tableFreezeMaxAge, tableFreezeTableAge;
	double settingVacFactor, settingAnlFactor;
	double tableVacFactor, tableAnlFactor;

	/* Toast Table */
	bool toastTableVacEnabled, toastTableHasVacuum;
	long toastTableVacBaseThr, toastTableAnlBaseThr, toastTableCostDelay, toastTableCostLimit,
	     toastTableFreezeMinAge, toastTableFreezeMaxAge, toastTableFreezeTableAge;
	double toastTableVacFactor, toastTableAnlFactor;

	DECLARE_EVENT_TABLE()
};

#endif
