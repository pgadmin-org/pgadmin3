//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgTable.h - Table property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_TABLEPROP
#define __DLG_TABLEPROP

#include "dlg/dlgProperty.h"
#include "ctl/ctlSeclabelPanel.h"

enum
{
	COL_NAME = 0,
	COL_DEFINITION,
	COL_INHERIT,
	COL_SQLCHANGE,
	COL_COMMENTS,
	COL_STATISTICS,
	COL_PGCOLUMN,
	COL_TYPEOID,
	COL_CHANGEDCOL,
	COL_VARIABLE_LIST,
	COL_SECLABEL_LIST
};

enum
{
	TAB_PROPERTIES = 0,
	TAB_DEFINITION,
	TAB_INHERITS,
	TAB_LIKE,
	TAB_COLUMNS,
	TAB_CONSTRAINTS,
	TAB_AUTOVACUUM,
	TAB_PRIVILEGES,
	TAB_SECLABEL,
	TAB_SQL
};

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
	bool Destroy();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();
	~dlgTable();

private:
	pgSchema *schema;
	pgTable *table;
	ctlSeclabelPanel *seclabelPage;
	dataTypeCache dtCache;

	void OnChange(wxCommandEvent &event);

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

	wxArrayString previousColumns, previousConstraints, previousTables, constraintsDefinition;
	wxArrayString tableOids, inheritedTableOids;
	wxTreeItemId columnsItem, constraintsItem;

	wxString GetNumString(wxTextCtrl *ctl, bool enabled, const wxString &val);
	wxString AppendNum(bool &changed, wxTextCtrl *ctl, wxString val);

	bool tableVacEnabled, hasVacuum, settingAutoVacuum;
	wxString settingVacBaseThr, settingAnlBaseThr, settingCostDelay,
	         settingCostLimit, settingFreezeMinAge, settingFreezeMaxAge,
	         settingFreezeTableAge;
	wxString tableVacBaseThr, tableAnlBaseThr, tableCostDelay,
	         tableCostLimit, tableFreezeMinAge, tableFreezeMaxAge,
	         tableFreezeTableAge;
	wxString settingVacFactor, settingAnlFactor;
	wxString tableVacFactor, tableAnlFactor;

	/* Toast Table */
	bool toastTableVacEnabled, toastTableHasVacuum;
	wxString toastTableVacBaseThr,
	         toastTableCostDelay, toastTableCostLimit,
	         toastTableFreezeMinAge, toastTableFreezeMaxAge,
	         toastTableFreezeTableAge;
	wxString toastTableVacFactor;

	DECLARE_EVENT_TABLE()
};

#endif
