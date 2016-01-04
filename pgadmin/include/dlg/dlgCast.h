//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgCast.h - Cast property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_CASTPROP
#define __DLG_CASTPROP

#include "dlg/dlgProperty.h"

class pgCast;

class dlgCast : public dlgTypeProperty
{
public:
	dlgCast(pgaFactory *factory, frmMain *frame, pgCast *ca);
	int Go(bool modal);

	void CheckChange();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();

private:
	void OnChangeType(wxCommandEvent &ev);
	void OnChangeTypeSrc(wxCommandEvent &ev);
	void OnChangeTypeTrg(wxCommandEvent &ev);

	pgCast *cast;
	wxArrayString functions;

	DECLARE_EVENT_TABLE()
};


#endif
