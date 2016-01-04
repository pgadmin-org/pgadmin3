//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgPgpassConfig.h - Configure setting
//
//////////////////////////////////////////////////////////////////////////

#ifndef __DLGPGPASSCONFIG_H
#define __DLGPGPASSCONFIG_H

#include "dlg/dlgClasses.h"
#include "utils/pgconfig.h"

// Class declarations
class dlgPgpassConfig : public DialogWithHelp
{
public:
	dlgPgpassConfig(pgFrame *parent, pgPassConfigLine *line);
	~dlgPgpassConfig();
	wxString GetHelpPage() const;

	int Go();

private:
	pgPassConfigLine *line;

	void OnOK(wxCommandEvent &ev);
	void OnCancel(wxCommandEvent &ev);
	void OnChange(wxCommandEvent &ev);

	wxString database, user;

	bool databaseAdding, userAdding;

	DECLARE_EVENT_TABLE()
};

#endif
