//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgDatabase.h - Database property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_DATABASEPROP
#define __DLG_DATABASEPROP

#include "dlg/dlgProperty.h"
#include "ctl/ctlSeclabelPanel.h"

class pgDatabase;

class dlgDatabase : public dlgDefaultSecurityProperty
{
public:
	dlgDatabase(pgaFactory *factory, frmMain *frame, pgDatabase *db);
	int Go(bool modal);

	void CheckChange();
	wxString GetSql();
	wxString GetSql2();
	bool GetDisconnectFirst();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();
	wxString GetHelpPage() const;

private:
	pgDatabase *database;
	wxArrayString varInfo;
	bool schemaRestrictionOk;
	ctlSeclabelPanel *seclabelPage;

	void OnChange(wxCommandEvent &event);

#ifdef __WXMAC__
	void OnChangeSize(wxSizeEvent &ev);
#endif
	void OnChangeRestr(wxCommandEvent &ev);
	void OnGroupAdd(wxCommandEvent &ev);
	void OnGroupRemove(wxCommandEvent &ev);

	void OnVarAdd(wxCommandEvent &ev);
	void OnVarRemove(wxCommandEvent &ev);
	void OnVarSelChange(wxListEvent &ev);
	void OnCollateSelChange(wxCommandEvent &ev);
	void OnCTypeSelChange(wxCommandEvent &ev);
	void OnConnLimitChange(wxCommandEvent &ev);

	void OnVarnameSelChange(wxCommandEvent &ev);
	void OnOK(wxCommandEvent &ev);

	void SetupVarEditor(int var);
	bool executeDDLSql(const wxString &strSql);

	bool dirtyVars;

	DECLARE_EVENT_TABLE()

	friend class pgDatabaseFactory;
};


#endif
