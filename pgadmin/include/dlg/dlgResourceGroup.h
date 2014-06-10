//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2014, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgResourceGroup.h - Resource Group property
//
//////////////////////////////////////////////////////////////////////////

#ifndef __DLG_RESOURCEGROUPPROP
#define __DLG_RESOURCEGROUPPROP

#include "dlg/dlgProperty.h"

class edbResourceGroup;

class dlgResourceGroup : public dlgProperty
{
public:
	dlgResourceGroup(pgaFactory *factory, frmMain *frame, edbResourceGroup *node = 0);
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();

	void CheckChange();
	int Go(bool modal);

	bool WannaSplitQueries()
	{
		return true;
	}

private:
	void OnChange(wxCommandEvent &event);
	void OnOK(wxCommandEvent &ev);

private:
	edbResourceGroup *resourceGroup;
	DECLARE_EVENT_TABLE()
};

#endif
