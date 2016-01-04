//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmMaintenance.h - Maintenance options selection dialogue
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMMAINTENANCE_H
#define FRMMAINTENANCE_H

#include "dlg/dlgClasses.h"
#include "utils/factory.h"

// Class declarations
class frmMaintenance : public ExecutionDialog
{
public:
	frmMaintenance(frmMain *form, pgObject *_object);
	~frmMaintenance();
	wxString GetSql();

	void Go();

private:
	wxString GetHelpPage() const;
	void OnAction(wxCommandEvent &ev);

	DECLARE_EVENT_TABLE()
};


class maintenanceFactory : public contextActionFactory
{
public:
	maintenanceFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};


#endif
