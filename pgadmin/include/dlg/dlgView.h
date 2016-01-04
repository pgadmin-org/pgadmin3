//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgView.h - View property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_VIEWPROP
#define __DLG_VIEWPROP

#include "dlg/dlgProperty.h"
#include "ctl/ctlSeclabelPanel.h"

class pgSchema;
class pgView;
class ctlSQLBox;

class dlgView : public dlgSecurityProperty
{
public:
	dlgView(pgaFactory *factory, frmMain *frame, pgView *v, pgSchema *sch);
	int Go(bool modal);

	void CheckChange();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();

	void SetObject(pgObject *obj)
	{
		view = (pgView *)obj;
	}

private:

	virtual bool IsUpToDate();
	void OnChangeVacuum(wxCommandEvent &ev);
	void OnCheckMaterializedView(wxCommandEvent &ev);
	void FillAutoVacuumParameters(wxString &setStr, wxString &resetStr, const wxString &parameter, const wxString &val);
	wxString AppendNum(bool &changed, wxTextCtrl *ctl, wxString val);
	void DisableMaterializedView();
	void DisableStorageParameters();
	int GetIndexCheckOption(const wxString &str) const;

	pgSchema *schema;
	pgView *view;
	ctlSeclabelPanel *seclabelPage;
	wxString oldDefinition;
	wxTextValidator mviewNumericValidator;

	void OnChange(wxCommandEvent &event);

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

	bool forceSecurityBarrierChanged;

	DECLARE_EVENT_TABLE()
};


#endif
