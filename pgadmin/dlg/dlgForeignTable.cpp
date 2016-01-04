//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgForeignTable.cpp - PostgreSQL Foreign Table Property
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>


// App headers
#include "utils/misc.h"
#include "dlg/dlgForeignTable.h"
#include "schema/pgSchema.h"
#include "schema/pgForeignTable.h"
#include "schema/pgDatatype.h"
#include "ctl/ctlSeclabelPanel.h"


// pointer to controls
#define cbForeignServer      CTRL_COMBOBOX("cbForeignServer")
#define lstMembers           CTRL_LISTVIEW("lstMembers")
#define txtMembername        CTRL_TEXT("txtMembername")
#define btnAddMember         CTRL_BUTTON("btnAddMember")
#define btnChangeMember      CTRL_BUTTON("btnChangeMember")
#define btnRemoveMember      CTRL_BUTTON("btnRemoveMember")
#define lstOptions           CTRL_LISTVIEW("lstOptions")
#define txtOption            CTRL_TEXT("txtOption")
#define txtValue             CTRL_TEXT("txtValue")
#define btnAdd               CTRL_BUTTON("wxID_ADD")
#define btnRemove            CTRL_BUTTON("wxID_REMOVE")
#define chkNotNull           CTRL_CHECKBOX("chkNotNull")


BEGIN_EVENT_TABLE(dlgForeignTable, dlgTypeProperty)
	EVT_BUTTON(XRCID("btnAddMember"),               dlgForeignTable::OnMemberAdd)
	EVT_BUTTON(XRCID("btnChangeMember"),            dlgForeignTable::OnMemberChange)
	EVT_BUTTON(XRCID("btnRemoveMember"),            dlgForeignTable::OnMemberRemove)
	EVT_LIST_ITEM_SELECTED(XRCID("lstMembers"),     dlgForeignTable::OnMemberSelChange)
	EVT_TEXT(XRCID("cbDatatype"),                   dlgForeignTable::OnSelChangeTyp)
	EVT_COMBOBOX(XRCID("cbDatatype"),               dlgForeignTable::OnSelChangeTyp)
	EVT_TEXT(XRCID("txtMembername"),                dlgForeignTable::OnChangeMember)
	EVT_TEXT(XRCID("cbDatatype"),                   dlgForeignTable::OnSelChangeTyp)
	EVT_COMBOBOX(XRCID("cbDatatype"),               dlgForeignTable::OnSelChangeTyp)
	EVT_TEXT(XRCID("txtLength"),                    dlgForeignTable::OnSelChangeTypOrLen)
	EVT_TEXT(XRCID("txtPrecision"),                 dlgForeignTable::OnSelChangeTypOrLen)
	EVT_CHECKBOX(XRCID("chkNotNull"),               dlgForeignTable::OnChangeMember)
	EVT_LIST_ITEM_SELECTED(XRCID("lstOptions"),     dlgForeignTable::OnSelChangeOption)
	EVT_TEXT(XRCID("txtOption"),                    dlgForeignTable::OnChangeOptionName)
	EVT_BUTTON(wxID_ADD,                            dlgForeignTable::OnAddOption)
	EVT_BUTTON(wxID_REMOVE,                         dlgForeignTable::OnRemoveOption)
END_EVENT_TABLE();


dlgProperty *pgForeignTableFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgForeignTable(this, frame, (pgForeignTable *)node, (pgSchema *)parent);
}


dlgForeignTable::dlgForeignTable(pgaFactory *f, frmMain *frame, pgForeignTable *node, pgSchema *sch)
	: dlgTypeProperty(f, frame, wxT("dlgForeignTable"))
{
	foreigntable = node;
	schema = sch;

	seclabelPage = new ctlSeclabelPanel(nbNotebook);

	lstMembers->CreateColumns(0, _("Member"), _("Data type"), _("Constraint"), -1);

	queriesToBeSplitted = false;
}


void dlgForeignTable::OnChangeMember(wxCommandEvent &ev)
{
	btnAddMember->Enable(
	    !txtMembername->GetValue().Strip(wxString::both).IsEmpty()
	    && cbDatatype->GetGuessedSelection() >= 0);
	btnChangeMember->Enable(true);
}


pgObject *dlgForeignTable::GetObject()
{
	return foreigntable;
}


int dlgForeignTable::Go(bool modal)
{
	seclabelPage->SetConnection(connection);
	seclabelPage->SetObject(foreigntable);
	this->Connect(EVT_SECLABELPANEL_CHANGE, wxCommandEventHandler(dlgForeignTable::OnChange));

	// Fill owner combobox
	if (!foreigntable)
		cbOwner->Append(wxT(""));
	AddGroups();
	AddUsers(cbOwner);

	// Fill datatype combobox
	FillDatatype(cbDatatype);

	// Initialize options listview and buttons
	lstOptions->AddColumn(_("Option"), 80);
	lstOptions->AddColumn(_("Value"), 40);
	txtOption->SetValue(wxT(""));
	txtValue->SetValue(wxT(""));
	btnAdd->Disable();
	btnRemove->Disable();

	if (foreigntable)
	{
		// Edit Mode
		cbForeignServer->SetValue(foreigntable->GetForeignServer());
		cbForeignServer->Disable();

		txtMembername->Enable(true);
		btnAddMember->Enable(true);
		btnChangeMember->Enable(false);
		btnRemoveMember->Enable(false);

		wxArrayString elements = foreigntable->GetTypesArray();
		wxString fullType, typeName, typeLength, typePrecision;
		size_t pos;
		size_t i;
		for (i = 0 ; i < elements.GetCount() ; i += 3)
		{
			lstMembers->AppendItem(0, elements.Item(i), elements.Item(i + 1), elements.Item(i + 2));

			fullType = elements.Item(i + 1);
			typeName = fullType;
			typeLength = wxEmptyString;
			typePrecision = wxEmptyString;

			if (fullType.Find(wxT("(")) > 0)
			{
				// there is at least a length
				typeName = fullType.BeforeFirst('(');
				if (fullType.Find(wxT(",")) > 0)
				{
					// there is also a precision
					typeLength = fullType.AfterFirst('(').BeforeFirst(',');
					typePrecision = fullType.AfterFirst(',').BeforeFirst(')');
				}
				else
					typeLength = fullType.AfterFirst('(').BeforeFirst(')');
			}

			for (pos = 0; pos < cbDatatype->GetCount() - 1; pos++)
			{
				if (cbDatatype->GetString(pos) == typeName)
				{
					memberTypes.Add(GetTypeInfo(pos));
					break;
				}
			}
			memberLengths.Add(typeLength);
			memberPrecisions.Add(typePrecision);
			memberNotNulls.Add(elements.Item(i + 2));
		}

		cbDatatype->Enable();
		txtLength->Enable();

		wxArrayString options = foreigntable->GetOptionsArray();
		wxString optionname, optionvalue;
		for (unsigned int index = 0; index < options.Count(); index += 2)
		{
			optionname = options.Item(index);
			optionvalue = options.Item(index + 1);
			lstOptions->AppendItem(optionname, optionvalue);
		}
	}
	else
	{
		// Create mode
		cbOwner->Append(wxEmptyString);
		cbOwner->Disable();

		pgSet *set = connection->ExecuteSet(
		                 wxT("SELECT srvname\n")
		                 wxT("  FROM pg_foreign_server\n")
		                 wxT("  ORDER BY srvname"));
		if (set)
		{
			while (!set->Eof())
			{
				wxString srvname = set->GetVal(wxT("srvname"));
				cbForeignServer->Append(srvname);
				set->MoveNext();
			}
			delete set;
		}
		cbForeignServer->SetSelection(0);
	}

	txtLength->SetValidator(numericValidator);

	return dlgTypeProperty::Go(modal);
}


void dlgForeignTable::OnSelChangeTyp(wxCommandEvent &ev)
{
	txtLength->SetValue(wxEmptyString);
	txtPrecision->SetValue(wxEmptyString);
	cbDatatype->GuessSelection(ev);
	chkNotNull->SetValue(false);
	OnSelChangeTypOrLen(ev);
}


void dlgForeignTable::OnSelChangeTypOrLen(wxCommandEvent &ev)
{
	CheckLenEnable();
	txtLength->Enable(isVarLen);
	txtPrecision->Enable(isVarPrec);
	CheckChange();
	OnChangeMember(ev);
}


void dlgForeignTable::CheckChange()
{
	bool enable = true;
	if (foreigntable)
	{
		enable = txtComment->GetValue() != foreigntable->GetComment()
		         || cbSchema->GetValue() != foreigntable->GetSchema()->GetName()
		         || cbOwner->GetValue() != foreigntable->GetOwner()
		         || GetSqlForTypes() != wxEmptyString
		         || GetSql().Length() > 0;
		if (seclabelPage && connection->BackendMinimumVersion(9, 1))
			enable = enable || !(seclabelPage->GetSqlForSecLabels().IsEmpty());
	}
	else
	{
		wxString name = GetName();

		CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
		CheckValid(enable, cbForeignServer->GetCurrentSelection() >= 0, _("Please specify a foreign server."));
	}
	EnableOK(enable);
}


void dlgForeignTable::OnMemberSelChange(wxListEvent &ev)
{
	long pos = lstMembers->GetSelection();
	if (pos >= 0)
	{
		txtMembername->SetValue(lstMembers->GetText(pos));
		cbDatatype->SetValue(memberTypes.Item(pos).AfterFirst(':'));
		txtLength->SetValue(memberLengths.Item(pos));
		txtLength->Enable(!txtLength->GetValue().IsEmpty());
		txtPrecision->SetValue(memberPrecisions.Item(pos));
		txtPrecision->Enable(!txtPrecision->GetValue().IsEmpty());
		chkNotNull->SetValue(memberNotNulls.Item(pos) == wxT("NOT NULL"));
		chkNotNull->Enable();
		btnChangeMember->Enable();
		btnRemoveMember->Enable();
	}
}


void dlgForeignTable::OnMemberAdd(wxCommandEvent &ev)
{
	wxString name = txtMembername->GetValue().Strip(wxString::both);
	wxString type = cbDatatype->GetValue();
	wxString length = wxEmptyString;
	wxString precision = wxEmptyString;
	wxString notnull = wxEmptyString;

	if (txtLength->GetValue() != wxT("") && txtLength->IsEnabled())
		length = txtLength->GetValue();
	if (txtPrecision->GetValue() != wxT("") && txtPrecision->IsEnabled())
		precision = txtPrecision->GetValue();
	notnull = chkNotNull->GetValue() ? wxT("NOT NULL") : wxEmptyString;

	if (!length.IsEmpty())
	{
		type += wxT("(") + length;
		if (!precision.IsEmpty())
			type += wxT(",") + precision;
		type += wxT(")");
	}

	if (!name.IsEmpty())
	{
		size_t pos = lstMembers->GetItemCount();
		lstMembers->InsertItem(pos, name, 0);
		lstMembers->SetItem(pos, 1, type);
		lstMembers->SetItem(pos, 2, notnull);
		memberTypes.Add(GetTypeInfo(cbDatatype->GetGuessedSelection()));
		memberLengths.Add(length);
		memberPrecisions.Add(precision);
		memberNotNulls.Add(notnull);
	}

	txtMembername->SetValue(wxEmptyString);
	cbDatatype->SetValue(wxEmptyString);
	txtLength->SetValue(wxEmptyString);
	txtPrecision->SetValue(wxEmptyString);
	chkNotNull->SetValue(false);

	CheckChange();
}


void dlgForeignTable::OnMemberChange(wxCommandEvent &ev)
{
	wxString name = txtMembername->GetValue().Strip(wxString::both);
	wxString type = cbDatatype->GetValue();
	wxString length = wxEmptyString;
	wxString precision = wxEmptyString;
	wxString notnull = wxEmptyString;

	if (txtLength->GetValue() != wxT("") && txtLength->IsEnabled())
		length = txtLength->GetValue();
	if (txtPrecision->GetValue() != wxT("") && txtPrecision->IsEnabled())
		precision = txtPrecision->GetValue();
	notnull = chkNotNull->GetValue() ? wxT("NOT NULL") : wxEmptyString;

	if (!length.IsEmpty())
	{
		type += wxT("(") + length;
		if (!precision.IsEmpty())
			type += wxT(",") + precision;
		type += wxT(")");
	}

	if (!name.IsEmpty())
	{
		long pos = lstMembers->GetFirstSelected();
		if (pos >= 0)
		{
			lstMembers->SetItem(pos, 0, name);
			lstMembers->SetItem(pos, 1, type);
			lstMembers->SetItem(pos, 2, notnull);
			memberTypes.Insert(GetTypeInfo(cbDatatype->GetGuessedSelection()), pos);
			memberLengths.Insert(length, pos);
			memberPrecisions.Insert(precision, pos);
			memberNotNulls.Insert(notnull, pos);
			memberTypes.RemoveAt(pos + 1);
			memberLengths.RemoveAt(pos + 1);
			memberPrecisions.RemoveAt(pos + 1);
			memberNotNulls.RemoveAt(pos + 1);
		}
	}

	CheckChange();
}


void dlgForeignTable::OnMemberRemove(wxCommandEvent &ev)
{
	long pos = lstMembers->GetSelection();

	if (pos >= 0)
	{
		lstMembers->DeleteItem(pos);
		memberTypes.RemoveAt(pos);
		memberLengths.RemoveAt(pos);
		memberPrecisions.RemoveAt(pos);
		memberNotNulls.RemoveAt(pos);
	}
	CheckChange();
}


pgObject *dlgForeignTable::CreateObject(pgCollection *collection)
{
	pgObject *obj = 0;
	return obj;
}


void dlgForeignTable::OnChangeOptionName(wxCommandEvent &ev)
{
	btnAdd->Enable(txtOption->GetValue().Length() > 0);
}


void dlgForeignTable::OnSelChangeOption(wxListEvent &ev)
{
	int row = lstOptions->GetSelection();
	if (row >= 0)
	{
		txtOption->SetValue(lstOptions->GetText(row, 0));
		txtValue->SetValue(lstOptions->GetText(row, 1));
	}

	btnRemove->Enable(row >= 0);
}


void dlgForeignTable::OnAddOption(wxCommandEvent &ev)
{
	bool found = false;

	for (int pos = 0 ; pos < lstOptions->GetItemCount() ; pos++)
	{
		if (lstOptions->GetText(pos).IsSameAs(txtOption->GetValue(), false))
		{
			lstOptions->SetItem(pos, 1, txtValue->GetValue());
			found = true;
			break;
		}
	}

	if (!found)
	{
		lstOptions->AppendItem(txtOption->GetValue(), txtValue->GetValue());
	}

	txtOption->SetValue(wxT(""));
	txtValue->SetValue(wxT(""));
	btnAdd->Disable();

	CheckChange();
}


void dlgForeignTable::OnRemoveOption(wxCommandEvent &ev)
{
	int sel = lstOptions->GetSelection();
	lstOptions->DeleteItem(sel);

	txtOption->SetValue(wxT(""));
	txtValue->SetValue(wxT(""));
	btnRemove->Disable();

	CheckChange();
}


wxString dlgForeignTable::GetOptionsSql()
{
	wxArrayString options = foreigntable->GetOptionsArray();
	wxString optionname, optionvalue, sqloptions;
	bool found;
	int pos;

	for (unsigned int index = 0; index < options.Count(); index += 2)
	{
		optionname = options.Item(index);
		optionvalue = options.Item(index + 1);

		// check for options
		found = false;
		for (pos = 0 ; pos < lstOptions->GetItemCount() && !found; pos++)
		{
			found = lstOptions->GetText(pos, 0).Cmp(optionname) == 0;
			if (found) break;
		}

		if (found)
		{
			if (lstOptions->GetText(pos, 1).Cmp(optionvalue) != 0)
			{
				if (sqloptions.Length() > 0)
					sqloptions += wxT(", ");
				sqloptions += wxT("SET ") + optionname + wxT(" '") + lstOptions->GetText(pos, 1) + wxT("'");
			}
		}
		else
		{
			if (sqloptions.Length() > 0)
				sqloptions += wxT(", ");
			sqloptions += wxT("DROP ") + optionname;
		}
	}

	for (pos = 0 ; pos < lstOptions->GetItemCount() ; pos++)
	{
		options = foreigntable->GetOptionsArray();
		found = false;

		for (unsigned int index = 0; index < options.Count() && !found; index += 2)
		{
			optionname = options.Item(index);
			optionvalue = options.Item(index + 1);
			found = lstOptions->GetText(pos, 0).Cmp(optionname) == 0;
		}

		if (!found)
		{
			if (sqloptions.Length() > 0)
				sqloptions += wxT(", ");
			sqloptions += wxT("ADD ") + lstOptions->GetText(pos, 0) + wxT(" '") + lstOptions->GetText(pos, 1) + wxT("'");
		}
	}

	return sqloptions;
}


wxString dlgForeignTable::GetSql()
{
	wxString sql;
	wxString name;

	if (foreigntable)
	{
		// Edit Mode
		name = qtIdent(foreigntable->GetSchema()->GetName()) + wxT(".") + qtIdent(GetName());

		AppendNameChange(sql, wxT("FOREIGN TABLE ") + foreigntable->GetQuotedFullIdentifier());
		AppendOwnerChange(sql, wxT("FOREIGN TABLE ") + name);

		sql += GetSqlForTypes();

		wxString sqloptions = GetOptionsSql();
		if (sqloptions.Length() > 0)
		{
			sql += wxT("ALTER FOREIGN TABLE ") + name
			       + wxT("\n  OPTIONS (") + sqloptions + wxT(");\n");
		}
		AppendSchemaChange(sql, wxT("FOREIGN TABLE ") + name);
	}
	else
	{
		name = qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName());

		// Create Mode
		sql = wxT("CREATE FOREIGN TABLE " + name);
		sql += wxT(" (");

		int i;
		for (i = 0 ; i < lstMembers->GetItemCount() ; i++)
		{
			if (i)
				sql += wxT(",\n    ");
			sql += qtIdent(lstMembers->GetItemText(i)) + wxT(" ")
			       + GetFullTypeName(i);
		}

		sql += wxT(") SERVER ") + cbForeignServer->GetValue();

		// check for options
		if (lstOptions->GetItemCount() > 0)
		{
			wxString options = wxEmptyString;
			for (int pos = 0 ; pos < lstOptions->GetItemCount() ; pos++)
			{
				if (options.Length() > 0)
					options += wxT(", ");

				options += lstOptions->GetText(pos, 0)
				           + wxT(" '") + lstOptions->GetText(pos, 1) + wxT("' ");
			}
			sql += wxT("\n  OPTIONS (") + options + wxT(")");
		}

		sql += wxT(";\n");
	}
	AppendComment(sql, wxT("FOREIGN TABLE ") + qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()), foreigntable);

	if (seclabelPage && connection->BackendMinimumVersion(9, 1))
		sql += seclabelPage->GetSqlForSecLabels(wxT("FOREIGN TABLE "), qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()));

	return sql;
}

wxString dlgForeignTable::GetFullTypeName(int type)
{
	wxString typname = memberTypes.Item(type).AfterFirst(':');

	if (!memberLengths.Item(type).IsEmpty())
	{
		typname += wxT("(") + memberLengths.Item(type);
		if (!memberPrecisions.Item(type).IsEmpty())
			typname += wxT(",") + memberPrecisions.Item(type);
		typname += wxT(")");
	}

	typname += wxT(" ") + memberNotNulls.Item(type);

	return typname;
}

wxString dlgForeignTable::GetSqlForTypes()
{
	wxString sql = wxEmptyString;
	wxString old_name, old_type, new_name, new_type;
	wxArrayString elements = foreigntable->GetTypesArray();
	bool modified = lstMembers->GetItemCount() * 3 != (int)elements.GetCount();
	size_t i;

	// Check if there is a change
	for (int i = 0 ; i < lstMembers->GetItemCount() && !modified; i++)
	{
		old_name = elements.Item(i * 3);
		old_type = elements.Item(i * 3 + 1) + wxT(" ") + elements.Item(i * 3 + 2);
		new_name = lstMembers->GetItemText(i);
		new_type = GetFullTypeName(i);
		modified = modified || old_name != new_name || old_type != new_type;
	}

	if (modified)
	{
		// Drop all old attributes
		for (i = 0 ; i < elements.GetCount() ; i += 3)
		{
			old_name = elements.Item(i);
			sql += wxT("ALTER FOREIGN TABLE ") + foreigntable->GetQuotedFullIdentifier()
			       + wxT("\n  DROP COLUMN ") + old_name + wxT(";\n");
		}

		// Add all new attributes
		for (int i = 0 ; i < lstMembers->GetItemCount() ; i++)
		{
			new_name = lstMembers->GetItemText(i);
			new_type = GetFullTypeName(i);
			sql += wxT("ALTER FOREIGN TABLE ") + foreigntable->GetQuotedFullIdentifier()
			       + wxT("\n  ADD COLUMN ") + new_name + wxT(" ") + new_type + wxT(";\n");
		}
	}

	return sql;
}

void dlgForeignTable::OnChange(wxCommandEvent &event)
{
	CheckChange();
}
