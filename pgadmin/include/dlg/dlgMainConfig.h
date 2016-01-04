//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgMainConfig.h - Configure setting
//
//////////////////////////////////////////////////////////////////////////

#ifndef __DLGMAINCONFIG_H
#define __DLGMAINCONFIG_H

#include "dlg/dlgClasses.h"
#include "utils/pgconfig.h"

// Class declarations
class dlgMainConfig : public DialogWithHelp
{
public:
	dlgMainConfig(pgFrame *parent, pgSettingItem *item);
	~dlgMainConfig();
	wxString GetHelpPage() const;

	int Go();

private:
	pgSettingItem *item;
	wxString GetValue();

	void OnOK(wxCommandEvent &ev);
	void OnCancel(wxCommandEvent &ev);
	void OnChange(wxCommandEvent &ev);
	DECLARE_EVENT_TABLE()
};

#endif
