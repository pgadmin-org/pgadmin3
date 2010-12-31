//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgAggregate.h - Aggregate property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_AGGREGATEPROP
#define __DLG_AGGREGATEPROP

#include "dlg/dlgProperty.h"

class pgSchema;
class pgAggregate;

class dlgAggregate : public dlgTypeProperty
{
public:
	dlgAggregate(pgaFactory *factory, frmMain *frame, pgAggregate *agg, pgSchema *sch);
	int Go(bool modal);

	void CheckChange();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();

private:
	pgSchema *schema;
	pgAggregate *aggregate;

	virtual wxString GetDisplayName()
	{
		return GetName() + wxT("(") + GetInputTypesList() + wxT(")");
	};

#ifdef __WXMAC__
	void OnChangeSize(wxSizeEvent &ev);
#endif

	void OnChangeType(wxCommandEvent &ev);
	void OnChangeTypeBase(wxCommandEvent &ev);
	void OnChangeTypeState(wxCommandEvent &ev);

	void OnAddInputType(wxCommandEvent &ev);
	void OnRemoveInputType(wxCommandEvent &ev);
	void OnSelectInputType(wxListEvent &ev);

	long GetInputTypeOid(int param);
	wxString GetInputTypesList();
	wxString GetInputTypesOidList();

	DECLARE_EVENT_TABLE()
};


#endif
