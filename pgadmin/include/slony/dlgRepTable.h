//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgRepTable.h - Slony-I table property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_REPTABLEPROP
#define __DLG_REPTABLEPROP

#include "slony/dlgRepProperty.h"

class slSet;
class slTable;

class dlgRepTable : public dlgRepProperty
{
public:
	dlgRepTable(pgaFactory *factory, frmMain *frame, slTable *tab, slSet *s);
	int Go(bool modal);
	wxString GetHelpPage() const
	{
		return wxT("slony-set#table");
	}

	void CheckChange();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();

private:

	slTable *table;
	slSet *set;
	long lastTableSelection;

	void LoadTrigger(OID relid);

	void OnChangeTable(wxCommandEvent &ev);
	void OnChangeTableSel(wxCommandEvent &ev);

	DECLARE_EVENT_TABLE()
};


#endif
