//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgLanguage.h - Language property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_LANGUAGEPROP
#define __DLG_LANGUAGEPROP

#include "dlg/dlgProperty.h"

class pgLanguage;

class dlgLanguage : public dlgSecurityProperty
{
public:
	dlgLanguage(pgaFactory *factory, frmMain *frame, pgLanguage *db);
	int Go(bool modal);

	void CheckChange();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();

private:
	pgLanguage *language;
	void OnChangeName(wxCommandEvent &ev);
#ifdef __WXMAC__
	void OnChangeSize(wxSizeEvent &ev);
#endif

	DECLARE_EVENT_TABLE()
};


#endif
