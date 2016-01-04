//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgLanguage.h - Language properties dialog
//
//////////////////////////////////////////////////////////////////////////

#ifndef __DLG_LANGUAGEPROP
#define __DLG_LANGUAGEPROP

#include "dlg/dlgProperty.h"
#include "ctl/ctlSeclabelPanel.h"

class pgLanguage;

class dlgLanguage : public dlgSecurityProperty
{
public:
	dlgLanguage(pgaFactory *factory, frmMain *frame, pgLanguage *node, pgObject *parent);
	int Go(bool modal);

	void CheckChange();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();

private:
	pgLanguage *language;
	ctlSeclabelPanel *seclabelPage;

	void OnChange(wxCommandEvent &event);
	void OnChangeName(wxCommandEvent &ev);
#ifdef __WXMAC__
	void OnChangeSize(wxSizeEvent &ev);
#endif

	DECLARE_EVENT_TABLE()
};

#endif
