//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgDomain.cpp - PostgreSQL Domain Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "frm/frmMain.h"
#include "utils/misc.h"
#include "utils/pgDefs.h"

#include "dlg/dlgDomain.h"
#include "dlg/dlgCheck.h"
#include "schema/pgSchema.h"
#include "schema/pgCheck.h"
#include "schema/pgDomain.h"
#include "schema/pgDatatype.h"
#include "ctl/ctlSeclabelPanel.h"


// pointer to controls
#define chkNotNull          CTRL_CHECKBOX("chkNotNull")
#define txtDefault          CTRL_TEXT("txtDefault")
#define cbCollation         CTRL_COMBOBOX("cbCollation")
#define lstConstraints      CTRL_LISTVIEW("lstConstraints")
#define btnAddConstr        CTRL_BUTTON("btnAddConstr")
#define cbConstrType        CTRL_COMBOBOX("cbConstrType")
#define btnRemoveConstr     CTRL_BUTTON("btnRemoveConstr")

BEGIN_EVENT_TABLE(dlgDomain, dlgTypeProperty)
	EVT_TEXT(XRCID("txtLength"),                    dlgProperty::OnChange)
	EVT_TEXT(XRCID("txtPrecision"),                 dlgProperty::OnChange)
	EVT_TEXT(XRCID("cbDatatype"),                   dlgDomain::OnSelChangeTyp)
	EVT_COMBOBOX(XRCID("cbDatatype"),               dlgDomain::OnSelChangeTyp)
	EVT_TEXT(XRCID("txLength"),                     dlgProperty::OnChange)
	EVT_TEXT(XRCID("txtDefault"),                   dlgProperty::OnChange)
	EVT_CHECKBOX(XRCID("chkNotNull"),               dlgProperty::OnChange)
	EVT_BUTTON(XRCID("btnAddConstr"),               dlgDomain::OnAddConstr)
	EVT_BUTTON(XRCID("btnRemoveConstr"),            dlgDomain::OnRemoveConstr)
	EVT_LIST_ITEM_SELECTED(XRCID("lstConstraints"), dlgDomain::OnSelChangeConstr)
END_EVENT_TABLE();


dlgProperty *pgDomainFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgDomain(this, frame, (pgDomain *)node, (pgSchema *)parent);
}


dlgDomain::dlgDomain(pgaFactory *f, frmMain *frame, pgDomain *node, pgSchema *sch)
	: dlgTypeProperty(f, frame, wxT("dlgDomain"))
{
	schema = sch;
	domain = node;

	seclabelPage = new ctlSeclabelPanel(nbNotebook);

	txtLength->Disable();
	txtPrecision->Disable();

	lstConstraints->CreateColumns(0, _("Constraint name"), _("Definition"), 90);
}


pgObject *dlgDomain::GetObject()
{
	return domain;
}


int dlgDomain::Go(bool modal)
{
	if (connection->BackendMinimumVersion(9, 1))
	{
		seclabelPage->SetConnection(connection);
		seclabelPage->SetObject(domain);
		this->Connect(EVT_SECLABELPANEL_CHANGE, wxCommandEventHandler(dlgDomain::OnChange));
	}
	else
		seclabelPage->Disable();

	if (domain)
	{
		// edit mode
		cbSchema->Enable(connection->BackendMinimumVersion(8, 1));
		cbDatatype->Append(domain->GetBasetype());
		AddType(wxT(" "), 0, domain->GetBasetype());
		cbDatatype->SetSelection(0);
		if (domain->GetLength() >= 0)
		{
			txtLength->SetValue(NumToStr(domain->GetLength()));
			if (domain->GetPrecision() >= 0)
				txtPrecision->SetValue(NumToStr(domain->GetPrecision()));
		}
		chkNotNull->SetValue(domain->GetNotNull());
		txtDefault->SetValue(domain->GetDefault());

		wxCookieType cookie;
		pgObject *data = 0;
		wxTreeItemId item = mainForm->GetBrowser()->GetFirstChild(domain->GetId(), cookie);
		while (item)
		{
			data = mainForm->GetBrowser()->GetObject(item);
			pgaFactory *factory = data->GetFactory();
			if (factory == checkFactory.GetCollectionFactory())
				constraintsItem = item;
			else if (data->GetMetaType() == PGM_CONSTRAINT)
				constraintsItem = item;

			if (constraintsItem)
				break;

			item = mainForm->GetBrowser()->GetNextChild(domain->GetId(), cookie);
		}

		if (constraintsItem)
		{
			pgCollection *coll = (pgCollection *)mainForm->GetBrowser()->GetObject(constraintsItem);
			// make sure all constraints are appended
			coll->ShowTreeDetail(mainForm->GetBrowser());
			// this is the constraints collection
			item = mainForm->GetBrowser()->GetFirstChild(constraintsItem, cookie);

			// add constraints
			while (item)
			{
				data = mainForm->GetBrowser()->GetObject(item);
				switch (data->GetMetaType())
				{
					case PGM_CHECK:
					{
						pgCheck *obj = (pgCheck *)data;

						lstConstraints->AppendItem(data->GetIconId(), obj->GetName(), obj->GetDefinition());
						constraintsDefinition.Add(obj->GetDefinition());
						previousConstraints.Add(obj->GetQuotedIdentifier()
						                        + wxT(" ") + obj->GetTypeName().Upper() + wxT(" ") + obj->GetDefinition());
						break;
					}
				}

				item = mainForm->GetBrowser()->GetNextChild(constraintsItem, cookie);
			}
		}

		cbDatatype->Disable();

		cbCollation->SetValue(domain->GetQuotedCollation());
		cbCollation->Disable();

		if (!connection->BackendMinimumVersion(7, 4))
		{
			cbOwner->Disable();
			txtDefault->Disable();
			chkNotNull->Disable();
		}
	}
	else
	{
		// create mode
		FillDatatype(cbDatatype, false);

		cbCollation->Enable(connection->BackendMinimumVersion(9, 1));
		if (connection->BackendMinimumVersion(9, 1))
		{
			// fill collation combobox
			cbCollation->Append(wxEmptyString);
			pgSet *set = connection->ExecuteSet(
			                 wxT("SELECT nspname, collname\n")
			                 wxT("  FROM pg_collation c, pg_namespace n\n")
			                 wxT("  WHERE c.collnamespace=n.oid\n")
			                 wxT("  ORDER BY nspname, collname"));
			if (set)
			{
				while (!set->Eof())
				{
					wxString name = qtIdent(set->GetVal(wxT("nspname"))) + wxT(".") + qtIdent(set->GetVal(wxT("collname")));
					cbCollation->Append(name);
					set->MoveNext();
				}
				delete set;
			}
			cbCollation->SetSelection(0);
		}
	}

	cbConstrType->Clear();
	cbConstrType->Append(_("Check"));
	cbConstrType->SetSelection(0);
	btnRemoveConstr->Disable();

	return dlgProperty::Go(modal);
}


pgObject *dlgDomain::CreateObject(pgCollection *collection)
{
	wxString name = GetName();

	pgObject *obj = domainFactory.CreateObjects(collection, 0,
	                wxT("   AND d.typname=") + qtDbString(name) +
	                wxT("\n   AND d.typnamespace=") + schema->GetOidStr() +
	                wxT("\n"));
	return obj;
}


void dlgDomain::CheckChange()
{
	bool enable = true;

	if (domain)
	{
		enable = false;
		if (connection->BackendMinimumVersion(7, 4) || lstColumns->GetItemCount() > 0)
		{
			enable = !GetSql().IsEmpty();
		}
		if (seclabelPage && connection->BackendMinimumVersion(9, 1))
			enable = enable || !(seclabelPage->GetSqlForSecLabels().IsEmpty());
	}
	else
	{
		wxString name = GetName();
		long varlen = StrToLong(txtLength->GetValue()),
		     varprec = StrToLong(txtPrecision->GetValue());

		txtPrecision->Enable(isVarPrec && varlen > 0);

		CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
		CheckValid(enable, cbDatatype->GetGuessedSelection() >= 0, _("Please select a datatype."));
		CheckValid(enable, !isVarLen || txtLength->GetValue().IsEmpty()
		           || (varlen >= minVarLen && varlen <= maxVarLen && NumToStr(varlen) == txtLength->GetValue()),
		           _("Please specify valid length."));
		CheckValid(enable, !txtPrecision->IsEnabled()
		           || (varprec >= 0 && varprec <= varlen && NumToStr(varprec) == txtPrecision->GetValue()),
		           _("Please specify valid numeric precision (0..") + NumToStr(varlen) + wxT(")."));
	}
	EnableOK(enable);
}



void dlgDomain::OnSelChangeTyp(wxCommandEvent &ev)
{
	if (!domain)
	{
		cbDatatype->GuessSelection(ev);
		CheckLenEnable();
		txtLength->Enable(isVarLen);
		CheckChange();
	}
}


void dlgDomain::OnChangeValidate(wxCommandEvent &ev)
{
	CheckChange();
}


wxString dlgDomain::GetSql()
{
	wxString sql, name;
	int pos;
	wxString definition;
	int index = -1;
	wxArrayString tmpDef = previousConstraints;
	wxString tmpsql = wxEmptyString;

	if (domain)
	{
		// edit mode
		name = GetName();

		if (txtName->GetValue() != domain->GetName())
		{

			if (connection->BackendMinimumVersion(9, 2))
				AppendNameChange(sql, wxT("DOMAIN ") + domain->GetQuotedFullIdentifier());
			else
				AppendNameChange(sql, wxT("TYPE ") + domain->GetQuotedFullIdentifier());
		}
		if (chkNotNull->GetValue() != domain->GetNotNull())
		{
			sql += wxT("ALTER DOMAIN ") + domain->GetQuotedFullIdentifier();
			if (chkNotNull->GetValue())
				sql += wxT("\n  SET NOT NULL;\n");
			else
				sql += wxT("\n  DROP NOT NULL;\n");
		}
		if (txtDefault->GetValue() != domain->GetDefault())
		{
			sql += wxT("ALTER DOMAIN ") + domain->GetQuotedFullIdentifier();
			if (txtDefault->GetValue().IsEmpty())
				sql += wxT("\n  DROP DEFAULT;\n");
			else
				sql += wxT("\n  SET DEFAULT ") + txtDefault->GetValue() + wxT(";\n");
		}

		// Build a temporary list of ADD CONSTRAINTs, and fixup the list to remove
		for (pos = 0; pos < lstConstraints->GetItemCount() ; pos++)
		{
			wxString conname = qtIdent(lstConstraints->GetItemText(pos));
			definition = conname;
			definition += wxT(" CHECK ") + constraintsDefinition.Item(pos);
			index = tmpDef.Index(definition);
			if (index >= 0)
				tmpDef.RemoveAt(index);
			else
			{
				tmpsql += wxT("ALTER DOMAIN ") + domain->GetQuotedFullIdentifier()
				          +  wxT("\n  ADD");
				if (!conname.IsEmpty())
					tmpsql += wxT(" CONSTRAINT ");

				tmpsql += definition + wxT(";\n");
			}
		}

		// Add the DROP CONSTRAINTs...
		for (index = 0 ; index < (int)tmpDef.GetCount() ; index++)
		{
			definition = tmpDef.Item(index);
			if (definition[0U] == '"')
				definition = definition.Mid(1).BeforeFirst('"');
			else
				definition = definition.BeforeFirst(' ');
			sql += wxT("ALTER DOMAIN ") + domain->GetQuotedFullIdentifier()
			       + wxT("\n  DROP CONSTRAINT ") + qtIdent(definition) + wxT(";\n");

		}

		// Add the ADD CONSTRAINTs...
		sql += tmpsql;

		AppendOwnerChange(sql, wxT("DOMAIN ") + domain->GetQuotedFullIdentifier());
		AppendSchemaChange(sql, wxT("DOMAIN ") + domain->GetQuotedFullIdentifier());
	}
	else
	{
		// create mode
		name = qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName());
		sql = wxT("CREATE DOMAIN ") + name
		      + wxT("\n   AS ") + GetQuotedTypename(cbDatatype->GetGuessedSelection());

		if (!cbCollation->GetValue().IsEmpty() && cbCollation->GetValue() != wxT("pg_catalog.\"default\""))
			sql += wxT("\n   COLLATE ") + cbCollation->GetValue();

		AppendIfFilled(sql, wxT("\n   DEFAULT "), txtDefault->GetValue());

		if (chkNotNull->GetValue())
			sql += wxT("\n   NOT NULL");

		for (pos = 0 ; pos < lstConstraints->GetItemCount() ; pos++)
		{
			wxString name = lstConstraints->GetItemText(pos);
			wxString definition = constraintsDefinition.Item(pos);
			if (!name.IsEmpty())
				sql += wxT("\n   CONSTRAINT ") + qtIdent(name) + wxT(" CHECK ") + definition;
			else
				sql += wxT("\n   CHECK ") + definition;
		}

		sql += wxT(";\n");

		AppendOwnerNew(sql, wxT("DOMAIN ") + name);
	}

	AppendComment(sql, wxT("DOMAIN ") + qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()), domain);

	if (seclabelPage && connection->BackendMinimumVersion(9, 1))
		sql += seclabelPage->GetSqlForSecLabels(wxT("DOMAIN"), name);

	return sql;
}

void dlgDomain::OnChange(wxCommandEvent &event)
{
	CheckChange();
}

void dlgDomain::OnAddConstr(wxCommandEvent &ev)
{
	int sel = cbConstrType->GetCurrentSelection();

	switch (sel)
	{
		case 0: // Check
		{
			dlgCheck chk(&checkFactory, mainForm);
			chk.CenterOnParent();
			chk.SetDatabase(database);
			if (chk.Go(true) != wxID_CANCEL)
			{
				wxString tmpDef = chk.GetDefinition();
				tmpDef.Replace(wxT("\n"), wxT(" "));

				lstConstraints->AppendItem(checkFactory.GetIconId(), chk.GetName(), tmpDef);
				constraintsDefinition.Add(tmpDef);
			}
			break;
		}
	}
	CheckChange();
}


void dlgDomain::OnRemoveConstr(wxCommandEvent &ev)
{
	if (settings->GetConfirmDelete())
	{
		if (wxMessageBox(_("Are you sure you wish to remove the selected constraint?"), _("Remove constraint?"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION) != wxYES)
			return;
	}

	int pos = lstConstraints->GetSelection();
	if (pos < 0)
		return;

	lstConstraints->DeleteItem(pos);
	constraintsDefinition.RemoveAt(pos);
	btnRemoveConstr->Disable();

	CheckChange();
}


void dlgDomain::OnSelChangeConstr(wxListEvent &ev)
{
	btnRemoveConstr->Enable();
}


