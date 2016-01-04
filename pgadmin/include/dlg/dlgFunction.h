//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgFunction.h - Function property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_FUNCTIONPROP
#define __DLG_FUNCTIONPROP

#include "dlg/dlgProperty.h"
#include "ctl/ctlSeclabelPanel.h"

class pgSchema;
class pgFunction;

class dlgFunction : public dlgSecurityProperty
{
public:
	dlgFunction(pgaFactory *factory, frmMain *frame, pgFunction *func, pgSchema *sch);
	int Go(bool modal);

	void CheckChange();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();
	void SetObject(pgObject *obj)
	{
		function = (pgFunction *)obj;
	}

	wxString GetHelpPage(bool forCreate) const
	{
		return wxT("pg/sql-createfunction");
	}

private:
	pgSchema *schema;
	pgFunction *function;
	ctlSeclabelPanel *seclabelPage;
	wxArrayString varInfo;

	void OnChange(wxCommandEvent &event);

#ifdef __WXMAC__
	void OnChangeSize(wxSizeEvent &ev);
#endif

	void OnChangeArgName(wxCommandEvent &ev);
	void OnChangeReturn(wxCommandEvent &ev);
	void OnChangeSetof(wxCommandEvent &ev);
	void OnSelChangeLanguage(wxCommandEvent &ev);
	void OnSelChangeArg(wxListEvent &ev);
	void OnSelChangeType(wxCommandEvent &ev);
	void OnAddArg(wxCommandEvent &ev);
	void OnChangeArg(wxCommandEvent &ev);
	void OnChangeArgMode(wxCommandEvent &ev);
	void OnRemoveArg(wxCommandEvent &ev);

	void OnVarAdd(wxCommandEvent &ev);
	void OnVarRemove(wxCommandEvent &ev);
	void OnVarSelChange(wxListEvent &ev);
	void OnVarnameSelChange(wxCommandEvent &ev);
	void OnChangeWindow(wxCommandEvent &ev);
	void SetupVarEditor(int var);

	wxString GetSelectedDirection();
	wxString GetArgs(const bool withNames = true, const bool inOnly = false);
	void ReplaceSizer(wxWindow *w, bool isC, int border);

	virtual bool IsUpToDate();

	wxArrayString typOids;
	wxArrayString types;
	wxArrayString argOids;

	int typeColNo;

protected:
	bool isProcedure;
	bool isBackendMinVer84;
	bool isEdbWrapped;

	DECLARE_EVENT_TABLE()
};


class dlgProcedure : public dlgFunction
{
public:
	dlgProcedure(pgaFactory *factory, frmMain *frame, pgFunction *node, pgSchema *sch);

	wxString GetHelpPage(bool forCreate) const
	{
		return wxT("pg/sql-createprocedure");
	}
};


#endif
