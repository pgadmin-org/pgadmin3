//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgPackage.h - EnterpriseDB Package property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_PACKAGEPROP
#define __DLG_PACKAGEPROP

#include "dlg/dlgProperty.h"

class edbPackage;

class dlgPackage : public dlgSecurityProperty
{
public:
	dlgPackage(pgaFactory *f, frmMain *frame, edbPackage *node, pgSchema *sch);
	int Go(bool modal);

	void CheckChange();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();
	void SetObject(pgObject *obj)
	{
		package = (edbPackage *)obj;
	}

	wxString GetHelpPage(bool forCreate) const
	{
		return wxT("pg/packages-create");
	}

private:
	edbPackage *package;
	pgSchema *schema;

	virtual bool IsUpToDate();
#ifdef __WXMAC__
	void OnChangeSize(wxSizeEvent &ev);
#endif

	DECLARE_EVENT_TABLE()
};


#endif
