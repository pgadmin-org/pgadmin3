//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmExport.h - The export file dialogue
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMEXPORT_H
#define FRMEXPORT_H


class ctlSQLResult;
class pgSet;

#include "dlg/dlgClasses.h"

// Class declarations
class frmExport : public pgDialog
{
public:
	frmExport(wxWindow *parent);
	~frmExport();

	bool Export(pgSet *set);

private:
	void OnChange(wxCommandEvent &ev);
	void OnHelp(wxCommandEvent &ev);
	void OnOK(wxCommandEvent &ev);
	void OnCancel(wxCommandEvent &ev);
	void OnBrowseFile(wxCommandEvent &ev);

	wxWindow *parent;

	DECLARE_EVENT_TABLE()
};

#endif
