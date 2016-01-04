//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgColumn.h - Column property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_COLUMNPROP
#define __DLG_COLUMNPROP

#include "dlg/dlgProperty.h"
#include "ctl/ctlSeclabelPanel.h"

class pgColumn;
class pgTable;

class dlgColumn : public dlgTypeProperty
{
public:
	dlgColumn(pgaFactory *factory, frmMain *frame, pgColumn *column, pgTable *parentNode);
	void SetSecurityPage(const pgColumn *node);
	void CheckChange();
	void SetChangedCol(pgColumn *changedCol);
	void ApplyChangesToObj(pgColumn *changedCol);
	void ApplyChangesToDlg();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();
	wxString GetDefinition();
	void GetVariableList(wxArrayString &);
	void GetSecLabelList(wxArrayString &);
	wxString GetPreviousDefinition()
	{
		return previousDefinition;
	}
	wxString GetComment()
	{
		return txtComment->GetValue();
	}
	wxString GetStatistics()
	{
		return CTRL_TEXT("txtAttstattarget")->GetValue();
	}
	wxString GetTypeOid();

	int Go(bool modal);

	wxString GetHelpPage(bool forCreate) const
	{
		return wxT("pg/sql-createtable");
	}

protected:
	/*
	*  Column Level Privileges:
	*  - Did not inherit dlgTypeProperty & dlgSecurityProperty as, it will
	*    lead to a lot of problem later
	**/

	ctlSecurityPanel *securityPage;
	wxArrayString currentAcl;
	wxArrayString groups;
	bool securityChanged;
	ctlSeclabelPanel *seclabelPage;

	void OnChange(wxCommandEvent &event);

	wxString GetGrant(const wxString &allPattern, const wxString &grantObject);

	void OnAddPriv(wxCommandEvent &ev);
	void OnDelPriv(wxCommandEvent &ev);

#ifdef __WXMAC__
	void OnChangeSize(wxSizeEvent &ev);
#endif

	void OnVarAdd(wxCommandEvent &ev);
	void OnVarRemove(wxCommandEvent &ev);
	void OnVarSelChange(wxListEvent &ev);
	void OnVarnameSelChange(wxCommandEvent &ev);
	void SetupVarEditor(int var);

private:
	pgColumn *column;
	pgTable *table;
	pgColumn *changedColumn;
	wxArrayString varInfo;
	bool dirtyVars;

	void OnSelChangeTyp(wxCommandEvent &ev);

	wxString previousDefinition;
	wxArrayString sequences;

	DECLARE_EVENT_TABLE()
};


#endif
