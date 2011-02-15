//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgDomain.h - Domain property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_DOMAINPROP
#define __DLG_DOMAINPROP

#include "dlg/dlgProperty.h"

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

	pgSchema *schema;
	pgDomain *domain;

	DECLARE_EVENT_TABLE()
};


#endif
