//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgCollation.h - Collation property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_COLLATIONPROP
#define __DLG_COLLATIONPROP

#include "dlg/dlgProperty.h"

class pgSchema;
class pgCollation;

class dlgCollation : public dlgTypeProperty
{
public:
	dlgCollation(pgaFactory *factory, frmMain *frame, pgCollation *coll, pgSchema *sch);
	int Go(bool modal);

	void CheckChange();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();

private:
	void OnSelChangeTyp(wxCommandEvent &ev);

	pgSchema *schema;
	pgCollation *collation;

	DECLARE_EVENT_TABLE()
};


#endif
