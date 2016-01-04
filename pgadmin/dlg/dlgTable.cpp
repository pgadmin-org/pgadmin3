//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgTable.cpp - PostgreSQL Table Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "frm/frmMain.h"
#include "frm/frmHint.h"

#include "dlg/dlgTable.h"
#include "dlg/dlgColumn.h"
#include "dlg/dlgIndexConstraint.h"
#include "dlg/dlgForeignKey.h"
#include "dlg/dlgCheck.h"

#include "schema/gpPartition.h"
#include "schema/pgSchema.h"
#include "schema/pgTable.h"
#include "schema/pgColumn.h"
#include "schema/pgCheck.h"
#include "schema/pgForeignKey.h"
#include "schema/pgIndexConstraint.h"
#include "schema/pgDatatype.h"
#include "ctl/ctlSeclabelPanel.h"


#define stUnlogged      CTRL_STATIC("stUnlogged")
#define chkUnlogged     CTRL_CHECKBOX("chkUnlogged")
#define stHasOids       CTRL_STATIC("stHasOids")
#define chkHasOids      CTRL_CHECKBOX("chkHasOids")
#define lbTables        CTRL_LISTBOX("lbTables")
#define btnAddTable     CTRL_BUTTON("btnAddTable")
#define btnRemoveTable  CTRL_BUTTON("btnRemoveTable")
#define cbTables        CTRL_COMBOBOX2("cbTables")
#define cbTablespace    CTRL_COMBOBOX("cbTablespace")
#define cbOfType        CTRL_COMBOBOX("cbOfType")
#define txtFillFactor   CTRL_TEXT("txtFillFactor")

#define btnAddCol       CTRL_BUTTON("btnAddCol")
#define btnChangeCol    CTRL_BUTTON("btnChangeCol")
#define btnRemoveCol    CTRL_BUTTON("btnRemoveCol")

#define lstConstraints  CTRL_LISTVIEW("lstConstraints")
#define btnAddConstr    CTRL_BUTTON("btnAddConstr")
#define cbConstrType    CTRL_COMBOBOX("cbConstrType")
#define btnRemoveConstr CTRL_BUTTON("btnRemoveConstr")

#define cbLikeRelation          CTRL_COMBOBOX("cbLikeRelation")
#define chkIncludingDefaults    CTRL_CHECKBOX("chkIncludingDefaults")
#define chkIncludingConstraints CTRL_CHECKBOX("chkIncludingConstraints")
#define chkIncludingIndexes     CTRL_CHECKBOX("chkIncludingIndexes")
#define chkIncludingStorage     CTRL_CHECKBOX("chkIncludingStorage")
#define chkIncludingComments    CTRL_CHECKBOX("chkIncludingComments")

/* AutoVacuum Settings */
#define nbVaccum            CTRL_NOTEBOOK("nbVacuum")
#define chkCustomVac        CTRL_CHECKBOX("chkCustomVac")
#define chkVacEnabled       CTRL_CHECKBOX("chkVacEnabled")
#define txtBaseVac          CTRL_TEXT("txtBaseVac")
#define stBaseVacCurr       CTRL_STATIC("stBaseVacCurr")
#define txtBaseAn           CTRL_TEXT("txtBaseAn")
#define stBaseAnCurr        CTRL_STATIC("stBaseAnCurr")
#define txtFactorVac        CTRL_TEXT("txtFactorVac")
#define stFactorVacCurr     CTRL_STATIC("stFactorVacCurr")
#define txtFactorAn         CTRL_TEXT("txtFactorAn")
#define stFactorAnCurr      CTRL_STATIC("stFactorAnCurr")
#define txtVacDelay         CTRL_TEXT("txtVacDelay")
#define stVacDelayCurr      CTRL_STATIC("stVacDelayCurr")
#define txtVacLimit         CTRL_TEXT("txtVacLimit")
#define stVacLimitCurr      CTRL_STATIC("stVacLimitCurr")
#define txtFreezeMinAge     CTRL_TEXT("txtFreezeMinAge")
#define stFreezeMinAgeCurr  CTRL_STATIC("stFreezeMinAgeCurr")
#define txtFreezeMaxAge     CTRL_TEXT("txtFreezeMaxAge")
#define stFreezeMaxAgeCurr  CTRL_STATIC("stFreezeMaxAgeCurr")
#define txtFreezeTableAge   CTRL_TEXT("txtFreezeTableAge")
#define stFreezeTableAgeCurr CTRL_STATIC("stFreezeTableAgeCurr")

/* TOAST TABLE AutoVacuum Settings */
#define chkCustomToastVac         CTRL_CHECKBOX("chkCustomToastVac")
#define chkToastVacEnabled        CTRL_CHECKBOX("chkToastVacEnabled")
#define txtBaseToastVac           CTRL_TEXT("txtBaseToastVac")
#define stBaseToastVacCurr        CTRL_STATIC("stBaseToastVacCurr")
#define txtFactorToastVac         CTRL_TEXT("txtFactorToastVac")
#define stFactorToastVacCurr      CTRL_STATIC("stFactorToastVacCurr")
#define txtToastVacDelay          CTRL_TEXT("txtToastVacDelay")
#define stToastVacDelayCurr       CTRL_STATIC("stToastVacDelayCurr")
#define txtToastVacLimit          CTRL_TEXT("txtToastVacLimit")
#define stToastVacLimitCurr       CTRL_STATIC("stToastVacLimitCurr")
#define txtToastFreezeMinAge      CTRL_TEXT("txtToastFreezeMinAge")
#define stToastFreezeMinAgeCurr   CTRL_STATIC("stToastFreezeMinAgeCurr")
#define txtToastFreezeMaxAge      CTRL_TEXT("txtToastFreezeMaxAge")
#define stToastFreezeMaxAgeCurr   CTRL_STATIC("stToastFreezeMaxAgeCurr")
#define txtToastFreezeTableAge    CTRL_TEXT("txtToastFreezeTableAge")
#define stToastFreezeTableAgeCurr CTRL_STATIC("stToastFreezeTableAgeCurr")


BEGIN_EVENT_TABLE(dlgTable, dlgSecurityProperty)
	EVT_CHECKBOX(XRCID("chkUnlogged"),              dlgProperty::OnChange)
	EVT_TEXT(XRCID("cbTablespace"),                 dlgProperty::OnChange)
	EVT_COMBOBOX(XRCID("cbTablespace"),             dlgProperty::OnChange)
	EVT_TEXT(XRCID("txtFillFactor"),		dlgProperty::OnChange)
	EVT_COMBOBOX(XRCID("cbOfType"),                 dlgTable::OnChangeOfType)
	EVT_CHECKBOX(XRCID("chkHasOids"),               dlgProperty::OnChange)
	EVT_TEXT(XRCID("cbTables"),                     dlgTable::OnChangeTable)
	EVT_BUTTON(XRCID("btnAddTable"),                dlgTable::OnAddTable)
	EVT_BUTTON(XRCID("btnRemoveTable"),             dlgTable::OnRemoveTable)
	EVT_LISTBOX(XRCID("lbTables"),                  dlgTable::OnSelChangeTable)

	EVT_BUTTON(XRCID("btnAddCol"),                  dlgTable::OnAddCol)
	EVT_BUTTON(XRCID("btnChangeCol"),               dlgTable::OnChangeCol)
	EVT_BUTTON(XRCID("btnRemoveCol"),               dlgTable::OnRemoveCol)
	EVT_LIST_ITEM_SELECTED(XRCID("lstColumns"),     dlgTable::OnSelChangeCol)

	EVT_BUTTON(XRCID("btnAddConstr"),               dlgTable::OnAddConstr)
	EVT_BUTTON(XRCID("btnRemoveConstr"),            dlgTable::OnRemoveConstr)
	EVT_LIST_ITEM_SELECTED(XRCID("lstConstraints"), dlgTable::OnSelChangeConstr)

	/* AutoVacuum Settings */
	EVT_CHECKBOX(XRCID("chkCustomVac"),             dlgTable::OnChangeVacuum)
	EVT_CHECKBOX(XRCID("chkVacEnabled"),            dlgTable::OnChangeVacuum)
	EVT_TEXT(XRCID("txtBaseVac"),                   dlgTable::OnChangeVacuum)
	EVT_TEXT(XRCID("txtBaseAn"),                    dlgTable::OnChangeVacuum)
	EVT_TEXT(XRCID("txtFactorVac"),                 dlgTable::OnChangeVacuum)
	EVT_TEXT(XRCID("txtFactorAn"),                  dlgTable::OnChangeVacuum)
	EVT_TEXT(XRCID("txtVacDelay"),                  dlgTable::OnChangeVacuum)
	EVT_TEXT(XRCID("txtVacLimit"),                  dlgTable::OnChangeVacuum)
	EVT_TEXT(XRCID("txtFreezeMinAge"),              dlgTable::OnChangeVacuum)
	EVT_TEXT(XRCID("txtFreezeMaxAge"),              dlgTable::OnChangeVacuum)
	EVT_TEXT(XRCID("txtFreezeTableAge"),            dlgTable::OnChangeVacuum)

	/* TOAST TABLE AutoVacuum Settings */
	EVT_CHECKBOX(XRCID("chkCustomToastVac"),            dlgTable::OnChangeVacuum)
	EVT_CHECKBOX(XRCID("chkToastVacEnabled"),           dlgTable::OnChangeVacuum)
	EVT_TEXT(XRCID("txtBaseToastVac"),                  dlgTable::OnChangeVacuum)
	EVT_TEXT(XRCID("txtFactorToastVac"),                dlgTable::OnChangeVacuum)
	EVT_TEXT(XRCID("txtToastVacDelay"),                 dlgTable::OnChangeVacuum)
	EVT_TEXT(XRCID("txtToastVacLimit"),                 dlgTable::OnChangeVacuum)
	EVT_TEXT(XRCID("txtToastFreezeMinAge"),             dlgTable::OnChangeVacuum)
	EVT_TEXT(XRCID("txtToastFreezeMaxAge"),             dlgTable::OnChangeVacuum)
	EVT_TEXT(XRCID("txtToastFreezeTableAge"),           dlgTable::OnChangeVacuum)

	EVT_BUTTON(wxID_OK,                             dlgTable::OnOK)

#ifdef __WXMAC__
	EVT_SIZE(                                       dlgTable::OnChangeSize)
#endif
END_EVENT_TABLE();


dlgProperty *pgTableFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgTable(this, frame, (pgTable *)node, (pgSchema *)parent);
}

dlgProperty *gpPartitionFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgTable(this, frame, (gpPartition *)node, (pgSchema *)parent);
}

dlgTable::dlgTable(pgaFactory *f, frmMain *frame, pgTable *node, pgSchema *sch)
	: dlgSecurityProperty(f, frame, node, wxT("dlgTable"), wxT("INSERT,SELECT,UPDATE,DELETE,TRUNCATE,RULE,REFERENCES,TRIGGER"), "arwdDRxt")
{
	schema = sch;
	table = node;

	seclabelPage = new ctlSeclabelPanel(nbNotebook);

	btnAddTable->Disable();
	btnRemoveTable->Disable();

	// Visible columns
	lstColumns->AddColumn(_("Column name"), 90);
	lstColumns->AddColumn(_("Definition"), 135);
#ifndef __WXMAC__
	lstColumns->AddColumn(_("Inherited from table"), 40);
#else
	lstColumns->AddColumn(_("Inherited from table"), 80);
#endif
	// Invisible columns
	// ... sql definition
	lstColumns->AddColumn(_("Column definition"), 0);
	// ... new comment
	lstColumns->AddColumn(_("Column comment"), 0);
	// ... new statistics
	lstColumns->AddColumn(_("Column statistics"), 0);
	// ... pgColumn* handle (used for new columns)
	lstColumns->AddColumn(_("Column"), 0);
	// ... new type OID
	lstColumns->AddColumn(_("Column type oid"), 0);
	// ... pgColumn* handle (used for changed columns)
	lstColumns->AddColumn(_("Changed column"), 0);
	// ... pgColumn* handle (used for variable list)
	lstColumns->AddColumn(_("Variable List"), 0);
	// ... pgColumn* handle (used for security label list)
	lstColumns->AddColumn(_("Security Label List"), 0);
	lstConstraints->CreateColumns(0, _("Constraint name"), _("Definition"), 90);
}

dlgTable::~dlgTable()
{
	//Clear the cached datatypes
	size_t i;
	for (i = 0; i < dtCache.GetCount(); i++)
		delete dtCache.Item(i);
}

bool dlgTable::Destroy()
{
	for(int pos = 0; pos < lstColumns->GetItemCount(); pos++)
	{
		pgColumn *column2 = (pgColumn *) StrToLong(lstColumns->GetText(pos, COL_CHANGEDCOL));
		if(column2) delete column2;
	}
	return dlgProperty::Destroy();
}

pgObject *dlgTable::GetObject()
{
	return table;
}


int dlgTable::Go(bool modal)
{
	PrepareTablespace(cbTablespace);
	PopulateDatatypeCache();

	if (connection->BackendMinimumVersion(9, 1))
	{
		seclabelPage->SetConnection(connection);
		seclabelPage->SetObject(table);
		this->Connect(EVT_SECLABELPANEL_CHANGE, wxCommandEventHandler(dlgTable::OnChange));
	}
	else
		seclabelPage->Disable();

	// new "of type" combobox
	wxString typeQuery = wxT("SELECT t.oid, t.typname ")
	                     wxT("FROM pg_type t, pg_namespace n ")
	                     wxT("WHERE t.typtype='c' AND t.typnamespace=n.oid ")
	                     wxT("AND NOT (n.nspname like 'pg_%' OR n.nspname='information_schema') ")
	                     wxT("ORDER BY typname");
	cbOfType->Insert(wxEmptyString, 0, (void *)0);
	cbOfType->FillOidKey(connection, typeQuery);
	cbOfType->SetSelection(0);

	// "like relation" tab
	nbNotebook->GetPage(TAB_LIKE)->Enable(!table);

	hasPK = false;

	if (table)
	{
		// edit mode
		chkUnlogged->SetValue(table->GetUnlogged());
		chkHasOids->SetValue(table->GetHasOids());

		if (table->GetTablespaceOid() != 0)
			cbTablespace->SetKey(table->GetTablespaceOid());

		if (table->GetOfTypeOid() != 0)
			cbOfType->SetKey(table->GetOfTypeOid());

		inheritedTableOids = table->GetInheritedTablesOidList();

		wxArrayString qitl = table->GetQuotedInheritedTablesList();
		size_t i;
		for (i = 0 ; i < qitl.GetCount() ; i++)
		{
			previousTables.Add(qitl.Item(i));
			lbTables->Append(qitl.Item(i));
		}

		btnAddTable->Enable(connection->BackendMinimumVersion(8, 2) && cbTables->GetGuessedSelection() >= 0);
		lbTables->Enable(connection->BackendMinimumVersion(8, 2));
		chkHasOids->Enable((connection->BackendMinimumVersion(8, 0) && table->GetHasOids())
		                   || connection->BackendMinimumVersion(8, 4));
		cbSchema->Enable(connection->BackendMinimumVersion(8, 1));
		cbTablespace->Enable(connection->BackendMinimumVersion(7, 5));

		wxCookieType cookie;
		pgObject *data = 0;
		wxTreeItemId item = mainForm->GetBrowser()->GetFirstChild(table->GetId(), cookie);
		while (item)
		{
			data = mainForm->GetBrowser()->GetObject(item);
			pgaFactory *factory = data->GetFactory();
			if (factory == columnFactory.GetCollectionFactory())
				columnsItem = item;
			else if (factory == checkFactory.GetCollectionFactory())
				constraintsItem = item;
			if (data->GetMetaType() == PGM_COLUMN && data->IsCollection())
				columnsItem = item;
			else if (data->GetMetaType() == PGM_CONSTRAINT)
				constraintsItem = item;

			if (columnsItem && constraintsItem)
				break;

			item = mainForm->GetBrowser()->GetNextChild(table->GetId(), cookie);
		}

		if (columnsItem)
		{
			pgCollection *coll = (pgCollection *)data;
			// make sure all columns are appended
			coll->ShowTreeDetail(mainForm->GetBrowser());
			// this is the columns collection
			item = mainForm->GetBrowser()->GetFirstChild(columnsItem, cookie);

			// add columns
			while (item)
			{
				data = mainForm->GetBrowser()->GetObject(item);
				if (data->IsCreatedBy(columnFactory))
				{
					pgColumn *column = (pgColumn *)data;
					// make sure column details are read
					column->ShowTreeDetail(mainForm->GetBrowser());

					if (column->GetColNumber() > 0)
					{
						bool inherited = (column->GetInheritedCount() != 0);
						int pos = lstColumns->AppendItem((inherited ? tableFactory.GetIconId() : column->GetIconId()),
						                                 column->GetName(), column->GetDefinition());
						previousColumns.Add(column->GetQuotedIdentifier()
						                    + wxT(" ") + column->GetDefinition());
						lstColumns->SetItem(pos, COL_PGCOLUMN, NumToStr((long)column));
						if (inherited)
							lstColumns->SetItem(pos, COL_INHERIT, column->GetInheritedTableName());
					}
				}

				item = mainForm->GetBrowser()->GetNextChild(columnsItem, cookie);
			}
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
					case PGM_PRIMARYKEY:
						hasPK = true;
					case PGM_UNIQUE:
					{
						pgIndexConstraint *obj = (pgIndexConstraint *)data;

						lstConstraints->AppendItem(data->GetIconId(), obj->GetName(), obj->GetDefinition());
						constraintsDefinition.Add(obj->GetDefinition());
						previousConstraints.Add(obj->GetQuotedIdentifier()
						                        + wxT(" ") + obj->GetTypeName().Upper() + wxT(" ") + obj->GetDefinition());
						break;
					}
					case PGM_EXCLUDE:
					{
						pgIndexConstraint *obj = (pgIndexConstraint *)data;

						lstConstraints->AppendItem(data->GetIconId(), obj->GetName(), obj->GetDefinition());
						constraintsDefinition.Add(obj->GetDefinition());
						previousConstraints.Add(obj->GetQuotedIdentifier()
						                        + wxT(" ") + obj->GetTypeName().Upper() + wxT(" ") + obj->GetDefinition());
						break;
					}
					case PGM_FOREIGNKEY:
					{
						pgForeignKey *obj = (pgForeignKey *)data;
						wxString def = obj->GetDefinition();

						lstConstraints->AppendItem(data->GetIconId(), obj->GetName(), def);
						constraintsDefinition.Add(obj->GetDefinition());
						previousConstraints.Add(obj->GetQuotedIdentifier()
						                        + wxT(" ") + obj->GetTypeName().Upper() + wxT(" ") + def);
						break;
					}
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
	}
	else
	{
		// create mode
		btnChangeCol->Hide();

		// Add the default tablespace
		cbTablespace->Insert(_("<default tablespace>"), 0, (void *)0);
		cbTablespace->SetSelection(0);

		// new "like relation" combobox
		wxString likeRelationQuery = wxT("SELECT c.oid, quote_ident(n.nspname)||'.'||quote_ident(c.relname) ")
		                             wxT("FROM pg_class c, pg_namespace n ")
		                             wxT("WHERE c.relnamespace=n.oid AND c.relkind IN ");
		if (connection->BackendMinimumVersion(9, 2))
		{
			likeRelationQuery += wxT("('r', 'v', 'f')");
		}
		else
		{
			likeRelationQuery += wxT("('r')");
		}
		if (!settings->GetShowSystemObjects())
			likeRelationQuery += wxT(" AND ") + connection->SystemNamespaceRestriction(wxT("n.nspname"));
		likeRelationQuery += wxT(" ORDER BY 1");
		cbLikeRelation->Insert(wxEmptyString, 0, (void *)0);
		cbLikeRelation->FillOidKey(connection, likeRelationQuery);
		cbLikeRelation->SetSelection(0);

		// Enable the "like relation" comboboxes
		chkIncludingDefaults->Enable();
		chkIncludingConstraints->Enable(connection->BackendMinimumVersion(8, 2));
		chkIncludingIndexes->Enable(connection->BackendMinimumVersion(8, 3));
		chkIncludingStorage->Enable(connection->BackendMinimumVersion(9, 0));
		chkIncludingComments->Enable(connection->BackendMinimumVersion(9, 0));
	}

	chkUnlogged->Enable(connection->BackendMinimumVersion(9, 1) && !table);
	cbOfType->Enable(connection->BackendMinimumVersion(9, 0) && !table);
	cbTables->Enable(connection->BackendMinimumVersion(8, 2) && cbOfType->GetCurrentSelection() == 0);

	if (connection->BackendMinimumVersion(8, 2) || !table)
	{
		wxString systemRestriction;
		if (!settings->GetShowSystemObjects())
			systemRestriction =
			    wxT("   AND ") + connection->SystemNamespaceRestriction(wxT("n.nspname"));

		if (table)
		{
			wxString oids = table->GetOidStr();
			int i;
			for (i = 0 ; i < (int)inheritedTableOids.GetCount() ; i++)
			{
				oids += wxT(", ") + inheritedTableOids.Item(i);
			}
			if (oids.Length() > 0)
				systemRestriction += wxT(" AND c.oid NOT IN (") + oids + wxT(")");
		}

		pgSet *set = connection->ExecuteSet(
		                 wxT("SELECT c.oid, c.relname , nspname\n")
		                 wxT("  FROM pg_class c\n")
		                 wxT("  JOIN pg_namespace n ON n.oid=c.relnamespace\n")
		                 wxT(" WHERE relkind='r'\n")
		                 + systemRestriction +
		                 wxT(" ORDER BY relnamespace, c.relname"));
		if (set)
		{
			while (!set->Eof())
			{
				cbTables->Append(database->GetQuotedSchemaPrefix(set->GetVal(wxT("nspname")))
				                 + qtIdent(set->GetVal(wxT("relname"))));

				tableOids.Add(set->GetVal(wxT("oid")));
				set->MoveNext();
			}
			delete set;
		}
	}

	FillConstraint();

	btnChangeCol->Disable();
	btnRemoveCol->Disable();
	btnRemoveConstr->Disable();
	btnOK->Disable();

	if ((connection->BackendMinimumVersion(8, 1) && table) || connection->BackendMinimumVersion(8, 4))
	{
		if (!connection->BackendMinimumVersion(8, 4))
		{
			txtFreezeTableAge->Disable();
			/* Remove Toast Table AutoVacuume setting page */
			nbVaccum->DeletePage(1);
		}

		settingAutoVacuum = false;

		pgSetIterator avSet(connection,
		                    wxT("SELECT name, setting FROM pg_settings WHERE name like '%vacuum%' ORDER BY name"));
		while (avSet.RowsLeft())
		{
			wxString name = avSet.GetVal(wxT("name"));
			wxString setting = avSet.GetVal(wxT("setting"));

			if (name == wxT("autovacuum_vacuum_cost_delay"))
				settingCostDelay = setting;
			else if (name == wxT("vacuum_cost_delay"))
			{
				if (StrToLong(settingCostDelay) < 0)
					settingCostDelay = setting;
			}
			else if (name == wxT("autovacuum_vacuum_cost_limit"))
				settingCostLimit = setting;
			else if (name == wxT("vacuum_cost_limit"))
			{
				if (StrToLong(settingCostLimit) < 0)
					settingCostLimit = setting;
			}
			else if (name == wxT("autovacuum_vacuum_scale_factor"))
				settingVacFactor = setting;
			else if (name == wxT("autovacuum_analyze_scale_factor"))
				settingAnlFactor = setting;
			else if (name == wxT("autovacuum_vacuum_threshold"))
				settingVacBaseThr = setting;
			else if (name == wxT("autovacuum_analyze_threshold"))
				settingAnlBaseThr = setting;
			else if (name == wxT("vacuum_freeze_min_age"))
				settingFreezeMinAge = setting;
			else if (name == wxT("autovacuum_freeze_max_age"))
				settingFreezeMaxAge = setting;
			else if (name == wxT("vacuum_freeze_table_age"))
				settingFreezeTableAge = setting;
			else
				settingAutoVacuum = avSet.GetBool(wxT("setting"));
		}

		tableVacBaseThr = wxT("-1");
		tableAnlBaseThr = wxT("-1");
		tableCostDelay = wxT("-1");
		tableCostLimit = wxT("-1");
		tableFreezeMinAge = wxT("-1");
		tableFreezeMaxAge = wxT("-1");
		tableVacFactor = wxT("-1");
		tableAnlFactor = wxT("-1");
		tableFreezeTableAge = wxT("-1");

		toastTableVacBaseThr = wxT("-1");
		toastTableCostDelay = wxT("-1");
		toastTableCostLimit = wxT("-1");
		toastTableFreezeMinAge = wxT("-1");
		toastTableFreezeMaxAge = wxT("-1");
		toastTableVacFactor = wxT("-1");
		toastTableFreezeTableAge = wxT("-1");

		toastTableHasVacuum = false;
		toastTableVacEnabled = false;

		if (!connection->BackendMinimumVersion(8, 4))
		{
			pgSetIterator set(connection, wxT("SELECT * FROM pg_autovacuum WHERE vacrelid=") + table->GetOidStr());
			if (set.RowsLeft())
			{
				hasVacuum = true;

				tableVacEnabled = set.GetBool(wxT("enabled"));
				chkVacEnabled->SetValue(tableVacEnabled);

				tableVacBaseThr = set.GetVal(wxT("vac_base_thresh"));
				tableAnlBaseThr = set.GetVal(wxT("anl_base_thresh"));
				tableCostDelay = set.GetVal(wxT("vac_cost_delay"));
				tableCostLimit = set.GetVal(wxT("vac_cost_limit"));
				tableVacFactor = set.GetVal(wxT("vac_scale_factor"));
				tableAnlFactor = set.GetVal(wxT("anl_scale_factor"));

				if (connection->BackendMinimumVersion(8, 2))
				{
					tableFreezeMinAge = set.GetVal(wxT("freeze_min_age"));
					tableFreezeMaxAge = set.GetVal(wxT("freeze_max_age"));
				}
			}
			else
			{
				hasVacuum = false;
				chkVacEnabled->SetValue(true);
			}
		}
		else if (table)
		{
			if (table->GetAutoVacuumEnabled() == 2)
				tableVacEnabled = settingAutoVacuum;
			else
				tableVacEnabled = table->GetAutoVacuumEnabled() == 1;
			if (!table->GetAutoVacuumVacuumThreshold().IsEmpty())
				tableVacBaseThr = table->GetAutoVacuumVacuumThreshold();
			if (!table->GetAutoVacuumAnalyzeThreshold().IsEmpty())
				tableAnlBaseThr = table->GetAutoVacuumAnalyzeThreshold();
			if (!table->GetAutoVacuumVacuumScaleFactor().IsEmpty())
				tableVacFactor = table->GetAutoVacuumVacuumScaleFactor();
			if (!table->GetAutoVacuumAnalyzeScaleFactor().IsEmpty())
				tableAnlFactor = table->GetAutoVacuumAnalyzeScaleFactor();
			if (!table->GetAutoVacuumVacuumCostDelay().IsEmpty())
				tableCostDelay = table->GetAutoVacuumVacuumCostDelay();
			if (!table->GetAutoVacuumVacuumCostLimit().IsEmpty())
				tableCostLimit = table->GetAutoVacuumVacuumCostLimit();
			if (!table->GetAutoVacuumFreezeMinAge().IsEmpty())
				tableFreezeMinAge = table->GetAutoVacuumFreezeMinAge();
			if (!table->GetAutoVacuumFreezeMaxAge().IsEmpty())
				tableFreezeMaxAge = table->GetAutoVacuumFreezeMaxAge();
			if (!table->GetAutoVacuumFreezeTableAge().IsEmpty())
				tableFreezeTableAge = table->GetAutoVacuumFreezeTableAge();

			hasVacuum = table->GetCustomAutoVacuumEnabled();
			chkVacEnabled->SetValue(hasVacuum ? tableVacEnabled : settingAutoVacuum);

			toastTableVacEnabled = false;

			if (!table->GetHasToastTable())
			{
				nbVaccum->GetPage(1)->Enable(false);
			}
			else
			{
				toastTableHasVacuum = table->GetToastCustomAutoVacuumEnabled();
				if (toastTableHasVacuum)
				{
					if (table->GetToastAutoVacuumEnabled() == 2)
						toastTableVacEnabled = settingAutoVacuum;
					else
						toastTableVacEnabled  = table->GetToastAutoVacuumEnabled() == 1;
					if (!table->GetToastAutoVacuumVacuumThreshold().IsEmpty())
						toastTableVacBaseThr = table->GetToastAutoVacuumVacuumThreshold();
					if (!table->GetToastAutoVacuumVacuumScaleFactor().IsEmpty())
						toastTableVacFactor = table->GetToastAutoVacuumVacuumScaleFactor();
					if (!table->GetToastAutoVacuumVacuumCostDelay().IsEmpty())
						toastTableCostDelay = table->GetToastAutoVacuumVacuumCostDelay();
					if (!table->GetToastAutoVacuumVacuumCostLimit().IsEmpty())
						toastTableCostLimit = table->GetToastAutoVacuumVacuumCostLimit();
					if (!table->GetToastAutoVacuumFreezeMinAge().IsEmpty())
						toastTableFreezeMinAge = table->GetToastAutoVacuumFreezeMinAge();
					if (!table->GetToastAutoVacuumFreezeMaxAge().IsEmpty())
						toastTableFreezeMaxAge = table->GetToastAutoVacuumFreezeMaxAge();
					if (!table->GetToastAutoVacuumFreezeTableAge().IsEmpty())
						toastTableFreezeTableAge = table->GetToastAutoVacuumFreezeTableAge();
				}
				chkToastVacEnabled->SetValue(toastTableHasVacuum ? toastTableVacEnabled : settingAutoVacuum);
			}
		}
		else
		{
			hasVacuum = false;
			chkVacEnabled->SetValue(settingAutoVacuum);
		}

		//txtBaseVac->SetValue(tableVacBaseThr);
		txtBaseAn->SetValue(tableAnlBaseThr);
		txtFactorVac->SetValue(tableVacFactor);
		txtFactorAn->SetValue(tableAnlFactor);
		txtVacDelay->SetValue(tableCostDelay);
		txtVacLimit->SetValue(tableCostLimit);

		if (connection->BackendMinimumVersion(8, 2))
		{
			txtFreezeMinAge->SetValue(tableFreezeMinAge);
			txtFreezeMaxAge->SetValue(tableFreezeMaxAge);
		}
		if (connection->BackendMinimumVersion(8, 4))
		{
			txtFreezeTableAge->SetValue(tableFreezeTableAge);
			txtBaseToastVac->SetValue(toastTableVacBaseThr);
			txtFactorToastVac->SetValue(toastTableVacFactor);
			txtToastVacDelay->SetValue(toastTableCostDelay);
			txtToastVacLimit->SetValue(toastTableCostLimit);
			txtToastFreezeMinAge->SetValue(toastTableFreezeMinAge);
			txtToastFreezeMaxAge->SetValue(toastTableFreezeMaxAge);
			txtToastFreezeTableAge->SetValue(toastTableFreezeTableAge);

			chkCustomToastVac->SetValue(toastTableHasVacuum);
			chkToastVacEnabled->SetValue(toastTableHasVacuum ? toastTableVacEnabled : settingAutoVacuum);
		}
		chkCustomVac->SetValue(hasVacuum);
		wxCommandEvent ev;
		OnChangeVacuum(ev);
	}
	else
	{
		/* Remove 'Vacuum Settings' Page */
		nbNotebook->DeletePage(TAB_AUTOVACUUM);
	}

	// Find, and disable the RULE ACL option if we're 8.2
	if (connection->BackendMinimumVersion(8, 2))
	{
		// Disable the checkbox
		if (!DisablePrivilege(wxT("RULE")))
		{
			wxLogError(_("Failed to disable the RULE privilege checkbox!"));
		}

		if (table)
		{
			txtFillFactor->SetValue(table->GetFillFactor());
		}

		txtFillFactor->SetValidator(numericValidator);
		txtFillFactor->Enable();
	}
	else
	{
		txtFillFactor->Disable();
	}

	return dlgSecurityProperty::Go(modal);
}



wxString dlgTable::GetItemConstraintType(ctlListView *list, long pos)
{
	wxString con;
	wxListItem item;
	item.SetId(pos);
	item.SetColumn(0);
	item.SetMask(wxLIST_MASK_IMAGE);
	list->GetItem(item);
	if (item.GetImage() == primaryKeyFactory.GetIconId())
		con = wxT("PRIMARY KEY");
	if (item.GetImage() == foreignKeyFactory.GetIconId())
		con = wxT("FOREIGN KEY");
	if (item.GetImage() == excludeFactory.GetIconId())
		con = wxT("EXCLUDE");
	if (item.GetImage() == uniqueFactory.GetIconId())
		con = wxT("UNIQUE");
	if (item.GetImage() == checkFactory.GetIconId())
		con = wxT("CHECK");
	return con;
}


wxString dlgTable::GetSql()
{
	int pos;
	wxString sql;
	wxString tabname;

	if (table)
	{
		int pos;
		int index = -1;

		wxString definition;

		wxArrayString tmpDef = previousColumns;
		wxString tmpsql;

		tabname = schema->GetQuotedPrefix() + qtIdent(GetName());

		// Build a temporary list of ADD COLUMNs, and fixup the list to remove
		for (pos = 0; pos < lstColumns->GetItemCount() ; pos++)
		{
			index = -1;

			if (lstColumns->GetText(pos, COL_INHERIT).IsEmpty())
			{
				definition = lstColumns->GetText(pos, COL_SQLCHANGE);
				if (definition.IsEmpty())
				{
					definition = qtIdent(lstColumns->GetText(pos)) + wxT(" ") + lstColumns->GetText(pos, COL_DEFINITION);
					index = tmpDef.Index(definition);
					if (index < 0)
					{
						tmpsql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
						          +  wxT("\n  ADD COLUMN ") + definition + wxT(";\n");
					}
				}
				else
				{
					tmpsql += definition;

					pgColumn *column = (pgColumn *) StrToLong(lstColumns->GetText(pos, COL_PGCOLUMN));
					if (column)
					{
						index = tmpDef.Index(column->GetQuotedIdentifier()
						                     + wxT(" ") + column->GetDefinition());
					}
				}
			}
			else
			{
				if (! lstColumns->GetText(pos, COL_INHERIT).IsEmpty())
				{
					definition = qtIdent(lstColumns->GetText(pos)) + wxT(" ") + lstColumns->GetText(pos, COL_DEFINITION);
					index = tmpDef.Index(definition);
				}
			}
			if (index >= 0 && index < (int)tmpDef.GetCount())
				tmpDef.RemoveAt(index);
		}


		for (index = 0 ; index < (int)tmpDef.GetCount() ; index++)
		{
			definition = tmpDef.Item(index);
			if (definition[0U] == '"')
				definition = definition.Mid(1).BeforeFirst('"');
			else
				definition = definition.BeforeFirst(' ');
			sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
			       +  wxT("\n  DROP COLUMN ") + qtIdent(definition) + wxT(";\n");
		}
		// Add the ADD COLUMNs...
		sql += tmpsql;

		AppendNameChange(sql);
		AppendOwnerChange(sql, wxT("TABLE ") + tabname);

		tmpDef = previousTables;
		tmpsql.Empty();

		// Build a temporary list of INHERIT tables, and fixup the list to remove
		for (pos = 0 ; pos < (int)lbTables->GetCount() ; pos++)
		{
			definition = lbTables->GetString(pos);
			index = tmpDef.Index(definition);
			if (index < 0)
				tmpsql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
				          +  wxT("\n  INHERIT ") + definition + wxT(";\n");
			else
				tmpDef.RemoveAt(index);
		}

		for (index = 0 ; index < (int)tmpDef.GetCount() ; index++)
		{
			definition = tmpDef.Item(index);
			// We don't need to quote the table because it's already quoted
			sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
			       + wxT("\n  NO INHERIT ") + definition + wxT(";\n");
		}
		// Add the INHERIT COLUMNs...
		sql += tmpsql;

		tmpDef = previousConstraints;
		tmpsql.Empty();

		// Build a temporary list of ADD CONSTRAINTs, and fixup the list to remove
		for (pos = 0; pos < lstConstraints->GetItemCount() ; pos++)
		{
			wxString conname = qtIdent(lstConstraints->GetItemText(pos));
			definition = conname;
			definition += wxT(" ") + GetItemConstraintType(lstConstraints, pos)
			              + wxT(" ") + constraintsDefinition.Item(pos);
			index = tmpDef.Index(definition);
			if (index >= 0)
				tmpDef.RemoveAt(index);
			else
			{
				tmpsql += wxT("ALTER TABLE ") + tabname
				          +  wxT("\n  ADD");
				if (!conname.IsEmpty())
					tmpsql += wxT(" CONSTRAINT ");

				tmpsql += definition + wxT(";\n");
			}
		}

		for (index = 0 ; index < (int)tmpDef.GetCount() ; index++)
		{
			definition = tmpDef.Item(index);
			if (definition[0U] == '"')
				definition = definition.Mid(1).BeforeFirst('"');
			else
				definition = definition.BeforeFirst(' ');
			sql += wxT("ALTER TABLE ") + tabname
			       + wxT("\n  DROP CONSTRAINT ") + qtIdent(definition) + wxT(";\n");

		}
		// Add the ADD CONSTRAINTs...
		sql += tmpsql;

		if (!chkHasOids->GetValue() && table->GetHasOids())
		{
			sql += wxT("ALTER TABLE ") + tabname
			       +  wxT("\n  SET WITHOUT OIDS;\n");
		}
		if (chkHasOids->GetValue() && !table->GetHasOids())
		{
			sql += wxT("ALTER TABLE ") + tabname
			       +  wxT("\n  SET WITH OIDS;\n");
		}
		if (connection->BackendMinimumVersion(8, 0) && cbTablespace->GetOIDKey() != table->GetTablespaceOid())
			sql += wxT("ALTER TABLE ") + tabname
			       +  wxT("\n  SET TABLESPACE ") + qtIdent(cbTablespace->GetValue())
			       + wxT(";\n");

		if (txtFillFactor->GetValue().Trim().Length() > 0 && txtFillFactor->GetValue() != table->GetFillFactor())
		{
			sql += wxT("ALTER TABLE ") + tabname
			       +  wxT("\n  SET (FILLFACTOR=")
			       +  txtFillFactor->GetValue() + wxT(");\n");
		}

		if (connection->BackendMinimumVersion(8, 1))
		{
			if (!chkCustomVac->GetValue())
			{
				if (hasVacuum)
				{
					if (connection->BackendMinimumVersion(8, 4))
						sql += wxT("ALTER TABLE ") + tabname
						       +  wxT(" RESET(\n")
						       wxT("  autovacuum_enabled,\n")
						       wxT("  autovacuum_vacuum_threshold,\n")
						       wxT("  autovacuum_analyze_threshold,\n")
						       wxT("  autovacuum_vacuum_scale_factor,\n")
						       wxT("  autovacuum_analyze_scale_factor,\n")
						       wxT("  autovacuum_vacuum_cost_delay,\n")
						       wxT("  autovacuum_vacuum_cost_limit,\n")
						       wxT("  autovacuum_freeze_min_age,\n")
						       wxT("  autovacuum_freeze_max_age,\n")
						       wxT("  autovacuum_freeze_table_age\n")
						       wxT(");\n");
					else
						sql += wxT("DELETE FROM pg_autovacuum WHERE vacrelid=") + table->GetOidStr() + wxT(";\n");
				}
			}
			else
			{
				wxString vacStr;
				bool changed = (chkVacEnabled->GetValue() != tableVacEnabled);
				if (connection->BackendMinimumVersion(8, 4))
				{
					bool valChanged = false;
					wxString newVal;
					wxString setStr;
					wxString resetStr;
					if (changed)
					{
						FillAutoVacuumParameters(setStr, resetStr, wxT("autovacuum_enabled"), BoolToStr(chkVacEnabled->GetValue()));
					}
					newVal =  AppendNum(valChanged, txtBaseVac, tableVacBaseThr);
					if (valChanged)
					{
						valChanged = false;
						FillAutoVacuumParameters(setStr, resetStr, wxT("autovacuum_vacuum_threshold"), newVal);
					}

					newVal = AppendNum(valChanged, txtBaseAn, tableAnlBaseThr);
					if (valChanged)
					{
						valChanged = false;
						FillAutoVacuumParameters(setStr, resetStr, wxT("autovacuum_analyze_threshold"), newVal);
					}

					newVal = AppendNum(valChanged, txtFactorVac, tableVacFactor);
					if (valChanged)
					{
						valChanged = false;
						FillAutoVacuumParameters(setStr, resetStr, wxT("autovacuum_vacuum_scale_factor"), newVal);
					}

					newVal = AppendNum(valChanged, txtFactorAn, tableAnlFactor);
					if (valChanged)
					{
						valChanged = false;
						FillAutoVacuumParameters(setStr, resetStr, wxT("autovacuum_analyze_scale_factor"), newVal);
					}

					newVal = AppendNum(valChanged, txtVacDelay, tableCostDelay);
					if (valChanged)
					{
						valChanged = false;
						FillAutoVacuumParameters(setStr, resetStr, wxT("autovacuum_vacuum_cost_delay"), newVal);
					}

					newVal = AppendNum(valChanged, txtVacLimit, tableCostLimit);
					if (valChanged)
					{
						valChanged = false;
						FillAutoVacuumParameters(setStr, resetStr, wxT("autovacuum_vacuum_cost_limit"), newVal);
					}

					newVal = AppendNum(valChanged, txtFreezeMinAge, tableFreezeMinAge);
					if (valChanged)
					{
						valChanged = false;
						FillAutoVacuumParameters(setStr, resetStr, wxT("autovacuum_freeze_min_age"), newVal);
					}

					newVal = AppendNum(valChanged, txtFreezeMaxAge, tableFreezeMaxAge);
					if (valChanged)
					{
						valChanged = false;
						FillAutoVacuumParameters(setStr, resetStr, wxT("autovacuum_freeze_max_age"), newVal);
					}

					newVal = AppendNum(valChanged, txtFreezeTableAge, tableFreezeTableAge);
					if (valChanged)
					{
						valChanged = false;
						FillAutoVacuumParameters(setStr, resetStr, wxT("autovacuum_freeze_table_age"), newVal);
					}

					if (!setStr.IsEmpty())
					{
						vacStr = wxT("ALTER TABLE ") + tabname + setStr + wxT("\n);\n");;
						changed = true;
					}
					if (!resetStr.IsEmpty())
					{
						vacStr += wxT("ALTER TABLE ") + tabname + resetStr + wxT("\n);\n");;
						changed = true;
					}
				}
				else if (!hasVacuum)
				{
					if (connection->BackendMinimumVersion(8, 2))
					{
						vacStr = wxT("INSERT INTO pg_autovacuum(vacrelid, enabled, vac_base_thresh, anl_base_thresh, vac_scale_factor, anl_scale_factor, vac_cost_delay, vac_cost_limit, freeze_min_age, freeze_max_age)")
						         wxT("\n   VALUES(")
						         + table->GetOidStr() + wxT(", ")
						         + BoolToStr(chkVacEnabled->GetValue()) + wxT(", ")
						         + AppendNum(changed, txtBaseVac, tableVacBaseThr) + wxT(", ")
						         + AppendNum(changed, txtBaseAn, tableAnlBaseThr) + wxT(", ")
						         + AppendNum(changed, txtFactorVac, tableVacFactor) + wxT(", ")
						         + AppendNum(changed, txtFactorAn, tableAnlFactor) + wxT(", ")
						         + AppendNum(changed, txtVacDelay, tableCostDelay) + wxT(", ")
						         + AppendNum(changed, txtVacLimit, tableCostLimit) + wxT(", ")
						         + AppendNum(changed, txtFreezeMinAge, tableFreezeMinAge) + wxT(", ")
						         + AppendNum(changed, txtFreezeMaxAge, tableFreezeMaxAge) + wxT(");\n");
					}
					else
					{
						vacStr = wxT("INSERT INTO pg_autovacuum(vacrelid, enabled, vac_base_thresh, anl_base_thresh, vac_scale_factor, anl_scale_factor, vac_cost_delay, vac_cost_limit)")
						         wxT("\n   VALUES(")
						         + table->GetOidStr() + wxT(", ")
						         + BoolToStr(chkVacEnabled->GetValue()) + wxT(", ")
						         + AppendNum(changed, txtBaseVac, tableVacBaseThr) + wxT(", ")
						         + AppendNum(changed, txtBaseAn, tableAnlBaseThr) + wxT(", ")
						         + AppendNum(changed, txtFactorVac, tableVacFactor) + wxT(", ")
						         + AppendNum(changed, txtFactorAn, tableAnlFactor) + wxT(", ")
						         + AppendNum(changed, txtVacDelay, tableCostDelay) + wxT(", ")
						         + AppendNum(changed, txtVacLimit, tableCostLimit) + wxT(");\n");
					}
				}
				else
				{
					if (connection->BackendMinimumVersion(8, 2))
					{
						vacStr = wxT("UPDATE pg_autovacuum\n")
						         wxT("   SET enabled=")
						         + BoolToStr(chkVacEnabled->GetValue())
						         + wxT(", vac_base_thresh = ") + AppendNum(changed, txtBaseVac, tableVacBaseThr)
						         + wxT(", anl_base_thresh = ") + AppendNum(changed, txtBaseAn, tableAnlBaseThr)
						         + wxT(", vac_scale_factor = ") + AppendNum(changed, txtFactorVac, tableVacFactor)
						         + wxT(", anl_scale_factor = ") + AppendNum(changed, txtFactorAn, tableAnlFactor)
						         + wxT(", vac_cost_delay = ") + AppendNum(changed, txtVacDelay, tableCostDelay)
						         + wxT(", vac_cost_limit = ") + AppendNum(changed, txtVacLimit, tableCostLimit)
						         + wxT(", freeze_min_age = ") + AppendNum(changed, txtFreezeMinAge, tableFreezeMinAge)
						         + wxT(", freeze_max_age = ") + AppendNum(changed, txtFreezeMaxAge, tableFreezeMaxAge)
						         + wxT("\n WHERE vacrelid=") + table->GetOidStr() + wxT(";\n");
					}
					else
					{
						vacStr = wxT("UPDATE pg_autovacuum\n")
						         wxT("   SET enabled=")
						         + BoolToStr(chkVacEnabled->GetValue())
						         + wxT(", vac_base_thresh = ") + AppendNum(changed, txtBaseVac, tableVacBaseThr)
						         + wxT(", anl_base_thresh = ") + AppendNum(changed, txtBaseAn, tableAnlBaseThr)
						         + wxT(", vac_scale_factor = ") + AppendNum(changed, txtFactorVac, tableVacFactor)
						         + wxT(", anl_scale_factor = ") + AppendNum(changed, txtFactorAn, tableAnlFactor)
						         + wxT(", vac_cost_delay = ") + AppendNum(changed, txtVacDelay, tableCostDelay)
						         + wxT(", vac_cost_limit = ") + AppendNum(changed, txtVacLimit, tableCostLimit)
						         + wxT("\n WHERE vacrelid=") + table->GetOidStr() + wxT(";\n");
					}

				}
				if (changed)
					sql += vacStr;
			}
		}
		if (connection->BackendMinimumVersion(8, 4))
		{
			if (!chkCustomToastVac->GetValue())
			{
				if (toastTableHasVacuum)
				{
					sql += wxT("ALTER TABLE ") + tabname
					       +  wxT(" RESET(\n")
					       wxT("  toast.autovacuum_enabled,\n")
					       wxT("  toast.autovacuum_vacuum_threshold,\n")
					       wxT("  toast.autovacuum_analyze_threshold,\n")
					       wxT("  toast.autovacuum_vacuum_scale_factor,\n")
					       wxT("  toast.autovacuum_analyze_scale_factor,\n")
					       wxT("  toast.autovacuum_vacuum_cost_delay,\n")
					       wxT("  toast.autovacuum_vacuum_cost_limit,\n")
					       wxT("  toast.autovacuum_freeze_min_age,\n")
					       wxT("  toast.autovacuum_freeze_max_age,\n")
					       wxT("  toast.autovacuum_freeze_table_age\n")
					       wxT(");\n");
				}
			}
			else
			{
				wxString vacStr;
				bool changed = (chkToastVacEnabled->GetValue() != toastTableVacEnabled);
				bool valChanged = false;
				wxString newVal;
				wxString setStr;
				wxString resetStr;
				if (changed)
				{
					FillAutoVacuumParameters(setStr, resetStr, wxT("toast.autovacuum_enabled"), BoolToStr(chkToastVacEnabled->GetValue()));
				}
				newVal =  AppendNum(valChanged, txtBaseToastVac, toastTableVacBaseThr);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(setStr, resetStr, wxT("toast.autovacuum_vacuum_threshold"), newVal);
				}

				newVal = AppendNum(valChanged, txtFactorToastVac, toastTableVacFactor);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(setStr, resetStr, wxT("toast.autovacuum_vacuum_scale_factor"), newVal);
				}

				newVal = AppendNum(valChanged, txtToastVacDelay, toastTableCostDelay);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(setStr, resetStr, wxT("toast.autovacuum_vacuum_cost_delay"), newVal);
				}

				newVal = AppendNum(valChanged, txtToastVacLimit, toastTableCostLimit);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(setStr, resetStr, wxT("toast.autovacuum_vacuum_cost_limit"), newVal);
				}

				newVal = AppendNum(valChanged, txtToastFreezeMinAge, toastTableFreezeMinAge);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(setStr, resetStr, wxT("toast.autovacuum_freeze_min_age"), newVal);
				}

				newVal = AppendNum(valChanged, txtToastFreezeMaxAge, toastTableFreezeMaxAge);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(setStr, resetStr, wxT("toast.autovacuum_freeze_max_age"), newVal);
				}

				newVal = AppendNum(valChanged, txtToastFreezeTableAge, toastTableFreezeTableAge);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(setStr, resetStr, wxT("toast.autovacuum_freeze_table_age"), newVal);
				}

				if (!setStr.IsEmpty())
				{
					vacStr = wxT("ALTER TABLE ") + tabname + setStr + wxT("\n);\n");;
					changed = true;
				}
				if (!resetStr.IsEmpty())
				{
					vacStr += wxT("ALTER TABLE ") + tabname + resetStr + wxT("\n);\n");;
					changed = true;
				}
				if (changed)
					sql += vacStr;
			}
		}
		// This needs to always be last so that other statements use correct schema
		if (connection->BackendMinimumVersion(8, 1) && cbSchema->GetValue() != table->GetSchema()->GetName())
		{
			AppendSchemaChange(sql, wxT("TABLE ") + tabname);
		}
	}
	else
	{
		bool needComma = false;
		bool typedTable = cbOfType->GetCurrentSelection() > 0 && cbOfType->GetOIDKey() > 0;

		tabname = qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName());

		sql = wxT("CREATE ");
		if (chkUnlogged->GetValue())
			sql +=  wxT("UNLOGGED ");
		sql += wxT("TABLE ") + tabname;
		if (typedTable)
			sql += wxT("\nOF ") + qtIdent(cbOfType->GetValue());

		if (!typedTable || (typedTable && lstConstraints->GetItemCount() > 0))
			sql += wxT("\n(");

		if (!cbLikeRelation->GetValue().IsEmpty())
		{
			sql += wxT("\n   LIKE ") + cbLikeRelation->GetValue();
			if (chkIncludingDefaults->GetValue())
				sql += wxT(" INCLUDING DEFAULTS");
			if (chkIncludingConstraints->GetValue())
				sql += wxT(" INCLUDING CONSTRAINTS");
			if (chkIncludingIndexes->GetValue())
				sql += wxT(" INCLUDING INDEXES");
			if (chkIncludingStorage->GetValue())
				sql += wxT(" INCLUDING STORAGE");
			if (chkIncludingComments->GetValue())
				sql += wxT(" INCLUDING COMMENTS");
			needComma = true;
		}

		if (!typedTable)
		{
			for (pos = 0 ; pos < lstColumns->GetItemCount() ; pos++)
			{
				if (lstColumns->GetText(pos, COL_INHERIT).IsEmpty())
				{
					// standard definition, not inherited
					if (needComma)
						sql += wxT(", ");
					else
						needComma = true;

					wxString name = lstColumns->GetText(pos);
					wxString definition = lstColumns->GetText(pos, COL_DEFINITION);

					sql += wxT("\n   ") + qtIdent(name)
					       + wxT(" ") + definition;
				}
			}
		}

		for (pos = 0 ; pos < lstConstraints->GetItemCount() ; pos++)
		{
			wxString name = lstConstraints->GetItemText(pos);
			wxString definition = constraintsDefinition.Item(pos);

			if (needComma)
				sql += wxT(", ");
			else
				needComma = true;

			sql += wxT("\n   ");
			if (!name.IsEmpty())
				sql += wxT("CONSTRAINT ") + qtIdent(name) + wxT(" ");
			sql += GetItemConstraintType(lstConstraints, pos) + wxT(" ") + definition;
		}

		if (!typedTable || (typedTable && lstConstraints->GetItemCount() > 0))
			sql += wxT("\n) ");


		if (lbTables->GetCount() > 0)
		{
			sql += wxT("\nINHERITS (");

			unsigned int i;
			for (i = 0 ; i < lbTables->GetCount() ; i++)
			{
				if (i)
					sql += wxT(", ");
				sql += lbTables->GetString(i);
			}
			sql += wxT(")");
		}

		if (connection->BackendMinimumVersion(8, 2))
		{
			sql += wxT("\nWITH (");
			if (txtFillFactor->GetValue().Trim().Length() > 0)
				sql += wxT("\n  FILLFACTOR = ") + txtFillFactor->GetValue() + wxT(", ");
			if (chkHasOids->GetValue())
				sql +=  wxT("\n  OIDS = TRUE");
			else
				sql +=  wxT("\n  OIDS = FALSE");
			if (connection->BackendMinimumVersion(8, 4) && chkCustomVac->GetValue())
			{
				bool valChanged = false;
				wxString newVal;
				wxString resetStr;

				FillAutoVacuumParameters(sql, resetStr, wxT("autovacuum_enabled"), BoolToStr(chkVacEnabled->GetValue()));
				newVal =  AppendNum(valChanged, txtBaseVac, tableVacBaseThr);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(sql, resetStr, wxT("autovacuum_vacuum_threshold"), newVal);
				}

				newVal = AppendNum(valChanged, txtBaseAn, tableAnlBaseThr);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(sql, resetStr, wxT("autovacuum_analyze_threshold"), newVal);
				}

				newVal = AppendNum(valChanged, txtFactorVac, tableVacFactor);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(sql, resetStr, wxT("autovacuum_vacuum_scale_factor"), newVal);
				}

				newVal = AppendNum(valChanged, txtFactorAn, tableAnlFactor);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(sql, resetStr, wxT("autovacuum_analyze_scale_factor"), newVal);
				}

				newVal = AppendNum(valChanged, txtVacDelay, tableCostDelay);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(sql, resetStr, wxT("autovacuum_vacuum_cost_delay"), newVal);
				}

				newVal = AppendNum(valChanged, txtVacLimit, tableCostLimit);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(sql, resetStr, wxT("autovacuum_vacuum_cost_limit"), newVal);
				}

				newVal = AppendNum(valChanged, txtFreezeMinAge, tableFreezeMinAge);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(sql, resetStr, wxT("autovacuum_freeze_min_age"), newVal);
				}

				newVal = AppendNum(valChanged, txtFreezeMaxAge, tableFreezeMaxAge);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(sql, resetStr, wxT("autovacuum_freeze_max_age"), newVal);
				}

				newVal = AppendNum(valChanged, txtFreezeTableAge, tableFreezeTableAge);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(sql, resetStr, wxT("autovacuum_freeze_table_age"), newVal);
				}
			}
			if (connection->BackendMinimumVersion(8, 4) && chkCustomToastVac->GetValue())
			{
				bool valChanged = false;
				wxString newVal;
				wxString resetStr;

				FillAutoVacuumParameters(sql, resetStr, wxT("toast.autovacuum_enabled"), BoolToStr(chkToastVacEnabled->GetValue()));
				newVal =  AppendNum(valChanged, txtBaseToastVac, toastTableVacBaseThr);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(sql, resetStr, wxT("toast.autovacuum_vacuum_threshold"), newVal);
				}

				newVal = AppendNum(valChanged, txtFactorToastVac, toastTableVacFactor);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(sql, resetStr, wxT("toast.autovacuum_vacuum_scale_factor"), newVal);
				}

				newVal = AppendNum(valChanged, txtToastVacDelay, toastTableCostDelay);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(sql, resetStr, wxT("toast.autovacuum_vacuum_cost_delay"), newVal);
				}

				newVal = AppendNum(valChanged, txtToastVacLimit, toastTableCostLimit);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(sql, resetStr, wxT("toast.autovacuum_vacuum_cost_limit"), newVal);
				}

				newVal = AppendNum(valChanged, txtToastFreezeMinAge, toastTableFreezeMinAge);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(sql, resetStr, wxT("toast.autovacuum_freeze_min_age"), newVal);
				}

				newVal = AppendNum(valChanged, txtToastFreezeMaxAge, toastTableFreezeMaxAge);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(sql, resetStr, wxT("toast.autovacuum_freeze_max_age"), newVal);
				}

				newVal = AppendNum(valChanged, txtToastFreezeTableAge, toastTableFreezeTableAge);
				if (valChanged)
				{
					valChanged = false;
					FillAutoVacuumParameters(sql, resetStr, wxT("toast.autovacuum_freeze_table_age"), newVal);
				}
			}

			sql += wxT("\n)\n");
		}
		else
		{
			sql += (chkHasOids->GetValue() ? wxT("\nWITH OIDS") : wxT("\nWITHOUT OIDS"));
		}

		if (cbTablespace->GetCurrentSelection() > 0 && cbTablespace->GetOIDKey() > 0)
			sql += wxT("\nTABLESPACE ") + qtIdent(cbTablespace->GetValue());

		sql += wxT(";\n");

		AppendOwnerNew(sql, wxT("TABLE ") + tabname);

		// Extra column info
		// Statistics
		for (pos = 0 ; pos < lstColumns->GetItemCount() ; pos++)
		{
			if (!lstColumns->GetText(pos, COL_STATISTICS).IsEmpty())
				sql += wxT("ALTER TABLE ") + tabname
				       + wxT("\n  ALTER COLUMN ") + qtIdent(lstColumns->GetText(pos, COL_NAME))
				       + wxT("\n  SET STATISTICS ") + lstColumns->GetText(pos, COL_STATISTICS)
				       + wxT(";\n");
		}
	}

	//variables
	for (pos = 0; pos < lstColumns->GetItemCount(); pos++)
	{
		wxStringTokenizer varToken(lstColumns->GetText(pos, COL_VARIABLE_LIST), wxT(","));
		while (varToken.HasMoreTokens())
		{
			sql += wxT("ALTER TABLE ") + tabname
			       + wxT("\n  ALTER COLUMN ")
			       + qtIdent(lstColumns->GetText(pos, COL_NAME))
			       + wxT(" \nSET (");
			sql += varToken.GetNextToken() + wxT(");\n");
		}
	}

	//security labels
	for (pos = 0; pos < lstColumns->GetItemCount(); pos++)
	{
		wxStringTokenizer varToken(lstColumns->GetText(pos, COL_SECLABEL_LIST), wxT(","));
		wxString providerLabel = wxEmptyString;
		wxString provider = wxEmptyString;
		while (varToken.HasMoreTokens())
		{
			provider = varToken.GetNextToken();
			if(varToken.HasMoreTokens())
				providerLabel = varToken.GetNextToken();

			sql += wxT("SECURITY LABEL FOR ") + provider
			       + wxT("\n  ON COLUMN ") + qtIdent(lstColumns->GetText(pos, COL_NAME))
			       + wxT("\n  IS ") + connection->qtDbString(providerLabel) + wxT(";\n");
		}
	}

	// Comments
	for (pos = 0 ; pos < lstColumns->GetItemCount() ; pos++)
	{
		if (!lstColumns->GetText(pos, COL_COMMENTS).IsEmpty())
			sql += wxT("COMMENT ON COLUMN ") + tabname
			       + wxT(".") + qtIdent(lstColumns->GetText(pos, COL_NAME))
			       + wxT(" IS ") + qtDbString(lstColumns->GetText(pos, COL_COMMENTS))
			       + wxT(";\n");
	}

	AppendComment(sql, wxT("TABLE ") + qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()), table);

	if (seclabelPage && connection->BackendMinimumVersion(9, 1))
		sql += seclabelPage->GetSqlForSecLabels(wxT("TABLE"), qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()));

	if (connection->BackendMinimumVersion(8, 4))
		sql += GetGrant(wxT("arwdDxt"), wxT("TABLE ") + tabname);
	else if (connection->BackendMinimumVersion(8, 2))
		sql += GetGrant(wxT("arwdxt"), wxT("TABLE ") + tabname);
	else
		sql += GetGrant(wxT("arwdRxt"), wxT("TABLE ") + tabname);

	return sql;
}


void dlgTable::FillConstraint()
{
	cbConstrType->Clear();
	if (!hasPK)
		cbConstrType->Append(_("Primary Key"));

//    chkHasOids->Enable(!table || (table && table->GetHasOids() && hasPK && connection->BackendMinimumVersion(7, 4)));
	cbConstrType->Append(_("Foreign Key"));
	cbConstrType->Append(_("Exclude Constraint"));
	cbConstrType->Append(_("Unique"));
	cbConstrType->Append(_("Check"));
	cbConstrType->SetSelection(0);
}


pgObject *dlgTable::CreateObject(pgCollection *collection)
{
	wxString name = GetName();

	pgObject *obj = tableFactory.CreateObjects(collection, 0, wxT(
	                    "\n   AND rel.relname=") + qtDbString(name) + wxT(
	                    "\n   AND rel.relnamespace=") + schema->GetOidStr());

	return obj;
}


wxString dlgTable::GetNumString(wxTextCtrl *ctl, bool enabled, const wxString &val)
{
	if (!enabled)
		return val;
	wxString str = ctl->GetValue();
	if (str.IsEmpty() || StrToLong(val) < 0)
		return val;
	else
		return str;
}


wxString dlgTable::AppendNum(bool &changed, wxTextCtrl *ctl, wxString val)
{
	wxString str = ctl->GetValue();
	if (str.IsEmpty() || str.StartsWith(wxT("-")))
		str = wxT("-1");

	changed |= (str != val);
	return str;
}


#ifdef __WXMAC__
void dlgTable::OnChangeSize(wxSizeEvent &ev)
{
	if (lstConstraints)
	{
		lstConstraints->SetSize(wxDefaultCoord, wxDefaultCoord,
		                        ev.GetSize().GetWidth(), ev.GetSize().GetHeight() - 150);
	}

	lstColumns->SetSize(wxDefaultCoord, wxDefaultCoord,
	                    ev.GetSize().GetWidth(), ev.GetSize().GetHeight() - 150);

	dlgSecurityProperty::OnChangeSize(ev);
}
#endif


void dlgTable::OnChangeVacuum(wxCommandEvent &ev)
{
	if (connection->BackendMinimumVersion(8, 1))
	{
		bool vacEn = chkCustomVac->GetValue() && chkVacEnabled->GetValue();
		chkVacEnabled->Enable(chkCustomVac->GetValue());

		txtBaseVac->Enable(vacEn);
		txtBaseAn->Enable(vacEn);
		txtFactorVac->Enable(vacEn);
		txtFactorAn->Enable(vacEn);
		txtVacDelay->Enable(vacEn);
		txtVacLimit->Enable(vacEn);

		if (connection->BackendMinimumVersion(8, 2))
		{
			txtFreezeMinAge->Enable(vacEn);
			txtFreezeMaxAge->Enable(vacEn);
		}
		else
		{
			txtFreezeMinAge->Enable(false);
			txtFreezeMaxAge->Enable(false);
		}

		stBaseVacCurr->SetLabel(tableVacBaseThr == wxT("-1") ? settingVacBaseThr : tableVacBaseThr);
		stBaseAnCurr->SetLabel(tableAnlBaseThr == wxT("-1") ? settingAnlBaseThr : tableAnlBaseThr);
		stFactorVacCurr->SetLabel(tableVacFactor == wxT("-1") ? settingVacFactor : tableVacFactor);
		stFactorAnCurr->SetLabel(tableAnlFactor == wxT("-1") ? settingAnlFactor : tableAnlFactor);
		stVacDelayCurr->SetLabel(tableCostDelay == wxT("-1") ? settingCostDelay : tableCostDelay);
		stVacLimitCurr->SetLabel(tableCostLimit == wxT("-1") ? settingCostLimit : tableCostLimit);

		if (connection->BackendMinimumVersion(8, 2))
		{
			stFreezeMinAgeCurr->SetLabel(tableFreezeMinAge == wxT("-1") ? settingFreezeMinAge : tableFreezeMinAge);
			stFreezeMaxAgeCurr->SetLabel(tableFreezeMaxAge == wxT("-1") ? settingFreezeMaxAge : tableFreezeMaxAge);
		}
		if (connection->BackendMinimumVersion(8, 4))
		{
			txtFreezeTableAge->Enable(vacEn);
			stFreezeTableAgeCurr->SetLabel(tableFreezeTableAge == wxT("-1") ? settingFreezeTableAge : tableFreezeTableAge);
			/* Toast Table Vacuum Settings */
			bool toastVacEn = chkCustomToastVac->GetValue() && chkToastVacEnabled->GetValue();
			chkToastVacEnabled->Enable(chkCustomToastVac->GetValue());

			txtBaseToastVac->Enable(toastVacEn);
			txtFactorToastVac->Enable(toastVacEn);
			txtToastVacDelay->Enable(toastVacEn);
			txtToastVacLimit->Enable(toastVacEn);
			txtToastFreezeMinAge->Enable(toastVacEn);
			txtToastFreezeMaxAge->Enable(toastVacEn);
			txtToastFreezeTableAge->Enable(toastVacEn);

			stBaseToastVacCurr->SetLabel(toastTableVacBaseThr == wxT("-1") ? settingVacBaseThr : toastTableVacBaseThr);
			stFactorToastVacCurr->SetLabel(toastTableVacFactor == wxT("-1") ? settingVacFactor : toastTableVacFactor);
			stToastVacDelayCurr->SetLabel(toastTableCostDelay == wxT("-1") ? settingCostDelay : toastTableCostDelay);
			stToastVacLimitCurr->SetLabel(toastTableCostLimit == wxT("-1") ? settingCostLimit : toastTableCostLimit);
			stToastFreezeMinAgeCurr->SetLabel(toastTableFreezeMinAge == wxT("-1") ? settingFreezeMinAge : toastTableFreezeMinAge);
			stToastFreezeMaxAgeCurr->SetLabel(toastTableFreezeMaxAge == wxT("-1") ? settingFreezeMaxAge : toastTableFreezeMaxAge);
			txtToastFreezeTableAge->Enable(toastVacEn);
			stToastFreezeTableAgeCurr->SetLabel(toastTableFreezeTableAge == wxT("-1") ? settingFreezeTableAge : toastTableFreezeTableAge);
		}
		else
		{
			stFreezeTableAgeCurr->SetLabel(wxEmptyString);
			txtFreezeTableAge->Enable(false);
		}
	}
	OnChange(ev);
}


void dlgTable::OnChangeTable(wxCommandEvent &ev)
{
	cbTables->GuessSelection(ev);
	btnAddTable->Enable((table || connection->BackendMinimumVersion(8, 2)) && cbTables->GetGuessedSelection() >= 0);
}


void dlgTable::OnOK(wxCommandEvent &ev)
{
#ifdef __WXGTK__
	if (!btnOK->IsEnabled())
		return;
#endif
	if (lstColumns->GetItemCount() > 0 && !hasPK
	        && frmHint::ShowHint(this, HINT_PRIMARYKEY) == wxID_CANCEL)
		return;

	dlgProperty::OnOK(ev);
}


void dlgTable::CheckChange()
{
	bool enable = true;
	if (table)
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
		CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
		CheckValid(enable, connection->BackendMinimumVersion(7, 4) || lstColumns->GetItemCount() > 0,
		           _("Please specify columns."));
	}
	EnableOK(enable);
}


void dlgTable::OnAddTable(wxCommandEvent &ev)
{
	int sel = cbTables->GetGuessedSelection();
	if (sel >= 0)
	{
		wxString tabname = cbTables->GetValue();
		wxString taboid = tableOids.Item(sel);
		inheritedTableOids.Add(taboid);
		tableOids.RemoveAt(sel);

		lbTables->Append(tabname);
		cbTables->Delete(sel);

		pgSet *set = connection->ExecuteSet(
		                 wxT("SELECT attname, format_type(atttypid, NULL) AS atttype FROM pg_attribute\n")
		                 wxT (" WHERE NOT attisdropped AND attnum>0 AND attrelid=") + taboid);
		if (set)
		{
			bool found;
			while (!set->Eof())
			{
				found = false;

				size_t row = lstColumns->GetItemCount();
				while (row--)
				{
					if (set->GetVal(wxT("attname")).Cmp(lstColumns->GetText(row, COL_NAME)) == 0)
					{
						found = true;
						break;
					}
				}

				if (found)
				{
					lstColumns->SetItem(row, COL_INHERIT, tabname);
				}
				else
				{
					lstColumns->AppendItem(tableFactory.GetIconId(),
					                       set->GetVal(wxT("attname")),
					                       set->GetVal(wxT("atttype")),
					                       tabname);
				}
				set->MoveNext();
			}
			delete set;
		}
		CheckChange();
	}
}


void dlgTable::OnRemoveTable(wxCommandEvent &ev)
{
	if (settings->GetConfirmDelete())
	{
		if (wxMessageBox(_("Are you sure you wish to remove the selected table?"), _("Remove table?"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION) != wxYES)
			return;
	}

	int sel = lbTables->GetSelection();
	if (sel >= 0)
	{
		wxString tabname = lbTables->GetStringSelection();
		tableOids.Add(inheritedTableOids.Item(sel));
		inheritedTableOids.RemoveAt(sel);

		lbTables->Delete(sel);
		cbTables->Append(tabname);

		size_t row = lstColumns->GetItemCount();
		while (row--)
		{
			if (tabname == lstColumns->GetText(row, COL_INHERIT))
			{
				lstColumns->SetItem(row, COL_INHERIT, wxT(""));
			}
		}
		CheckChange();
	}
	btnRemoveTable->Disable();
}


void dlgTable::OnSelChangeTable(wxCommandEvent &ev)
{
	btnRemoveTable->Enable();
}


void dlgTable::OnChangeOfType(wxCommandEvent &ev)
{
	if (cbOfType->GetCurrentSelection() > 0)
	{
		if (settings->GetConfirmDelete() && lstColumns->GetItemCount() > 0)
		{
			if (wxMessageBox(_("A typed table only has the type's columns. All other columns will be dropped. Are you sure you want to do this?"), _("Remove all columns?"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION) == wxYES)
			{
				lstColumns->DeleteAllItems();
				lstConstraints->DeleteAllItems();
				constraintsDefinition.Clear();
				hasPK = false;
				FillConstraint();
			}
			else
			{
				cbOfType->SetSelection(0);
				return;
			}
		}

		pgSet *set = connection->ExecuteSet(
		                 wxT("SELECT a.attname, format_type(atttypid, NULL) AS atttypname ")
		                 wxT("FROM pg_attribute a, pg_class c\n")
		                 wxT("WHERE NOT a.attisdropped AND a.attnum>0 ")
		                 wxT("AND a.attrelid=c.oid AND c.relname='") + qtIdent(cbOfType->GetValue()) + wxT("'"));
		if (set)
		{
			while (!set->Eof())
			{
				lstColumns->AppendItem(tableFactory.GetIconId(),
				                       set->GetVal(wxT("attname")),
				                       set->GetVal(wxT("atttypname")),
				                       wxT(""));
				set->MoveNext();
			}
			delete set;
		}
	}
	else
	{
		if (settings->GetConfirmDelete() && lstColumns->GetItemCount() > 0)
		{
			if (wxMessageBox(_("All type's columns will be dropped. Are you sure you want to do this?"), _("Remove all columns?"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION) == wxYES)
			{
				lstColumns->DeleteAllItems();
				lstConstraints->DeleteAllItems();
				constraintsDefinition.Clear();
				hasPK = false;
				FillConstraint();
			}
		}
	}

	btnAddCol->Enable(cbOfType->GetCurrentSelection() == 0);
	cbTables->Enable(connection->BackendMinimumVersion(8, 2) && cbOfType->GetCurrentSelection() == 0);
	btnRemoveCol->Enable(false);
	CheckChange();
}

void dlgTable::OnChangeCol(wxCommandEvent &ev)
{
	long pos = lstColumns->GetSelection();
	pgColumn *column = (pgColumn *) StrToLong(lstColumns->GetText(pos, COL_PGCOLUMN));
	pgColumn *column2 = (pgColumn *) StrToLong(lstColumns->GetText(pos, COL_CHANGEDCOL));

	dlgColumn col(&columnFactory, mainForm, column, table);
	col.CenterOnParent();
	col.SetDatabase(database);
	col.SetChangedCol(column2);
	if (col.Go(true) != wxID_CANCEL)
	{
		if(column2 == NULL)
			column2 = new pgColumn(*column);

		col.ApplyChangesToObj(column2);
		lstColumns->SetItem(pos, COL_NAME, col.GetName());
		lstColumns->SetItem(pos, COL_DEFINITION, col.GetDefinition());
		lstColumns->SetItem(pos, COL_SQLCHANGE, col.GetSql());
		lstColumns->SetItem(pos, COL_STATISTICS, col.GetStatistics());
		lstColumns->SetItem(pos, COL_COMMENTS, col.GetComment());
		lstColumns->SetItem(pos, COL_CHANGEDCOL, NumToStr((long)column2));
	}
	CheckChange();
}

// Cache datatypes to avoid multiple calls to server when adding multiple columns to a table.
void dlgTable::PopulateDatatypeCache()
{
	DatatypeReader tr(database, true, true);
	while (tr.HasMore())
	{
		pgDatatype dt = tr.GetDatatype();

		dataType *dType = new dataType();
		dType->SetOid(tr.GetOid());
		dType->SetTypename(dt.GetQuotedSchemaPrefix(database) + dt.QuotedFullName());
		dtCache.Add(dType);

		tr.MoveNext();
	}
}


void dlgTable::OnAddCol(wxCommandEvent &ev)
{
	dlgColumn col(&columnFactory, mainForm, NULL, table);
	col.CenterOnParent();
	col.SetDatabase(database);
	col.SetDatatypeCache(dtCache);
	if (col.Go(true) != wxID_CANCEL)
	{
		long pos = lstColumns->AppendItem(columnFactory.GetIconId(), col.GetName(), col.GetDefinition());
		if (table && !connection->BackendMinimumVersion(8, 0))
			lstColumns->SetItem(pos, COL_SQLCHANGE, col.GetSql());
		lstColumns->SetItem(pos, COL_STATISTICS, col.GetStatistics());
		lstColumns->SetItem(pos, COL_COMMENTS, col.GetComment());
		lstColumns->SetItem(pos, COL_TYPEOID, col.GetTypeOid());

		wxString perColumnListString = wxEmptyString;

		//getting the variable list for each column
		wxArrayString perColumnList;
		col.GetVariableList(perColumnList);
		for(size_t index = 0; index < perColumnList.GetCount(); index++)
		{
			if (index == 0)
				perColumnListString	= perColumnList.Item(index);
			else
				perColumnListString += wxT(",") + perColumnList.Item(index);
		}
		lstColumns->SetItem(pos, COL_VARIABLE_LIST, perColumnListString);

		//getting the security labels list for each column
		if(connection->BackendMinimumVersion(9, 1))
		{
			wxString secLabelListString = wxEmptyString;
			wxArrayString secLabelList;
			col.GetSecLabelList(secLabelList);
			for(size_t index = 0; index < secLabelList.GetCount(); index++)
			{
				if (index == 0)
					secLabelListString	= secLabelList.Item(index);
				else
					secLabelListString += wxT(",") + secLabelList.Item(index);
			}
			lstColumns->SetItem(pos, COL_SECLABEL_LIST, secLabelListString);
		}
	}

	CheckChange();
}


void dlgTable::OnRemoveCol(wxCommandEvent &ev)
{
	if (settings->GetConfirmDelete())
	{
		if (wxMessageBox(_("Are you sure you wish to remove the selected column?"), _("Remove column?"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION) != wxYES)
			return;
	}

	lstColumns->DeleteCurrentItem();

	btnChangeCol->Disable();
	btnRemoveCol->Disable();

	CheckChange();
}


void dlgTable::OnSelChangeCol(wxListEvent &ev)
{
	long pos = lstColumns->GetSelection();
	wxString inheritedFromTable = lstColumns->GetText(pos, COL_INHERIT);

	btnAddCol->Enable(!(cbOfType->GetCurrentSelection() > 0 && cbOfType->GetOIDKey() > 0));
	btnRemoveCol->Enable(inheritedFromTable.IsEmpty() && !(cbOfType->GetCurrentSelection() > 0 && cbOfType->GetOIDKey() > 0));
	btnChangeCol->Enable(table != 0 && !lstColumns->GetText(pos, COL_PGCOLUMN).IsEmpty() && inheritedFromTable.IsEmpty());
}


void dlgTable::OnAddConstr(wxCommandEvent &ev)
{
	int sel = cbConstrType->GetCurrentSelection();
	if (hasPK)
		sel++;

	switch (sel)
	{
		case 0: // Primary Key
		{
			dlgPrimaryKey pk(&primaryKeyFactory, mainForm, lstColumns);
			pk.CenterOnParent();
			pk.SetDatabase(database);
			if (pk.Go(true) != wxID_CANCEL)
			{
				wxString tmpDef = pk.GetDefinition();
				tmpDef.Replace(wxT("\n"), wxT(" "));

				lstConstraints->AppendItem(primaryKeyFactory.GetIconId(), pk.GetName(), tmpDef);
				constraintsDefinition.Add(tmpDef);
				hasPK = true;
				FillConstraint();
			}
			break;
		}
		case 1: // Foreign Key
		{
			dlgForeignKey fk(&foreignKeyFactory, mainForm, lstColumns);
			fk.CenterOnParent();
			fk.SetDatabase(database);
			if (fk.Go(true) != wxID_CANCEL)
			{
				wxString tmpDef = fk.GetDefinition();
				tmpDef.Replace(wxT("\n"), wxT(" "));
				while (tmpDef.Contains(wxT("  ")))
					tmpDef.Replace(wxT("  "), wxT(" "));

				lstConstraints->AppendItem(foreignKeyFactory.GetIconId(), fk.GetName(), tmpDef);
				constraintsDefinition.Add(tmpDef);
			}
			break;
		}
		case 2: // Exclusion Constraint
		{
			dlgExclude ec(&excludeFactory, mainForm, lstColumns);
			ec.CenterOnParent();
			ec.SetDatabase(database);
			if (ec.Go(true) != wxID_CANCEL)
			{
				wxString tmpDef = ec.GetDefinition();
				tmpDef.Replace(wxT("\n"), wxT(" "));
				while (tmpDef.Contains(wxT("  ")))
					tmpDef.Replace(wxT("  "), wxT(" "));

				lstConstraints->AppendItem(excludeFactory.GetIconId(), ec.GetName(), tmpDef);
				constraintsDefinition.Add(tmpDef);
			}
			break;
		}
		case 3: // Unique
		{
			dlgUnique unq(&uniqueFactory, mainForm, lstColumns);
			unq.CenterOnParent();
			unq.SetDatabase(database);
			if (unq.Go(true) != wxID_CANCEL)
			{
				wxString tmpDef = unq.GetDefinition();
				tmpDef.Replace(wxT("\n"), wxT(" "));

				lstConstraints->AppendItem(uniqueFactory.GetIconId(), unq.GetName(), tmpDef);
				constraintsDefinition.Add(tmpDef);
			}
			break;
		}
		case 4: // Check
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


void dlgTable::OnRemoveConstr(wxCommandEvent &ev)
{
	if (settings->GetConfirmDelete())
	{
		if (wxMessageBox(_("Are you sure you wish to remove the selected constraint?"), _("Remove constraint?"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION) != wxYES)
			return;
	}

	int pos = lstConstraints->GetSelection();
	if (pos < 0)
		return;

	wxListItem item;
	item.SetId(pos);
	item.SetColumn(0);
	item.SetMask(wxLIST_MASK_IMAGE);
	lstConstraints->GetItem(item);
	if (item.GetImage() == primaryKeyFactory.GetIconId())
	{
		hasPK = false;
		FillConstraint();
	}

	lstConstraints->DeleteItem(pos);
	constraintsDefinition.RemoveAt(pos);
	btnRemoveConstr->Disable();

	CheckChange();
}


void dlgTable::OnSelChangeConstr(wxListEvent &ev)
{
	btnRemoveConstr->Enable();
}


void dlgTable::FillAutoVacuumParameters(wxString &setStr, wxString &resetStr,
                                        const wxString &parameter, const wxString &val)
{
	if (val == wxT("-1"))
	{
		if (resetStr.IsEmpty())
			resetStr = wxT(" RESET (");
		else
			resetStr += wxT(",");
		resetStr += wxT("\n  ") + parameter;
	}
	else
	{
		if (setStr.IsEmpty())
			setStr = wxT(" SET (");
		else
			setStr += wxT(",");
		setStr += wxT("\n  ") + parameter + wxT(" = ") + val;
	}
}


void dlgTable::OnChange(wxCommandEvent &event)
{
	CheckChange();
}
