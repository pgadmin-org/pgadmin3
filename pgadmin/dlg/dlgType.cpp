//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgType.cpp - PostgreSQL TYPE Property
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>


// App headers
#include "utils/misc.h"
#include "dlg/dlgType.h"
#include "schema/pgSchema.h"
#include "schema/pgType.h"
#include "schema/pgDatatype.h"
#include "ctl/ctlSeclabelPanel.h"
#include "frm/frmMain.h"
#include "schema/pgUser.h"
#include "schema/pgGroup.h"


// pointer to controls
#define rdbType                 CTRL_RADIOBOX("rdbType")

#define cbInput                 CTRL_COMBOBOX("cbInput")
#define cbOutput                CTRL_COMBOBOX("cbOutput")
#define cbReceive               CTRL_COMBOBOX("cbReceive")
#define cbSend                  CTRL_COMBOBOX("cbSend")
#define cbTypmodin              CTRL_COMBOBOX("cbTypmodin")
#define cbTypmodout             CTRL_COMBOBOX("cbTypmodout")
#define cbAnalyze               CTRL_COMBOBOX("cbAnalyze")
#define cbCategory              CTRL_COMBOBOX("cbCategory")
#define chkPrefered             CTRL_CHECKBOX("chkPrefered")
#define chkVariable             CTRL_CHECKBOX("chkVariable")
#define txtIntLength            CTRL_TEXT("txtIntLength")
#define txtDefault              CTRL_TEXT("txtDefault")
#define cbElement               CTRL_COMBOBOX2("cbElement")
#define txtDelimiter            CTRL_TEXT("txtDelimiter")
#define chkByValue              CTRL_CHECKBOX("chkByValue")
#define cbAlignment             CTRL_COMBOBOX("cbAlignment")
#define cbStorage               CTRL_COMBOBOX("cbStorage")
#define lstMembers              CTRL_LISTVIEW("lstMembers")
#define txtMembername           CTRL_TEXT("txtMembername")
#define lstLabels               CTRL_LISTVIEW("lstLabels")
#define txtLabel                CTRL_TEXT("txtLabel")
#define btnAddMember            CTRL_BUTTON("btnAddMember")
#define btnChangeMember         CTRL_BUTTON("btnChangeMember")
#define btnRemoveMember         CTRL_BUTTON("btnRemoveMember")
#define btnAddAfterLabel        CTRL_BUTTON("btnAddAfterLabel")
#define btnAddBeforeLabel       CTRL_BUTTON("btnAddBeforeLabel")
#define btnRemoveLabel          CTRL_BUTTON("btnRemoveLabel")
#define pnlDefinition           CTRL_PANEL("pnlDefinition")
#define pnlDefinitionExtern     CTRL_PANEL("pnlDefinitionExtern")
#define pnlDefinitionComposite  CTRL_PANEL("pnlDefinitionComposite")
#define pnlDefinitionEnum       CTRL_PANEL("pnlDefinitionEnum")
#define pnlDefinitionRange      CTRL_PANEL("pnlDefinitionRange")
#define chkCollatable           CTRL_CHECKBOX("chkCollatable")
#define cbCollation             CTRL_COMBOBOX("cbCollation")
#define cbSubtype               CTRL_COMBOBOX("cbSubtype")
#define cbSubtypeOpclass        CTRL_COMBOBOX("cbSubtypeOpclass")
#define cbRngCollation          CTRL_COMBOBOX("cbRngCollation")
#define cbCanonical             CTRL_COMBOBOX("cbCanonical")
#define cbSubtypeDiff           CTRL_COMBOBOX("cbSubtypeDiff")


BEGIN_EVENT_TABLE(dlgType, dlgTypeProperty)
	EVT_RADIOBOX(XRCID("rdbType"),                  dlgType::OnTypeChange)

	EVT_TEXT(XRCID("cbInput"),                      dlgProperty::OnChange)
	EVT_COMBOBOX(XRCID("cbInput"),                  dlgProperty::OnChange)
	EVT_TEXT(XRCID("cbOutput"),                     dlgProperty::OnChange)
	EVT_COMBOBOX(XRCID("cbOutput"),                 dlgProperty::OnChange)
	EVT_TEXT(XRCID("txtIntLength"),                 dlgProperty::OnChange)
	EVT_CHECKBOX(XRCID("chkVariable"),              dlgProperty::OnChange)

	EVT_BUTTON(XRCID("btnAddMember"),               dlgType::OnMemberAdd)
	EVT_BUTTON(XRCID("btnChangeMember"),            dlgType::OnMemberChange)
	EVT_BUTTON(XRCID("btnRemoveMember"),            dlgType::OnMemberRemove)
	EVT_BUTTON(XRCID("btnAddBeforeLabel"),          dlgType::OnLabelAddBefore)
	EVT_BUTTON(XRCID("btnAddAfterLabel"),           dlgType::OnLabelAddAfter)
	EVT_BUTTON(XRCID("btnRemoveLabel"),             dlgType::OnLabelRemove)
	EVT_LIST_ITEM_SELECTED(XRCID("lstMembers"),     dlgType::OnMemberSelChange)
	EVT_LIST_ITEM_SELECTED(XRCID("lstLabels"),      dlgType::OnLabelSelChange)
	EVT_TEXT(XRCID("cbDatatype"),                   dlgType::OnSelChangeTyp)
	EVT_COMBOBOX(XRCID("cbDatatype"),               dlgType::OnSelChangeTyp)
	EVT_TEXT(XRCID("txtMembername"),                dlgType::OnChangeMember)
	EVT_TEXT(XRCID("txtLength"),                    dlgType::OnSelChangeTypOrLen)
	EVT_TEXT(XRCID("txtPrecision"),                 dlgType::OnSelChangeTypOrLen)
	EVT_BUTTON(CTL_ADDPRIV,                         dlgType::OnAddPriv)
	EVT_BUTTON(CTL_DELPRIV,                         dlgType::OnDelPriv)
	EVT_TEXT(XRCID("cbSubtype"),                    dlgType::OnSubtypeChange)
	EVT_COMBOBOX(XRCID("cbSubtype"),                dlgType::OnSubtypeChange)
	EVT_TEXT(XRCID("txtName"),                      dlgType::OnNameChange)
#ifdef __WXMAC__
	EVT_SIZE(                                       dlgType::OnChangeSize)
#endif
END_EVENT_TABLE();


dlgProperty *pgTypeFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgType(this, frame, (pgType *)node, (pgSchema *)parent);
}


dlgType::dlgType(pgaFactory *f, frmMain *frame, pgType *node, pgSchema *sch)
	: dlgTypeProperty(f, frame, wxT("dlgType"))
{
	type = node;
	schema = sch;

	seclabelPage = new ctlSeclabelPanel(nbNotebook);

	lstMembers->CreateColumns(0, _("Member"), _("Data type"), _("Collation"), -1);
	lstLabels->InsertColumn(0, _("Label"), wxLIST_FORMAT_LEFT, GetClientSize().GetWidth());

	cbStorage->Append(wxT("PLAIN"));
	cbStorage->Append(wxT("MAIN"));
	cbStorage->Append(wxT("EXTERNAL"));
	cbStorage->Append(wxT("EXTENDED"));

	queriesToBeSplitted = false;

	/* Type Privileges */
	securityChanged = false;
	if (node)
		connection = node->GetConnection();
	securityPage = new ctlSecurityPanel(nbNotebook, wxT("USAGE"), "U", frame->GetImageList());
	if (connection && connection->BackendMinimumVersion(9, 2) && (!node || node->CanCreate()))
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

		if (node)
		{
			wxString strAcl = node->GetAcl();
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


#ifdef __WXMAC__
void dlgType::OnChangeSize(wxSizeEvent &ev)
{
	securityPage->lbPrivileges->SetSize(wxDefaultCoord, wxDefaultCoord,
	                                    ev.GetSize().GetWidth(), ev.GetSize().GetHeight() - 550);
	if (GetAutoLayout())
	{
		Layout();
	}
}
#endif


void dlgType::OnChangeMember(wxCommandEvent &ev)
{
	wxString name = txtMembername->GetValue().Strip(wxString::both);

	btnAddMember->Enable(
	    ((type && connection->BackendMinimumVersion(9, 1)) || !type)
	    && !name.IsEmpty()
	    && lstMembers->FindItem(-1, name, false) == -1
	    && cbDatatype->GetGuessedSelection() >= 0);
	btnChangeMember->Enable(
	    lstMembers->FindItem(-1, name, false) == lstMembers->GetFirstSelected()
	    || lstMembers->FindItem(-1, name, false) == -1);
}

void dlgType::showDefinition(int panel)
{
	pnlDefinitionExtern->Show(false);
	pnlDefinitionComposite->Show(false);
	pnlDefinitionEnum->Show(false);
	pnlDefinitionRange->Show(false);

	switch (panel)
	{
		case 0:
			pnlDefinitionComposite->Show(true);
			break;
		case 1:
			pnlDefinitionEnum->Show(true);
			break;
		case 2:
			pnlDefinitionExtern->Show(true);
			break;
		case 3:
			pnlDefinitionRange->Show(true);
			break;
	}

	pnlDefinitionComposite->GetParent()->Layout();
	// we don't need to call GetParent()->Layout() for all four panels
	// because they all share the same parent
}


void dlgType::OnTypeChange(wxCommandEvent &ev)
{
	showDefinition(rdbType->GetSelection());

	CheckChange();
}


pgObject *dlgType::GetObject()
{
	return type;
}


int dlgType::Go(bool modal)
{
	pgSet *set;

	cbRngCollation->Enable(false);
	if (connection->BackendMinimumVersion(9, 2))
	{
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

		// Load the range combox (the 403 opcmethod is btree)
		set = connection->ExecuteSet(
		          wxT("SELECT DISTINCT typ.typname\n")
		          wxT("  FROM pg_opclass opc\n")
		          wxT("  JOIN pg_type typ ON opc.opcintype=typ.oid\n")
		          wxT("  WHERE opc.opcmethod=403\n")
		          wxT("  ORDER BY typname"));
		if (set)
		{
			while (!set->Eof())
			{
				wxString name = qtTypeIdent(set->GetVal(wxT("typname")));
				cbSubtype->Append(name);
				set->MoveNext();
			}
			delete set;
		}
		cbSubtype->SetSelection(0);
	}
	else
	{
		rdbType->Enable(3, false);
	}

	if (connection->BackendMinimumVersion(9, 1))
	{
		seclabelPage->SetConnection(connection);
		seclabelPage->SetObject(type);
		this->Connect(EVT_SECLABELPANEL_CHANGE, wxCommandEventHandler(dlgType::OnChange));
	}
	else
		seclabelPage->Disable();

	FillDatatype(cbDatatype, cbElement);

	if (connection->BackendMinimumVersion(9, 1))
	{
		// fill collation combobox
		cbCollation->Append(wxEmptyString);
		cbRngCollation->Append(wxEmptyString);
		set = connection->ExecuteSet(
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
				cbRngCollation->Append(name);
				set->MoveNext();
			}
			delete set;
		}
		cbCollation->SetSelection(0);
		cbRngCollation->SetSelection(0);
	}

	if (type)
	{
		// Edit Mode
		cbSchema->Enable(connection->BackendMinimumVersion(8, 1));
		txtName->Enable(connection->BackendMinimumVersion(8, 4));
		rdbType->SetSelection(type->GetTypeClass());
		rdbType->Disable();

		showDefinition(type->GetTypeClass());

		cbInput->Append(type->GetInputFunction());
		cbInput->SetSelection(0);
		cbInput->Disable();
		cbOutput->Append(type->GetOutputFunction());
		cbOutput->SetSelection(0);
		cbOutput->Disable();
		cbReceive->Append(type->GetReceiveFunction());
		cbReceive->SetSelection(0);
		cbReceive->Disable();
		cbSend->Append(type->GetSendFunction());
		cbSend->SetSelection(0);
		cbSend->Disable();
		cbTypmodin->Append(type->GetTypmodinFunction());
		cbTypmodin->SetSelection(0);
		cbTypmodin->Disable();
		cbTypmodout->Append(type->GetTypmodoutFunction());
		cbTypmodout->SetSelection(0);
		cbTypmodout->Disable();
		cbAnalyze->Append(type->GetAnalyzeFunction());
		cbAnalyze->SetSelection(0);
		cbAnalyze->Disable();
		cbCategory->Append(catGetText(type->GetCategory()));
		cbCategory->SetSelection(0);
		cbCategory->Disable();
		chkPrefered->SetValue(type->GetPrefered());
		chkPrefered->Disable();

		chkVariable->SetValue(type->GetInternalLength() < 0);
		chkVariable->Disable();
		if (type->GetInternalLength() > 0)
			txtIntLength->SetValue(NumToStr(type->GetInternalLength()));
		txtIntLength->Disable();
		txtDefault->SetValue(type->GetDefault());
		txtDefault->Disable();
		cbElement->Append(type->GetElement());
		cbElement->SetSelection(0);
		cbElement->Disable();
		txtDelimiter->SetValue(type->GetDelimiter());
		txtDelimiter->Disable();
		chkByValue->SetValue(type->GetPassedByValue());
		chkByValue->Disable();
		cbAlignment->SetValue(type->GetAlignment());
		cbAlignment->Disable();
		cbStorage->SetValue(type->GetStorage());
		cbStorage->Disable();
		chkCollatable->SetValue(type->GetCollatable());
		chkCollatable->Disable();

		bool changeok = connection->BackendMinimumVersion(9, 1);
		txtMembername->Enable(changeok);
		cbCollation->Enable(changeok);
		btnAddMember->Enable(false);
		btnChangeMember->Enable(false);
		btnRemoveMember->Enable(false);

		txtLabel->Enable(connection->BackendMinimumVersion(9, 1));
		btnAddBeforeLabel->Enable(connection->BackendMinimumVersion(9, 1));
		btnAddAfterLabel->Enable(connection->BackendMinimumVersion(9, 1));
		btnRemoveLabel->Disable();

		wxArrayString elements = type->GetTypesArray();
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
			memberCollations.Add(elements.Item(i + 2));
			memberOriginalNames.Add(elements.Item(i));
		}

		cbDatatype->Enable(changeok);
		txtLength->Enable(changeok);

		// Load the enum labels
		elements = type->GetLabelArray();
		for (i = 0 ; i < elements.GetCount() ; i++)
			lstLabels->AppendItem(0, elements.Item(i));

		// Load the RANGE informations
		cbSubtype->SetValue(type->GetSubtypeFunctionStr());
		cbSubtype->Disable();
		cbSubtypeOpclass->Append(type->GetSubtypeOpClassFunctionStr());
		cbSubtypeOpclass->SetSelection(0);
		cbSubtypeOpclass->Disable();
		cbRngCollation->Append(type->GetCollationFunctionStr());
		cbRngCollation->SetSelection(0);
		cbRngCollation->Disable();
		cbCanonical->Append(type->GetCanonical());
		cbCanonical->SetSelection(0);
		cbCanonical->Disable();
		cbSubtypeDiff->Append(type->GetSubtypeDiff());
		cbSubtypeDiff->SetSelection(0);
		cbSubtypeDiff->Disable();

		if (!connection->BackendMinimumVersion(7, 5))
			cbOwner->Disable();
	}
	else
	{
		// Create mode
		cbOwner->Disable();

		bool hasSendRcv = connection->BackendMinimumVersion(7, 4);
		bool hasTypmod = connection->BackendMinimumVersion(8, 3);

		cbCategory->Enable(connection->BackendMinimumVersion(8, 4));
		chkPrefered->Enable(connection->BackendMinimumVersion(8, 4));

		if (hasSendRcv)
		{
			cbReceive->Append(wxEmptyString);
			cbSend->Append(wxEmptyString);
			cbAnalyze->Append(wxEmptyString);
		}
		else
		{
			cbReceive->Disable();
			cbSend->Disable();
			cbAnalyze->Disable();
		}

		if (hasTypmod)
		{
			cbTypmodin->Append(wxEmptyString);
			cbTypmodout->Append(wxEmptyString);
		}
		else
		{
			cbTypmodin->Disable();
			cbTypmodout->Disable();
		}

		if (!connection->BackendMinimumVersion(8, 3))
			rdbType->Enable(TYPE_ENUM, false);

		chkCollatable->Enable(connection->BackendMinimumVersion(9, 1));
		cbCollation->Enable(connection->BackendMinimumVersion(9, 1));

		set = connection->ExecuteSet(
		          wxT("SELECT proname, nspname\n")
		          wxT("  FROM (\n")
		          wxT("        SELECT proname, nspname, max(proargtypes[0]) AS arg0, max(proargtypes[1]) AS arg1\n")
		          wxT("          FROM pg_proc p\n")
		          wxT("          JOIN pg_namespace n ON n.oid=pronamespace\n")
		          wxT("         GROUP BY proname, nspname\n")
		          wxT("        HAVING count(proname) = 1   ) AS uniquefunc\n")
		          wxT(" WHERE arg0 <> 0 AND arg1 = 0"));

		if (set)
		{
			while (!set->Eof())
			{
				wxString pn = database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("proname"));

				cbInput->Append(pn);
				cbOutput->Append(pn);
				if (hasSendRcv)
				{
					cbReceive->Append(pn);
					cbSend->Append(pn);
					cbAnalyze->Append(pn);
				}
				if (hasTypmod)
				{
					cbTypmodin->Append(pn);
					cbTypmodout->Append(pn);
				}
				set->MoveNext();
			}
			delete set;
		}

		if (hasTypmod)
		{
			set = connection->ExecuteSet(
			          wxT("SELECT proname, nspname\n")
			          wxT("  FROM pg_proc p\n")
			          wxT("  JOIN pg_namespace n ON n.oid=pronamespace\n")
			          wxT("  WHERE prorettype=(SELECT oid FROM pg_type WHERE typname='int4')")
			          wxT("    AND proargtypes[0]=(SELECT oid FROM pg_type WHERE typname='_cstring')")
			          wxT("    AND proargtypes[1] IS NULL")
			          wxT("  ORDER BY nspname, proname"));

			if (set)
			{
				while (!set->Eof())
				{
					wxString pn = database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("proname"));

					cbTypmodin->Append(pn);
					set->MoveNext();
				}
				delete set;
			}

			set = connection->ExecuteSet(
			          wxT("SELECT proname, nspname\n")
			          wxT("  FROM pg_proc p\n")
			          wxT("  JOIN pg_namespace n ON n.oid=pronamespace\n")
			          wxT("  WHERE prorettype=(SELECT oid FROM pg_type WHERE typname='cstring')")
			          wxT("    AND proargtypes[0]=(SELECT oid FROM pg_type WHERE typname='int4')")
			          wxT("    AND proargtypes[1] IS NULL")
			          wxT("  ORDER BY nspname, proname"));

			if (set)
			{
				while (!set->Eof())
				{
					wxString pn = database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("proname"));

					cbTypmodout->Append(pn);
					set->MoveNext();
				}
				delete set;
			}
		}
	}

	txtLength->SetValidator(numericValidator);

	return dlgTypeProperty::Go(modal);
}


void dlgType::OnSelChangeTyp(wxCommandEvent &ev)
{
	txtLength->SetValue(wxEmptyString);
	txtPrecision->SetValue(wxEmptyString);
	cbDatatype->GuessSelection(ev);
	cbCollation->SetValue(wxEmptyString);
	OnSelChangeTypOrLen(ev);
}



void dlgType::OnNameChange(wxCommandEvent &ev)
{
	wxString qry;
	wxString shelltype;
	pgSet *set;

	cbCanonical->Clear();
	if (!txtName->GetValue().IsEmpty())
	{
		// Grab the shelltype
		qry = wxT("SELECT oid FROM pg_type WHERE typname='") + qtIdent(txtName->GetValue()) + wxT("'");
		set = connection->ExecuteSet(qry);
		if (set)
		{
			if (!set->Eof())
			{
				shelltype = set->GetVal(wxT("oid"));
			}
			delete set;
		}

		if (!shelltype.IsEmpty())
		{
			// Load the canonical functions
			qry = wxT("SELECT proname, nspname\n")
			      wxT("  FROM pg_proc\n")
			      wxT("  JOIN pg_namespace n ON n.oid=pronamespace\n")
			      wxT("  WHERE prorettype=") + shelltype +
			      wxT("    AND proargtypes='") + shelltype + wxT("'\n")
			      wxT("  ORDER BY proname\n");

			cbCanonical->Append(wxEmptyString);
			set = connection->ExecuteSet(qry);
			if (set)
			{
				while (!set->Eof())
				{
					wxString procname = database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("proname"));
					cbCanonical->Append(procname);
					set->MoveNext();
				}
				delete set;
			}
			cbCanonical->SetSelection(0);
		}
	}
	CheckChange();
}


void dlgType::OnSubtypeChange(wxCommandEvent &ev)
{
	wxString subtypeoid;
	cbSubtypeOpclass->Clear();
	cbSubtypeDiff->Clear();

	if (!cbSubtype->GetValue().IsEmpty())
	{
		cbSubtypeOpclass->Append(wxEmptyString);
		pgSet *set = connection->ExecuteSet(
		                 wxT("SELECT opc.opcname, opc.opcintype\n")
		                 wxT("  FROM pg_opclass opc\n")
		                 wxT("  JOIN pg_type typ ON opc.opcintype=typ.oid ")
		                 wxT("   AND typ.typname='") + cbSubtype->GetValue() + wxT("'\n")
		                 wxT("  WHERE opc.opcmethod=403\n")
		                 wxT("  ORDER BY opcname"));
		if (set)
		{
			while (!set->Eof())
			{
				wxString name = qtIdent(set->GetVal(wxT("opcname")));
				subtypeoid = set->GetVal(wxT("opcintype"));
				cbSubtypeOpclass->Append(name);
				set->MoveNext();
			}
			delete set;
		}
		cbSubtypeOpclass->SetSelection(0);

		set = connection->ExecuteSet(
		          wxT("SELECT typcollation FROM pg_type WHERE typname='") + cbSubtype->GetValue() + wxT("'\n"));
		if (set)
		{
			if (!set->Eof())
			{
				cbRngCollation->Enable(set->GetLong(wxT("typcollation")) > 0);
			}
			delete set;
		}
		else
		{
			cbRngCollation->Enable(false);
		}

		// Load the subtypediff functions (701 is double precision type)

		set = connection->ExecuteSet(
		          wxT("SELECT proname, nspname\n")
		          wxT("  FROM pg_proc\n")
		          wxT("  JOIN pg_namespace n ON n.oid=pronamespace\n")
		          wxT("  WHERE prorettype=701 ")
		          wxT("    AND proargtypes='") + subtypeoid + wxT(" ") + subtypeoid + wxT("'\n")
		          wxT("  ORDER BY proname\n"));
		if (set)
		{
			while (!set->Eof())
			{
				wxString procname = database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("proname"));
				cbSubtypeDiff->Append(procname);
				set->MoveNext();
			}
			delete set;
		}
	}
}


void dlgType::OnSelChangeTypOrLen(wxCommandEvent &ev)
{
	if ((type && connection->BackendMinimumVersion(9, 1)) || !type)
	{
		CheckLenEnable();
		txtLength->Enable(isVarLen);
		txtPrecision->Enable(isVarPrec);
		cbCollation->Enable(connection->BackendMinimumVersion(9, 1));
		CheckChange();
		OnChangeMember(ev);
	}
}


void dlgType::CheckChange()
{
	bool enable = true;

	if (rdbType->GetSelection() == TYPE_COMPOSITE)
	{
		CheckValid(enable, lstMembers->GetItemCount() > 1, _("Please specify at least two members."));
	}
	else if (rdbType->GetSelection() == TYPE_ENUM)
	{
		CheckValid(enable, lstLabels->GetItemCount() >= 1, _("Please specify at least one label."));
	}
	else if (rdbType->GetSelection() == TYPE_EXTERNAL)
	{
		txtLength->Enable(!chkVariable->GetValue());
		CheckValid(enable, cbInput->GetCurrentSelection() >= 0, _("Please specify input conversion function."));
		CheckValid(enable, cbOutput->GetCurrentSelection() >= 0, _("Please specify output conversion function."));
		CheckValid(enable, chkVariable->GetValue() || StrToLong(txtLength->GetValue()) > 0, _("Please specify internal storage length."));
	}
	else
	{
		CheckValid(enable, cbSubtype->GetCurrentSelection() >= 0, _("Please specify subtype function."));
	}

	if (type)
	{
		enable = enable && (GetName() != type->GetName()
		                    || txtComment->GetValue() != type->GetComment()
		                    || cbSchema->GetValue() != type->GetSchema()->GetName()
		                    || cbOwner->GetValue() != type->GetOwner()
		                    || (rdbType->GetSelection() == TYPE_COMPOSITE && GetSqlForTypes() != wxEmptyString)
		                    || (GetSql().Length() > 0 && connection->BackendMinimumVersion(9, 1)));
		if (seclabelPage && connection->BackendMinimumVersion(9, 1))
			enable = enable || !(seclabelPage->GetSqlForSecLabels().IsEmpty());
	}
	else
	{
		wxString name = GetName();

		CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
		CheckValid(enable, !name.StartsWith(wxT("_")), _("Name may not start with '_'."));
	}

	EnableOK(enable || securityChanged);
}


void dlgType::OnMemberSelChange(wxListEvent &ev)
{
	long pos = lstMembers->GetSelection();
	if (pos >= 0)
	{
		txtMembername->SetValue(lstMembers->GetText(pos));
		cbDatatype->SetValue(memberTypes.Item(pos).AfterFirst(':'));
		txtLength->SetValue(memberLengths.Item(pos));
		txtLength->Enable(((type && connection->BackendMinimumVersion(9, 1)) || !type) && !txtLength->GetValue().IsEmpty());
		txtPrecision->SetValue(memberPrecisions.Item(pos));
		txtPrecision->Enable(((type && connection->BackendMinimumVersion(9, 1)) || !type) && !txtPrecision->GetValue().IsEmpty());
		cbCollation->SetValue(memberCollations.Item(pos));
		cbCollation->Enable(connection->BackendMinimumVersion(9, 1));
		btnChangeMember->Enable((type && connection->BackendMinimumVersion(9, 1)) || !type);
		btnRemoveMember->Enable((type && connection->BackendMinimumVersion(9, 1)) || !type);
	}
}


void dlgType::OnMemberAdd(wxCommandEvent &ev)
{
	wxString name = txtMembername->GetValue().Strip(wxString::both);
	wxString type = cbDatatype->GetValue();
	wxString length = wxEmptyString;
	wxString precision = wxEmptyString;
	wxString collation = wxEmptyString;

	if (txtLength->GetValue() != wxT("") && txtLength->IsEnabled())
		length = txtLength->GetValue();
	if (txtPrecision->GetValue() != wxT("") && txtPrecision->IsEnabled())
		precision = txtPrecision->GetValue();
	if (cbCollation->GetValue() != wxT("") && cbCollation->IsEnabled())
		collation = cbCollation->GetValue();

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
		lstMembers->SetItem(pos, 2, collation);
		memberTypes.Add(GetTypeInfo(cbDatatype->GetGuessedSelection()));
		memberLengths.Add(length);
		memberPrecisions.Add(precision);
		memberCollations.Add(collation);
		memberOriginalNames.Add(wxEmptyString);
	}

	CheckChange();
}


void dlgType::OnMemberChange(wxCommandEvent &ev)
{
	wxString name = txtMembername->GetValue().Strip(wxString::both);
	wxString type = cbDatatype->GetValue();
	wxString length = wxEmptyString;
	wxString precision = wxEmptyString;
	wxString collation = wxEmptyString;

	if (txtLength->GetValue() != wxT("") && txtLength->IsEnabled())
		length = txtLength->GetValue();
	if (txtPrecision->GetValue() != wxT("") && txtPrecision->IsEnabled())
		precision = txtPrecision->GetValue();
	if (cbCollation->GetValue() != wxT("") && cbCollation->IsEnabled())
		collation = cbCollation->GetValue();

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
			lstMembers->SetItem(pos, 2, collation);
			memberTypes.Insert(GetTypeInfo(cbDatatype->GetGuessedSelection()), pos);
			memberLengths.Insert(length, pos);
			memberPrecisions.Insert(precision, pos);
			memberCollations.Insert(collation, pos);
			memberTypes.RemoveAt(pos + 1);
			memberLengths.RemoveAt(pos + 1);
			memberPrecisions.RemoveAt(pos + 1);
			memberCollations.RemoveAt(pos + 1);
		}
	}

	CheckChange();
}


void dlgType::OnMemberRemove(wxCommandEvent &ev)
{
	long pos = lstMembers->GetSelection();

	if (pos >= 0)
	{
		lstMembers->DeleteItem(pos);
		memberTypes.RemoveAt(pos);
		memberLengths.RemoveAt(pos);
		memberPrecisions.RemoveAt(pos);
		memberCollations.RemoveAt(pos);
		memberOriginalNames.RemoveAt(pos);
	}
	CheckChange();
}


void dlgType::OnLabelSelChange(wxListEvent &ev)
{
	long pos = lstLabels->GetSelection();
	if (pos >= 0)
	{
		txtLabel->SetValue(lstLabels->GetText(pos));
	}
}


void dlgType::OnLabelAddBefore(wxCommandEvent &ev)
{
	wxString label = txtLabel->GetValue().Strip(wxString::both);

	if (!label.IsEmpty())
	{
		long pos = lstLabels->FindItem(-1, label);
		if (pos < 0)
		{
			if (lstLabels->GetFirstSelected() >= 0)
				pos = lstLabels->GetFirstSelected();
			else
				pos = 0;
			lstLabels->InsertItem(pos, label, 0);
		}
	}
	txtLabel->SetValue(wxEmptyString);
	CheckChange();
}


void dlgType::OnLabelAddAfter(wxCommandEvent &ev)
{
	wxString label = txtLabel->GetValue().Strip(wxString::both);

	if (!label.IsEmpty())
	{
		long pos = lstLabels->FindItem(-1, label);
		if (pos < 0)
		{
			if (lstLabels->GetFirstSelected() >= 0)
				pos = lstLabels->GetFirstSelected() + 1;
			else
				pos = lstLabels->GetItemCount();
			lstLabels->InsertItem(pos, label, 0);
		}
	}
	txtLabel->SetValue(wxEmptyString);
	CheckChange();
}


void dlgType::OnLabelRemove(wxCommandEvent &ev)
{
	long pos = lstLabels->GetSelection();

	if (pos >= 0)
		lstLabels->DeleteItem(pos);

	CheckChange();
}


pgObject *dlgType::CreateObject(pgCollection *collection)
{
	pgObject *obj = 0; //pgType::ReadObjects(collection, 0, wxT("\n WHERE usename=") + qtDbString(name));
	return obj;
}


wxString dlgType::GetSql()
{
	wxString sql, direction, objname;
	size_t existingitems_index, listitems_index, offset;

	if (type)
	{
		// Edit Mode
		objname = schema->GetQuotedPrefix() + qtIdent(GetName());
		AppendNameChange(sql, wxT("TYPE ") + type->GetQuotedFullIdentifier());
		AppendOwnerChange(sql, wxT("TYPE ") + objname);

		sql += GetSqlForTypes();
		if (rdbType->GetSelection() == TYPE_ENUM && connection->BackendMinimumVersion(9, 1))
		{
			wxArrayString elements = type->GetLabelArray();
			existingitems_index = 0;
			for (listitems_index = 0 ; listitems_index < (size_t)lstLabels->GetItemCount() ; listitems_index++)
			{
				if (existingitems_index >= elements.GetCount() || lstLabels->GetItemText(listitems_index) != elements.Item(existingitems_index))
				{
					queriesToBeSplitted = true;
					if (listitems_index == 0)
					{
						direction = wxT("BEFORE");
						offset = 0;
					}
					else
					{
						direction = wxT("AFTER");
						offset = -1;
					}

					sql += wxT("ALTER TYPE ") + objname
					       +  wxT("\n  ADD VALUE ") + connection->qtDbString(lstLabels->GetItemText(listitems_index))
					       +  wxT(" ") + direction + wxT(" ")
					       + connection->qtDbString(elements.Item(existingitems_index + offset))
					       + wxT(";\n");
				}
				else
					existingitems_index++;
			}
		}
		AppendSchemaChange(sql, wxT("TYPE ") + objname);
	}
	else
	{
		// Create Mode
		sql = wxT("CREATE TYPE ") + schema->GetQuotedPrefix() + qtIdent(GetName());

		if (rdbType->GetSelection() == TYPE_COMPOSITE)
		{
			sql += wxT(" AS\n   (");

			int i;
			for (i = 0 ; i < lstMembers->GetItemCount() ; i++)
			{
				if (i)
					sql += wxT(",\n    ");
				sql += qtIdent(lstMembers->GetItemText(i)) + wxT(" ")
				       + GetFullTypeName(i);
			}
		}
		else if (rdbType->GetSelection() == TYPE_ENUM)
		{
			sql += wxT(" AS ENUM\n   (");

			int i;
			for (i = 0 ; i < lstLabels->GetItemCount() ; i++)
			{
				if (i)
					sql += wxT(",\n    ");
				sql += connection->qtDbString(lstLabels->GetItemText(i));
			}
		}
		else if (rdbType->GetSelection() == TYPE_EXTERNAL)
		{
			sql += wxT("\n   (INPUT=");
			AppendQuoted(sql, cbInput->GetValue());
			sql += wxT(", OUTPUT=");
			AppendQuoted(sql, cbOutput->GetValue());

			if (connection->BackendMinimumVersion(7, 4))
			{
				if (cbReceive->GetCurrentSelection() > 0 || cbSend->GetCurrentSelection() > 0)
				{
					if (cbReceive->GetCurrentSelection() > 0)
					{
						sql += wxT(",\n   RECEIVE=");
						AppendQuoted(sql, cbReceive->GetValue());
						if (cbSend->GetCurrentSelection() > 0)
						{
							sql += wxT(", SEND=");
							AppendQuoted(sql, cbSend->GetValue());
						}
					}
					else
					{
						sql += wxT(",\n   SEND=");
						AppendQuoted(sql, cbSend->GetValue());
					}
				}
				if (cbAnalyze->GetCurrentSelection() > 0)
				{
					sql += wxT(",\n   ANALYZE=");
					AppendQuoted(sql, cbAnalyze->GetValue());
				}

			}
			if (connection->BackendMinimumVersion(8, 3))
			{
				if (cbTypmodin->GetCurrentSelection() > 0 || cbTypmodout->GetCurrentSelection() > 0)
				{
					if (cbTypmodin->GetCurrentSelection() > 0)
					{
						sql += wxT(",\n   TYPMOD_IN=");
						AppendQuoted(sql, cbTypmodin->GetValue());
						if (cbTypmodout->GetCurrentSelection() > 0)
						{
							sql += wxT(", TYPMOD_OUT=");
							AppendQuoted(sql, cbTypmodout->GetValue());
						}
					}
					else
					{
						sql += wxT(",\n   TYPMOD_OUT=");
						AppendQuoted(sql, cbTypmodout->GetValue());
					}
				}

			}
			if (connection->BackendMinimumVersion(8, 4))
			{
				sql += wxT(",\n   CATEGORY=") + qtDbString(cbCategory->GetValue());
				if (chkPrefered->GetValue())
					sql += wxT(",\n   PREFERRED=true");
			}
			sql += wxT(",\n    INTERNALLENGTH=");
			if (chkVariable->GetValue())
				sql += wxT("VARIABLE");
			else
				sql += txtLength->GetValue();
			AppendIfFilled(sql, wxT(",\n    DEFAULT="), txtDefault->GetValue());
			if (!cbElement->GetValue().IsEmpty())
			{
				sql += wxT(",\n    ELEMENT=");
				AppendQuoted(sql, cbElement->GetValue());
				AppendIfFilled(sql, wxT(", DELIMITER="), qtDbString(txtDelimiter->GetValue().Strip(wxString::both)));
			}
			if (chkByValue->GetValue())
				sql += wxT(",\n    PASSEDBYVALUE");
			AppendIfFilled(sql, wxT(",\n    ALIGNMENT="), cbAlignment->GetValue());
			AppendIfFilled(sql, wxT(",\n    STORAGE="), cbStorage->GetValue());
			if (connection->BackendMinimumVersion(9, 1) && chkCollatable->GetValue())
				sql += wxT(",\n    COLLATABLE=true");
		}
		else
		{
			sql += wxT(" AS RANGE\n   (SUBTYPE=");
			AppendQuoted(sql, cbSubtype->GetValue());
			if (!cbSubtypeOpclass->GetValue().IsEmpty())
			{
				sql += wxT(", SUBTYPE_OPCLASS=");
				AppendQuoted(sql, cbSubtypeOpclass->GetValue());
			}
			if (!cbRngCollation->GetValue().IsEmpty())
			{
				sql += wxT(", COLLATION=") + cbRngCollation->GetValue();
			}
			if (!cbCanonical->GetValue().IsEmpty())
			{
				sql += wxT(", CANONICAL=");
				AppendQuoted(sql, cbCanonical->GetValue());
			}
			if (!cbSubtypeDiff->GetValue().IsEmpty())
			{
				sql += wxT(", SUBTYPE_DIFF=");
				AppendQuoted(sql, cbSubtypeDiff->GetValue());
			}
		}

		sql += wxT(");\n");
	}
	AppendComment(sql, wxT("TYPE ") + qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()), type);

	if (seclabelPage && connection->BackendMinimumVersion(9, 1))
		sql += seclabelPage->GetSqlForSecLabels(wxT("TYPE"), qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()));

	// securityPage will exists only for PG 9.2 and later
	if (connection->BackendMinimumVersion(9, 2))
		sql += securityPage->GetGrant(wxT("U"), wxT("TYPE ") + qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()), &currentAcl);
	return sql;
}

wxString dlgType::GetFullTypeName(int type)
{
	wxString typname = memberTypes.Item(type).AfterFirst(':');

	if (!memberLengths.Item(type).IsEmpty())
	{
		typname += wxT("(") + memberLengths.Item(type);
		if (!memberPrecisions.Item(type).IsEmpty())
			typname += wxT(",") + memberPrecisions.Item(type);
		typname += wxT(")");
	}
	if (!memberCollations.Item(type).IsEmpty() && memberCollations.Item(type) != wxT("pg_catalog.\"default\""))
		typname += wxT(" COLLATE ") + memberCollations.Item(type);

	return typname;
}

wxString dlgType::GetSqlForTypes()
{
	wxString sql = wxEmptyString;
	wxString objname, old_name, old_type, old_collation, new_name, new_type, new_full_type, new_collation, original_name;
	wxArrayString elements = type->GetTypesArray();
	int newindex;
	unsigned int oldindex = 0;
	int hold = 0;
	objname = schema->GetQuotedPrefix() + qtIdent(GetName());

	for (newindex = 0 ; newindex < lstMembers->GetItemCount() ; newindex = newindex + 1 - hold)
	{
		// this will decide whether we progress to the next new item,
		// or whether we need to continue checking the old list first
		hold = 0;

		// these are a copy of the list before any changes
		if (elements.GetCount() >= (oldindex * 3) + 3)
		{
			old_name = elements.Item(oldindex * 3);
			old_type = elements.Item(oldindex * 3 + 1);
			old_collation = elements.Item(oldindex * 3 + 2);
		}
		else
		{
			// we've now used up all the old attributes
			old_name = wxEmptyString;
			old_type = wxEmptyString;
			old_collation = wxEmptyString;
		}

		// this is the original name of the type before editing
		original_name = memberOriginalNames.Item(newindex);

		new_name = lstMembers->GetItemText(newindex);
		new_type = memberTypes.Item(newindex).AfterFirst(':');
		new_full_type = GetFullTypeName(newindex);
		new_collation = memberCollations.Item(newindex);

		if (!original_name.IsEmpty() && original_name == old_name && (new_name != old_name
		        || new_type != old_type || new_collation != old_collation))
		{
			// if this was originally in the list and the name has changed then rename it

			if (new_name != old_name)
			{
				sql += wxT("ALTER TYPE ") + objname + wxT("\n  RENAME ATTRIBUTE ")
				       + qtIdent(original_name) + wxT(" TO ") + qtIdent(new_name) + wxT(";\n");
			}

			if (new_type != old_type || new_collation != old_collation)
			{
				sql += wxT("ALTER TYPE ") + objname + wxT("\n  ALTER ATTRIBUTE ")
				       + qtIdent(new_name);

				// the syntax for alter attribute requires that we always specify the type
				sql += wxT(" SET DATA TYPE ") + new_type;

				if (new_collation != old_collation)
					sql += wxT(" COLLATE ") + new_collation;

				sql += wxT(";\n");
			}
		}
		else if (!original_name.IsEmpty() && original_name != old_name)
		{
			// the old attribute isn't in the new list so drop it

			// don't move through new list yet
			hold = 1;

			sql += wxT("ALTER TYPE ") + objname + wxT("\n  DROP ATTRIBUTE ")
			       + qtIdent(old_name) + wxT(";\n");
		}
		else if (original_name.IsEmpty())
		{
			if (!old_name.IsEmpty())
			{
				sql += wxT("ALTER TYPE ") + objname + wxT("\n  DROP ATTRIBUTE ")
				       + qtIdent(old_name) + wxT(";\n");
			}

			sql += wxT("ALTER TYPE ") + objname + wxT("\n  ADD ATTRIBUTE ")
			       + qtIdent(new_name) + wxT(" ") + new_full_type + wxT(";\n");
		}
		else
		{
			// do nothing
		}

		oldindex++;

		if (newindex + 1 - hold == lstMembers->GetItemCount() && elements.GetCount() >= (oldindex * 3) + 3)
		{
			// remove remaining old attributes
			for (; elements.GetCount() >= (oldindex * 3) + 3; oldindex++)
			{
				old_name = elements.Item(oldindex * 3);
				old_type = elements.Item(oldindex * 3 + 1);
				old_collation = elements.Item(oldindex * 3 + 2);

				sql += wxT("ALTER TYPE ") + objname + wxT("\n  DROP ATTRIBUTE ")
				       + qtIdent(old_name) + wxT(";\n");
			}
			break;
		}
	}

	return sql;
}


void dlgType::OnChange(wxCommandEvent &event)
{
	CheckChange();
}


void dlgType::OnAddPriv(wxCommandEvent &ev)
{
	securityChanged = true;
	CheckChange();
}


void dlgType::OnDelPriv(wxCommandEvent &ev)
{
	securityChanged = true;
	CheckChange();
}


wxString dlgType::catGetText(wxString c)
{
	if (c == wxT("A"))
		return wxT("Array types");
	if (c == wxT("B"))
		return wxT("Boolean types");
	if (c == wxT("C"))
		return wxT("Composite types");
	if (c == wxT("D"))
		return wxT("Date/time types");
	if (c == wxT("E"))
		return wxT("Enum types");
	if (c == wxT("G"))
		return wxT("Geometric types");
	if (c == wxT("I"))
		return wxT("Network address types");
	if (c == wxT("N"))
		return wxT("Numeric types");
	if (c == wxT("P"))
		return wxT("Pseudo-types");
	if (c == wxT("S"))
		return wxT("String types");
	if (c == wxT("T"))
		return wxT("Timespan types");
	if (c == wxT("U"))
		return wxT("User-defined types");
	if (c == wxT("V"))
		return wxT("Bit-string types");
	if (c == wxT("X"))
		return wxT("unknown type");
	return wxEmptyString;
}


wxString dlgType::catGetChar(wxString t)
{
	if (t == wxT("Array types"))
		return wxT("A");
	if (t == wxT("Boolean types"))
		return wxT("B");
	if (t == wxT("Composite types"))
		return wxT("C");
	if (t == wxT("Date/time types"))
		return wxT("D");
	if (t == wxT("Enum types"))
		return wxT("E");
	if (t == wxT("Geometric types"))
		return wxT("G");
	if (t == wxT("Network address types"))
		return wxT("I");
	if (t == wxT("Numeric types"))
		return wxT("N");
	if (t == wxT("Pseudo-types"))
		return wxT("P");
	if (t == wxT("String types"))
		return wxT("S");
	if (t == wxT("Timespan types"))
		return wxT("T");
	if (t == wxT("User-defined types"))
		return wxT("U");
	if (t == wxT("Bit-string types"))
		return wxT("V");
	if (t == wxT("unknown type"))
		return wxT("X");
	return wxEmptyString;
}
