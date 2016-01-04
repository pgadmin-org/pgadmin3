//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmBackupGlobals.h - Backup globals dialogue
//
//////////////////////////////////////////////////////////////////////////


#ifndef FRMBACKUPGLOBALS_H
#define FRMBACKUPGLOBALS_H

#include "dlg/dlgClasses.h"
#include "utils/factory.h"

class frmMain;

class frmBackupGlobals : public ExternProcessDialog
{
public:
	frmBackupGlobals(frmMain *form, pgObject *_object);
	~frmBackupGlobals();

	void Go();
	wxString GetDisplayCmd(int step);
	wxString GetCmd(int step);

private:
	wxString GetHelpPage() const;
	void OnChange(wxCommandEvent &ev);
	void OnSelectFilename(wxCommandEvent &ev);
	wxString getCmdPart1();
	wxString getCmdPart2();
	void OnOK(wxCommandEvent &ev);

	pgObject *object;
	wxString processedFile;
	wxString backupExecutable;

	DECLARE_EVENT_TABLE()
};


class backupGlobalsFactory : public contextActionFactory
{
public:
	backupGlobalsFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};

#endif
