//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgTextSearchParser.h - Text Search Parser property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_TSPARSERPROP
#define __DLG_TSPARSERPROP

#include "dlg/dlgProperty.h"

class pgSchema;
class pgTextSearchParser;

class dlgTextSearchParser : public dlgTypeProperty
{
public:
	dlgTextSearchParser(pgaFactory *factory, frmMain *frame, pgTextSearchParser *cfg, pgSchema *sch);
	int Go(bool modal);

	void CheckChange();
	wxString GetSql();
	virtual wxString GetHelpPage(bool forCreate) const
	{
		return wxT("pg/sql-createtsparser");
	}
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();

private:
	void OnChange(wxCommandEvent &ev);

	pgSchema *schema;
	pgTextSearchParser *parser;

	DECLARE_EVENT_TABLE()
};


#endif
