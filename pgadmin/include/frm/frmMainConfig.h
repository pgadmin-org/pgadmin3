//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmMainConfig.h - Backend configuration tool
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMMAINCONFIG_H
#define FRMMAINCONFIG_H

#include "utils/pgconfig.h"
#include "frm/frmConfig.h"
#include "dlg/dlgClasses.h"
#include "utils/factory.h"

class ctlListView;
class pgServer;

WX_DECLARE_OBJARRAY(pgConfigOrgLine, pgConfigOrgLineArray);

class frmMainConfig : public frmConfig
{
public:
	frmMainConfig(const wxString &title, const wxString &configFile);
	frmMainConfig(frmMain *parent, pgServer *server = 0);

	~frmMainConfig();

protected:
	void DisplayFile(const wxString &str);
	void WriteFile(pgConn *conn = 0);
	wxString GetHintString();
	wxString GetHelpPage() const;

private:
	void Init();
	void Init(pgSettingReader *reader);
	void InitForm();

	void FillList(const wxString &categoryMember, const wxString &altCategoryMember = wxEmptyString);
	void FillList(wxArrayString *category);

	void OnContents(wxCommandEvent &event);
	void OnUndo(wxCommandEvent &event);
	void OnEditSetting(wxListEvent &event);
	void OnSelectSetting(wxListEvent &event);

	void UpdateLine(int line);

	void OnOpen(wxCommandEvent &event);


	ctlListView *cfgList;

	pgSettingItemHashmap options;
	pgCategoryHashmap categories;
	pgConfigOrgLineArray lines;
	wxString serverVersionNumber;

	DECLARE_EVENT_TABLE()
};


class mainConfigFactory : public actionFactory
{
public:
	mainConfigFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};


class mainConfigFileFactory : public actionFactory
{
public:
	mainConfigFileFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
};

#endif
