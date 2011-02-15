//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgView.h - View property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_VIEWPROP
#define __DLG_VIEWPROP

#include "dlg/dlgProperty.h"

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
	void OnApply(wxCommandEvent &ev);

	pgSchema *schema;
	pgView *view;
	wxString oldDefinition;

	DECLARE_EVENT_TABLE()
};


#endif
