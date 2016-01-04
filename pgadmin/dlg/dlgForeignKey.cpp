//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgForeignKey.cpp - PostgreSQL Foreign Key Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "frm/frmMain.h"
#include "frm/frmHint.h"
#include "schema/pgTable.h"
#include "schema/pgForeignKey.h"
#include "dlg/dlgForeignKey.h"
#include "schema/pgColumn.h"


#define chkDeferrable   CTRL_CHECKBOX("chkDeferrable")
#define chkDeferred     CTRL_CHECKBOX("chkDeferred")
#define cbReferences    CTRL_COMBOBOX("cbReferences")
#define chkMatchFull    CTRL_CHECKBOX("chkMatchFull")
#define chkDontValidate CTRL_CHECKBOX("chkDontValidate")

#define chkAutoIndex    CTRL_CHECKBOX("chkAutoIndex")
#define txtIndexName    CTRL_TEXT("txtIndexName")

#define cbRefColumns    CTRL_COMBOBOX("cbRefColumns")
#define btnAddRef       CTRL_BUTTON("btnAddRef")
#define btnRemoveRef    CTRL_BUTTON("btnRemoveRef")

#define rbOnUpdate      CTRL_RADIOBOX("rbOnUpdate")
#define rbOnDelete      CTRL_RADIOBOX("rbOnDelete")


BEGIN_EVENT_TABLE(dlgForeignKey, dlgProperty)
	EVT_CHECKBOX(XRCID("chkDeferrable"),        dlgProperty::OnChange)
	EVT_CHECKBOX(XRCID("chkAutoIndex") ,        dlgProperty::OnChange)
	EVT_TEXT(XRCID("txtIndexName"),             dlgProperty::OnChange)

	EVT_CHECKBOX(XRCID("chkDontValidate"),      dlgForeignKey::OnChangeValidate)
	EVT_LIST_ITEM_SELECTED(XRCID("lstColumns"), dlgForeignKey::OnSelChangeCol)
	EVT_TEXT(XRCID("cbReferences"),             dlgForeignKey::OnSelChangeRef)
	EVT_COMBOBOX(XRCID("cbReferences"),         dlgForeignKey::OnSelChangeRef)
	EVT_TEXT(XRCID("cbColumns"),                dlgForeignKey::OnSelChangeRefCol)
	EVT_COMBOBOX(XRCID("cbColumns"),            dlgForeignKey::OnSelChangeRefCol)
	EVT_TEXT(XRCID("cbRefColumns"),             dlgForeignKey::OnSelChangeRefCol)
	EVT_COMBOBOX(XRCID("cbRefColumns"),         dlgForeignKey::OnSelChangeRefCol)
	EVT_BUTTON(XRCID("btnAddRef"),              dlgForeignKey::OnAddRef)
	EVT_BUTTON(XRCID("btnRemoveRef"),           dlgForeignKey::OnRemoveRef)
	EVT_BUTTON(wxID_OK,                         dlgForeignKey::OnOK)
#ifdef __WXMAC__
	EVT_SIZE(                                   dlgForeignKey::OnChangeSize)
#endif
END_EVENT_TABLE();


dlgProperty *pgForeignKeyFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgForeignKey(this, frame, (pgForeignKey *)node, (pgTable *)parent);
}


dlgForeignKey::dlgForeignKey(pgaFactory *f, frmMain *frame, pgForeignKey *node, pgTable *parentNode)
	: dlgCollistProperty(f, frame, wxT("dlgForeignKey"), parentNode)
{
	foreignKey = node;
}


dlgForeignKey::dlgForeignKey(pgaFactory *f, frmMain *frame, ctlListView *colList)
	: dlgCollistProperty(f, frame, wxT("dlgForeignKey"), colList)
{
	foreignKey = 0;
}


wxString dlgForeignKey::DefaultIndexName(const wxString &name)
{
	if (name.IsEmpty())
		return wxEmptyString;

	if (name.Left(3) == wxT("fk_"))
		return wxT("fki_") + name.Mid(3);
	else if (name.Left(3) == wxT("FK_"))
		return wxT("FKI_") + name.Mid(3);
	else
		return wxT("fki_") + name;
}


void dlgForeignKey::OnOK(wxCommandEvent &ev)
{
	if (chkAutoIndex->IsEnabled() && !chkAutoIndex->GetValue()
	        && frmHint::ShowHint(this, HINT_FKINDEX) == wxID_CANCEL)
		return;

	dlgProperty::OnOK(ev);
}


#ifdef __WXMAC__
void dlgForeignKey::OnChangeSize(wxSizeEvent &ev)
{
	lstColumns->SetSize(wxDefaultCoord, wxDefaultCoord,
	                    ev.GetSize().GetWidth(), ev.GetSize().GetHeight() - 450);
	if (GetAutoLayout())
	{
		Layout();
	}
}
#endif


void dlgForeignKey::CheckChange()
{
	if (processing)
		return;

	processing = true;

	wxString name = GetName();

	wxString cols;
	int pos;
	for (pos = 0 ; pos < lstColumns->GetItemCount() ; pos++)
	{
		if (pos)
			cols += wxT(", ");
		cols += qtIdent(lstColumns->GetText(pos));
	}

	bool canDef = chkDeferrable->GetValue();
	if (!canDef)
		chkDeferred->SetValue(false);
	chkDeferred->Enable(canDef);

	txtIndexName->Enable(table && chkAutoIndex->GetValue());

	wxString coveringIndex;
	if (table)
	{
		coveringIndex = table->GetCoveringIndex(mainForm->GetBrowser(), cols);

		if (coveringIndex.IsEmpty())
		{
			if (!chkAutoIndex->IsEnabled())
			{
				chkAutoIndex->Enable();
				chkAutoIndex->SetValue(true);
				txtIndexName->Enable();
				txtIndexName->SetValue(savedIndexName);
			}

			wxString idxName = txtIndexName->GetValue().Strip(wxString::both);

			if (name != savedFKName || idxName == savedIndexName)
			{
				if (idxName.IsEmpty() || idxName == DefaultIndexName(savedFKName))
				{
					idxName = DefaultIndexName(name);
					txtIndexName->SetValue(idxName);
				}
			}
			savedIndexName = idxName;
		}
		else
		{
			if (chkAutoIndex->IsEnabled())
				savedIndexName = txtIndexName->GetValue();

			txtIndexName->SetValue(coveringIndex);
			chkAutoIndex->SetValue(false);

			txtIndexName->Disable();
			chkAutoIndex->Disable();
		}
	}

	savedFKName = name;
	processing = false;

	if (foreignKey)
	{
		bool enable = true;
		if (chkAutoIndex->GetValue())
		{
			CheckValid(enable, !txtIndexName->GetValue().IsEmpty(),
			           _("Please specify covering index name."));
		}
		else
			enable = txtName->GetValue() != foreignKey->GetName() || txtComment->GetValue() != foreignKey->GetComment();

		if (connection->BackendMinimumVersion(9, 1) && !foreignKey->GetValid() && !chkDontValidate->GetValue())
			enable = true;

		EnableOK(enable);
	}
	else
	{
		bool enable = true;
		txtComment->Enable(!name.IsEmpty());
		CheckValid(enable, lstColumns->GetItemCount() > 0, _("Please specify columns."));
		CheckValid(enable, !chkAutoIndex->GetValue() || !txtIndexName->GetValue().IsEmpty(),
		           _("Please specify covering index name."));
		EnableOK(enable);
	}

}


void dlgForeignKey::OnChangeValidate(wxCommandEvent &ev)
{
	CheckChange();
}


void dlgForeignKey::OnSelChangeCol(wxListEvent &ev)
{
	btnRemoveRef->Enable();
}


void dlgForeignKey::OnSelChangeRefCol(wxCommandEvent &ev)
{
	btnAddRef->Enable(cbColumns->GetCurrentSelection() >= 0 && cbRefColumns->GetCurrentSelection() >= 0);
}


void dlgForeignKey::OnSelChangeRef(wxCommandEvent &ev)
{
	cbRefColumns->Clear();

	wxString tab = cbReferences->GetValue();
	wxString nsp;
	if (tab.Find('.') >= 0)
	{
		nsp = tab.BeforeFirst('.');
		tab = tab.AfterFirst('.');
	}
	else
		nsp = database->GetDefaultSchema();

	pgSet *set = connection->ExecuteSet(
	                 wxT("SELECT attname\n")
	                 wxT("  FROM pg_attribute att, pg_class cl, pg_namespace nsp\n")
	                 wxT(" WHERE attrelid=cl.oid AND relnamespace=nsp.oid\n")
	                 wxT("   AND nspname=") + qtDbString(nsp) +
	                 wxT("\n   AND relname=") + qtDbString(tab) +
	                 wxT("\n   AND attnum > 0\n")
	                 wxT("\n   AND NOT attisdropped\n")
	                 wxT("\n ORDER BY attnum"));
	if (set)
	{
		while (!set->Eof())
		{
			cbRefColumns->Append(set->GetVal(0));
			set->MoveNext();
		}
		delete set;

		if (cbRefColumns->GetCount())
			cbRefColumns->SetSelection(0);
	}

	OnSelChangeRefCol(ev);
}


void dlgForeignKey::OnAddRef(wxCommandEvent &ev)
{
	wxString col = cbColumns->GetValue();
	wxString ref = cbRefColumns->GetValue();
	if (!col.IsEmpty() && !ref.IsEmpty())
	{
		lstColumns->AppendItem(columnFactory.GetIconId(), col, ref);
		cbColumns->Delete(cbColumns->GetCurrentSelection());
		cbRefColumns->Delete(cbRefColumns->GetCurrentSelection());
		cbReferences->Disable();

		if (cbColumns->GetCount())
			cbColumns->SetSelection(0);

		if (cbRefColumns->GetCount())
			cbRefColumns->SetSelection(0);

		OnSelChangeRefCol(ev);
		CheckChange();
	}
}


void dlgForeignKey::OnRemoveRef(wxCommandEvent &ev)
{
	long pos = lstColumns->GetSelection();

	if (pos >= 0)
	{
		wxString col = lstColumns->GetText(pos);
		wxString ref = lstColumns->GetText(pos, 1);
		cbColumns->Append(col);
		cbRefColumns->Append(ref);

		lstColumns->DeleteItem(pos);
		cbReferences->Enable(lstColumns->GetItemCount() == 0);
		btnRemoveRef->Disable();
	}
}


pgObject *dlgForeignKey::GetObject()
{
	return foreignKey;
}


pgObject *dlgForeignKey::CreateObject(pgCollection *collection)
{
	wxString name = GetName();
	if (name.IsEmpty())
		return 0;

	pgObject *obj = foreignKeyFactory.CreateObjects(collection, 0, wxT(
	                    "\n   AND conname=") + qtDbString(name) + wxT(
	                    "\n   AND cl.relnamespace=") + table->GetSchema()->GetOidStr());
	return obj;
}


int dlgForeignKey::Go(bool modal)
{
	lstColumns->CreateColumns(0, _("Local"), _("Referenced"), -1);

	processing = true;  // protect from OnChange execution

	btnAddRef->Disable();
	btnRemoveRef->Disable();

	if (readOnly)
	{
		chkAutoIndex->Disable();
		txtIndexName->Disable();
	}


	if (foreignKey)
	{
		// edit mode
		txtName->Enable(connection->BackendMinimumVersion(9, 2));
		cbReferences->Append(foreignKey->GetReferences());
		cbReferences->SetValue(foreignKey->GetReferences());
		cbReferences->Disable();

		chkDeferrable->SetValue(foreignKey->GetDeferrable());
		chkDeferred->SetValue(foreignKey->GetDeferred());
		chkMatchFull->SetValue(foreignKey->GetMatch() == wxT("FULL"));
		if (connection->BackendMinimumVersion(9, 1))
			chkDontValidate->SetValue(!foreignKey->GetValid());
		chkDeferrable->Disable();
		chkDeferred->Disable();
		chkMatchFull->Disable();
		chkDontValidate->Enable(connection->BackendMinimumVersion(9, 1));
		if(!connection->BackendMinimumVersion(9, 1))
		{
			chkDontValidate->SetValue(true);
		}
		rbOnUpdate->SetStringSelection(foreignKey->GetOnUpdate());
		rbOnDelete->SetStringSelection(foreignKey->GetOnDelete());
		rbOnUpdate->Disable();
		rbOnDelete->Disable();

		chkAutoIndex->SetValue(false);
		txtIndexName->SetValue(foreignKey->GetCoveringIndex());
		if (!txtIndexName->GetValue().IsEmpty())
		{
			chkAutoIndex->Disable();
			txtIndexName->Disable();
		}

		btnAddRef->Disable();
		btnRemoveRef->Disable();
		cbColumns->Disable();
		cbRefColumns->Disable();

		int pos = 0;
		wxStringTokenizer cols(foreignKey->GetFkColumns(), wxT(","));
		wxStringTokenizer refs(foreignKey->GetRefColumns(), wxT(","));
		while (cols.HasMoreTokens())
		{
			wxString col = cols.GetNextToken().Trim(false).Trim(true);
			wxString ref = refs.GetNextToken().Trim(false).Trim(true);
			if (pos++)
			{
				if (col.Last() == ',')
					col.RemoveLast();
				if (ref.Last() == ',')
					ref.RemoveLast();
			}
			lstColumns->AppendItem(columnFactory.GetIconId(), col, ref);
		}
	}
	else
	{
		// create mode
		txtComment->Disable();

		chkDontValidate->Enable(connection->BackendMinimumVersion(9, 1));

		wxString systemRestriction;
		if (!settings->GetShowSystemObjects())
			systemRestriction = wxT("   AND ") + connection->SystemNamespaceRestriction(wxT("nsp.nspname"));

		wxString sql =  wxT("SELECT nspname, relname FROM pg_namespace nsp, pg_class cl\n")
		                wxT(" WHERE relnamespace=nsp.oid AND relkind='r'\n");

		if (connection->BackendMinimumVersion(8, 1))
			sql += wxT("   AND nsp.nspname NOT LIKE E'pg\\_temp\\_%'\n");
		else
			sql += wxT("   AND nsp.nspname NOT LIKE 'pg\\_temp\\_%'\n");

		sql += systemRestriction +
		       wxT(" ORDER BY nspname, relname");

		pgSet *set = connection->ExecuteSet(sql);

		if (set)
		{
			while (!set->Eof())
			{
				cbReferences->Append(database->GetSchemaPrefix(set->GetVal(0)) + set->GetVal(1));
				set->MoveNext();
			}
			delete set;
			cbReferences->SetSelection(0);
		}
		if (!table)
		{
			chkAutoIndex->Disable();
			chkAutoIndex->SetValue(false);
			txtIndexName->Disable();
			cbClusterSet->Disable();
			cbClusterSet = 0;
		}
	}

	processing = false;

	// Reset the labels as the XRC defined values will have been localised :-(

	rbOnUpdate->SetString(0, wxT("NO ACTION"));
	rbOnUpdate->SetString(1, wxT("RESTRICT"));
	rbOnUpdate->SetString(2, wxT("CASCADE"));
	rbOnUpdate->SetString(3, wxT("SET NULL"));
	rbOnUpdate->SetString(4, wxT("SET DEFAULT"));

	rbOnDelete->SetString(0, wxT("NO ACTION"));
	rbOnDelete->SetString(1, wxT("RESTRICT"));
	rbOnDelete->SetString(2, wxT("CASCADE"));
	rbOnDelete->SetString(3, wxT("SET NULL"));
	rbOnDelete->SetString(4, wxT("SET DEFAULT"));

	wxCommandEvent nullEvent;
	OnSelChangeRef(nullEvent);

	return dlgCollistProperty::Go(modal);
}


wxString dlgForeignKey::GetSql()
{
	wxString sql;
	wxString name = GetName();

	if (!foreignKey)
	{
		sql = wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
		      + wxT("\n  ADD");
		AppendIfFilled(sql, wxT(" CONSTRAINT "), qtIdent(name));
		sql += wxT(" FOREIGN KEY ") + GetDefinition()
		       + wxT(";\n");
	}
	else
	{
		if (foreignKey->GetName() != name)
		{
			sql = wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
			      + wxT("\n  RENAME CONSTRAINT ") + qtIdent(foreignKey->GetName())
			      + wxT(" TO ") + qtIdent(name) + wxT(";\n");
		}
		if (connection->BackendMinimumVersion(9, 1) && !foreignKey->GetValid() && !chkDontValidate->GetValue())
		{
			sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
			       + wxT("\n  VALIDATE CONSTRAINT ") + qtIdent(name) + wxT(";\n");
		}
	}

	if (!name.IsEmpty())
		AppendComment(sql, wxT("CONSTRAINT ") + qtIdent(name)
		              + wxT(" ON ") + table->GetQuotedFullIdentifier(), foreignKey);

	if (chkAutoIndex->GetValue())
	{
		sql += wxT("CREATE INDEX ") + qtIdent(txtIndexName->GetValue())
		       +  wxT("\n  ON ") + table->GetQuotedFullIdentifier()
		       +  wxT("(");

		int pos;
		for (pos = 0 ; pos < lstColumns->GetItemCount() ; pos++)
		{
			if (pos)
				sql += wxT(", ");

			sql += qtIdent(lstColumns->GetText(pos));
		}

		sql += wxT(");\n");
	}
	return sql;
}


wxString dlgForeignKey::GetDefinition()
{
	wxString sql;
	wxString cols, refs;

	int pos;

	for (pos = 0 ; pos < lstColumns->GetItemCount() ; pos++)
	{
		if (pos)
		{
			cols += wxT(", ");
			refs += wxT(", ");
		}
		cols += qtIdent(lstColumns->GetText(pos));
		refs += qtIdent(lstColumns->GetText(pos, 1));
	}

	sql = wxT("(") + cols
	      + wxT(") REFERENCES ");
	AppendQuoted(sql, cbReferences->GetValue());
	sql += wxT(" (") + refs
	       + wxT(")");

	if (chkMatchFull->GetValue())
		sql += wxT(" MATCH FULL");

	sql += wxT("\n  ")
	       wxT(" ON UPDATE ") + rbOnUpdate->GetStringSelection() +
	       wxT(" ON DELETE ") + rbOnDelete->GetStringSelection();

	if (chkDeferrable->GetValue())
		sql += wxT("\n   DEFERRABLE");
	if (chkDeferred->GetValue())
		sql += wxT(" INITIALLY DEFERRED");

	if (chkDontValidate->GetValue())
		sql += wxT("\n   NOT VALID");

	return sql;
}
