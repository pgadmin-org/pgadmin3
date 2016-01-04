//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgAggregate.cpp - Aggregate properties dialog
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "utils/pgDefs.h"

#include "dlg/dlgAggregate.h"
#include "schema/pgSchema.h"
#include "schema/pgAggregate.h"
#include "schema/pgDatatype.h"
#include "ctl/ctlSeclabelPanel.h"
#include "frm/frmMain.h"
#include "schema/pgUser.h"
#include "schema/pgGroup.h"

#include "images/aggregate.pngc"

// pointer to controls
#define cbInputType         CTRL_COMBOBOX2("cbInputType")
#define lstInputTypes       CTRL_LISTCTRL("lstInputTypes")
#define cbStateType         CTRL_COMBOBOX2("cbStateType")
#define cbStateFunc         CTRL_COMBOBOX("cbStateFunc")
#define cbFinalFunc         CTRL_COMBOBOX("cbFinalFunc")
#define cbSortOp            CTRL_COMBOBOX("cbSortOp")
#define txtInitial          CTRL_TEXT("txtInitial")
#define btnAddType          CTRL_BUTTON("btnAddType")
#define btnRemoveType       CTRL_BUTTON("btnRemoveType")

dlgProperty *pgAggregateFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgAggregate(this, frame, (pgAggregate *)node, (pgSchema *)parent);
}

BEGIN_EVENT_TABLE(dlgAggregate, dlgTypeProperty)
	EVT_TEXT(XRCID("cbInputType"),                  dlgAggregate::OnChangeTypeBase)
	EVT_TEXT(XRCID("cbStateType"),                  dlgAggregate::OnChangeTypeState)
	EVT_COMBOBOX(XRCID("cbStateType"),              dlgAggregate::OnChangeType)
	EVT_COMBOBOX(XRCID("cbStateFunc"),              dlgProperty::OnChange)
	EVT_COMBOBOX(XRCID("cbSortOp"),                 dlgProperty::OnChange)
	EVT_TEXT(XRCID("cbStateFunc"),                  dlgProperty::OnChange)
	EVT_BUTTON(XRCID("btnAddType"),                 dlgAggregate::OnAddInputType)
	EVT_BUTTON(XRCID("btnRemoveType"),              dlgAggregate::OnRemoveInputType)
	EVT_LIST_ITEM_SELECTED(XRCID("lstInputTypes"),  dlgAggregate::OnSelectInputType)
	EVT_BUTTON(CTL_ADDPRIV,                         dlgAggregate::OnAddPriv)
	EVT_BUTTON(CTL_DELPRIV,                         dlgAggregate::OnDelPriv)
#ifdef __WXMAC__
	EVT_SIZE(                                       dlgAggregate::OnChangeSize)
#endif
END_EVENT_TABLE();

dlgAggregate::dlgAggregate(pgaFactory *f, frmMain *frame, pgAggregate *agg, pgSchema *sch)
	: dlgTypeProperty(f, frame, wxT("dlgAggregate"))
{
	schema = sch;
	aggregate = agg;
	seclabelPage = new ctlSeclabelPanel(nbNotebook);

	/* Aggregate Privileges */
	securityChanged = false;
	if (agg)
		connection = agg->GetConnection();
	securityPage = new ctlSecurityPanel(nbNotebook, wxT("EXECUTE"), "X", frame->GetImageList());
	if (connection && (!agg || agg->CanCreate()))
	{
		// Fetch Groups Information
		pgSet *setGrp = connection->ExecuteSet(wxT("SELECT groname FROM pg_group ORDER BY groname"));

		if (setGrp)
		{
			while (!setGrp->Eof())
			{
				groups.Add(setGrp->GetVal(0));
				setGrp->MoveNext();
			}
			delete setGrp;
		}

		if (agg)
		{
			wxString strAcl = agg->GetAcl();
			if (!strAcl.IsEmpty())
			{
				wxArrayString aclArray;
				strAcl = strAcl.Mid(1, strAcl.Length() - 2);
				getArrayFromCommaSeparatedList(strAcl, aclArray);
				wxString roleName;
				for (unsigned int index = 0; index < aclArray.Count(); index++)
				{
					wxString strCurrAcl = aclArray[index];

					/*
					* In rare case, we can have ',' (comma) in the user name.
					* But, we need to handle them also
					*/
					if (strCurrAcl.Find(wxChar('=')) == wxNOT_FOUND)
					{
						// Check it is start of the ACL
						if (strCurrAcl[0U] == (wxChar)'"')
							roleName = strCurrAcl + wxT(",");
						continue;
					}
					else
						strCurrAcl = roleName + strCurrAcl;

					if (strCurrAcl[0U] == (wxChar)'"')
						strCurrAcl = strCurrAcl.Mid(1, strCurrAcl.Length() - 1);
					roleName = strCurrAcl.BeforeLast('=');

					wxString value = strCurrAcl.Mid(roleName.Length() + 1).BeforeLast('/');

					int icon = userFactory.GetIconId();

					if (roleName.Left(6).IsSameAs(wxT("group "), false))
					{
						icon = groupFactory.GetIconId();
						roleName = wxT("group ") + qtStrip(roleName.Mid(6));
					}
					else if (roleName.IsEmpty())
					{
						icon = PGICON_PUBLIC;
						roleName = wxT("public");
					}
					else
					{
						roleName = qtStrip(roleName);
						for (unsigned int index = 0; index < groups.Count(); index++)
							if (roleName == groups[index])
							{
								roleName = wxT("group ") + roleName;
								icon = groupFactory.GetIconId();
								break;
							}
					}

					securityPage->lbPrivileges->AppendItem(icon, roleName, value);
					currentAcl.Add(roleName + wxT("=") + value);

					// Reset roleName
					roleName.Empty();
				}
			}
		}
	}
	else
		securityPage->Disable();
}

pgObject *dlgAggregate::GetObject()
{
	return aggregate;
}

int dlgAggregate::Go(bool modal)
{
	if (!connection->BackendMinimumVersion(8, 0))
		cbOwner->Disable();

	if (!connection->BackendMinimumVersion(8, 1))
		cbSortOp->Disable();

	if (connection->BackendMinimumVersion(9, 1))
	{
		seclabelPage->SetConnection(connection);
		seclabelPage->SetObject(aggregate);
		this->Connect(EVT_SECLABELPANEL_CHANGE, wxCommandEventHandler(dlgAggregate::OnChange));
	}
	else
		seclabelPage->Disable();

	lstInputTypes->InsertColumn(0, _("Input types"), wxLIST_FORMAT_LEFT, lstInputTypes->GetSize().x - 10);

	if (aggregate)
	{
		// edit mode
		txtName->Enable(connection->BackendMinimumVersion(7, 4));
		cbSchema->Enable(connection->BackendMinimumVersion(8, 1));

		for (unsigned int x = 0; x < aggregate->GetInputTypesArray().Count(); x++ )
		{
			lstInputTypes->InsertItem(x, aggregate->GetInputTypesArray().Item(x));
			AddType(wxT(" "), 0, aggregate->GetInputTypesArray().Item(x));
		}

		cbStateType->Append(aggregate->GetStateType());
		cbStateType->SetSelection(0);

		cbStateFunc->Append(aggregate->GetStateFunction());
		cbStateFunc->SetSelection(0);
		cbFinalFunc->Append(aggregate->GetFinalFunction());
		cbFinalFunc->SetSelection(0);

		cbSortOp->Append(aggregate->GetSortOp());
		cbSortOp->SetSelection(0);

		txtInitial->SetValue(aggregate->GetInitialCondition());

		cbInputType->Disable();
		btnAddType->Disable();
		btnRemoveType->Disable();
		cbStateFunc->Disable();
		cbStateType->Disable();
		cbFinalFunc->Disable();
		cbSortOp->Disable();
		txtInitial->Disable();
	}
	else
	{
		// create mode
		AddType(wxT(" "), PGOID_TYPE_ANY, wxT("\"any\""));
		cbInputType->Append(wxT("ANY"));

		FillDatatype(cbInputType, cbStateType, false);
		btnRemoveType->Disable();
	}

	securityPage->SetConnection(connection);

	if (securityPage->cbGroups)
	{
		// Fetch Groups Information
		for ( unsigned int index = 0; index < groups.Count();)
			securityPage->cbGroups->Append(wxT("group ") + groups[index++]);

		// Fetch Users Information
		if (settings->GetShowUsersForPrivileges())
		{
			securityPage->stGroup->SetLabel(_("Group/User"));
			dlgProperty::AddUsers(securityPage->cbGroups);
		}
	}
	securityPage->lbPrivileges->GetParent()->Layout();

	return dlgProperty::Go(modal);
}

pgObject *dlgAggregate::CreateObject(pgCollection *collection)
{
	pgObject *obj = aggregateFactory.CreateObjects(collection, 0,
	                wxT("\n   AND proname=") + qtDbString(GetName()) +
	                wxT("\n   AND pronamespace=") + schema->GetOidStr());

	return obj;
}

#ifdef __WXMAC__
void dlgAggregate::OnChangeSize(wxSizeEvent &ev)
{
	lstInputTypes->SetSize(wxDefaultCoord, wxDefaultCoord,
	                       ev.GetSize().GetWidth(), ev.GetSize().GetHeight() - 350);
	securityPage->lbPrivileges->SetSize(wxDefaultCoord, wxDefaultCoord,
	                                    ev.GetSize().GetWidth(), ev.GetSize().GetHeight() - 550);
	if (GetAutoLayout())
	{
		Layout();
	}
}
#endif

void dlgAggregate::CheckChange()
{
	bool enable = true;
	if (aggregate)
	{
		enable = GetName() != aggregate->GetName()
		         || cbSchema->GetValue() != aggregate->GetSchema()->GetName()
		         || txtComment->GetValue() != aggregate->GetComment()
		         || cbOwner->GetValue() != aggregate->GetOwner();
		if (seclabelPage && connection->BackendMinimumVersion(9, 1))
			enable = enable || !(seclabelPage->GetSqlForSecLabels().IsEmpty());
	}
	else
	{
		// For pre 8.2 servers, we can only have one input type.
		if (!connection->BackendMinimumVersion(8, 2))
		{
			if (lstInputTypes->GetItemCount() >= 1)
				btnAddType->Disable();
			else
				btnAddType->Enable();
		}

		// Multi parameter aggregates cannot have sort ops.
		if (connection->BackendMinimumVersion(8, 2))
		{
			if (lstInputTypes->GetItemCount() > 1)
			{
				cbSortOp->SetValue(wxEmptyString);
				cbSortOp->Disable();
			}
			else
				cbSortOp->Enable();
		}

		wxString name = GetName();
		CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
		CheckValid(enable, cbStateType->GetGuessedSelection() >= 0, _("Please select state datatype."));
		CheckValid(enable, cbStateFunc->GetCurrentSelection() >= 0, _("Please specify state function."));
	}
	EnableOK(enable || securityChanged);
}

void dlgAggregate::OnChangeTypeBase(wxCommandEvent &ev)
{
	cbInputType->GuessSelection(ev);
	OnChangeType(ev);
}

void dlgAggregate::OnChangeTypeState(wxCommandEvent &ev)
{
	cbStateType->GuessSelection(ev);
	OnChangeType(ev);
}

void dlgAggregate::OnChangeType(wxCommandEvent &ev)
{
	cbStateFunc->Clear();
	cbFinalFunc->Clear();
	cbSortOp->Clear();

	pgSet *set;
	wxString qry;

	// Get the possible state functions. They must return the state type, and take
	// input_types + 1 parameters, the first of which is of the state type.
	// If there are no input_types specified, assume "ANY" for a count(*) style aggregate.
	if (lstInputTypes->GetItemCount() > 0 && cbStateType->GetGuessedSelection() >= 0)
	{
		set = connection->ExecuteSet(
		          wxT("SELECT proname, nspname, prorettype\n")
		          wxT("  FROM pg_proc p\n")
		          wxT("  JOIN pg_type t ON t.oid=p.prorettype\n")
		          wxT("  JOIN pg_namespace n ON n.oid=pronamespace\n")
		          wxT(" WHERE prorettype = ") + GetTypeOid(cbStateType->GetGuessedSelection() + 1) +
		          wxT("\n   AND proisagg = FALSE")
		          wxT("\n   AND proargtypes = '") + GetTypeOid(cbStateType->GetGuessedSelection() + 1) + wxT(" ") + GetInputTypesOidList() + wxT("'"));

		if (set)
		{
			while (!set->Eof())
			{
				cbStateFunc->Append(database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("proname")),
				                    database->GetQuotedSchemaPrefix(set->GetVal(wxT("nspname"))) + qtIdent(set->GetVal(wxT("proname"))));

				set->MoveNext();
			}
			delete set;
		}

		// Get the possible final_func options. This may be nothing, or a
		// function taking an argument of state_type
		cbFinalFunc->Append(wxT(" "));

		set = connection->ExecuteSet(
		          wxT("SELECT proname, nspname, prorettype\n")
		          wxT("  FROM pg_proc p\n")
		          wxT("  JOIN pg_type t ON t.oid=p.prorettype\n")
		          wxT("  JOIN pg_namespace n ON n.oid=pronamespace\n")
		          wxT(" WHERE proisagg = FALSE")
		          wxT("\n   AND proargtypes = '") + GetTypeOid(cbStateType->GetGuessedSelection() + 1) + wxT("'"));

		if (set)
		{
			while (!set->Eof())
			{
				cbFinalFunc->Append(database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("proname")),
				                    database->GetQuotedSchemaPrefix(set->GetVal(wxT("nspname"))) + qtIdent(set->GetVal(wxT("proname"))));

				set->MoveNext();
			}
			delete set;
		}

		// Get the sort operators. This is only valid for a single arguement operator.
		cbSortOp->Append(wxT(" "), wxEmptyString);

		set = connection->ExecuteSet(
		          wxT("SELECT oprname, nspname\n")
		          wxT("  FROM pg_operator op\n")
		          wxT("  JOIN pg_namespace nsp on nsp.oid=oprnamespace\n")
		          wxT(" WHERE oprleft = ") + NumToStr(GetInputTypeOid(0)) + wxT("\n")
		          wxT("   AND oprright = ") + NumToStr(GetInputTypeOid(0)) + wxT("\n"));

		if (set)
		{
			while (!set->Eof())
			{
				wxString key = database->GetQuotedSchemaPrefix(set->GetVal(wxT("nspname"))) + qtIdent(set->GetVal(wxT("oprname")));
				wxString txt = database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("oprname"));
				cbSortOp->Append(txt, key);
				set->MoveNext();
			}
			delete set;
		}
	}

	CheckChange();
}

wxString dlgAggregate::GetSql()
{
	wxString sql, name;

	if (aggregate)
	{
		// edit mode
		name = schema->GetQuotedPrefix() + qtIdent(GetName()) + wxT("(") + GetInputTypesList() + wxT(")");

		AppendNameChange(sql, wxT("AGGREGATE ") + aggregate->GetQuotedFullName());
		AppendOwnerChange(sql, wxT("AGGREGATE ") + name);
		AppendSchemaChange(sql,  wxT("AGGREGATE ") + name);
	}
	else
	{
		// create mode
		name = qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName());

		if (connection->BackendMinimumVersion(8, 2))
		{
			sql = wxT("CREATE AGGREGATE ") + name
			      + wxT("(") + GetInputTypesList() + wxT(") (\n");
		}
		else
		{
			sql = wxT("CREATE AGGREGATE ") + name
			      + wxT("(\n   BASETYPE=") + GetInputTypesList() + wxT(",\n");
		}

		sql += wxT("   SFUNC=") + cbStateFunc->GetStringKey()
		       + wxT(",\n   STYPE=") + GetQuotedTypename(cbStateType->GetGuessedSelection() + 1); // skip "any" type

		if (cbFinalFunc->GetCurrentSelection() > 0)
		{
			sql += wxT(",\n   FINALFUNC=")
			       + cbFinalFunc->GetStringKey();
		}
		wxString initial = txtInitial->GetValue().Strip(wxString::both);
		if (!initial.IsEmpty())
		{
			if (initial == wxT("''")) // Empty string
				sql += wxT(",\n   INITCOND=''");
			else if (initial == wxT("\\'\\'")) // Pair of quotes
				sql += wxT(",\n   INITCOND=''''''");
			else
				sql += wxT(",\n   INITCOND=") + qtDbString(initial);
		}

		if (!cbSortOp->GetValue().IsEmpty())
		{
			wxString opr = cbSortOp->GetStringKey();
			if (!opr.IsEmpty())
				sql += wxT(",\n   SORTOP=") + opr;
		}

		sql += wxT("\n);\n");

		AppendOwnerNew(sql, wxT("AGGREGATE ") + name + wxT("(") + GetInputTypesList() + wxT(")"));
	}

	AppendComment(sql, wxT("AGGREGATE ") + qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName())
	              + wxT("(") + GetInputTypesList() + wxT(")"), aggregate);

	sql += securityPage->GetGrant(wxT("X"),
	                              wxT("FUNCTION ") + qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()) + wxT("(") + GetInputTypesList() + wxT(")"),
	                              &currentAcl);

	if (seclabelPage && connection->BackendMinimumVersion(9, 1))
		sql += seclabelPage->GetSqlForSecLabels(wxT("AGGREGATE"),
		                                        qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName())
		                                        + wxT("(") + GetInputTypesList() + wxT(")"));

	return sql;
}

// Return the list of input types
wxString dlgAggregate::GetInputTypesList()
{
	wxString types;

	for (int i = 0; i < lstInputTypes->GetItemCount(); i++)
	{
		if (i > 0)
			types += wxT(", ");

		if (lstInputTypes->GetItemText(i) == wxT("ANY") || lstInputTypes->GetItemText(i) == wxT("*"))
			return wxT("*");
		else
			types += qtTypeIdent(lstInputTypes->GetItemText(i));
	}
	return types;
}

void dlgAggregate::OnAddInputType(wxCommandEvent &ev)
{
	if (cbInputType->GetValue() != wxEmptyString)
	{
		if (cbInputType->GetValue() == wxT("ANY") && lstInputTypes->GetItemCount() > 0)
		{
			wxLogError(_("The ANY pseudo-datatype cannot be used in multi parameter aggregates."));
			return;
		}

		wxListItem itm;
		itm.SetMask(wxLIST_MASK_DATA | wxLIST_MASK_TEXT);

		itm.SetData(StrToLong(GetTypeOid(cbInputType->GetGuessedSelection())));
		itm.SetText(cbInputType->GetValue());

		lstInputTypes->InsertItem(itm);

		if (cbInputType->GetValue() == wxT("ANY"))
			btnAddType->Disable();

		OnChangeType(ev);
	}
}

void dlgAggregate::OnRemoveInputType(wxCommandEvent &ev)
{
	long pos = lstInputTypes->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);;
	if (pos >= 0)
	{
		lstInputTypes->DeleteItem(pos);

		OnChangeType(ev);
		btnRemoveType->Disable();

		// Re-enable the Add button, in case it was disabled because
		// ANY was specified.
		btnAddType->Enable();
	}
}

void dlgAggregate::OnSelectInputType(wxListEvent &ev)
{
	if (!aggregate)
		btnRemoveType->Enable();
}

// Returnt the datatype OID for the given parameter number
long dlgAggregate::GetInputTypeOid(int param)
{
	wxListItem itm;
	itm.SetMask(wxLIST_MASK_DATA | wxLIST_MASK_TEXT);
	itm.SetId(param);

	lstInputTypes->GetItem(itm);

	return itm.GetData();
}

// Return the list of datatype OIDs
wxString dlgAggregate::GetInputTypesOidList()
{
	wxString types;

	for (int i = 0; i < lstInputTypes->GetItemCount(); i++)
	{
		if (i > 0)
			types += wxT(" ");

		types += NumToStr(GetInputTypeOid(i));
	}
	return types;
}

void dlgAggregate::OnChange(wxCommandEvent &event)
{
	CheckChange();
}

void dlgAggregate::OnAddPriv(wxCommandEvent &ev)
{
	securityChanged = true;
	CheckChange();
}

void dlgAggregate::OnDelPriv(wxCommandEvent &ev)
{
	securityChanged = true;
	CheckChange();
}
