//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgResourceGroup.cpp - Resource Group Property
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "dlg/dlgResourceGroup.h"
#include "schema/edbResourceGroup.h"


// pointer to controls
#define txtResGrpName	CTRL_TEXT("txtResGrpName")
#define txtCPURate		CTRL_TEXT("txtCPURate")
#define txtDirtyRate	CTRL_TEXT("txtDirtyRate")

dlgProperty *edbResourceGroupFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgResourceGroup(this, frame, (edbResourceGroup *)node);
}


BEGIN_EVENT_TABLE(dlgResourceGroup, dlgProperty)
	EVT_TEXT(XRCID("txtCPURate"), dlgResourceGroup::OnChange)
	EVT_TEXT(XRCID("txtDirtyRate"), dlgResourceGroup::OnChange)
	EVT_BUTTON(wxID_OK, dlgResourceGroup::OnOK)
END_EVENT_TABLE();

dlgResourceGroup::dlgResourceGroup(pgaFactory *f, frmMain *frame, edbResourceGroup *node)
	: dlgProperty(f, frame, wxT("dlgResourceGroup"))
{
	resourceGroup = node;
	m_cpuRate = wxEmptyString;
	m_dirtyRate = wxEmptyString;
	m_isNameChange = false;
}

pgObject *dlgResourceGroup::GetObject()
{
	return resourceGroup;
}

int dlgResourceGroup::Go(bool modal)
{
	txtCPURate->SetValidator(numericValidator);
	txtDirtyRate->SetValidator(numericValidator);

	if (resourceGroup)
	{
		wxString sql = wxT("SELECT rgrpcpuratelimit, rgrpdirtyratelimit from edb_resource_group WHERE rgrpname = '")
		               + resourceGroup->GetName() + wxT("'");

		pgSet *set = connection->ExecuteSet(sql);
		if (set && set->NumRows() > 0)
		{
			txtCPURate->SetValue(set->GetVal(0));
			txtDirtyRate->SetValue(set->GetVal(1));
			delete set;
		}
	}
	else
	{
		txtCPURate->SetValue(wxT("0"));
		txtDirtyRate->SetValue(wxT("0"));
	}

	m_cpuRate = txtCPURate->GetValue();
	m_dirtyRate = txtDirtyRate->GetValue();

	return dlgProperty::Go(modal);
}

void dlgResourceGroup::OnChange(wxCommandEvent &event)
{
	CheckChange();
}

void dlgResourceGroup::CheckChange()
{
	if (resourceGroup)
	{
		EnableOK(!GetSql().IsEmpty());
	}
	else
	{
		wxString name = GetName();
		wxString cpuRate = txtCPURate->GetValue();
		wxString dirtyRate = txtDirtyRate->GetValue();

		bool enable = true;
		CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
		CheckValid(enable, !cpuRate.IsEmpty(), _("Please specify CPU rate limit."));
		CheckValid(enable, !dirtyRate.IsEmpty(), _("Please specify Dirty rate limit."));

		EnableOK(enable);
	}
}

pgObject *dlgResourceGroup::CreateObject(pgCollection *collection)
{
	wxString name = GetName();

	pgObject *obj = resourceGroupFactory.CreateObjects(collection, 0, wxT("\n WHERE rgrpname= ") + qtDbString(name));
	return obj;
}

wxString dlgResourceGroup::GetSql()
{
	wxString sql = wxEmptyString;
	wxString name = GetName();
	wxString cpuRate = txtCPURate->GetValue();
	wxString dirtyRate = txtDirtyRate->GetValue();

	if (resourceGroup)
	{
		AppendNameChange(sql, wxT("RESOURCE GROUP ") + resourceGroup->GetQuotedFullIdentifier());

		// Update the flag if resource group name is changed so that next
		// ALTER command will be displayed in the second SQL text box.
		m_isNameChange = !sql.IsEmpty();

		// Check the "cpu rate/dirty rate" limit is changed or not
		if (!m_isNameChange && (m_cpuRate.compare(cpuRate) != 0 || m_dirtyRate.compare(dirtyRate) != 0))
		{
			sql += wxT("ALTER RESOURCE GROUP ") + qtIdent(name) + wxT(" SET cpu_rate_limit = ") +
			       cpuRate + wxT(", dirty_rate_limit = ") + dirtyRate + wxT(";\n");
		}
	}
	else
	{
		sql = wxT("CREATE RESOURCE GROUP ") + qtIdent(name) + wxT(";\n");
	}

	return sql;
}

wxString dlgResourceGroup::GetSql2()
{
	wxString sql = wxEmptyString;
	wxString name = GetName();
	wxString cpuRate = txtCPURate->GetValue();
	wxString dirtyRate = txtDirtyRate->GetValue();

	if (!resourceGroup || m_isNameChange)
	{
		// Check the "cpu rate/dirty rate" limit is changed or not
		if (m_cpuRate.compare(cpuRate) != 0 || m_dirtyRate.compare(dirtyRate) != 0)
		{
			sql = wxT("ALTER RESOURCE GROUP ") + qtIdent(name) + wxT(" SET cpu_rate_limit = ") +
			      cpuRate + wxT(", dirty_rate_limit = ") + dirtyRate + wxT(";\n");
		}
	}

	return sql;
}

void dlgResourceGroup::OnOK(wxCommandEvent &ev)
{
	dlgProperty::OnOK(ev);
}
