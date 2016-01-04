//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgIndexConstraint.cpp - PostgreSQL IndexConstraint Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "frm/frmMain.h"
#include "dlg/dlgIndexConstraint.h"
#include "schema/pgIndex.h"
#include "schema/pgColumn.h"
#include "schema/pgTable.h"
#include "schema/pgIndexConstraint.h"


#define cbTablespace    CTRL_COMBOBOX("cbTablespace")
#define cbIndex         CTRL_COMBOBOX("cbIndex")
#define cbType          CTRL_COMBOBOX("cbType")
#define txtFillFactor   CTRL_TEXT("txtFillFactor")
#define txtWhere        CTRL_TEXT("txtWhere")
#define chkDeferrable   CTRL_CHECKBOX("chkDeferrable")
#define chkDeferred     CTRL_CHECKBOX("chkDeferred")
#define cbOpClass       CTRL_COMBOBOX("cbOpClass")
#define chkDesc         CTRL_CHECKBOX("chkDesc")
#define rdbNullsFirst   CTRL_RADIOBUTTON("rdbNullsFirst")
#define rdbNullsLast    CTRL_RADIOBUTTON("rdbNullsLast")
#define cbOperator      CTRL_COMBOBOX("cbOperator")

BEGIN_EVENT_TABLE(dlgIndexConstraint, dlgIndexBase)
	EVT_CHECKBOX(XRCID("chkDeferrable"),            dlgProperty::OnChange)
	EVT_BUTTON(XRCID("btnAddCol"),                  dlgIndexConstraint::OnAddCol)
	EVT_BUTTON(XRCID("btnRemoveCol"),               dlgIndexConstraint::OnRemoveCol)
#ifdef __WXMAC__
	EVT_SIZE(                                       dlgIndexConstraint::OnChangeSize)
#endif
	EVT_COMBOBOX(XRCID("cbIndex"),                  dlgIndexConstraint::OnChangeIndex)
	EVT_COMBOBOX(XRCID("cbType"),                   dlgIndexConstraint::OnSelectType)
	EVT_COMBOBOX(XRCID("cbColumns"),                dlgIndexConstraint::OnSelectComboCol)
END_EVENT_TABLE();


dlgIndexConstraint::dlgIndexConstraint(pgaFactory *f, frmMain *frame, const wxString &resName, pgIndexBase *index, pgTable *parentNode)
	: dlgIndexBase(f, frame, resName, index, parentNode)
{
	lstColumns->AddColumn(_("Column name"), 90);
	lstColumns->AddColumn(_("Order"), 40);
	lstColumns->AddColumn(_("NULLs order"), 50);
	lstColumns->AddColumn(_("Op. class"), 40);
	lstColumns->AddColumn(_("Operator"), 40);
}


dlgIndexConstraint::dlgIndexConstraint(pgaFactory *f, frmMain *frame, const wxString &resName, ctlListView *colList)
	: dlgIndexBase(f, frame, resName, colList)
{
	lstColumns->AddColumn(_("Column name"), 90);
	lstColumns->AddColumn(_("Order"), 40);
	lstColumns->AddColumn(_("NULLs order"), 50);
	lstColumns->AddColumn(_("Op. class"), 40);
	lstColumns->AddColumn(_("Operator"), 40);
}


wxString dlgIndexConstraint::GetColumns()
{
	wxString sql;

	int pos;
	// iterate cols
	for (pos = 0 ; pos < lstColumns->GetItemCount() ; pos++)
	{
		if (pos)
			sql += wxT(", ");

		sql += qtIdent(lstColumns->GetItemText(pos));

		wxString opclass = lstColumns->GetText(pos, 3);
		if (!opclass.IsEmpty())
			sql += wxT(" ") + opclass;

		wxString order = lstColumns->GetText(pos, 1);
		if (!order.IsEmpty())
			sql += wxT(" ") + order;

		wxString nullsOrder = lstColumns->GetText(pos, 2);
		if (!nullsOrder.IsEmpty())
			sql += wxT(" NULLS ") + nullsOrder;

		wxString oper = lstColumns->GetText(pos, 4);
		if (!oper.IsEmpty())
			sql += wxT(" WITH ") + oper;
	}
	return sql;
}


int dlgIndexConstraint::Go(bool modal)
{
	pgSet *set;

	PrepareTablespace(cbTablespace);

	if (wxString(factory->GetTypeName()).Upper() == wxT("EXCLUDE"))
	{
		cbIndex->Disable();
	}
	else
	{
		cbIndex->Enable(connection->BackendMinimumVersion(9, 1));
		cbType->Disable();
		txtWhere->Disable();
		cbOpClass->Disable();
		chkDesc->Disable();
		rdbNullsFirst->Disable();
		rdbNullsLast->Disable();
		cbOperator->Disable();
	}

	if (index)
	{
		pgIndexConstraint *idc = (pgIndexConstraint *)index;

		// We only display the column options (ASC/DESC, NULLS FIRST/LAST)
		// on exclude constraints with btree
		wxArrayString colsArr = index->GetColumnList();
		wxString colDef, colRest, descDef, nullsDef, opclassDef, withDef;
		const wxString firstOrder = wxT(" NULLS FIRST"), lastOrder = wxT(" NULLS LAST");
		const wxString descOrder = wxT(" DESC");
		if (wxString(factory->GetTypeName()).Upper() == wxT("EXCLUDE") && index->GetIndexType() == wxT("btree"))
		{
			for (int colIdx = 0, colsCount = colsArr.Count(); colIdx < colsCount; colIdx++)
			{
				colDef = colsArr.Item(colIdx);

				int withStartPoint = colDef.Find(wxT(" WITH "));
				if (withStartPoint > 0)
				{
					withDef = colDef.Mid(withStartPoint + 6, colDef.Length() - withStartPoint - 6);
					colDef = colDef.Mid(0, withStartPoint);
				}
				else
					withDef = wxT("");

				if (colDef.EndsWith(firstOrder.GetData(), &colRest))
				{
					colDef = colRest;
					nullsDef = wxT("FIRST");
				}
				else if (colDef.EndsWith(lastOrder.GetData(), &colRest))
				{
					colDef = colRest;
					nullsDef = wxT("LAST");
				}
				else
					nullsDef = wxT("");

				if (colDef.EndsWith(descOrder.GetData(), &colRest))
				{
					colDef = colRest;
					descDef = wxT("DESC");
					if (nullsDef.IsEmpty())
						nullsDef = wxT("FIRST");
				}
				else
				{
					descDef = wxT("ASC");
					if (nullsDef.IsEmpty())
						nullsDef = wxT("LAST");
				}

				int pos = colDef.First(wxT(" "));
				if (pos > 0)
				{
					opclassDef = colDef.Mid(pos + 1);
					colDef = colDef.Mid(0, pos);
				}
				else
					opclassDef = wxEmptyString;

				lstColumns->InsertItem(colIdx, colDef, columnFactory.GetIconId());
				lstColumns->SetItem(colIdx, 1, descDef);
				lstColumns->SetItem(colIdx, 2, nullsDef);
				lstColumns->SetItem(colIdx, 3, opclassDef);
				lstColumns->SetItem(colIdx, 4, withDef);
			}
		}
		else
		{
			for (int colIdx = 0, colsCount = colsArr.Count(); colIdx < colsCount; colIdx++)
			{
				colDef = colsArr.Item(colIdx);

				int withStartPoint = colDef.Find(wxT(" WITH "));
				if (withStartPoint > 0)
				{
					withDef = colDef.Mid(withStartPoint + 6, colDef.Length() - withStartPoint - 6);
					colDef = colDef.Mid(0, withStartPoint);
				}
				else
					withDef = wxT("");

				int pos = colDef.First(wxT(" "));
				if (pos > 0)
				{
					opclassDef = colDef.Mid(pos + 1);
					colDef = colDef.Mid(0, pos);
				}
				else
					opclassDef = wxEmptyString;

				lstColumns->InsertItem(colIdx, colDef, columnFactory.GetIconId());
				lstColumns->SetItem(colIdx, 3, cbOpClass->GetValue());
				lstColumns->SetItem(colIdx, 4, withDef);
			}
		}

		if (idc->GetTablespaceOid() != 0)
			cbTablespace->SetKey(idc->GetTablespaceOid());
		cbTablespace->Enable(connection->BackendMinimumVersion(8, 0));

		if (txtFillFactor)
		{
			txtFillFactor->SetValue(idc->GetFillFactor());
		}

		if (index->GetIndexType().Length() > 0)
		{
			cbType->Append(index->GetIndexType());
			cbType->SetSelection(0);
			cbType->Disable();
		}

		chkDeferrable->SetValue(index->GetDeferrable());
		chkDeferred->SetValue(index->GetDeferred());
		chkDeferrable->Enable(false);
		chkDeferred->Enable(false);
	}
	else
	{
		txtComment->Disable();
		if (!table)
		{
			cbClusterSet->Disable();
			cbClusterSet = 0;
		}

		// Add the indexes
		if (table)
		{
			cbIndex->Append(wxT(""));
			set = connection->ExecuteSet(
			          wxT("SELECT relname FROM pg_class, pg_index WHERE pg_class.oid=indexrelid AND indrelid=") + table->GetOidStr());
			if (set)
			{
				while (!set->Eof())
				{
					cbIndex->Append(set->GetVal(0));
					set->MoveNext();
				}
				delete set;
			}
		}
		else
		{
			cbIndex->Disable();
		}

		// Add the default tablespace
		cbTablespace->Insert(_("<default tablespace>"), 0, (void *)0);
		cbTablespace->SetSelection(0);

		cbType->Append(wxT(""));
		set = connection->ExecuteSet(
		          wxT("SELECT oid, amname FROM pg_am ")
		          wxT("WHERE EXISTS (SELECT 1 FROM pg_proc WHERE oid=amgettuple) ")
		          wxT("ORDER BY amname"));
		if (set)
		{
			while (!set->Eof())
			{
				cbType->Append(set->GetVal(1), set->GetVal(0));
				set->MoveNext();
			}
			delete set;
		}

		chkDeferrable->Enable(connection->BackendMinimumVersion(9, 0));
		chkDeferred->Enable(connection->BackendMinimumVersion(9, 0));
	}

	txtFillFactor->SetValidator(numericValidator);
	if (connection->BackendMinimumVersion(8, 2))
		txtFillFactor->Enable();
	else
		txtFillFactor->Disable();

	return dlgIndexBase::Go(modal);
}


void dlgIndexConstraint::OnAddCol(wxCommandEvent &ev)
{
	wxString colName = cbColumns->GetValue();

	if (!colName.IsEmpty())
	{
		long colIndex = lstColumns->InsertItem(lstColumns->GetItemCount(), colName, columnFactory.GetIconId());

		if (chkDesc->GetValue())
		{
			if (chkDesc->IsEnabled())
				lstColumns->SetItem(colIndex, 1, wxT("DESC"));


			if (rdbNullsLast->GetValue())
			{
				if (rdbNullsLast->IsEnabled())
					lstColumns->SetItem(colIndex, 2, wxT("LAST"));
			}
			else
			{
				if (rdbNullsLast->IsEnabled())
					lstColumns->SetItem(colIndex, 2, wxT("FIRST"));
			}
		}
		else
		{
			if (chkDesc->IsEnabled())
				lstColumns->SetItem(colIndex, 1, wxT("ASC"));

			if (rdbNullsFirst->GetValue())
			{
				if (rdbNullsFirst->IsEnabled())
					lstColumns->SetItem(colIndex, 2, wxT("FIRST"));
			}
			else
			{
				if (rdbNullsLast->IsEnabled())
					lstColumns->SetItem(colIndex, 2, wxT("LAST"));
			}
		}

		lstColumns->SetItem(colIndex, 3, cbOpClass->GetValue());
		lstColumns->SetItem(colIndex, 4, cbOperator->GetValue());

		cbColumns->Delete(cbColumns->GetCurrentSelection());
		if (cbColumns->GetCount())
			cbColumns->SetSelection(0);

		CheckChange();
		if (!cbColumns->GetCount())
			btnAddCol->Disable();
	}
}


void dlgIndexConstraint::OnRemoveCol(wxCommandEvent &ev)
{
	long pos = lstColumns->GetSelection();
	if (pos >= 0)
	{
		wxString colName = lstColumns->GetItemText(pos);

		lstColumns->DeleteItem(pos);
		cbColumns->Append(colName);

		CheckChange();
		btnRemoveCol->Disable();
	}
}

#ifdef __WXMAC__
void dlgIndexConstraint::OnChangeSize(wxSizeEvent &ev)
{
	lstColumns->SetSize(wxDefaultCoord, wxDefaultCoord,
	                    ev.GetSize().GetWidth(), ev.GetSize().GetHeight() - 350);
	if (GetAutoLayout())
	{
		Layout();
	}
}
#endif


void dlgIndexConstraint::OnSelectComboCol(wxCommandEvent &ev)
{
	cbOperator->Clear();
	cbOperator->Append(wxT(""));

	if (cbColumns->GetValue().Length() > 0)
	{
		pgSet *set = connection->ExecuteSet(
		                 wxT("SELECT DISTINCT oprname FROM pg_operator \n")
		                 wxT("WHERE (")
		                 wxT("    oprleft=") + NumToStr(cbColumns->GetOIDKey(cbColumns->GetCurrentSelection())) +
		                 wxT(" OR oprright=") + NumToStr(cbColumns->GetOIDKey(cbColumns->GetCurrentSelection())) +
		                 wxT(") AND oprcom > 0 \n")
		                 wxT("ORDER BY oprname"));
		if (set)
		{
			while (!set->Eof())
			{
				cbOperator->Append(set->GetVal(0));
				set->MoveNext();
			}
			delete set;
		}
	}

	dlgIndexBase::OnSelectComboCol(ev);
}


void dlgIndexConstraint::OnChangeIndex(wxCommandEvent &ev)
{
	bool indexselected = cbIndex->GetCurrentSelection() > 0;
	bool btreeindex = cbType->GetValue() == wxT("btree") || cbType->GetValue() == wxEmptyString;
	bool excludeconstraint = wxString(factory->GetTypeName()).Upper() == wxT("EXCLUDE");

	cbTablespace->Enable(!indexselected && connection->BackendMinimumVersion(8, 0));
	cbType->Enable(!indexselected && excludeconstraint);
	txtFillFactor->Enable(!indexselected && connection->BackendMinimumVersion(8, 2));
	txtWhere->Enable(!indexselected && excludeconstraint);
	chkDeferrable->Enable(!indexselected && connection->BackendMinimumVersion(9, 0));
	chkDeferred->Enable(!indexselected && connection->BackendMinimumVersion(9, 0));
	cbOpClass->Enable(!indexselected && excludeconstraint && btreeindex);
	chkDesc->Enable(!indexselected && excludeconstraint && btreeindex);
	rdbNullsFirst->Enable(!indexselected && excludeconstraint && btreeindex);
	rdbNullsLast->Enable(!indexselected && excludeconstraint && btreeindex);
	cbOperator->Enable(!indexselected && excludeconstraint);
	cbColumns->Enable(!indexselected);
	btnAddCol->Enable(!indexselected);
	btnRemoveCol->Enable(!indexselected);
}


void dlgIndexConstraint::OnSelectType(wxCommandEvent &ev)
{
	// The column options available change depending on the
	// index type. We need to clear the column list, and
	// setup some of the other controls accordingly.

	wxString newType = cbType->GetValue();
	bool changingDefault = false;

	// Detect if we're changing between default and btree (which are the same) to
	// avoid annoying the user needlessly.
	if ((m_previousType == wxEmptyString && cbType->GetValue() == wxT("btree")) ||
	        (m_previousType == wxT("btree") && cbType->GetValue() == wxEmptyString))
		changingDefault = true;

	if (lstColumns->GetItemCount() > 0 && !changingDefault)
	{
		if (wxMessageBox(_("Changing the index type will cause the column list to be cleared. Do you wish to continue?"), _("Change index type?"), wxYES_NO) != wxYES)
		{
			cbType->SetValue(m_previousType);
			return;
		}

		// Move all the columns back to the combo
		for (int pos = lstColumns->GetItemCount(); pos > 0; pos--)
		{
			wxString colName = lstColumns->GetItemText(pos - 1);

			lstColumns->DeleteItem(pos - 1);
			cbColumns->Append(colName);
		}
	}

	if (newType == wxT("btree") || newType == wxEmptyString)
	{
		cbOpClass->Enable(true);
		chkDesc->Enable(true);
		rdbNullsFirst->Enable(true);
		rdbNullsLast->Enable(true);
	}
	else
	{
		cbOpClass->Enable(false);
		chkDesc->Enable(false);
		rdbNullsFirst->Enable(false);
		rdbNullsLast->Enable(false);
	}

	// Make a note of the type so we can compare if it changes again.
	m_previousType = cbType->GetValue();
}


void dlgIndexConstraint::CheckChange()
{
	if (cbIndex->GetCurrentSelection() > 0)
		EnableOK(true);
	else
		dlgIndexBase::CheckChange();
}


wxString dlgIndexConstraint::GetDefinition()
{
	wxString sql = wxEmptyString;

	if (cbIndex->GetCurrentSelection() > 0)
	{
		sql += wxT(" USING INDEX ") + qtIdent(cbIndex->GetValue());
	}
	else
	{
		if (cbType->GetCurrentSelection() > 0)
			AppendIfFilled(sql, wxT(" USING "), cbType->GetValue());

		sql += wxT("(") + GetColumns() + wxT(")");

		if (txtFillFactor)
		{
			if (connection->BackendMinimumVersion(8, 2) && txtFillFactor->GetValue().Length() > 0)
				sql += wxT("\n  WITH (FILLFACTOR=") + txtFillFactor->GetValue() + wxT(")");
		}

		if (cbTablespace->GetOIDKey() > 0)
			sql += wxT(" USING INDEX TABLESPACE ") + qtIdent(cbTablespace->GetValue());

		if (chkDeferrable->GetValue())
		{
			sql += wxT(" DEFERRABLE");
			if (chkDeferred->GetValue())
				sql += wxT(" INITIALLY DEFERRED");
		}

		if (txtWhere->GetValue().Length() > 0)
			sql += wxT(" WHERE (") + txtWhere->GetValue() + wxT(")");
	}

	return sql;
}


wxString dlgIndexConstraint::GetSql()
{
	wxString sql;
	wxString name = GetName();

	if (!index)
	{
		sql = wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
		      + wxT("\n  ADD");
		AppendIfFilled(sql, wxT(" CONSTRAINT "), qtIdent(name));

		sql += wxT(" ") + wxString(factory->GetTypeName()).Upper() + wxT(" ") + GetDefinition()
		       + wxT(";\n");
	}
	else
	{
		if (index->GetName() != name)
		{
			sql = wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
			      + wxT("\n  RENAME CONSTRAINT ") + qtIdent(index->GetName())
			      + wxT(" TO ") + qtIdent(name) + wxT(";\n");
		}
		if (connection->BackendMinimumVersion(8, 0) && cbTablespace->GetOIDKey() != index->GetTablespaceOid())
		{
			sql += wxT("ALTER INDEX ") + index->GetSchema()->GetQuotedIdentifier() + wxT(".") + qtIdent(name)
			       +  wxT("\n  SET TABLESPACE ") + qtIdent(cbTablespace->GetValue())
			       + wxT(";\n");
		}

		if (txtFillFactor->GetValue().Trim().Length() > 0 && txtFillFactor->GetValue() != index->GetFillFactor())
		{
			sql += wxT("ALTER INDEX ") + index->GetSchema()->GetQuotedIdentifier() + wxT(".") + qtIdent(name)
			       +  wxT("\n  SET (FILLFACTOR=")
			       +  txtFillFactor->GetValue() + wxT(");\n");
		}
	}

	if (!name.IsEmpty())
		AppendComment(sql, wxT("CONSTRAINT ") + qtIdent(name)
		              + wxT(" ON ") + table->GetQuotedFullIdentifier(), index);

	return sql;
}



dlgProperty *pgPrimaryKeyFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgPrimaryKey(this, frame, (pgPrimaryKey *)node, (pgTable *)parent);
}




dlgPrimaryKey::dlgPrimaryKey(pgaFactory *f, frmMain *frame, pgPrimaryKey *index, pgTable *parentNode)
	: dlgIndexConstraint(f, frame, wxT("dlgIndexConstraint"), index, parentNode)
{
}


dlgPrimaryKey::dlgPrimaryKey(pgaFactory *f, frmMain *frame, ctlListView *colList)
	: dlgIndexConstraint(f, frame, wxT("dlgIndexConstraint"), colList)
{
}


pgObject *dlgPrimaryKey::CreateObject(pgCollection *collection)
{
	wxString name = GetName();
	if (name.IsEmpty())
		return 0;

	pgObject *obj = primaryKeyFactory.CreateObjects(collection, 0, wxT(
	                    "\n   AND cls.relname=") + qtDbString(name) + wxT(
	                    "\n   AND cls.relnamespace=") + table->GetSchema()->GetOidStr());

	return obj;
}



dlgProperty *pgUniqueFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgUnique(this, frame, (pgUnique *)node, (pgTable *)parent);
}


dlgUnique::dlgUnique(pgaFactory *f, frmMain *frame, pgUnique *index, pgTable *parentNode)
	: dlgIndexConstraint(f, frame, wxT("dlgIndexConstraint"), index, parentNode)
{
}


dlgUnique::dlgUnique(pgaFactory *f, frmMain *frame, ctlListView *colList)
	: dlgIndexConstraint(f, frame, wxT("dlgIndexConstraint"), colList)
{
}


pgObject *dlgUnique::CreateObject(pgCollection *collection)
{
	wxString name = GetName();

	pgObject *obj = uniqueFactory.CreateObjects(collection, 0, wxT(
	                    "\n   AND cls.relname=") + qtDbString(name) + wxT(
	                    "\n   AND cls.relnamespace=") + table->GetSchema()->GetOidStr());
	return obj;
}


dlgProperty *pgExcludeFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgExclude(this, frame, (pgExclude *)node, (pgTable *)parent);
}


dlgExclude::dlgExclude(pgaFactory *f, frmMain *frame, pgExclude *index, pgTable *parentNode)
	: dlgIndexConstraint(f, frame, wxT("dlgIndexConstraint"), index, parentNode)
{
}


dlgExclude::dlgExclude(pgaFactory *f, frmMain *frame, ctlListView *colList)
	: dlgIndexConstraint(f, frame, wxT("dlgIndexConstraint"), colList)
{
}


pgObject *dlgExclude::CreateObject(pgCollection *collection)
{
	wxString name = GetName();

	pgObject *obj = excludeFactory.CreateObjects(collection, 0, wxT(
	                    "\n   AND cls.relname=") + qtDbString(name) + wxT(
	                    "\n   AND cls.relnamespace=") + table->GetSchema()->GetOidStr());
	return obj;
}
