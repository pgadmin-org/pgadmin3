//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgRepTable.cpp - PostgreSQL Slony-I Table Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"

#include "slony/dlgRepTable.h"
#include "slony/slCluster.h"
#include "slony/slSet.h"
#include "slony/slTable.h"
#include "schema/pgDatatype.h"


// pointer to controls
#define txtID               CTRL_TEXT("txtID")
#define cbTable             CTRL_COMBOBOX2("cbTable")
#define cbIndex             CTRL_COMBOBOX("cbIndex")
#define chkTrigger          CTRL_CHECKLISTBOX("chkTrigger")



BEGIN_EVENT_TABLE(dlgRepTable, dlgRepProperty)
	EVT_TEXT(XRCID("txtID"),                dlgRepTable::OnChange)
	EVT_COMBOBOX(XRCID("cbTable"),          dlgRepTable::OnChangeTableSel)
	EVT_TEXT(XRCID("cbTable"),              dlgRepTable::OnChangeTable)
	EVT_COMBOBOX(XRCID("cbIndex"),          dlgRepTable::OnChange)
	EVT_CHECKLISTBOX(XRCID("chkTrigger"),   dlgRepTable::OnChange)
END_EVENT_TABLE();


dlgProperty *slSlTableFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgRepTable(this, frame, (slTable *)node, (slSet *)parent);
}


dlgRepTable::dlgRepTable(pgaFactory *f, frmMain *frame, slTable *node, slSet *s)
	: dlgRepProperty(f, frame, s->GetCluster(), wxT("dlgRepTable"))
{
	table = node;
	set = s;
}


pgObject *dlgRepTable::GetObject()
{
	return table;
}


int dlgRepTable::Go(bool modal)
{
	txtID->SetValidator(numericValidator);

	if (table)
	{
		// edit mode
		cbTable->Append(table->GetName(), (void *)0);
		cbTable->SetSelection(0);
		cbIndex->Append(table->GetIndexName());
		cbIndex->SetSelection(0);

		txtID->SetValue(NumToStr(table->GetSlId()));

		cbTable->Disable();
		cbIndex->Disable();
		txtComment->Disable();
		txtID->Disable();

		LoadTrigger(table->GetOid());

		size_t i;
		for (i = 0 ; i < table->GetTriggers().GetCount() ; i++)
		{
			int sel = chkTrigger->FindString(table->GetTriggers()[i]);
			if (sel >= 0)
				chkTrigger->Check(sel);
		}
	}
	else
	{
		// create mode

		wxString restriction;
		if (!settings->GetShowSystemObjects())
			restriction = wxT("\n   AND ") + connection->SystemNamespaceRestriction(wxT("nspname"));

		pgSet *tabs = connection->ExecuteSet(
		                  wxT("SELECT DISTINCT cl.oid, nspname, relname\n")
		                  wxT("  FROM pg_class cl\n")
		                  wxT("  JOIN pg_namespace nsp ON relnamespace=nsp.oid\n")
		                  wxT("  JOIN pg_index on indrelid=cl.oid AND indisunique\n")
		                  wxT("  LEFT JOIN ") + cluster->GetSchemaPrefix() + wxT("sl_table t ON t.tab_reloid=cl.oid\n")
		                  wxT("  WHERE t.tab_id IS NULL AND cl.relkind = 'r'") + restriction + wxT("\n")
		                  wxT(" ORDER BY nspname, relname")
		              );

		if (tabs)
		{
			while (!tabs->Eof())
			{
				cbTable->Append(tabs->GetVal(wxT("nspname")) + wxT(".") + tabs->GetVal(wxT("relname")), (void *)tabs->GetOid(wxT("oid")));
				tabs->MoveNext();
			}
			delete tabs;
		}
	}

	return dlgProperty::Go(modal);
}


pgObject *dlgRepTable::CreateObject(pgCollection *collection)
{
	pgObject *obj = slTableFactory.CreateObjects(collection, 0,
	                wxT(" WHERE tab_reloid = ") + NumToStr((OID)cbTable->wxItemContainer::GetClientData(cbTable->GetGuessedSelection())));

	return obj;
}



void dlgRepTable::OnChangeTableSel(wxCommandEvent &ev)
{
	cbTable->GuessSelection(ev);
	OnChangeTable(ev);
}


void dlgRepTable::OnChangeTable(wxCommandEvent &ev)
{
	int sel = cbTable->GetGuessedSelection();

	cbIndex->Clear();
	chkTrigger->Clear();

	if (sel >= 0)
	{
		OID relid = (OID)cbTable->wxItemContainer::GetClientData(sel);

		pgSet *idx = connection->ExecuteSet(
		                 wxT("SELECT relname\n")
		                 wxT("  FROM pg_index JOIN pg_class cl ON cl.oid=indexrelid\n")
		                 wxT(" WHERE indrelid=") + NumToStr(relid) + wxT("\n")
		                 wxT(" ORDER BY NOT indisprimary, relname")
		             );
		if (idx)
		{
			while (!idx->Eof())
			{
				cbIndex->Append(idx->GetVal(wxT("relname")));
				idx->MoveNext();
			}
			delete idx;

			cbIndex->SetSelection(0);
		}

		LoadTrigger(relid);
	}

	OnChange(ev);
}


void dlgRepTable::LoadTrigger(OID relid)
{
	wxString sql = wxT("SELECT tgname FROM pg_trigger\n")
	               wxT("  JOIN pg_proc pr ON pr.oid=tgfoid\n")
	               wxT("  JOIN pg_namespace ns ON ns.oid=pronamespace\n")
	               wxT(" WHERE tgrelid=") + NumToStr(relid);
	if (connection->BackendMinimumVersion(8, 5))
		sql += wxT("   AND tgconstraint=0\n");
	else
		sql += wxT("   AND NOT tgisconstraint\n");
	sql += wxT("   AND nspname <> ") + qtDbString(wxT("_") + set->GetCluster()->GetName()) + wxT("\n")
	       wxT(" ORDER BY tgname");

	pgSet *trg = connection->ExecuteSet(sql);

	if (trg)
	{
		while (!trg->Eof())
		{
			chkTrigger->Append(trg->GetVal(wxT("tgname")));
			trg->MoveNext();
		}
		delete trg;
	}
}

void dlgRepTable::CheckChange()
{
	if (table)
	{
		bool tgChanged = false;

		unsigned int i;
		int cnt = 0;

		for (i = 0 ; !tgChanged && i < chkTrigger->GetCount() ; i++)
		{
			if (chkTrigger->IsChecked(i))
			{
				cnt++;
				if (table->GetTriggers().Index(chkTrigger->GetString(i)) < 0)
					tgChanged = true;

			}
		}

		EnableOK(tgChanged
		         || (int)table->GetTriggers().GetCount() != cnt
		         || txtComment->GetValue() != table->GetComment());
	}
	else
	{
		bool enable = true;
		CheckValid(enable, cbTable->GetGuessedSelection() >= 0, _("Please select table to replicate."));
		CheckValid(enable, cbIndex->GetCurrentSelection() >= 0, _("Please select index."));

		EnableOK(enable);
	}
}



wxString dlgRepTable::GetSql()
{
	wxString sql;
	wxString id = txtID->GetValue();;

	wxArrayString newTriggers;

	unsigned int i;
	for (i = 0 ; i < chkTrigger->GetCount() ; i++)
	{
		if (chkTrigger->IsChecked(i))
			newTriggers.Add(chkTrigger->GetString(i));
	}

	if (table)
	{
		// edit mode

		wxArrayString oldTriggers = table->GetTriggers();

		i = oldTriggers.GetCount();
		while (i--)
		{
			int j = newTriggers.Index(oldTriggers[i]);
			if (j >= 0)
			{
				newTriggers.RemoveAt(j);
				oldTriggers.RemoveAt(i);
			}
		}

		// these triggers have been deleted
		for (i = 0 ; i < (unsigned int)oldTriggers.GetCount() ; i++)
		{
			sql += wxT("SELECT ") + cluster->GetSchemaPrefix() + wxT("droptrigger(")
			       +  id + wxT(", ")
			       +  qtDbString(oldTriggers[i]) + wxT(");\n");
		}
	}
	else
	{
		// create mode
		sql = wxT("SELECT ") + cluster->GetSchemaPrefix() + wxT("setaddtable(")
		      + NumToStr(set->GetSlId()) + wxT(", ");

		if (StrToLong(id) > 0)
		{
			sql += id;
		}
		else
		{
			sql += wxT("(SELECT COALESCE(MAX(tab_id), 0) + 1 FROM ") + cluster->GetSchemaPrefix() + wxT("sl_table)");


			id = wxT("(SELECT tab_id FROM ") + cluster->GetSchemaPrefix() + wxT("sl_table\n")
			     wxT("   JOIN pg_class cl ON cl.oid=tab_reloid\n")
			     wxT("   JOIN pg_namespace nsp ON nsp.oid=relnamespace\n")
			     wxT("  WHERE tab_set = ") + NumToStr(set->GetSlId()) +
			     wxT("    AND nspname ||'.' || relname = ") + qtDbString(cbTable->GetGuessedStringSelection()) +
			     wxT(")");

		}
		sql += wxT(", ") + qtDbString(cbTable->GetGuessedStringSelection())
		       +  wxT(", ") + qtDbString(cbIndex->GetStringSelection())
		       +  wxT(", ") + qtDbString(txtComment->GetValue())
		       + wxT(");\n");
	}


	// these triggers have been added
	for (i = 0 ; i < (unsigned int)newTriggers.GetCount() ; i++)
	{
		sql += wxT("SELECT ") + cluster->GetSchemaPrefix() + wxT("storetrigger(")
		       +  id + wxT(", ")
		       +  qtDbString(newTriggers[i]) + wxT(");\n");
	}

	return sql;
}
