//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgExtension.h - Extension property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_EXTENSIONPROP
#define __DLG_EXTENSIONPROP

#include "dlg/dlgProperty.h"

class pgExtension;

class dlgExtension : public dlgProperty
{
public:
	dlgExtension(pgaFactory *factory, frmMain *frame, pgExtension *ext);
	int Go(bool modal);

	void CheckChange();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();

private:
	pgExtension *extension;
	void OnChangeName(wxCommandEvent &ev);
#ifdef __WXMAC__
	void OnChangeSize(wxSizeEvent &ev);
#endif

	DECLARE_EVENT_TABLE()
};


#endif
