//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgSchema.h - Schemaproperty
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_SCHEMAPROP
#define __DLG_SCHEMAPROP

#include "dlg/dlgProperty.h"
#include "ctl/ctlSeclabelPanel.h"

class pgSchema;

class dlgSchema : public dlgDefaultSecurityProperty
{
public:
	dlgSchema(pgaFactory *factory, frmMain *frame, pgSchema *node, pgObject *parent);
	int Go(bool modal);

	void CheckChange();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();

private:
	pgSchema *schema;
	ctlSeclabelPanel *seclabelPage;

	void OnChange(wxCommandEvent &event);
#ifdef __WXMAC__
	void OnChangeSize(wxSizeEvent &ev);
#endif

	DECLARE_EVENT_TABLE()
};


#endif
