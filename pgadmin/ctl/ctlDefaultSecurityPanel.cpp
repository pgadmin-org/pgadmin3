//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ctlDefaultSecurityPanel.cpp - Panel with default security information
//
//////////////////////////////////////////////////////////////////////////


// wxWindows headers
#include <wx/wx.h>
#include <wx/settings.h>
#include <wx/imaglist.h>

// App headers
#include "pgAdmin3.h"
#include "ctl/ctlDefaultSecurityPanel.h"
#include "db/pgConn.h"
#include "dlg/dlgProperty.h"
#include "schema/pgGroup.h"
#include "schema/pgUser.h"
#include "utils/sysLogger.h"

#include <wx/arrimpl.cpp>

defaultPrivilegesOn g_defPrivTables('r', wxT("Tables"), wxT("arwdDxt")),
                    g_defPrivSequences('S', wxT("Sequences"), wxT("rwU")),
                    g_defPrivFunctions('f', wxT("Functions"), wxT("X")),
                    g_defPrivTypes('T', wxT("Types"), wxT("U"));

defaultPrivilegesOn::defaultPrivilegesOn(const wxChar privType, const wxString &privOn, const wxString &privileges)
	: m_privilegeType(privType), m_privilegesOn(privOn), m_privileges(privileges) {}

ctlDefaultSecurityPanel::ctlDefaultSecurityPanel(pgConn *conn, wxNotebook *nb, wxImageList *imgList)
	: wxPanel(nb, -1, wxDefaultPosition, wxDefaultSize), nbNotebook(NULL)
{
	nb->AddPage(this, _("Default Privileges"));

	wxFlexGridSizer *mainSizer = new wxFlexGridSizer(1, 1, 1, 1);
	mainSizer->AddGrowableCol(0);
	mainSizer->AddGrowableRow(0);

	nbNotebook = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize, 0, _("Default ACLs"));

	m_defPrivOnTablesPanel = new ctlDefaultPrivilegesPanel(this, nbNotebook, g_defPrivTables, imgList);
	m_defPrivOnSeqsPanel   = new ctlDefaultPrivilegesPanel(this, nbNotebook, g_defPrivSequences, imgList);
	m_defPrivOnFuncsPanel  = new ctlDefaultPrivilegesPanel(this, nbNotebook, g_defPrivFunctions, imgList);
	if (conn->BackendMinimumVersion(9, 2))
		m_defPrivOnTypesPanel  = new ctlDefaultPrivilegesPanel(this, nbNotebook, g_defPrivTypes, imgList);
	else
		m_defPrivOnTypesPanel = NULL;

	mainSizer->Add(nbNotebook, 0, wxEXPAND | wxALL, 2);

	this->SetSizer(mainSizer);
	mainSizer->Fit(this);
}

void  ctlDefaultSecurityPanel::UpdatePrivilegePages(bool createDefPrivs, const wxString &defPrivsOnTables,
        const wxString &defPrivsOnSeqs, const wxString &defPrivsOnFuncs, const wxString &defPrivsOnTypes)
{
	if (!createDefPrivs)
	{
		nbNotebook->Enable(false);
		return;
	}
	m_defPrivOnTablesPanel->Update(defPrivsOnTables);
	m_defPrivOnSeqsPanel->Update(defPrivsOnSeqs);
	m_defPrivOnFuncsPanel->Update(defPrivsOnFuncs);
	if (m_defPrivOnTypesPanel)
		m_defPrivOnTypesPanel->Update(defPrivsOnTypes);
}


wxString ctlDefaultSecurityPanel::GetDefaultPrivileges(const wxString &schemaName)
{
	wxString strDefPrivs;
	int nPageCount = nbNotebook->GetPageCount();
	for (int index = 0; index < nPageCount; index++)
	{
		strDefPrivs += (dynamic_cast<ctlDefaultPrivilegesPanel *>(nbNotebook->GetPage(index)))->GetDefaultPrivileges(schemaName);
	}
	return strDefPrivs;
}


///////////////////////////////////////////////////////////////////////////////
// ctlDefaultPrivilegesPanel
///////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(ctlDefaultPrivilegesPanel, wxPanel)
	EVT_LIST_ITEM_SELECTED(CTL_DEFLBPRIV,  ctlDefaultPrivilegesPanel::OnPrivSelChange)
	EVT_BUTTON(CTL_DEFADDPRIV,             ctlDefaultPrivilegesPanel::OnAddPriv)
	EVT_BUTTON(CTL_DEFDELPRIV,             ctlDefaultPrivilegesPanel::OnDelPriv)
	EVT_TEXT(CTL_DEFCBGROUP,               ctlDefaultPrivilegesPanel::OnGroupChange)
	EVT_COMBOBOX(CTL_DEFCBGROUP,           ctlDefaultPrivilegesPanel::OnGroupChange)
	EVT_CHECKBOX(CTL_DEFALLPRIV,           ctlDefaultPrivilegesPanel::OnPrivCheckAll)
	EVT_CHECKBOX(CTL_DEFALLPRIVGRANT,      ctlDefaultPrivilegesPanel::OnPrivCheckAllGrant)
	EVT_CHECKBOX(CTL_DEFPRIVCB,            ctlDefaultPrivilegesPanel::OnPrivCheck)
	EVT_CHECKBOX(CTL_DEFPRIVCB + 2,          ctlDefaultPrivilegesPanel::OnPrivCheck)
	EVT_CHECKBOX(CTL_DEFPRIVCB + 4,          ctlDefaultPrivilegesPanel::OnPrivCheck)
	EVT_CHECKBOX(CTL_DEFPRIVCB + 6,          ctlDefaultPrivilegesPanel::OnPrivCheck)
	EVT_CHECKBOX(CTL_DEFPRIVCB + 8,          ctlDefaultPrivilegesPanel::OnPrivCheck)
	EVT_CHECKBOX(CTL_DEFPRIVCB + 10,         ctlDefaultPrivilegesPanel::OnPrivCheck)
	EVT_CHECKBOX(CTL_DEFPRIVCB + 12,         ctlDefaultPrivilegesPanel::OnPrivCheck)
	EVT_CHECKBOX(CTL_DEFPRIVCB + 14,         ctlDefaultPrivilegesPanel::OnPrivCheck)
	EVT_CHECKBOX(CTL_DEFPRIVCB + 16,         ctlDefaultPrivilegesPanel::OnPrivCheck)
END_EVENT_TABLE();

DEFINE_LOCAL_EVENT_TYPE(EVT_DEFAULTSECURITYPANEL_CHANGE)

ctlDefaultPrivilegesPanel::ctlDefaultPrivilegesPanel(ctlDefaultSecurityPanel *defSecurityPanel, wxNotebook *nb,
        defaultPrivilegesOn &privOn, wxImageList *imgList)
	: wxPanel(nb, -1, wxDefaultPosition, wxDefaultSize), m_defPrivChanged(false),
	  m_privilegeType(privOn), m_defSecurityPanel(defSecurityPanel)
{

	nb->AddPage(this, m_privilegeType.m_privilegesOn);

	allPrivileges = 0;
	privCheckboxes = 0;
	m_currentSelectedPriv = NULL;

	privilegeCount = m_privilegeType.m_privileges.Length();

	wxFlexGridSizer *item0 = new wxFlexGridSizer(3, 1, 5, 5);
	item0->AddGrowableCol(0);
	item0->AddGrowableRow(0);

	privCheckboxes = new wxCheckBox*[privilegeCount * 2];

	wxFlexGridSizer *itemSizer1 = new wxFlexGridSizer(1, 1, 5, 5);
	itemSizer1->AddGrowableCol(0);
	itemSizer1->AddGrowableRow(0);

	wxString strGroupLabel = settings->GetShowUsersForPrivileges() ? _("Role/Group") : _("Role");

	lbPrivileges = new ctlListView(this, CTL_DEFLBPRIV, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER | wxLC_REPORT);
	lbPrivileges->SetImageList(imgList, wxIMAGE_LIST_SMALL);
	lbPrivileges->AddColumn(_("Role/Group"), 60, wxLIST_FORMAT_LEFT);
	lbPrivileges->AddColumn(_("Privileges"), 60, wxLIST_FORMAT_LEFT);
	itemSizer1->Add(lbPrivileges, 0, wxEXPAND | wxALIGN_CENTRE_VERTICAL | wxTOP | wxLEFT | wxRIGHT, 4);
	item0->Add(itemSizer1, 0, wxEXPAND | wxALL, 5);

	wxBoxSizer *itemSizer2 = new wxBoxSizer(wxHORIZONTAL);
	btnAddPriv = new wxButton(this, CTL_DEFADDPRIV, _("Add/Change"));
	itemSizer2->Add(btnAddPriv, 0, wxEXPAND | wxALIGN_CENTRE_VERTICAL | wxTOP | wxLEFT | wxRIGHT, 4);
	btnDelPriv = new wxButton(this, CTL_DEFDELPRIV, _("Remove"));
	itemSizer2->Add(btnDelPriv, 0, wxEXPAND | wxALIGN_CENTRE_VERTICAL | wxTOP | wxLEFT | wxRIGHT, 4);
	item0->Add(itemSizer2, 0, wxEXPAND | wxALL, 0);

	wxStaticBox *sb = new wxStaticBox(this, -1, _("Privileges"));
	wxBoxSizer *itemSizer3 = new wxStaticBoxSizer( sb, wxVERTICAL );
	item0->Add(itemSizer3, 0, wxEXPAND | wxALL, 5);

	wxBoxSizer *itemSizer4a = new wxBoxSizer(wxHORIZONTAL);
	stGroup = new wxStaticText(this, CTL_DEFSTATICGROUP, strGroupLabel);
	itemSizer4a->Add(stGroup, 0, wxEXPAND | wxALIGN_CENTRE_VERTICAL | wxTOP | wxLEFT | wxRIGHT, 4);
	cbGroups = new ctlComboBox(this, CTL_DEFCBGROUP, wxDefaultPosition, wxDefaultSize);
	cbGroups->Append(wxT("public"));
	cbGroups->SetSelection(0);
	itemSizer4a->Add(cbGroups, wxEXPAND | wxALIGN_CENTRE_VERTICAL | wxTOP | wxLEFT | wxRIGHT);
	itemSizer3->Add(itemSizer4a, 0, wxEXPAND | wxALL, 0);

	/* border size depends on the plateform */
#ifdef __WXMSW__
	int bordersize = 4;
#endif
#ifdef __WXMAC__
	int bordersize = 3;
#endif
#ifdef __WXGTK__
	int bordersize = 0;
#endif

	wxBoxSizer *itemSizer5 = new wxBoxSizer(wxHORIZONTAL);
	allPrivileges = new wxCheckBox(this, CTL_DEFALLPRIV, wxT("ALL"));
	itemSizer5->Add(allPrivileges, wxEXPAND | wxALIGN_CENTRE_VERTICAL | wxTOP | wxLEFT | wxRIGHT);
	allPrivilegesGrant = new wxCheckBox(this, CTL_DEFALLPRIVGRANT, wxT("WITH GRANT OPTION"));
	itemSizer5->Add(allPrivilegesGrant, wxEXPAND | wxALIGN_CENTRE_VERTICAL | wxTOP | wxLEFT | wxRIGHT);
	allPrivilegesGrant->Disable();
	itemSizer3->Add(itemSizer5, 0, wxALL, bordersize);

	for (int index = 0; index < privilegeCount; index++)
	{
		int i = index * 2;
		wxChar privilege = m_privilegeType.m_privileges.GetChar(index);
		wxString priv = pgObject::GetPrivilegeName(privilege);

		wxCheckBox *cb;
		wxBoxSizer *itemSizer6 = new wxBoxSizer(wxHORIZONTAL);
		cb = new wxCheckBox(this, CTL_DEFPRIVCB + i, priv);
		itemSizer6->Add(cb, wxEXPAND | wxALIGN_CENTRE_VERTICAL | wxTOP | wxLEFT | wxRIGHT);
		privCheckboxes[i++] = cb;
		cb = new wxCheckBox(this, CTL_DEFPRIVCB + i, wxT("WITH GRANT OPTION"));
		itemSizer6->Add(cb, wxEXPAND | wxALIGN_CENTRE_VERTICAL | wxTOP | wxLEFT | wxRIGHT);
		cb->Disable();
		privCheckboxes[i] = cb;
		itemSizer3->Add(itemSizer6, 0, wxALL, bordersize);

	}

	this->SetSizer(item0);
	item0->Fit(this);
}


ctlDefaultPrivilegesPanel::~ctlDefaultPrivilegesPanel()
{
	cbGroups->Clear();
	if (privCheckboxes)
		delete[] privCheckboxes;
}

// Find the privileges for the selected user from the map
// and select the check-boxes accordingly
bool ctlDefaultPrivilegesPanel::PrivCheckBoxUpdate(wxString &strRole)
{
	int index = 0;
	wxString strKey  = strRole;
	defPrivHash::iterator priv = m_privileges.find(strKey);
	bool canGrant = CanGrant();

	if (priv == m_privileges.end())
	{
		m_currentSelectedPriv = NULL;
		allPrivileges->SetValue(true);
		allPrivilegesGrant->Enable(canGrant);
		for (; index < privilegeCount; index++)
		{
			privCheckboxes[index * 2]->Enable();
			privCheckboxes[index * 2]->SetValue(true);
			privCheckboxes[index * 2 + 1]->Enable(canGrant);
		}
		return false;
	}
	else
	{
		allPrivileges->SetValue(false);
		allPrivilegesGrant->SetValue(false);
		allPrivilegesGrant->Enable(false);
	}

	wxString currentPrivileges = priv->second.m_modified ? priv->second.m_newPriv : priv->second.m_origPriv;
	m_currentSelectedPriv = &priv->second;

	for (index = 0; index < privilegeCount; index++)
	{
		int privAt = currentPrivileges.Find(m_privilegeType.m_privileges.GetChar(index));
		if (privAt != wxNOT_FOUND)
		{
			privCheckboxes[index * 2]->SetValue(true);
			privCheckboxes[index * 2 + 1]->Enable(canGrant);
			if (canGrant && (unsigned int)privAt < currentPrivileges.Length() - 1 && currentPrivileges.GetChar(privAt + 1) == wxT('*'))
				privCheckboxes[index * 2 + 1]->SetValue(true);
			else
				privCheckboxes[index * 2 + 1]->SetValue(false);
		}
		else
		{
			privCheckboxes[index * 2]->SetValue(false);
			privCheckboxes[index * 2 + 1]->SetValue(false);
			privCheckboxes[index * 2 + 1]->Disable();
		}
	}
	return false;
}


bool ctlDefaultPrivilegesPanel::CanGrant()
{
	if (cbGroups->GetValue() == wxT("public"))
		return false;
	return true;
}


void ctlDefaultPrivilegesPanel::OnGroupChange(wxCommandEvent &ev)
{
	cbGroups->GuessSelection(ev);

	wxString strRole   = cbGroups->GetValue();

	if (strRole.IsEmpty())
		return;

	btnAddPriv->Enable(!PrivCheckBoxUpdate(strRole));
}


void ctlDefaultPrivilegesPanel::OnPrivCheckAll(wxCommandEvent &ev)
{
	bool all = allPrivileges->GetValue();
	allPrivilegesGrant->Enable(all && CanGrant());

	for (int i = 0; i < privilegeCount; i++)
	{
		privCheckboxes[i * 2]->SetValue(all);
		CheckGrantOpt(i);
	}
}

void ctlDefaultPrivilegesPanel::OnPrivCheckAllGrant(wxCommandEvent &ev)
{
	bool grant = allPrivilegesGrant->GetValue();
	for (int i = 0 ; i < privilegeCount ; i++)
		privCheckboxes[i * 2 + 1]->SetValue(grant);
}


void ctlDefaultPrivilegesPanel::OnPrivCheck(wxCommandEvent &ev)
{
	int id = (ev.GetId() - CTL_DEFPRIVCB) / 2;
	CheckGrantOpt(id);
	btnAddPriv->Enable();
	if (!privCheckboxes[id * 2]->GetValue())
		allPrivileges->SetValue(false);
}


void ctlDefaultPrivilegesPanel::CheckGrantOpt(int id)
{
	bool canGrant = (privCheckboxes[id * 2]->GetValue() && CanGrant());
	if (canGrant)
		privCheckboxes[id * 2 + 1]->Enable(true);
	else
	{
		privCheckboxes[id * 2 + 1]->SetValue(false);
		privCheckboxes[id * 2 + 1]->Disable();
	}
	btnAddPriv->Enable();
}


void ctlDefaultPrivilegesPanel::OnDelPriv(wxCommandEvent &ev)
{
	int pos;
	wxString strRole;
	if ((pos = lbPrivileges->GetFirstSelected()) == -1)
		return;

	wxListItem info;
	info.m_itemId = pos;
	info.m_col    = 0;
	info.m_mask = wxLIST_MASK_TEXT;

	if (lbPrivileges->GetItem(info))
		strRole = info.m_text;

	wxString strKey  = strRole;
	defPrivHash::iterator priv = m_privileges.find(strKey);

	if (priv != m_privileges.end())
	{
		priv->second.m_modified = true;
		priv->second.m_newPriv = wxT("");
		if (m_currentSelectedPriv == &priv->second)
			PrivCheckBoxUpdate(strRole);
	}

	lbPrivileges->DeleteCurrentItem();
	m_defPrivChanged = true;

	ev.Skip();
}


void ctlDefaultPrivilegesPanel::OnAddPriv(wxCommandEvent &ev)
{
	wxString strRole, strPriv;
	bool isPresent = (m_currentSelectedPriv != NULL &&
	                  ((m_currentSelectedPriv->m_modified &&
	                    !m_currentSelectedPriv->m_newPriv.IsEmpty()) ||
	                   (!m_currentSelectedPriv->m_modified)))
	                 || lbPrivileges->FindItem(-1, cbGroups->GetGuessedStringSelection()) >= 0;

	if (allPrivileges && allPrivileges->GetValue())
	{
		if (allPrivilegesGrant->GetValue())
		{
			for (int index = 0; index < privilegeCount; index++)
			{
				strPriv += m_privilegeType.m_privileges.GetChar(index);
				strPriv += wxT('*');
			}
		}
		else
			strPriv =  m_privilegeType.m_privileges;
	}
	else
	{
		for (int index = 0; index < privilegeCount; index++)
		{
			if (privCheckboxes[index * 2]->GetValue())
			{
				strPriv += m_privilegeType.m_privileges.GetChar(index);
				if(privCheckboxes[index * 2 + 1]->IsEnabled() && privCheckboxes[index * 2 + 1]->GetValue())
					strPriv += wxT('*');
			}
		}
	}

	if (!m_currentSelectedPriv)
	{
		strRole   = cbGroups->GetGuessedStringSelection();

		if (strRole.IsEmpty())
			return;

		defPrivilege priv;
		priv.m_username = strRole;
		priv.m_origPriv = wxT("");
		priv.m_modified = true;
		priv.m_newPriv  = strPriv;

		wxString strKey = strRole;
		m_privileges[strKey] = priv;

		defPrivHash::iterator itr = m_privileges.find(strKey);
		m_currentSelectedPriv = &itr->second;
	}
	else
	{
		if (m_currentSelectedPriv->m_modified ?
		        m_currentSelectedPriv->m_newPriv == strPriv :
		        m_currentSelectedPriv->m_origPriv == strPriv)
		{
			ev.Skip();
			return;
		}
		strRole   = m_currentSelectedPriv->m_username;
		m_currentSelectedPriv->m_modified = true;
		m_currentSelectedPriv->m_newPriv  = strPriv;
	}

	if (!isPresent)
	{
		int icon;
		if (strRole.IsSameAs(wxT("public"), true))
			icon = PGICON_PUBLIC;
		else if (strRole.StartsWith(wxT("group ")))
			icon = groupFactory.GetIconId();
		else
			icon = userFactory.GetIconId();
		long pos = lbPrivileges->GetItemCount();

		lbPrivileges->InsertItem(pos, strRole, icon);
		lbPrivileges->SetItem(pos, 1, strPriv);
	}
	else
	{
		long nCount =  lbPrivileges->GetItemCount();
		wxListItem info;

		for (int index = 0; index < nCount; index++)
		{
			wxString strTempRole;

			info.m_itemId = index;
			info.m_col    = 0;
			info.m_mask = wxLIST_MASK_TEXT;

			if (lbPrivileges->GetItem(info))
				strTempRole = info.m_text;

			if (strTempRole == strRole)
			{
				lbPrivileges->SetItem(index, 1, strPriv);
				break;
			}
		}
	}
	m_defPrivChanged = true;

	ev.Skip();
}


void ctlDefaultPrivilegesPanel::OnPrivSelChange(wxListEvent &ev)
{
	if (!cbGroups)
		return;
	if (allPrivileges)
	{
		allPrivileges->SetValue(false);
		allPrivilegesGrant->SetValue(false);
		allPrivilegesGrant->Disable();
	}
	long pos = lbPrivileges->GetSelection();
	if (pos >= 0)
	{
		wxString name = lbPrivileges->GetText(pos);
		wxString value = lbPrivileges->GetText(pos, 1);

		pos = cbGroups->FindString(name);
		if (pos < 0)
		{
			cbGroups->Append(name);
			pos = cbGroups->GetCount() - 1;
		}
		cbGroups->SetSelection(pos);

		int i;
		for (i = 0 ; i < privilegeCount ; i++)
		{
			if (privCheckboxes[i * 2]->GetName() != wxT("DISABLE"))
			{
				privCheckboxes[i * 2]->Enable();
				int index = value.Find(m_privilegeType.m_privileges.GetChar(i));
				if (index >= 0)
				{
					privCheckboxes[i * 2]->SetValue(true);
					privCheckboxes[i * 2 + 1]->SetValue(value.Mid(index + 1, 1) == wxT("*"));
				}
				else
					privCheckboxes[i * 2]->SetValue(false);
			}
			CheckGrantOpt(i);
		}
		PrivCheckBoxUpdate(name);
	}
	btnAddPriv->Enable();
}


void ctlDefaultPrivilegesPanel::Update(wxString strDefPrivs)
{
	unsigned int index = 0;

	lbPrivileges->DeleteAllItems();

	m_privileges.clear();

	cbGroups->Clear();
	cbGroups->Append(wxT("public"));
	for (; index < m_defSecurityPanel->m_groups.GetCount(); index++)
	{
		cbGroups->Append(m_defSecurityPanel->m_groups[index]);
	}

	if (!strDefPrivs.IsEmpty())
	{
		wxString strRole, strPriv;
		strDefPrivs.Replace(wxT("\\\""), wxT("\""), true);
		strDefPrivs.Replace(wxT("\\\\"), wxT("\\"), true);

		// Removing starting brace '{' and ending brace '}'
		strDefPrivs = strDefPrivs.SubString(1, strDefPrivs.Length() - 1);

		long pos = 0;

		while (pgObject::findUserPrivs(strDefPrivs, strRole, strPriv))
		{
			int icon;
			if (strRole.IsSameAs(wxT("public"), true))
				icon = PGICON_PUBLIC;
			else if (cbGroups->FindString(strRole) != wxNOT_FOUND)
				icon = userFactory.GetIconId();
			else if (cbGroups->FindString(wxT("group ") + strRole) != wxNOT_FOUND)
			{
				icon = groupFactory.GetIconId();
				strRole = wxT("group ") + strRole;
			}
			else
				continue;

			defPrivilege priv;
			priv.m_username = strRole;
			priv.m_origPriv = strPriv;
			priv.m_modified = false;
			priv.m_newPriv  = wxT("");

			wxString strKey = strRole;
			m_privileges[strKey] = priv;

			pos = lbPrivileges->GetItemCount();

			lbPrivileges->InsertItem(pos, strRole, icon);
			lbPrivileges->SetItem(pos, 1, strPriv);

			strRole = wxT("");
			strPriv = wxT("");
			pos++;
		}
	}
}


wxString ctlDefaultPrivilegesPanel::GetDefaultPrivileges(const wxString &schemaName)
{
	if(!m_defPrivChanged)
		return wxT("");

	wxString strDefPrivs;
	defPrivHash::iterator itr = m_privileges.begin();

	for (; itr != m_privileges.end(); itr++)
	{
		if(itr->second.m_modified)
		{
			wxString strRole   = itr->second.m_username;

			if (strRole.StartsWith(wxT("group ")))
			{
				strRole = strRole.Mid(6);
				strRole = qtIdent(strRole);
			}
			else if (strRole != wxT("public"))
				strRole = qtIdent(strRole);

			wxString strOrigDefPrivs = itr->second.m_origPriv;
			wxString strNewDefPrivs  = itr->second.m_newPriv;

			strDefPrivs += pgObject::GetDefaultPrivileges(m_privilegeType.m_privilegesOn.Upper(), m_privilegeType.m_privileges, schemaName, strOrigDefPrivs, strNewDefPrivs, strRole);
		}
	}
	return strDefPrivs;
}


