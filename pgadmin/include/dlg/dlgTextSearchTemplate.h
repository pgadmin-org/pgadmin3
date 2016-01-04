//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgTextSearchTemplate.h - Text Search Template property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_TSTEMPLATEPROP
#define __DLG_TSTEMPLATEPROP

#include "dlg/dlgProperty.h"

class pgSchema;
class pgTextSearchTemplate;

class dlgTextSearchTemplate : public dlgTypeProperty
{
public:
	dlgTextSearchTemplate(pgaFactory *factory, frmMain *frame, pgTextSearchTemplate *cfg, pgSchema *sch);
	int Go(bool modal);

	void CheckChange();
	wxString GetSql();
	virtual wxString GetHelpPage(bool forCreate) const
	{
		return wxT("pg/sql-createtstemplate");
	}
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();

private:
	void OnChange(wxCommandEvent &ev);

	pgSchema *schema;
	pgTextSearchTemplate *tmpl;

	DECLARE_EVENT_TABLE()
};


#endif
