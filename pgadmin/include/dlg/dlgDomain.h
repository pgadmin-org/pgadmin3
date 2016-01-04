//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgDomain.h - Domain property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_DOMAINPROP
#define __DLG_DOMAINPROP

#include "dlg/dlgProperty.h"
#include "ctl/ctlSeclabelPanel.h"

class pgSchema;
class pgDomain;

class dlgDomain : public dlgTypeProperty
{
public:
	dlgDomain(pgaFactory *factory, frmMain *frame, pgDomain *dom, pgSchema *sch);
	int Go(bool modal);

	void CheckChange();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();

private:
	void OnSelChangeTyp(wxCommandEvent &ev);
	void OnChangeValidate(wxCommandEvent &ev);

	void OnAddConstr(wxCommandEvent &ev);
	void OnRemoveConstr(wxCommandEvent &ev);
	void OnSelChangeConstr(wxListEvent &ev);

	pgSchema *schema;
	pgDomain *domain;
	ctlSeclabelPanel *seclabelPage;
	wxArrayString previousConstraints, constraintsDefinition;
	wxTreeItemId constraintsItem;

	void OnChange(wxCommandEvent &event);

	DECLARE_EVENT_TABLE()
};


#endif
