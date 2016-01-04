//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgForeignTable.h - Foreign Table property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_FOREIGNTABLEPROP
#define __DLG_FOREIGNTABLEPROP

#include "dlg/dlgProperty.h"
#include "ctl/ctlSeclabelPanel.h"

class pgForeignTable;

class dlgForeignTable : public dlgTypeProperty
{
public:
	dlgForeignTable(pgaFactory *factory, frmMain *frame, pgForeignTable *node, pgSchema *schema);

	void CheckChange();
	wxString GetSql();
	wxString GetSqlForTypes();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();

	bool WannaSplitQueries()
	{
		return queriesToBeSplitted;
	}

	int Go(bool modal);

private:
	pgSchema *schema;
	pgForeignTable *foreigntable;
	ctlSeclabelPanel *seclabelPage;

	void OnMemberAdd(wxCommandEvent &ev);
	void OnMemberChange(wxCommandEvent &ev);
	void OnMemberRemove(wxCommandEvent &ev);
	void OnMemberSelChange(wxListEvent &ev);
	void OnSelChangeTyp(wxCommandEvent &ev);
	void OnSelChangeTypOrLen(wxCommandEvent &ev);
	void OnChangeMember(wxCommandEvent &ev);

	void OnChange(wxCommandEvent &event);
	void OnSelChangeOption(wxListEvent &ev);
	void OnChangeOptionName(wxCommandEvent &ev);
	void OnAddOption(wxCommandEvent &ev);
	void OnChangeOption(wxCommandEvent &ev);
	void OnRemoveOption(wxCommandEvent &ev);

	wxString GetOptionsSql();

	void showDefinition(int panel);
	wxString GetFullTypeName(int type);

	wxArrayString memberTypes, memberLengths, memberPrecisions, memberNotNulls;
	bool queriesToBeSplitted;

	DECLARE_EVENT_TABLE()
};


#endif
