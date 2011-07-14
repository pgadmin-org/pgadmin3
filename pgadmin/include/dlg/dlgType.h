//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgType.h - Typeproperty
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_TYPEPROP
#define __DLG_TYPEPROP

#include "dlg/dlgProperty.h"

class pgType;

class dlgType : public dlgTypeProperty
{
public:
	dlgType(pgaFactory *factory, frmMain *frame, pgType *node, pgSchema *schema);

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
	pgType *type;
	void OnTypeChange(wxCommandEvent &ev);
	void OnMemberAdd(wxCommandEvent &ev);
	void OnMemberChange(wxCommandEvent &ev);
	void OnMemberRemove(wxCommandEvent &ev);
	void OnMemberSelChange(wxListEvent &ev);
	void OnLabelAddBefore(wxCommandEvent &ev);
	void OnLabelAddAfter(wxCommandEvent &ev);
	void OnLabelRemove(wxCommandEvent &ev);
	void OnLabelSelChange(wxListEvent &ev);
	void OnSelChangeTyp(wxCommandEvent &ev);
	void OnSelChangeTypOrLen(wxCommandEvent &ev);
	void OnChangeMember(wxCommandEvent &ev);

	void showDefinition(int panel);
	wxString GetFullTypeName(int type);

	wxArrayString memberTypes, memberLengths, memberPrecisions, memberCollations, memberOriginalNames;
	bool queriesToBeSplitted;

	DECLARE_EVENT_TABLE()
};


#endif
