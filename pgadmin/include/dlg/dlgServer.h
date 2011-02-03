//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgServer.h - Server property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_SERVERPROP
#define __DLG_SERVERPROP

#include "dlg/dlgProperty.h"
#include <wx/clrpicker.h>

class pgServer;

class dlgServer : public dlgProperty
{
public:
	dlgServer(pgaFactory *factory, frmMain *frame, pgServer *s);
	~dlgServer();
	int Go(bool modal);
	int GoNew();

	void CheckChange();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();
	wxString GetHelpPage() const;

	wxString GetPassword();
	bool GetTryConnect();

private:
	pgServer *server;
	bool dbRestrictionOk;

	void OnOK(wxCommandEvent &ev);
	void OnChangeRestr(wxCommandEvent &ev);
	void OnChangeTryConnect(wxCommandEvent &ev);
	void OnPageSelect(wxNotebookEvent &event);
	void OnChangeColour(wxColourPickerEvent &ev);

	DECLARE_EVENT_TABLE()
};

#endif
