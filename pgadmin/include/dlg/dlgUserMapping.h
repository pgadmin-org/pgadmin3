//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgUserMapping.h - User Mapping property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_USERMAPPINGPROP
#define __DLG_USERMAPPINGPROP

#include "dlg/dlgProperty.h"

class pgForeignServer;
class pgUserMapping;

class dlgUserMapping : public dlgProperty
{
public:
	dlgUserMapping(pgaFactory *factory, frmMain *frame, pgUserMapping *node, pgForeignServer *parent);
	int Go(bool modal);

	void CheckChange();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();

private:
	pgForeignServer *foreignserver;
	pgUserMapping *usermapping;

#ifdef __WXMAC__
	void OnChangeSize(wxSizeEvent &ev);
#endif

	void OnSelChangeOption(wxListEvent &ev);
	void OnChange(wxCommandEvent &ev);
	void OnChangeOptionName(wxCommandEvent &ev);
	void OnAddOption(wxCommandEvent &ev);
	void OnChangeOption(wxCommandEvent &ev);
	void OnRemoveOption(wxCommandEvent &ev);

	wxString GetOptionsSql();

	DECLARE_EVENT_TABLE()
};


#endif
