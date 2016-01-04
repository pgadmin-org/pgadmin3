//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmBackup.h - Backup database dialogue
//
//////////////////////////////////////////////////////////////////////////


#ifndef FRMIMPORT_H
#define FRMIMPORT_H

#include "dlg/dlgClasses.h"
#include "utils/factory.h"

class frmMain;

class frmImport : public pgDialog
{
public:
	frmImport(frmMain *form, pgObject *_object, pgConn *_conn);
	~frmImport();

private:
	void OnHelp(wxCommandEvent &ev);
	void OnSelectFilename(wxCommandEvent &ev);
	void OnChangeFormat(wxCommandEvent &ev);
	void OnOK(wxCommandEvent &ev);

	pgConn *connection;
	pgObject *object;
	bool done;

	DECLARE_EVENT_TABLE()
};


class importFactory : public contextActionFactory
{
public:
	importFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};

#endif
