//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgForeignServer.h - Foreign Server property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_FOREIGNSERVERPROP
#define __DLG_FOREIGNSERVERPROP

#include "dlg/dlgProperty.h"

class pgForeignDataWrapper;
class pgForeignServer;

class dlgForeignServer : public dlgSecurityProperty
{
public:
	dlgForeignServer(pgaFactory *factory, frmMain *frame, pgForeignServer *node, pgForeignDataWrapper *parent);
	int Go(bool modal);

	void CheckChange();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();

private:
	pgForeignDataWrapper *foreigndatawrapper;
	pgForeignServer *foreignserver;

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
