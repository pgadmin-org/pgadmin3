//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmGrantWizard.h - Grant Wizard Dialogue
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMGRANTWIZARD_H
#define FRMGRANTWIZARD_H

#include <wx/notebook.h>
#include "dlg/dlgClasses.h"
#include "utils/factory.h"

class ctlSecurityPanel;

DECLARE_LOCAL_EVENT_TYPE(EVT_SECURITYPANEL_CHANGE, -1)

class frmGrantWizard : public ExecutionDialog
{
public:
	frmGrantWizard(frmMain *form, pgObject *_object);
	~frmGrantWizard();

	void Go();
	wxString GetSql();
	wxString GetHelpPage() const;

private:

	void OnPageSelect(wxNotebookEvent &event);
	void OnCheckAll(wxCommandEvent &event);
	void OnUncheckAll(wxCommandEvent &event);
	void OnChange(wxCommandEvent &event);

	void AddObjects(pgCollection *collection);

	wxArrayPtrVoid objectArray;
	ctlSQLBox *sqlPane;
	wxNotebook *nbNotebook;
	ctlSecurityPanel *securityPage;

	DECLARE_EVENT_TABLE()
};

class grantWizardFactory : public contextActionFactory
{
public:
	grantWizardFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};

#endif
