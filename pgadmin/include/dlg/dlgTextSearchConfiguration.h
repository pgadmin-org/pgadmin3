//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgTextSearchConfiguration.h - Text Search Configuration property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_TSCONFIGURATIONPROP
#define __DLG_TSCONFIGURATIONPROP

#include "dlg/dlgProperty.h"

class pgSchema;
class pgTextSearchConfiguration;

class dlgTextSearchConfiguration : public dlgTypeProperty
{
public:
	dlgTextSearchConfiguration(pgaFactory *factory, frmMain *frame, pgTextSearchConfiguration *cfg, pgSchema *sch);
	int Go(bool modal);

	virtual wxString GetHelpPage(bool forCreate) const
	{
		return wxT("pg/sql-createtsconfig");
	}
	void CheckChange();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();

private:
	void OnChange(wxCommandEvent &ev);

	pgSchema *schema;
	pgTextSearchConfiguration *config;
	bool dirtyTokens;

#ifdef __WXMAC__
	void OnChangeSize(wxSizeEvent &ev);
#endif

	void OnSelChangeToken(wxListEvent &ev);
	void OnChangeCbToken(wxCommandEvent &ev);
	void OnChangeTxtDictionary(wxCommandEvent &ev);
	void OnChangeCbDictionary(wxCommandEvent &ev);
	void OnAddToken(wxCommandEvent &ev);
	void OnChangeToken(wxCommandEvent &ev);
	void OnRemoveToken(wxCommandEvent &ev);

	DECLARE_EVENT_TABLE()
};


#endif
