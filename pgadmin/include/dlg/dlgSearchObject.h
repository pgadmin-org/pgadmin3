//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgSearchObject.h - Search dialogue
//
//////////////////////////////////////////////////////////////////////////

#ifndef DLGSEARCHOBJECT_H
#define DLGSEARCHOBJECT_H

#include "dlg/dlgClasses.h"
#include "ctl/ctlListView.h"
#include "schema/pgDatabase.h"
#include "utils/sysSettings.h"
#include "schema/pgSchema.h"

// Class declarations
class dlgSearchObject : public pgDialog
{
public:
	dlgSearchObject(frmMain *p, pgDatabase *db, pgObject *obj);
	~dlgSearchObject();

private:
	void OnHelp(wxCommandEvent &ev);
	void OnSearch(wxCommandEvent &ev);
	void OnCancel(wxCommandEvent &ev);
	void OnChange(wxCommandEvent &ev);
	void OnSelSearchResult(wxListEvent &ev);
	void SaveSettings();
	void RestoreSettings();
	wxString TranslatePath(wxString &path);
	wxString getMapKeyByValue(wxString);
	void ToggleBtnSearch(bool enable);
	WX_DECLARE_STRING_HASH_MAP(wxString, LngMapping);
	LngMapping aMap;

	pgDatabase *currentdb;
	frmMain *parent;
	wxString header;
	wxArrayString sectionName, sectionData, sectionTableHeader, sectionTableRows, sectionTableInfo, sectionSql;
	wxString currentSchema;
	int cbSchemaIdxCurrent;

	DECLARE_EVENT_TABLE()
};

///////////////////////////////////////////////////////
// Search Object Factory base class
///////////////////////////////////////////////////////
class searchObjectBaseFactory : public actionFactory
{
private:
	searchObjectBaseFactory(menuFactoryList *list) : actionFactory(list) {}
	frmMain *GetFrmMain()
	{
		return parent;
	};

	frmMain *parent;
public:
	bool CheckEnable(pgObject *obj)
	{
		return false;
	};
};


class searchObjectFactory : public contextActionFactory
{
public:
	searchObjectFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);

};

#endif
