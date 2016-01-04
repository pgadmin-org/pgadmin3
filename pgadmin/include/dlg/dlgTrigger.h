//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgTrigger.h - Trigger property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_TRIGGERPROP
#define __DLG_TRIGGERPROP

#include "dlg/dlgProperty.h"

class pgTrigger;
class pgTable;

class dlgTrigger : public dlgCollistProperty
{
public:
	dlgTrigger(pgaFactory *factory, frmMain *frame, pgTrigger *trg, pgTable *sch);
	int Go(bool modal);

	void CheckChange();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();
	void SetObject(pgObject *obj)
	{
		trigger = (pgTrigger *)obj;
	}
	wxString GetColumns();


private:
	pgTable *table;
	pgTrigger *trigger;

	void OnChange(wxCommandEvent &ev);
	void OnChangeFunc(wxCommandEvent &ev);
	void OnChangeConstraint(wxCommandEvent &ev);
	void OnSelectComboCol(wxCommandEvent &ev);
	void OnSelectListCol(wxListEvent &ev);
	void OnSelectCol();
	void OnAddCol(wxCommandEvent &ev);
	void OnRemoveCol(wxCommandEvent &ev);

	virtual bool IsUpToDate();

	DECLARE_EVENT_TABLE()
};

#endif
