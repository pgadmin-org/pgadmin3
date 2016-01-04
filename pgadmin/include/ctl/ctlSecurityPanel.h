//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ctlSecurityPanel.h - Panel with security information
//
//////////////////////////////////////////////////////////////////////////


#ifndef CTL_SECPANEL_H
#define CTL_SECPANEL_H

#include <wx/wx.h>
#include <wx/notebook.h>

enum
{
	CTL_PROPSQL = 250,
	CTL_MSG,
	CTL_LBPRIV,
	CTL_STATICGROUP,
	CTL_CBGROUP,
	CTL_ADDPRIV,
	CTL_DELPRIV,
	CTL_ALLPRIV,
	CTL_ALLPRIVGRANT,
	CTL_PRIVCB          // base for all privilege checkboxes, must be last
};

DECLARE_LOCAL_EVENT_TYPE(EVT_SECURITYPANEL_CHANGE, -1)

class pgConn;

class ctlSecurityPanel : public wxPanel
{

public:

	ctlSecurityPanel(wxNotebook *nb, const wxString &privList, const char *privChars, wxImageList *imgList);
	~ctlSecurityPanel();

	ctlListView *lbPrivileges;
	ctlComboBox *cbGroups;
	wxStaticText *stGroup;
	void SetConnection(pgConn *conn);

	/*
	 *  Except column level privileges, column will be always an empty string in any case
	 */
	wxString GetGrant(const wxString &allPattern, const wxString &grantObject, wxArrayString *currentAcl = 0, wxString column = wxEmptyString);
	wxString GetUserPrivileges();
	bool DisablePrivilege(const wxString &priv);
protected:
	wxNotebook *nbNotebook;
	pgConn *connection;

	wxButton *btnAddPriv, *btnDelPriv;
	int privilegeCount;
	const char *privilegeChars;
	wxCheckBox **privCheckboxes;
	wxCheckBox *allPrivileges, *allPrivilegesGrant;

	void OnPrivSelChange(wxListEvent &ev);
	void OnAddPriv(wxCommandEvent &ev);
	void OnGroupChange(wxCommandEvent &ev);
	void OnDelPriv(wxCommandEvent &ev);
	void OnPrivCheck(wxCommandEvent &ev);
	void OnPrivCheckAll(wxCommandEvent &ev);
	void OnPrivCheckAllGrant(wxCommandEvent &ev);

	void CheckGrantOpt(int index);
	bool GrantAllowed() const;


	DECLARE_EVENT_TABLE()
};



#endif
