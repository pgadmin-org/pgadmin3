//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// 
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
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
#define txtBaseToastAn            CTRL_TEXT("txtBaseToastAn")
#define stBaseToastAnCurr         CTRL_STATIC("stBaseToastAnCurr")
#define txtFactorToastVac         CTRL_TEXT("txtFactorToastVac")
#define stFactorToastVacCurr      CTRL_STATIC("stFactorToastVacCurr")
#define txtFactorToastAn          CTRL_TEXT("txtFactorToastAn")
#define stFactorToastAnCurr       CTRL_STATIC("stFactorToastAnCurr")
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
    EVT_CHECKBOX(XRCID("chkHasOids"),               dlgProperty::OnChange)
    EVT_TEXT(XRCID("cbTablespace"),                 dlgProperty::OnChange)
    EVT_COMBOBOX(XRCID("cbTablespace"),             dlgProperty::OnChange)
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
    EVT_TEXT(XRCID("txtBaseToastAn"),                   dlgTable::OnChangeVacuum)
    EVT_TEXT(XRCID("txtFactorToastVac"),                dlgTable::OnChangeVacuum)
    EVT_TEXT(XRCID("txtFactorToastAn"),                 dlgTable::OnChangeVacuum)
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
    return new dlgTable(this, frame, (pgTable*)node, (pgSchema*)parent);
}

dlgProperty *gpPartitionFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
    return new dlgTable(this, frame, (gpPartition*)node, (pgSchema*)parent);
}

dlgTable::dlgTable(pgaFactory *f, frmMain *frame, pgTable *node, pgSchema *sch)
: dlgSecurityProperty(f, frame, node, wxT("dlgTable"), wxT("INSERT,SELECT,UPDATE,DELETE,RULE,REFERENCES,TRIGGER"), "arwdRxt")
{
    schema=sch;
    table=node;

    btnAddTable->Disable();
    btnRemoveTable->Disable();

    lstColumns->AddColumn(_("Column name"), 90);
    lstColumns->AddColumn(_("Definition"), 135);
#ifndef __WXMAC__
    lstColumns->AddColumn(_("Inherited from table"), 40);
#else
    lstColumns->AddColumn(_("Inherited from table"), 80);
#endif
    lstColumns->AddColumn(_("Column definition"), 0);
    lstColumns->AddColumn(_("Column comment"), 0);
    lstColumns->AddColumn(_("Column statistics"), 0);
    lstColumns->AddColumn(_("Column"), 0);

    lstConstraints->CreateColumns(0, _("Constraint name"), _("Definition"), 90);
}

dlgTable::~dlgTable()
{
    //Clear the cached datatypes
    size_t i;
    for (i = 0; i < dtCache.GetCount(); i++)
        delete dtCache.Item(i);
}

pgObject *dlgTable::GetObject()
{
    return table;
}


int dlgTable::Go(bool modal)
{
    if (!table)
        cbOwner->Append(wxT(""));
    AddGroups(cbOwner);
    AddUsers(cbOwner);
    PrepareTablespace(cbTablespace);
    PopulateDatatypeCache();

    // new of type combobox
    wxString typeQuery = wxT("SELECT t.oid, t.typname ")
        wxT("FROM pg_type t, pg_namespace n ")
        wxT("WHERE t.typtype='c' AND t.typnamespace=n.oid ")
        wxT("AND NOT (n.nspname like 'pg_%' OR n.nspname='information_schema') ")
        wxT("ORDER BY typname");
    cbOfType->Insert(wxEmptyString, 0, (void *)0);
    cbOfType->FillOidKey(connection, typeQuery);
    cbOfType->SetSelection(0);

    hasPK=false;

    if (table)
    {
        // edit mode
        chkHasOids->SetValue(table->GetHasOids());

        if (table->GetTablespaceOid() != 0)
            cbTablespace->SetKey(table->GetTablespaceOid());

        if (table->GetOfTypeOid() != 0)
            cbOfType->SetKey(table->GetOfTypeOid());

        inheritedTableOids=table->GetInheritedTablesOidList();

        wxArrayString qitl=table->GetQuotedInheritedTablesList();
        size_t i;
        for (i=0 ; i < qitl.GetCount() ; i++)
        {
            previousTables.Add(qitl.Item(i));
            lbTables->Append(qitl.Item(i));
        }

        btnAddTable->Enable(connection->BackendMinimumVersion(8, 2) && cbTables->GetGuessedSelection() >= 0);
        lbTables->Enable(connection->BackendMinimumVersion(8, 2));
        chkHasOids->Enable(table->GetHasOids() && connection->BackendMinimumVersion(8, 0));
        cbTablespace->Enable(connection->BackendMinimumVersion(7, 5));

        wxCookieType cookie;
        pgObject *data=0;
        wxTreeItemId item=mainForm->GetBrowser()->GetFirstChild(table->GetId(), cookie);
        while (item)
        {
            data=mainForm->GetBrowser()->GetObject(item);
            pgaFactory *factory=data->GetFactory();
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

            item=mainForm->GetBrowser()->GetNextChild(table->GetId(), cookie);
        }

        if (columnsItem)
        {
            pgCollection *coll=(pgCollection*)data;
            // make sure all columns are appended
            coll->ShowTreeDetail(mainForm->GetBrowser());
            // this is the columns collection
            item=mainForm->GetBrowser()->GetFirstChild(columnsItem, cookie);

            // add columns
            while (item)
            {
                data=mainForm->GetBrowser()->GetObject(item);
                if (data->IsCreatedBy(columnFactory))
                {
                    pgColumn *column=(pgColumn*)data;
                    // make sure column details are read
                    column->ShowTreeDetail(mainForm->GetBrowser());

                    if (column->GetColNumber() > 0)
                    {
                        bool inherited = (column->GetInheritedCount() != 0);
                        int pos=lstColumns->AppendItem((inherited ? tableFactory.GetIconId() : column->GetIconId()), 
                            column->GetName(), column->GetDefinition());
                        previousColumns.Add(column->GetQuotedIdentifier() 
                            + wxT(" ") + column->GetDefinition());
                        lstColumns->SetItem(pos, 6, NumToStr((long)column));
                        if (inherited)
                            lstColumns->SetItem(pos, 2, column->GetInheritedTableName());
                    }
                }
                
                item=mainForm->GetBrowser()->GetNextChild(columnsItem, cookie);
            }
        }
        if (constraintsItem)
        {
            pgCollection *coll=(pgCollection*)mainForm->GetBrowser()->GetObject(constraintsItem);
            // make sure all constraints are appended
            coll->ShowTreeDetail(mainForm->GetBrowser());
            // this is the constraints collection
            item=mainForm->GetBrowser()->GetFirstChild(constraintsItem, cookie);

            // add constraints
            while (item)
            {
                data=mainForm->GetBrowser()->GetObject(item);
                switch (data->GetMetaType())
                {
                    case PGM_PRIMARYKEY:
                        hasPK = true;
                    case PGM_UNIQUE:
                    {
                        pgIndexConstraint *obj=(pgIndexConstraint*)data;

                        lstConstraints->AppendItem(data->GetIconId(), obj->GetName(), obj->GetDefinition());
                        previousConstraints.Add(obj->GetQuotedIdentifier() 
                            + wxT(" ") + obj->GetTypeName().Upper() + wxT(" ") + obj->GetDefinition());
                        break;
                    }
                    case PGM_EXCLUDE:
                    {
                        pgIndexConstraint *obj=(pgIndexConstraint*)data;

                        lstConstraints->AppendItem(data->GetIconId(), obj->GetName(), obj->GetDefinition());
                        previousConstraints.Add(obj->GetQuotedIdentifier() 
                            + wxT(" ") + obj->GetTypeName().Upper() + wxT(" ") + obj->GetDefinition());
                        break;
                    }
                    case PGM_FOREIGNKEY:
                    {
                        pgForeignKey *obj=(pgForeignKey*)data;
                        wxString def = obj->GetDefinition();

                        def.Replace(wxT("\n"), wxT(" "));
                        while (def.Contains(wxT("  ")))
                            def.Replace(wxT("  "), wxT(" "));

                        lstConstraints->AppendItem(data->GetIconId(), obj->GetName(), def);
                        previousConstraints.Add(obj->GetQuotedIdentifier() 
                            + wxT(" ") + obj->GetTypeName().Upper() + wxT(" ") + def);
                        break;
                    }
                    case PGM_CHECK:
                    {
                        pgCheck *obj=(pgCheck*)data;

                       lstConstraints->AppendItem(data->GetIconId(), obj->GetName(), obj->GetDefinition());
                        previousConstraints.Add(obj->GetQuotedIdentifier() 
                            + wxT(" ") + obj->GetTypeName().Upper() + wxT(" ") + obj->GetDefinition());
                        break;
                    }
                }
                
                item=mainForm->GetBrowser()->GetNextChild(constraintsItem, cookie);
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
    }

    cbOfType->Enable(connection->BackendMinimumVersion(9, 0) && !table);
    cbTables->Enable(connection->BackendMinimumVersion(8, 2) && cbOfType->GetCurrentSelection() == 0);

    if (connection->BackendMinimumVersion(8,2) || !table)
    {
        wxString systemRestriction;
        if (!settings->GetShowSystemObjects())
        systemRestriction = 
            wxT("   AND ") + connection->SystemNamespaceRestriction(wxT("n.nspname"));
        
        if (table)
        {
            wxString oids = table->GetOidStr();
            int i;
            for (i=0 ; i < (int)inheritedTableOids.GetCount() ; i++)
            {
                oids += wxT(", ") + inheritedTableOids.Item(i);
            }
            if (oids.Length() > 0)
                systemRestriction += wxT(" AND c.oid NOT IN (") + oids + wxT(")");
        }
    
        pgSet *set=connection->ExecuteSet(
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

    if ((connection->BackendMinimumVersion(8,1) && table) || connection->BackendMinimumVersion(8,4))
    {
        txtBaseVac->SetValidator(numericValidator);
        txtBaseAn->SetValidator(numericValidator);
        txtFactorVac->SetValidator(numericValidator);
        txtFactorAn->SetValidator(numericValidator);
        txtVacDelay->SetValidator(numericValidator);
        txtVacLimit->SetValidator(numericValidator);
        txtFreezeMinAge->SetValidator(numericValidator);
        txtFreezeMaxAge->SetValidator(numericValidator);
        if (connection->BackendMinimumVersion(8,4))
        {
            txtFreezeTableAge->SetValidator(numericValidator);

            txtBaseToastVac->SetValidator(numericValidator);
            txtBaseToastAn->SetValidator(numericValidator);
            txtFactorToastVac->SetValidator(numericValidator);
            txtFactorToastAn->SetValidator(numericValidator);
            txtToastVacDelay->SetValidator(numericValidator);
            txtToastVacLimit->SetValidator(numericValidator);
            txtToastFreezeMinAge->SetValidator(numericValidator);
            txtToastFreezeMaxAge->SetValidator(numericValidator);
            txtToastFreezeTableAge->SetValidator(numericValidator);
        }
        else
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
            wxString name=avSet.GetVal(wxT("name"));
            wxString setting=avSet.GetVal(wxT("setting"));

            if (name == wxT("autovacuum_vacuum_cost_delay"))
                settingCostDelay = StrToLong(setting);
            else if (name == wxT("vacuum_cost_delay"))
            {
                if (settingCostDelay < 0)
                    settingCostDelay = StrToLong(setting);
            }
            else if (name == wxT("autovacuum_vacuum_cost_limit"))
                settingCostLimit = StrToLong(setting);
            else if (name == wxT("vacuum_cost_limit"))
            {
                if (settingCostLimit < 0)
                    settingCostLimit = StrToLong(setting);
            }
            else if (name == wxT("autovacuum_vacuum_scale_factor"))
                settingVacFactor = StrToDouble(setting);
            else if (name == wxT("autovacuum_analyze_scale_factor"))
                settingAnlFactor = StrToDouble(setting);
            else if (name == wxT("autovacuum_vacuum_threshold"))
                settingVacBaseThr = StrToLong(setting);
            else if (name == wxT("autovacuum_analyze_threshold"))
                settingAnlBaseThr = StrToLong(setting);
            else if (name == wxT("vacuum_freeze_min_age"))
                settingFreezeMinAge = StrToLong(setting);
            else if (name == wxT("autovacuum_freeze_max_age"))
                settingFreezeMaxAge = StrToLong(setting);
            else if (name == wxT("vacuum_freeze_table_age"))
                settingFreezeTableAge = StrToLong(setting);
            else
                settingAutoVacuum = avSet.GetBool(wxT("setting"));
        }

        tableVacBaseThr = -1;
        tableAnlBaseThr = -1;
        tableCostDelay = -1;
        tableCostLimit = -1;
        tableFreezeMinAge = -1;
        tableFreezeMaxAge = -1;
        tableVacFactor = -1;
        tableAnlFactor = -1;
        tableFreezeTableAge = -1;

        toastTableVacBaseThr = -1;
        toastTableAnlBaseThr = -1;
        toastTableCostDelay = -1;
        toastTableCostLimit = -1;
        toastTableFreezeMinAge = -1;
        toastTableFreezeMaxAge = -1;
        toastTableVacFactor = -1;
        toastTableAnlFactor = -1;
        toastTableFreezeTableAge = -1;

        toastTableHasVacuum = false;
        toastTableVacEnabled = false;

        if (!connection->BackendMinimumVersion(8,4))
        {
            pgSetIterator set(connection, wxT("SELECT * FROM pg_autovacuum WHERE vacrelid=") + table->GetOidStr());
            if (set.RowsLeft())
            {
                hasVacuum=true;
    
                tableVacEnabled = set.GetBool(wxT("enabled"));
                chkVacEnabled->SetValue(tableVacEnabled);
                
                tableVacBaseThr=set.GetLong(wxT("vac_base_thresh"));
                tableAnlBaseThr=set.GetLong(wxT("anl_base_thresh"));
                tableCostDelay=set.GetLong(wxT("vac_cost_delay"));
                tableCostLimit=set.GetLong(wxT("vac_cost_limit"));
                tableVacFactor=set.GetDouble(wxT("vac_scale_factor"));
                tableAnlFactor=set.GetDouble(wxT("anl_scale_factor"));
    
                if (connection->BackendMinimumVersion(8, 2))
                {
                    tableFreezeMinAge=set.GetLong(wxT("freeze_min_age"));
                    tableFreezeMaxAge=set.GetLong(wxT("freeze_max_age"));
                }
            }
            else
            {
               hasVacuum=false;
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
                table->GetAutoVacuumVacuumThreshold().ToLong(&tableVacBaseThr);
            if (!table->GetAutoVacuumAnalyzeThreshold().IsEmpty())
                table->GetAutoVacuumAnalyzeThreshold().ToLong(&tableAnlBaseThr);
            if (!table->GetAutoVacuumVacuumScaleFactor().IsEmpty())
                table->GetAutoVacuumVacuumScaleFactor().ToDouble(&tableVacFactor);
            if (!table->GetAutoVacuumAnalyzeScaleFactor().IsEmpty())
                table->GetAutoVacuumAnalyzeScaleFactor().ToDouble(&tableAnlFactor);
            if (!table->GetAutoVacuumVacuumCostDelay().IsEmpty())
                table->GetAutoVacuumVacuumCostDelay().ToLong(&tableCostDelay);
            if (!table->GetAutoVacuumVacuumCostLimit().IsEmpty())
                table->GetAutoVacuumVacuumCostLimit().ToLong(&tableCostLimit);
            if (!table->GetAutoVacuumFreezeMinAge().IsEmpty())
                table->GetAutoVacuumFreezeMinAge().ToLong(&tableFreezeMinAge);
            if (!table->GetAutoVacuumFreezeMaxAge().IsEmpty())
                table->GetAutoVacuumFreezeMaxAge().ToLong(&tableFreezeMaxAge);
            if (!table->GetAutoVacuumFreezeTableAge().IsEmpty())
                table->GetAutoVacuumFreezeTableAge().ToLong(&tableFreezeTableAge);

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
                       table->GetToastAutoVacuumVacuumThreshold().ToLong(&toastTableVacBaseThr);
                   if (!table->GetToastAutoVacuumAnalyzeThreshold().IsEmpty())
                       table->GetToastAutoVacuumAnalyzeThreshold().ToLong(&toastTableAnlBaseThr);
                   if (!table->GetToastAutoVacuumVacuumScaleFactor().IsEmpty())
                       table->GetToastAutoVacuumVacuumScaleFactor().ToDouble(&toastTableVacFactor);
                   if (!table->GetToastAutoVacuumAnalyzeScaleFactor().IsEmpty())
                       table->GetToastAutoVacuumAnalyzeScaleFactor().ToDouble(&toastTableAnlFactor);
                   if (!table->GetToastAutoVacuumVacuumCostDelay().IsEmpty())
                       table->GetToastAutoVacuumVacuumCostDelay().ToLong(&toastTableCostDelay);
                   if (!table->GetToastAutoVacuumVacuumCostLimit().IsEmpty())
                       table->GetToastAutoVacuumVacuumCostLimit().ToLong(&toastTableCostLimit);
                   if (!table->GetToastAutoVacuumFreezeMinAge().IsEmpty())
                       table->GetToastAutoVacuumFreezeMinAge().ToLong(&toastTableFreezeMinAge);
                   if (!table->GetToastAutoVacuumFreezeMaxAge().IsEmpty())
                       table->GetToastAutoVacuumFreezeMaxAge().ToLong(&toastTableFreezeMaxAge);
                   if (!table->GetToastAutoVacuumFreezeTableAge().IsEmpty())
                       table->GetToastAutoVacuumFreezeTableAge().ToLong(&toastTableFreezeTableAge);
               }
               chkToastVacEnabled->SetValue(toastTableHasVacuum ? toastTableVacEnabled : settingAutoVacuum);
            }
        }
        else
        {
            hasVacuum=false;
            chkVacEnabled->SetValue(settingAutoVacuum);
        }

        if (tableVacBaseThr >= 0)
            txtBaseVac->SetValue(NumToStr(tableVacBaseThr));
        else
            txtBaseVac->SetValue(wxEmptyString);

        if (tableAnlBaseThr >= 0)
            txtBaseAn->SetValue(NumToStr(tableAnlBaseThr));
        else
            txtBaseAn->SetValue(wxEmptyString);

        if (tableVacFactor >= 0)
            txtFactorVac->SetValue(NumToStr(tableVacFactor));
        else
            txtFactorVac->SetValue(wxEmptyString);

        if (tableAnlFactor >= 0)
            txtFactorAn->SetValue(NumToStr(tableAnlFactor));
        else
          txtFactorAn->SetValue(wxEmptyString);

        if (tableCostDelay >= 0)
            txtVacDelay->SetValue(NumToStr(tableCostDelay));
        else
            txtVacDelay->SetValue(wxEmptyString);

        if (tableCostLimit >= 0)
            txtVacLimit->SetValue(NumToStr(tableCostLimit));
        else
            txtVacLimit->SetValue(wxEmptyString);

        if (connection->BackendMinimumVersion(8, 2))
        {
            if (tableFreezeMinAge >= 0)
                txtFreezeMinAge->SetValue(NumToStr(tableFreezeMinAge));
            else
                txtFreezeMinAge->SetValue(wxEmptyString);

            if (tableFreezeMaxAge >= 0)
                txtFreezeMaxAge->SetValue(NumToStr(tableFreezeMaxAge));
            else
                txtFreezeMaxAge->SetValue(wxEmptyString);
        }
        if (connection->BackendMinimumVersion(8, 4))
        {
            if (tableFreezeTableAge >= 0)
                txtFreezeTableAge->SetValue(NumToStr(tableFreezeTableAge));
            else
                txtFreezeTableAge->SetValue(wxEmptyString);

            if (toastTableVacBaseThr >= 0)
                txtBaseToastVac->SetValue(NumToStr(toastTableVacBaseThr));
            else
                txtBaseToastVac->SetValue(wxEmptyString);
    
            if (toastTableAnlBaseThr >= 0)
                txtBaseToastAn->SetValue(NumToStr(toastTableAnlBaseThr));
            else
                txtBaseToastAn->SetValue(wxEmptyString);
    
            if (toastTableVacFactor >= 0)
                txtFactorToastVac->SetValue(NumToStr(toastTableVacFactor));
            else
                txtFactorToastVac->SetValue(wxEmptyString);
    
            if (toastTableAnlFactor >= 0)
                txtFactorToastAn->SetValue(NumToStr(toastTableAnlFactor));
            else
              txtFactorToastAn->SetValue(wxEmptyString);
    
            if (toastTableCostDelay >= 0)
                txtToastVacDelay->SetValue(NumToStr(toastTableCostDelay));
            else
                txtToastVacDelay->SetValue(wxEmptyString);
    
            if (toastTableCostLimit >= 0)
                txtToastVacLimit->SetValue(NumToStr(toastTableCostLimit));
            else
                txtToastVacLimit->SetValue(wxEmptyString);
    
            if (toastTableFreezeMinAge >= 0)
                txtToastFreezeMinAge->SetValue(NumToStr(toastTableFreezeMinAge));
            else
                txtToastFreezeMinAge->SetValue(wxEmptyString);

            if (toastTableFreezeMaxAge >= 0)
                txtToastFreezeMaxAge->SetValue(NumToStr(toastTableFreezeMaxAge));
            else
                txtToastFreezeMaxAge->SetValue(wxEmptyString);
   
            if (toastTableFreezeTableAge >= 0)
                txtToastFreezeTableAge->SetValue(NumToStr(toastTableFreezeTableAge));
            else
                txtToastFreezeTableAge->SetValue(wxEmptyString);

            chkCustomToastVac->SetValue(toastTableHasVacuum);
            chkToastVacEnabled->SetValue(toastTableHasVacuum?toastTableVacEnabled:settingAutoVacuum);
        }
        chkCustomVac->SetValue(hasVacuum);
        wxCommandEvent ev;
        OnChangeVacuum(ev);
    }
    else
    {
        /* Remove 'Vacuum Settings' Page */
        nbNotebook->DeletePage(4);
    }

    // Find, and disable the RULE ACL option if we're 8.2
    if (connection->BackendMinimumVersion(8, 2))
    {
        // Disable the checkbox
        if (!DisablePrivilege(wxT("RULE")))
            wxLogError(_("Failed to disable the RULE privilege checkbox!"));

        if (table)
        {
            txtFillFactor->SetValue(table->GetFillFactor());
            txtFillFactor->Disable();
        }
        else
        {
            txtFillFactor->Enable();
            txtFillFactor->SetValidator(numericValidator);
        }
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
    wxString sql;
    wxString tabname=schema->GetQuotedPrefix() + qtIdent(GetName());

    if (table)
    {
        int pos;
        int index=-1;

        wxString definition;

        wxArrayString tmpDef=previousColumns;
        wxString tmpsql;

        // Build a temporary list of ADD COLUMNs, and fixup the list to remove
        for (pos=0; pos < lstColumns->GetItemCount() ; pos++)
        {
            index = -1;
            if (lstColumns->GetText(pos, 2).IsEmpty())
            {
                definition = lstColumns->GetText(pos, 3);
                if (definition.IsEmpty())
                {
                    definition=qtIdent(lstColumns->GetText(pos)) + wxT(" ") + lstColumns->GetText(pos, 1);
                    index=tmpDef.Index(definition);
                    if (index < 0)
                        tmpsql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                            +  wxT(" ADD COLUMN ") + definition + wxT(";\n");
                }
                else
                {
                    tmpsql += definition;

                    pgColumn *column=(pgColumn*) StrToLong(lstColumns->GetText(pos, 6));
                    if (column)
                    {
                        index=tmpDef.Index(column->GetQuotedIdentifier() 
                                    + wxT(" ") + column->GetDefinition());
                    }
                }
            }
            else
            {
                if (! lstColumns->GetText(pos, 2).IsEmpty())
                {
                    definition=qtIdent(lstColumns->GetText(pos)) + wxT(" ") + lstColumns->GetText(pos, 1);
                    index=tmpDef.Index(definition);
                }
            }
            if (index >= 0 && index < (int)tmpDef.GetCount())
                tmpDef.RemoveAt(index);
        }


        for (index=0 ; index < (int)tmpDef.GetCount() ; index++)
        {
            definition = tmpDef.Item(index);
            if (definition[0U] == '"')
                definition = definition.Mid(1).BeforeFirst('"');
            else
                definition = definition.BeforeFirst(' ');
            sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                +  wxT(" DROP COLUMN ") + qtIdent(definition) + wxT(";\n");
        }
        // Add the ADD COLUMNs...
        sql += tmpsql;

        AppendNameChange(sql);
        AppendOwnerChange(sql, wxT("TABLE ") + tabname);

        tmpDef=previousTables;
        tmpsql.Empty();

        // Build a temporary list of INHERIT tables, and fixup the list to remove
        for (pos = 0 ; pos < (int)lbTables->GetCount() ; pos++)
        {
            definition = lbTables->GetString(pos);
            index = tmpDef.Index(definition);
            if (index < 0)
                tmpsql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                    +  wxT(" INHERIT ") + definition + wxT(";\n");
            else
                tmpDef.RemoveAt(index);
        }

        for (index = 0 ; index < (int)tmpDef.GetCount() ; index++)
        {
            definition = tmpDef.Item(index);
            sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                +  wxT(" NO INHERIT ") + qtIdent(definition) + wxT(";\n");
        }
        // Add the INHERIT COLUMNs...
        sql += tmpsql;

        tmpDef=previousConstraints;
        tmpsql.Empty();

        // Build a temporary list of ADD CONSTRAINTs, and fixup the list to remove
        for (pos=0; pos < lstConstraints->GetItemCount() ; pos++)
        {
            wxString conname= qtIdent(lstConstraints->GetItemText(pos));
            definition = conname;
            definition += wxT(" ") + GetItemConstraintType(lstConstraints, pos) 
                        + wxT(" ") + lstConstraints->GetText(pos, 1);
            index=tmpDef.Index(definition);
            if (index >= 0)
                tmpDef.RemoveAt(index);
            else
            {
                tmpsql += wxT("ALTER TABLE ") + tabname
                    +  wxT(" ADD");
                if (!conname.IsEmpty())
                    tmpsql += wxT(" CONSTRAINT ");

                tmpsql += definition + wxT(";\n");
            }
        }

        for (index=0 ; index < (int)tmpDef.GetCount() ; index++)
        {
            definition = tmpDef.Item(index);
            if (definition[0U] == '"')
                definition = definition.Mid(1).BeforeFirst('"');
            else
                definition = definition.BeforeFirst(' ');
                sql = wxT("ALTER TABLE ") + tabname
                    + wxT(" DROP CONSTRAINT ") + qtIdent(definition) + wxT(";\n")
                    + sql;

        }
        // Add the ADD CONSTRAINTs...
        sql += tmpsql;

        if (chkHasOids->GetValue() != table->GetHasOids())
        {
            sql += wxT("ALTER TABLE ") + tabname 
                +  wxT(" SET WITHOUT OIDS;\n");
        }
        if (connection->BackendMinimumVersion(8, 0) && cbTablespace->GetOIDKey() != table->GetTablespaceOid())
            sql += wxT("ALTER TABLE ") + tabname 
                +  wxT(" SET TABLESPACE ") + qtIdent(cbTablespace->GetValue())
                + wxT(";\n");

        if (connection->BackendMinimumVersion(8,1))
        {
            if (!chkCustomVac->GetValue())
            {
                if (hasVacuum)
                {
                    if (connection->BackendMinimumVersion(8,4))
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

                newVal = AppendNum(valChanged, txtBaseToastAn, toastTableAnlBaseThr);
                if (valChanged)
                {
                    valChanged = false;
                    FillAutoVacuumParameters(setStr, resetStr, wxT("toast.autovacuum_analyze_threshold"), newVal);
                }

                newVal = AppendNum(valChanged, txtFactorToastVac, toastTableVacFactor);
                if (valChanged)
                {
                    valChanged = false;
                    FillAutoVacuumParameters(setStr, resetStr, wxT("toast.autovacuum_vacuum_scale_factor"), newVal);
                }

                newVal = AppendNum(valChanged, txtFactorToastAn, toastTableAnlFactor);
                if (valChanged)
                {
                    valChanged = false;
                    FillAutoVacuumParameters(setStr, resetStr, wxT("toast.autovacuum_analyze_scale_factor"), newVal);
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
    }
    else
    {
        bool typedTable = cbOfType->GetCurrentSelection() > 0 && cbOfType->GetOIDKey() > 0;
        sql = wxT("CREATE TABLE ") + tabname;
        if (typedTable)
            sql += wxT("\nOF ") + qtIdent(cbOfType->GetValue());

        if (!typedTable || (typedTable && lstConstraints->GetItemCount() > 0))
            sql += wxT("\n(");

        int pos;
        bool needComma=false;
        if (!typedTable)
        {
            for (pos=0 ; pos < lstColumns->GetItemCount() ; pos++)
            {
                if (lstColumns->GetText(pos, 2).IsEmpty())
                {
                    // standard definition, not inherited
                    if (needComma)
                        sql += wxT(", ");
                    else
                        needComma=true;

                    wxString name=lstColumns->GetText(pos);
                    wxString definition = lstColumns->GetText(pos, 1);

                    sql += wxT("\n   ") + qtIdent(name)
                        + wxT(" ") + definition;
                }
            }
        }

        for (pos=0 ; pos < lstConstraints->GetItemCount() ; pos++)
        {
            wxString name=lstConstraints->GetItemText(pos);
            wxString definition = lstConstraints->GetText(pos, 1);

            if (needComma)
                sql += wxT(", ");
            else
                needComma=true;

            sql += wxT("\n   ");
            AppendIfFilled(sql, wxT("CONSTRAINT "), qtIdent(name));

            sql += wxT(" ") + GetItemConstraintType(lstConstraints, pos) + wxT(" ") + definition;
        }
        
        if (!typedTable || (typedTable && lstConstraints->GetItemCount() > 0))
            sql += wxT("\n) ");


        if (lbTables->GetCount() > 0)
        {
            sql += wxT("\nINHERITS (");

            unsigned int i;
            for (i=0 ; i < lbTables->GetCount() ; i++)
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

                newVal = AppendNum(valChanged, txtBaseToastAn, toastTableAnlBaseThr);
                if (valChanged)
                {
                    valChanged = false;
                    FillAutoVacuumParameters(sql, resetStr, wxT("toast.autovacuum_analyze_threshold"), newVal);
                }

                newVal = AppendNum(valChanged, txtFactorToastVac, toastTableVacFactor);
                if (valChanged)
                {
                    valChanged = false;
                    FillAutoVacuumParameters(sql, resetStr, wxT("toast.autovacuum_vacuum_scale_factor"), newVal);
                }

                newVal = AppendNum(valChanged, txtFactorToastAn, toastTableAnlFactor);
                if (valChanged)
                {
                    valChanged = false;
                    FillAutoVacuumParameters(sql, resetStr, wxT("toast.autovacuum_analyze_scale_factor"), newVal);
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
        for (pos=0 ; pos < lstColumns->GetItemCount() ; pos++)
        {
            if (!lstColumns->GetText(pos, 4).IsEmpty())
                sql += wxT("ALTER TABLE ") + tabname
                    + wxT(" ALTER COLUMN ") + qtIdent(lstColumns->GetText(pos, 0))
                    + wxT(" SET STATISTICS ") + lstColumns->GetText(pos, 4)
                    + wxT(";\n");
        }

        // Comments
        for (pos=0 ; pos < lstColumns->GetItemCount() ; pos++)
        {
            if (!lstColumns->GetText(pos, 5).IsEmpty())
                sql += wxT("COMMENT ON COLUMN ") + tabname
                    + wxT(".") + qtIdent(lstColumns->GetText(pos, 0))
                    + wxT(" IS ") + qtDbString(lstColumns->GetText(pos, 5))
                    + wxT(";\n");
        }

    }

    AppendComment(sql, wxT("TABLE"), schema, table);

    if (connection->BackendMinimumVersion(8, 2))
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
    wxString name=GetName();

    pgObject *obj=tableFactory.CreateObjects(collection, 0, wxT(
        "\n   AND rel.relname=") + qtDbString(name) + wxT(
        "\n   AND rel.relnamespace=") + schema->GetOidStr());

    return obj;
}


wxString dlgTable::GetNumString(wxTextCtrl *ctl, bool enabled, const wxString &val)
{
    if (!enabled)
        return val;
    wxString str=ctl->GetValue();
    if (str.IsEmpty() || StrToLong(val) < 0)
        return val;
    else
        return str;
}


wxString dlgTable::AppendNum(bool &changed, wxTextCtrl *ctl, long val)
{
    wxString str=ctl->GetValue();
    long v=StrToLong(str);
    if (str.IsEmpty() || v < 0)
        v=-1;
    
    changed |= (v != val);
    return NumToStr(v);
}


wxString dlgTable::AppendNum(bool &changed, wxTextCtrl *ctl, double val)
{
    wxString str=ctl->GetValue();
    double v=StrToDouble(str);
    if (str.IsEmpty() || v < 0)
        v=-1.;

    changed |= (v != val);
    return NumToStr(v);
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
        bool vacEn=chkCustomVac->GetValue() && chkVacEnabled->GetValue();
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

        stBaseVacCurr->SetLabel(NumToStr((tableVacBaseThr == -1) ? settingVacBaseThr : tableVacBaseThr));
        stBaseAnCurr->SetLabel(NumToStr((tableAnlBaseThr == -1) ? settingAnlBaseThr : tableAnlBaseThr));
        stFactorVacCurr->SetLabel(NumToStr((tableVacFactor == -1) ? settingVacFactor : tableVacFactor));
        stFactorAnCurr->SetLabel(NumToStr((tableAnlFactor == -1) ? settingAnlFactor : tableAnlFactor));
        stVacDelayCurr->SetLabel(NumToStr((tableCostDelay == -1) ? settingCostDelay : tableCostDelay));
        stVacLimitCurr->SetLabel(NumToStr((tableCostLimit == -1) ? settingCostLimit : tableCostLimit));

        if (connection->BackendMinimumVersion(8, 2))
        {
            stFreezeMinAgeCurr->SetLabel(NumToStr((tableFreezeMinAge == -1) ? settingFreezeMinAge : tableFreezeMinAge));
            stFreezeMaxAgeCurr->SetLabel(NumToStr((tableFreezeMaxAge == -1) ? settingFreezeMaxAge : tableFreezeMaxAge));
        }
        if (connection->BackendMinimumVersion(8, 4))
        {
            txtFreezeTableAge->Enable(vacEn);
            stFreezeTableAgeCurr->SetLabel(NumToStr((tableFreezeTableAge == -1) ? settingFreezeTableAge : tableFreezeTableAge));

            /* Toast Table Vacuum Settings */
            bool toastVacEn=chkCustomToastVac->GetValue() && chkToastVacEnabled->GetValue();
            chkToastVacEnabled->Enable(chkCustomToastVac->GetValue());
    
            txtBaseToastVac->Enable(toastVacEn);
            txtBaseToastAn->Enable(toastVacEn);
            txtFactorToastVac->Enable(toastVacEn);
            txtFactorToastAn->Enable(toastVacEn);
            txtToastVacDelay->Enable(toastVacEn);
            txtToastVacLimit->Enable(toastVacEn);
            txtToastFreezeMinAge->Enable(toastVacEn);
            txtToastFreezeMaxAge->Enable(toastVacEn);
            txtToastFreezeTableAge->Enable(toastVacEn);
    
            stBaseToastVacCurr->SetLabel(NumToStr((toastTableVacBaseThr == -1) ? settingVacBaseThr : toastTableVacBaseThr));
            stBaseToastAnCurr->SetLabel(NumToStr((toastTableAnlBaseThr == -1) ? settingAnlBaseThr : toastTableAnlBaseThr));
            stFactorToastVacCurr->SetLabel(NumToStr((toastTableVacFactor == -1) ? settingVacFactor : toastTableVacFactor));
            stFactorToastAnCurr->SetLabel(NumToStr((toastTableAnlFactor == -1) ? settingAnlFactor : toastTableAnlFactor));
            stToastVacDelayCurr->SetLabel(NumToStr((toastTableCostDelay == -1) ? settingCostDelay : toastTableCostDelay));
            stToastVacLimitCurr->SetLabel(NumToStr((toastTableCostLimit == -1) ? settingCostLimit : toastTableCostLimit));
            stToastFreezeMinAgeCurr->SetLabel(NumToStr((toastTableFreezeMinAge == -1) ? settingFreezeMinAge : toastTableFreezeMinAge));
            stToastFreezeMaxAgeCurr->SetLabel(NumToStr((toastTableFreezeMaxAge == -1) ? settingFreezeMaxAge : toastTableFreezeMaxAge));
            txtToastFreezeTableAge->Enable(toastVacEn);
            stToastFreezeTableAgeCurr->SetLabel(NumToStr((toastTableFreezeTableAge == -1) ? settingFreezeTableAge : toastTableFreezeTableAge));
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
    if (table)
    {
        bool changed=false;
        if (connection->BackendMinimumVersion(7, 4) || lstColumns->GetItemCount() > 0)
        {
            changed = !GetSql().IsEmpty();
        }
        EnableOK(changed);
    }
    else
    {
        wxString name=GetName();
        bool enable=true;
        CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
        CheckValid(enable, connection->BackendMinimumVersion(7, 4) || lstColumns->GetItemCount() > 0, 
            _("Please specify columns."));
        EnableOK(enable);
    }
}


void dlgTable::OnAddTable(wxCommandEvent &ev)
{
    int sel=cbTables->GetGuessedSelection();
    if (sel >= 0)
    {
        wxString tabname=cbTables->GetValue();
        wxString taboid=tableOids.Item(sel);
        inheritedTableOids.Add(taboid);
        tableOids.RemoveAt(sel);

        lbTables->Append(tabname);
        cbTables->Delete(sel);

        pgSet *set=connection->ExecuteSet(
            wxT("SELECT attname, format_type(atttypid, NULL) AS atttype FROM pg_attribute\n")
            wxT (" WHERE NOT attisdropped AND attnum>0 AND attrelid=") + taboid);
        if (set)
        {
            bool found;
            while (!set->Eof())
            {
                found = false;

                size_t row=lstColumns->GetItemCount();
                while (row--)
                {
                    if (set->GetVal(wxT("attname")).Cmp(lstColumns->GetText(row, 0)) == 0)
                    {
                        found = true;
                        break;
                    }
                }

                if (found)
                {
                    lstColumns->SetItem(row, 2, tabname);
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
        if (wxMessageBox(_("Are you sure you wish to remove the selected table?"), _("Remove table?"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION) == wxNO)
            return;
    }

    int sel=lbTables->GetSelection();
    if (sel >= 0)
    {
        wxString tabname=lbTables->GetStringSelection();
        tableOids.Add(inheritedTableOids.Item(sel));
        inheritedTableOids.RemoveAt(sel);

        lbTables->Delete(sel);
        cbTables->Append(tabname);

        size_t row=lstColumns->GetItemCount();
        while (row--)
        {
            if (tabname == lstColumns->GetText(row, 2))
            {
                lstColumns->SetItem(row, 2, wxT(""));
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
                hasPK = false;
                FillConstraint();
            }
            else
            {
                cbOfType->SetSelection(0);
                return;
            }
        }

        pgSet *set=connection->ExecuteSet(
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
    long pos=lstColumns->GetSelection();
    pgColumn *column=(pgColumn*) StrToLong(lstColumns->GetText(pos, 6));

    dlgColumn col(&columnFactory, mainForm, column, table);
    col.CenterOnParent();
    col.SetDatabase(database);
    if (col.Go(true) != wxID_CANCEL)
    {
        lstColumns->SetItem(pos, 0, col.GetName());
        lstColumns->SetItem(pos, 1, col.GetDefinition());
        lstColumns->SetItem(pos, 3, col.GetSql());
        lstColumns->SetItem(pos, 4, col.GetStatistics());
        lstColumns->SetItem(pos, 5, col.GetComment());
    }
    CheckChange();
}

// Cache datatypes to avoid multiple calls to server when adding multiple columns to a table. 
void dlgTable::PopulateDatatypeCache()
{
    DatatypeReader tr(database, true);
    while (tr.HasMore())
    {
        pgDatatype dt=tr.GetDatatype();

        dataType *dType = new dataType();
        dType->SetOid(tr.GetOid());
        dType->SetTypename(dt.FullName());
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
            lstColumns->SetItem(pos, 3, col.GetSql());
        lstColumns->SetItem(pos, 4, col.GetStatistics());
        lstColumns->SetItem(pos, 5, col.GetComment());

    }

    CheckChange();
}


void dlgTable::OnRemoveCol(wxCommandEvent &ev)
{
    if (settings->GetConfirmDelete())
    {
        if (wxMessageBox(_("Are you sure you wish to remove the selected column?"), _("Remove column?"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION) == wxNO)
            return;
    }

    lstColumns->DeleteCurrentItem();

    btnRemoveCol->Disable();

    CheckChange();
}


void dlgTable::OnSelChangeCol(wxListEvent &ev)
{
    long pos=lstColumns->GetSelection();
    wxString inheritedFromTable=lstColumns->GetText(pos, 2);
    
    btnAddCol->Enable(!(cbOfType->GetCurrentSelection() > 0 && cbOfType->GetOIDKey() > 0));
    btnRemoveCol->Enable(inheritedFromTable.IsEmpty() && !(cbOfType->GetCurrentSelection() > 0 && cbOfType->GetOIDKey() > 0));
    btnChangeCol->Enable(table != 0 && !lstColumns->GetText(pos, 6).IsEmpty() && inheritedFromTable.IsEmpty());
}


void dlgTable::OnAddConstr(wxCommandEvent &ev)
{
    int sel=cbConstrType->GetCurrentSelection();
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
                hasPK=true;
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
        if (wxMessageBox(_("Are you sure you wish to remove the selected constraint?"), _("Remove constraint?"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION) == wxNO)
            return;
    }

    int pos=lstConstraints->GetSelection();
    if (pos < 0)
        return;

    wxListItem item;
    item.SetId(pos);
    item.SetColumn(0);
    item.SetMask(wxLIST_MASK_IMAGE);
    lstConstraints->GetItem(item);
    if (item.GetImage() == primaryKeyFactory.GetIconId())
    {
        hasPK=false;
        FillConstraint();
    }
    
    lstConstraints->DeleteItem(pos);
    btnRemoveConstr->Disable();

    CheckChange();
}


void dlgTable::OnSelChangeConstr(wxListEvent &ev)
{
    btnRemoveConstr->Enable();
}


void dlgTable::FillAutoVacuumParameters(wxString& setStr, wxString& resetStr,
                                        const wxString& parameter, const wxString& val)
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
