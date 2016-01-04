//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ctlDefaultSecurityPanel.h - Panel with default security information
//
//////////////////////////////////////////////////////////////////////////


#ifndef CTL_DEFDEFSECPANEL_H
#define CTL_DEFDEFSECPANEL_H

#include <wx/wx.h>
#include <wx/dynarray.h>
#include <wx/notebook.h>

enum
{
	CTL_DEFPROPSQL = 500,
	CTL_DEFMSG,
	CTL_DEFLBPRIV,
	CTL_DEFSTATICGROUP,
	CTL_DEFCBGROUP,
	CTL_DEFADDPRIV,
	CTL_DEFDELPRIV,
	CTL_DEFALLPRIV,
	CTL_DEFALLPRIVGRANT,
	CTL_DEFPRIVCB          // base for all privilege checkboxes, must be last
};

class defaultPrivilegesOn
{
public:
	defaultPrivilegesOn(const wxChar, const wxString &, const wxString &);

	wxChar   m_privilegeType;
	wxString m_privilegesOn;
	wxString m_privileges;
	wxArrayString m_privilegesList;
};


DECLARE_LOCAL_EVENT_TYPE(EVT_DEFAULTSECURITYPANEL_CHANGE, -1)

class pgConn;
class ctlDefaultPrivilegesPanel;
class dlgDefaultSecurityProperty;

class ctlDefaultSecurityPanel : public wxPanel
{

public:
	ctlDefaultSecurityPanel(pgConn *, wxNotebook *, wxImageList *);

	wxString GetDefaultPrivileges(const wxString &schemaName);
	void     UpdatePrivilegePages(bool createDefPrivs, const wxString &defPrivsOnTables,
	                              const wxString &defPrivsOnSeqs, const wxString &defPrivsOnFuncs,
	                              const wxString &defPrivsOnTypes);

protected:
	wxNotebook    *nbNotebook;
	wxArrayString  m_groups;
	wxArrayString  m_namespaces;

	ctlDefaultPrivilegesPanel *m_defPrivOnTablesPanel, *m_defPrivOnSeqsPanel, *m_defPrivOnFuncsPanel, *m_defPrivOnTypesPanel;

	friend class ctlDefaultPrivilegesPanel;
	friend class dlgDefaultSecurityProperty;

};

class ctlDefaultPrivilegesPanel : public wxPanel
{

public:

	ctlDefaultPrivilegesPanel(ctlDefaultSecurityPanel *, wxNotebook *, defaultPrivilegesOn &, wxImageList *);
	~ctlDefaultPrivilegesPanel();

	void Update(wxString privs);
	wxString GetDefaultPrivileges(const wxString &schemaName);

protected:

	typedef struct
	{
		wxString m_username;
		wxString m_origPriv;
		wxString m_newPriv;
		bool     m_modified;
	} defPrivilege;

public:
	WX_DECLARE_STRING_HASH_MAP(defPrivilege, defPrivHash);

protected:

	bool                 m_defPrivChanged;
	int                  privilegeCount;
	defaultPrivilegesOn  m_privilegeType;
	defPrivHash          m_privileges;
	defPrivilege        *m_currentSelectedPriv;

	ctlDefaultSecurityPanel *m_defSecurityPanel;
	wxButton                *btnAddPriv, *btnDelPriv;
	wxCheckBox              **privCheckboxes;
	wxCheckBox              *allPrivileges, *allPrivilegesGrant;
	ctlListView             *lbPrivileges;
	ctlComboBox             *cbGroups;
	wxStaticText            *stGroup;

	void OnPrivSelChange(wxListEvent &ev);
	void OnAddPriv(wxCommandEvent &ev);
	void OnGroupChange(wxCommandEvent &ev);
	void OnDelPriv(wxCommandEvent &ev);
	void OnPrivCheck(wxCommandEvent &ev);
	void OnPrivCheckAll(wxCommandEvent &ev);
	void OnPrivCheckAllGrant(wxCommandEvent &ev);

	bool PrivCheckBoxUpdate(wxString &strUser);
	void CheckGrantOpt(int index);
	bool CanGrant();

	DECLARE_EVENT_TABLE()
};

#endif

