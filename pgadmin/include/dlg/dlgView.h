//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2012, The pgAdmin Development Team
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
		if(seclabelPage)
			seclabelPage->SetObject(obj);
	}

private:

	virtual bool IsUpToDate();
	void OnApply(wxCommandEvent &ev);

	pgSchema *schema;
	pgView *view;
	ctlSeclabelPanel *seclabelPage;
	wxString oldDefinition;

	void OnChange(wxCommandEvent &event);

	DECLARE_EVENT_TABLE()
};


#endif
