//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgRepSet.cpp - PostgreSQL Slony-I Set Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"

#include "slony/dlgRepSet.h"
#include "slony/slCluster.h"
#include "slony/slNode.h"
#include "slony/slSet.h"
#include "frm/frmMain.h"


// pointer to controls
#define txtOrigin           CTRL_TEXT("txtOrigin")
#define txtID               CTRL_TEXT("txtID")



BEGIN_EVENT_TABLE(dlgRepSet, dlgProperty)
END_EVENT_TABLE();


dlgProperty *slSetFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgRepSet(this, frame, (slSet *)node, (slCluster *)parent);
}

dlgRepSet::dlgRepSet(pgaFactory *f, frmMain *frame, slSet *s, slCluster *c)
	: dlgRepProperty(f, frame, c, wxT("dlgRepSet"))
{
	set = s;
}


int dlgRepSet::Go(bool modal)
{
	txtID->SetValidator(numericValidator);

	if (set)
	{
		// edit mode
		txtID->SetValue(NumToStr(set->GetSlId()));
		txtID->Disable();
		txtOrigin->SetValue(IdAndName(set->GetOriginId(), set->GetOriginNode()));
		txtComment->Disable();
	}
	else
	{
		// create mode
		txtOrigin->SetValue(IdAndName(cluster->GetLocalNodeID(), cluster->GetLocalNodeName()));
		EnableOK(true);
	}

	txtOrigin->Disable();

	return dlgProperty::Go(modal);
}


pgObject *dlgRepSet::CreateObject(pgCollection *collection)
{
	wxString restriction;
	if (StrToLong(txtID->GetValue()) > 0)
		restriction = txtID->GetValue();
	else
		restriction = wxT("(SELECT MAX(set_id) FROM ") + cluster->GetSchemaPrefix() + wxT("sl_set)");

	pgObject *obj = setFactory.CreateObjects(collection, 0,
	                wxT(" WHERE set_id = ") + restriction);

	return obj;
}


void dlgRepSet::CheckChange()
{
	if (set)
	{
		EnableOK(txtComment->GetValue() != set->GetComment());
	}
	else
	{
		bool enable = (!txtComment->IsEmpty());

		EnableOK(enable);
	}
}



wxString dlgRepSet::GetSql()
{
	wxString sql;

	sql = wxT("-- Create replication set\n\n")
	      wxT("SELECT ") + cluster->GetSchemaPrefix() + wxT("storeset(");

	if (StrToLong(txtID->GetValue()) > 0)
		sql += txtID->GetValue();
	else
		sql += wxT("(SELECT COALESCE(MAX(set_id), 0) + 1 FROM ")
		       +  cluster->GetSchemaPrefix() + wxT("sl_set)");

	sql += wxT(", ") + qtDbString(txtComment->GetValue())
	       +  wxT(");\n");

	return sql;
}


////////////////////////////////////////////////////////////////////////////////////////


#define cbTargetID  CTRL_COMBOBOX("cbTargetID")

BEGIN_EVENT_TABLE(dlgRepSetMerge, dlgProperty)
	EVT_COMBOBOX(XRCID("cbTargetID"),   dlgRepSetMerge::OnChange)
END_EVENT_TABLE();


dlgRepSetMerge::dlgRepSetMerge(pgaFactory *f, frmMain *frame, slSet *s)
	: dlgRepProperty(f, frame, s->GetCluster(), wxT("dlgRepSetMerge"))
{
	set = s;
}


int dlgRepSetMerge::Go(bool modal)
{
	txtID->SetValue(IdAndName(set->GetSlId(), set->GetName()));
	txtID->Disable();

	wxString sql =
	    wxT("SELECT set_id, set_comment\n")
	    wxT("  FROM ") + cluster->GetSchemaPrefix() + wxT("sl_set\n")
	    wxT("  LEFT JOIN ") + cluster->GetSchemaPrefix() + wxT("sl_subscribe ON set_id=sub_set\n")
	    wxT(" WHERE set_origin = ") + NumToStr(cluster->GetLocalNodeID()) +
	    wxT("   AND set_id <> ") + NumToStr(set->GetSlId());

	if (set->GetSubscriptionCount() > 0)
		sql += wxT("\n")
		       wxT(" GROUP BY set_id, set_comment\n")
		       wxT("HAVING COUNT(sub_set) > 0");
	else
		sql += wxT("\n")
		       wxT("   AND sub_set IS NULL");

	pgSet *sets = connection->ExecuteSet(sql);

	if (sets)
	{
		while (!sets->Eof())
		{
			long id = sets->GetLong(wxT("set_id"));
			cbTargetID->Append(IdAndName(id, sets->GetVal(wxT("set_comment"))), (void *)id);
			sets->MoveNext();
		}
		delete sets;
	}

	return dlgProperty::Go(modal);
}


void dlgRepSetMerge::CheckChange()
{
	bool enable = true;

	CheckValid(enable, cbTargetID->GetCount() > 0 , _("No set available to merge to."));
	CheckValid(enable, cbTargetID->GetCurrentSelection() >= 0, _("Please select replication set to merged to."));
	EnableOK(enable);
}


wxString dlgRepSetMerge::GetSql()
{
	wxString sql;
	wxString addId = NumToStr(set->GetSlId());
	wxString toId = NumToStr((long)cbTargetID->wxItemContainer::GetClientData(cbTargetID->GetCurrentSelection()));
	wxString prefix = cluster->GetSchemaPrefix();

	if (set->GetSubscriptionCount() > 0)
	{
		sql = wxT("SELECT ") + cluster->GetSchemaPrefix() + wxT("mergeset(") + toId  + wxT(", ") + addId + wxT(");\n");
	}
	else
	{
		sql = wxT("-- remember objects to replicate\n")
		      wxT("CREATE TEMPORARY TABLE pga_tmp_repl_tables(tab_id int4, tabname text, idxname text, comment text, trgname text);\n")
		      wxT("INSERT INTO pga_tmp_repl_tables(tab_id, tabname, idxname, comment, trgname)\n");

		if (cluster->ClusterMinimumVersion(2, 0))
		{
			sql += wxT("SELECT tab_id, nsp.nspname || '.' || relname, tab_idxname, tab_comment, tgname\n")
			       wxT("  FROM ") + prefix + wxT("sl_table\n")
			       wxT("  JOIN pg_class cl ON cl.oid=tab_reloid\n")
			       wxT("  JOIN pg_namespace nsp ON nsp.oid=relnamespace\n")
			       wxT("  LEFT JOIN pg_trigger t ON t.tgrelid = tab_reloid\n")
			       wxT("  JOIN pg_proc p ON t.tgfoid = p.oid\n")
			       wxT("  JOIN pg_namespace np ON p.pronamespace = np.oid\n")
			       wxT(" WHERE tab_set = ") + addId +
			       wxT("  AND np.nspname = ") + qtDbString(wxT("_") + cluster->GetName()) + wxT(";\n");
		}
		else
		{
			sql += wxT("SELECT tab_id, nspname || '.' || relname, tab_idxname, tab_comment, trig_tgname\n")
			       wxT("  FROM ") + prefix + wxT("sl_table\n")
			       wxT("  JOIN pg_class cl ON cl.oid=tab_reloid\n")
			       wxT("  JOIN pg_namespace nsp ON nsp.oid=relnamespace\n")
			       wxT("  LEFT JOIN ") + prefix + wxT("sl_trigger ON tab_id=trig_tabid")
			       wxT(" WHERE tab_set = ") + addId + wxT(";\n");
		}

		sql += wxT("\n")
		       wxT("CREATE TEMPORARY TABLE pga_tmp_repl_seqs(seq_id int4, seqname text, comment text);\n")
		       wxT("INSERT INTO pga_tmp_repl_seqs(seq_id, seqname, comment)\n")
		       wxT("SELECT seq_id, nspname ||'.' || relname, seq_comment\n")
		       wxT("  FROM ") + prefix + wxT("sl_sequence\n")
		       wxT("  JOIN pg_class cl ON cl.oid=seq_reloid\n")
		       wxT("  JOIN pg_namespace nsp ON nsp.oid=relnamespace\n")
		       wxT(" WHERE seq_set = ") + addId + wxT(";\n")
		       wxT("\n")

		       wxT("-- drop objects in old set: tables, sequences\n")
		       wxT("SELECT ") + prefix + wxT("setdroptable(tab_id)\n")
		       wxT("  FROM (SELECT DISTINCT tab_id FROM pga_tmp_repl_tables) AS tmp;\n")
		       wxT("\n")
		       wxT("SELECT ") + prefix + wxT("setdropsequence(seq_id)\n")
		       wxT("  FROM pga_tmp_repl_seqs;\n")
		       wxT("\n")

		       wxT("-- add objects to new set: tables, triggers, sequences\n")
		       wxT("SELECT ") + prefix + wxT("setaddtable(") + toId + wxT(", tab_id, tabname, idxname, comment)\n")
		       wxT("  FROM (SELECT DISTINCT tab_id, tabname, idxname, comment FROM pga_tmp_repl_tables) AS tmp;\n")
		       wxT("\n");

		if (!cluster->ClusterMinimumVersion(2, 0))
		{
			sql += wxT("SELECT ") + prefix + wxT("storetrigger(tab_id, trgname)\n")
			       wxT("  FROM pga_tmp_repl_tables WHERE trgname IS NOT NULL;\n")
			       wxT("\n");
		}

		sql += wxT("SELECT ") + prefix + wxT("setaddsequence(") + toId + wxT(", seq_id, seqname, comment)\n")
		       wxT("  FROM pga_tmp_repl_seqs;\n")
		       wxT("\n")

		       wxT("-- finally, drop old set\n")
		       wxT("SELECT ") + prefix + wxT("dropset(") + addId + wxT(");\n")
		       wxT("\n")
		       wxT("-- cleanup\n")
		       wxT("DROP TABLE pga_tmp_repl_tables;\n")
		       wxT("DROP TABLE pga_tmp_repl_seqs;\n");
	}

	return sql;
}


////////////////////////////////////////////////////////////////////////////////////////


#define cbTargetNode  CTRL_COMBOBOX("cbTargetNode")

BEGIN_EVENT_TABLE(dlgRepSetMove, dlgProperty)
	EVT_COMBOBOX(XRCID("cbTargetNode"),   dlgRepSetMove::OnChange)
END_EVENT_TABLE();


dlgRepSetMove::dlgRepSetMove(pgaFactory *f, frmMain *frame, slSet *s)
	: dlgRepProperty(f, frame, s->GetCluster(), wxT("dlgRepSetMove"))
{
	set = s;
}


int dlgRepSetMove::Go(bool modal)
{
	txtID->SetValue(IdAndName(set->GetSlId(), set->GetName()));
	txtID->Disable();

	pgSet *nodes = connection->ExecuteSet(
	                   wxT("SELECT no_id, no_comment\n")
	                   wxT("  FROM ") + cluster->GetSchemaPrefix() + wxT("sl_node\n")
	                   wxT("  JOIN ") + cluster->GetSchemaPrefix() + wxT("sl_subscribe ON sub_receiver=no_id\n")
	                   wxT(" WHERE sub_set = ") + NumToStr(set->GetSlId()));

	if (nodes)
	{
		while (!nodes->Eof())
		{
			long id = nodes->GetLong(wxT("no_id"));
			cbTargetNode->Append(IdAndName(id, nodes->GetVal(wxT("no_comment"))), (void *)id);
			nodes->MoveNext();
		}
		delete nodes;
	}
	return dlgProperty::Go(modal);
}


void dlgRepSetMove::CheckChange()
{
	bool enable = true;

	CheckValid(enable, cbTargetNode->GetCount() > 0 , _("No node available to move this set to."));
	CheckValid(enable, cbTargetNode->GetCurrentSelection() >= 0, _("Please select node to move this replication set to."));
	EnableOK(enable);
}


wxString dlgRepSetMove::GetSql()
{
	wxString toId = NumToStr((long)cbTargetNode->wxItemContainer::GetClientData(cbTargetNode->GetCurrentSelection()));

	wxString sql =
	    wxT("SELECT ") + cluster->GetSchemaPrefix() + wxT("moveset(") +
	    NumToStr(set->GetSlId()) + wxT(", ") + toId + wxT(");\n");
	return sql;
}
