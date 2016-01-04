//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgType.h - Typeproperty
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_TYPEPROP
#define __DLG_TYPEPROP

#include "dlg/dlgProperty.h"
#include "ctl/ctlSeclabelPanel.h"

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

protected:
	/*
	*  Type Privileges:
	*  - Did not inherit dlgTypeProperty & dlgSecurityProperty as it will
	*    lead to a lot of problem later
	**/

	ctlSecurityPanel *securityPage;
	wxArrayString currentAcl;
	wxArrayString groups;
	bool securityChanged;

	wxString GetGrant(const wxString &allPattern, const wxString &grantObject);

	void OnAddPriv(wxCommandEvent &ev);
	void OnDelPriv(wxCommandEvent &ev);

#ifdef __WXMAC__
	void OnChangeSize(wxSizeEvent &ev);
#endif

private:
	pgSchema *schema;
	pgType *type;
	ctlSeclabelPanel *seclabelPage;

	void OnChange(wxCommandEvent &event);
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
	void OnNameChange(wxCommandEvent &ev);
	void OnSubtypeChange(wxCommandEvent &ev);

	void showDefinition(int panel);
	wxString GetFullTypeName(int type);

	wxString catGetText(wxString c);
	wxString catGetChar(wxString t);

	wxArrayString memberTypes, memberLengths, memberPrecisions, memberCollations, memberOriginalNames;
	bool queriesToBeSplitted;

	DECLARE_EVENT_TABLE()
};


#endif
