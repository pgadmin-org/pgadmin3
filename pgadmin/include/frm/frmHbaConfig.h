//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmHbaConfig.h - Backend access configuration tool
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMHBACONFIG_H
#define FRMHBACONFIG_H

#include "frm/frmConfig.h"
#include "utils/pgconfig.h"

class pgConn;
class pgServer;
class ctlListView;


WX_DECLARE_OBJARRAY(pgHbaConfigLine, pgHbaConfigLineArray);

class frmHbaConfig : public frmConfig
{
public:
	frmHbaConfig(const wxString &title, const wxString &configFile);
	frmHbaConfig(frmMain *parent, pgServer *server = 0);
	~frmHbaConfig();

protected:
	void DisplayFile(const wxString &str);
	void WriteFile(pgConn *conn = 0);
	wxString GetHintString();
	wxString GetHelpPage() const;

private:
	void Init();
	void UpdateDisplay(pgHbaConfigLine &line);

	void OnContents(wxCommandEvent &event);
	void OnUndo(wxCommandEvent &event);
	void OnDelete(wxCommandEvent &event);
	void OnEditSetting(wxListEvent &event);
	void OnSelectSetting(wxListEvent &event);

	ctlListView *listEdit;
	pgHbaConfigLineArray lines;


	DECLARE_EVENT_TABLE()
};

class hbaConfigFactory : public actionFactory
{
public:
	hbaConfigFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};


class hbaConfigFileFactory : public actionFactory
{
public:
	hbaConfigFileFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
};

#endif
