//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgTable.cpp - PostgreSQL Table Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "frmMain.h"
#include "frmHint.h"

#include "dlgTable.h"
#include "dlgColumn.h"
#include "dlgIndexConstraint.h"
#include "dlgForeignKey.h"
#include "dlgCheck.h"

#include "pgSchema.h"
#include "pgTable.h"
#include "pgColumn.h"
#include "pgCheck.h"
#include "pgForeignKey.h"
#include "pgIndexConstraint.h"



#define stHasOids       CTRL_STATIC("stHasOids")
#define chkHasOids      CTRL_CHECKBOX("chkHasOids")
#define lbTables        CTRL_LISTBOX("lbTables")
#define btnAddTable     CTRL_BUTTON("btnAddTable")
#define btnRemoveTable  CTRL_BUTTON("btnRemoveTable")
#define cbTables        CTRL_COMBOBOX2("cbTables")
#define cbTablespace    CTRL_COMBOBOX("cbTablespace")

#define btnAddCol       CTRL_BUTTON("btnAddCol")
#define btnChangeCol    CTRL_BUTTON("btnChangeCol")
#define btnRemoveCol    CTRL_BUTTON("btnRemoveCol")

#define lstConstraints  CTRL_LISTVIEW("lstConstraints")
#define btnAddConstr    CTRL_BUTTON("btnAddConstr")
#define cbConstrType    CTRL_COMBOBOX("cbConstrType")
#define btnRemoveConstr CTRL_BUTTON("btnRemoveConstr")

#define chkCustomVac    CTRL_CHECKBOX("chkCustomVac")
#define chkVacEnabled   CTRL_CHECKBOX("chkVacEnabled")
#define txtBaseVac      CTRL_TEXT("txtBaseVac")
#define stBaseVacCurr   CTRL_STATIC("stBaseVacCurr")
#define txtBaseAn       CTRL_TEXT("txtBaseAn")
#define stBaseAnCurr    CTRL_STATIC("stBaseAnCurr")
#define txtFactorVac    CTRL_TEXT("txtFactorVac")
#define stFactorVacCurr CTRL_STATIC("stFactorVacCurr")
#define txtFactorAn     CTRL_TEXT("txtFactorAn")
#define stFactorAnCurr  CTRL_STATIC("stFactorAnCurr")
#define txtVacDelay     CTRL_TEXT("txtVacDelay")
#define stVacDelayCurr  CTRL_STATIC("stVacDelayCurr")
#define txtVacLimit     CTRL_TEXT("txtVacLimit")
#define stVacLimitCurr  CTRL_STATIC("stVacLimitCurr")

BEGIN_EVENT_TABLE(dlgTable, dlgSecurityProperty)
    EVT_CHECKBOX(XRCID("chkHasOids"),               dlgProperty::OnChange)
    EVT_TEXT(XRCID("cbTablespace"),                 dlgProperty::OnChange)
    EVT_COMBOBOX(XRCID("cbTablespace"),             dlgProperty::OnChange)
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

    EVT_CHECKBOX(XRCID("chkCustomVac"),             dlgTable::OnChangeVacuum)
    EVT_CHECKBOX(XRCID("chkVacEnabled"),            dlgTable::OnChangeVacuum)
    EVT_TEXT(XRCID("txtBaseVac"),                   dlgTable::OnChangeVacuum)
    EVT_TEXT(XRCID("txtBaseAn"),                    dlgTable::OnChangeVacuum)
    EVT_TEXT(XRCID("txtFactorVac"),                 dlgTable::OnChangeVacuum)
    EVT_TEXT(XRCID("txtFactorAn"),                  dlgTable::OnChangeVacuum)
    EVT_TEXT(XRCID("txtVacDelay"),                  dlgTable::OnChangeVacuum)
    EVT_TEXT(XRCID("txtVacLimit"),                  dlgTable::OnChangeVacuum)

    EVT_BUTTON(wxID_OK,                             dlgTable::OnOK)
END_EVENT_TABLE();


dlgProperty *pgTableFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
    return new dlgTable(this, frame, (pgTable*)node, (pgSchema*)parent);
}

dlgTable::dlgTable(pgaFactory *f, frmMain *frame, pgTable *node, pgSchema *sch)
: dlgSecurityProperty(f, frame, node, wxT("dlgTable"), wxT("INSERT,SELECT,UPDATE,DELETE,RULE,REFERENCES,TRIGGER"), "arwdRxt")
{
    schema=sch;
    table=node;

    btnRemoveTable->Disable();

    lstColumns->CreateColumns(0, _("Column name"), _("Definition"), 90);
    lstColumns->AddColumn(wxT("Inherited from table"), 0);
    lstColumns->AddColumn(wxT("Column definition"), 0);
    lstColumns->AddColumn(wxT("Column comment"), 0);
    lstColumns->AddColumn(wxT("Column statistics"), 0);
    lstColumns->AddColumn(wxT("Column"), 0);

    lstConstraints->CreateColumns(0, _("Constraint name"), _("Definition"), 90);
}


pgObject *dlgTable::GetObject()
{
    return table;
}


int dlgTable::Go(bool modal)
{
    if (!table)
        cbOwner->Append(wxT(""));
    AddGroups();
    AddUsers(cbOwner);
    PrepareTablespace(cbTablespace);

    hasPK=false;

    if (table)
    {
        // edit mode
        chkHasOids->SetValue(table->GetHasOids());

        if (!table->GetTablespace().IsEmpty())
            cbTablespace->SetValue(table->GetTablespace());

        wxArrayString qitl=table->GetQuotedInheritedTablesList();
        size_t i;
        for (i=0 ; i < qitl.GetCount() ; i++)
            lbTables->Append(qitl.Item(i));

        btnAddTable->Disable();
        lbTables->Disable();
        cbTables->Disable();
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
                            lstColumns->SetItem(pos, 2, _("Inherited"));
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
                    case PGM_FOREIGNKEY:
                    {
                        pgForeignKey *obj=(pgForeignKey*)data;

                        lstConstraints->AppendItem(data->GetIconId(), obj->GetName(), obj->GetDefinition());
                        previousConstraints.Add(obj->GetQuotedIdentifier() 
                            + wxT(" ") + obj->GetTypeName().Upper() + wxT(" ") + obj->GetDefinition());
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

        wxString systemRestriction;
        if (!settings->GetShowSystemObjects())
        systemRestriction = 
            wxT("   AND ") + connection->SystemNamespaceRestriction(wxT("n.nspname"));
            
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

    if (connection->BackendMinimumVersion(8,1) && table)
    {
        txtBaseVac->SetValidator(numericValidator);
        txtBaseAn->SetValidator(numericValidator);
        txtFactorVac->SetValidator(numericValidator);
        txtFactorAn->SetValidator(numericValidator);
        txtVacDelay->SetValidator(numericValidator);
        txtVacLimit->SetValidator(numericValidator);

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
        }

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
        }
        else
        {
            hasVacuum=false;
            chkVacEnabled->SetValue(true);
        }
        chkCustomVac->SetValue(hasVacuum);
        wxCommandEvent ev;
        OnChangeVacuum(ev);
    }
    else
    {
        nbNotebook->DeletePage(3);
    }

    return dlgSecurityProperty::Go();
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

        // Build a tmeporary list of ADD COLUMNs, and fixup the list to remove
        for (pos=0; pos < lstColumns->GetItemCount() ; pos++)
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
            if (index >= 0)
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

        tmpDef=previousConstraints;
        tmpsql.Empty();

        // Build a tmeporary list of ADD CONSTRAINTs, and fixup the list to remove
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
        if (cbTablespace->GetValue() != table->GetTablespace())
        {
            if (cbTablespace->GetSelection() > 0)
                sql += wxT("ALTER TABLE ") + tabname 
                    +  wxT(" SET TABLESPACE ") + qtIdent(cbTablespace->GetValue())
                    + wxT(";\n");
        }

        if (connection->BackendMinimumVersion(8,1))
        {
            if (!chkCustomVac->GetValue())
            {
                if (hasVacuum)
                    sql += wxT("DELETE FROM pg_autovacuum WHERE vacrelid=") + table->GetOidStr() + wxT(";\n");
            }
            else
            {
                wxString vacStr;
                bool changed = (chkVacEnabled->GetValue() != tableVacEnabled);
                if (!hasVacuum)
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
                else
                {
                    vacStr = wxT("UPDATE pg_autovacuum\n")
                             wxT("   SET enabled=")
                           + BoolToStr(chkVacEnabled->GetValue())
                           + wxT(", vac_base_thresh = ") + AppendNum(changed, txtBaseVac, tableVacBaseThr) 
                           + wxT(", anl_base_thresh = ") + AppendNum(changed, txtBaseAn, tableAnlBaseThr) 
                           + wxT(", base_scale_factor = ") + AppendNum(changed, txtFactorVac, tableVacFactor)
                           + wxT(", anl_scale_factor = ") + AppendNum(changed, txtFactorAn, tableAnlFactor)
                           + wxT(", vac_cost_delay = ") + AppendNum(changed, txtVacDelay, tableCostDelay)
                           + wxT(", vac_cost_limit = ") + AppendNum(changed, txtVacLimit, tableCostLimit)
                           + wxT("\n WHERE vacrelid=") + table->GetOidStr() + wxT(";\n");
                }
                if (changed)
                    sql += vacStr;
            }
        }
    }
    else
    {
        sql = wxT("CREATE TABLE ") + tabname
            + wxT("\n(");

        int pos;
        bool needComma=false;
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
        sql += wxT("\n) ");


        if (lbTables->GetCount() > 0)
        {
            sql += wxT("\nINHERITS (");

            int i;
            for (i=0 ; i < lbTables->GetCount() ; i++)
            {
                if (i)
                    sql += wxT(", ");
                sql += lbTables->GetString(i);
            }
            sql += wxT(")\n");
        }
        sql += (chkHasOids->GetValue() ? wxT("WITH OIDS") : wxT("WITHOUT OIDS"));
        if (cbTablespace->GetSelection() > 0)
            sql += wxT("\nTABLESPACE ") + qtIdent(cbTablespace->GetValue());

        sql += wxT(";\n");

        AppendOwnerNew(sql, wxT("TABLE ") + tabname);
    }

    // Extra column info
    int pos;

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
                + wxT(" IS ") + qtString(lstColumns->GetText(pos, 5))
                + wxT(";\n");
    }


    AppendComment(sql, wxT("TABLE"), schema, table);
    sql +=  GetGrant(wxT("arwdRxt"), wxT("TABLE ") + tabname);

    return sql;
}


void dlgTable::FillConstraint()
{
    cbConstrType->Clear();
    if (!hasPK)
        cbConstrType->Append(_("Primary Key"));

//    chkHasOids->Enable(!table || (table && table->GetHasOids() && hasPK && connection->BackendMinimumVersion(7, 4)));
    cbConstrType->Append(_("Foreign Key"));
    cbConstrType->Append(_("Unique"));
    cbConstrType->Append(_("Check"));
    cbConstrType->SetSelection(0);
}


pgObject *dlgTable::CreateObject(pgCollection *collection)
{
    wxString name=GetName();

    pgObject *obj=tableFactory.CreateObjects(collection, 0, wxT(
        "\n   AND rel.relname=") + qtString(name) + wxT(
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

        stBaseVacCurr->SetLabel(GetNumString(txtBaseVac, vacEn, NumToStr(settingVacBaseThr)));
        stBaseAnCurr->SetLabel(GetNumString(txtBaseAn, vacEn, NumToStr(settingAnlBaseThr)));
        stFactorVacCurr->SetLabel(GetNumString(txtFactorVac, vacEn, NumToStr(settingVacFactor)));
        stFactorAnCurr->SetLabel(GetNumString(txtFactorAn, vacEn, NumToStr(settingAnlFactor)));
        stVacDelayCurr->SetLabel(GetNumString(txtVacDelay, vacEn, NumToStr(settingCostDelay)));
        stVacLimitCurr->SetLabel(GetNumString(txtVacLimit, vacEn, NumToStr(settingCostLimit)));
    }
    OnChange(ev);
}


void dlgTable::OnChangeTable(wxCommandEvent &ev)
{
    cbTables->GuessSelection(ev);
}


void dlgTable::OnOK(wxCommandEvent &ev)
{
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
            wxT("SELECT attname FROM pg_attribute WHERE NOT attisdropped AND attnum>0 AND attrelid=") + taboid);
        if (set)
        {
            int row;
            while (!set->Eof())
            {
                row=lstColumns->AppendItem(tableFactory.GetIconId(), set->GetVal(wxT("attname")), 
                    wxString::Format(_("Inherited from table %s"), tabname.c_str()));
                lstColumns->SetItem(row, 2, tabname);
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
                lstColumns->DeleteItem(row);
        }
        CheckChange();
    }
    btnRemoveTable->Disable();
}


void dlgTable::OnSelChangeTable(wxCommandEvent &ev)
{
    btnRemoveTable->Enable();
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


void dlgTable::OnAddCol(wxCommandEvent &ev)
{
    dlgColumn col(&columnFactory, mainForm, NULL, table);
    col.CenterOnParent();
    col.SetDatabase(database);
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
    
    btnRemoveCol->Enable(inheritedFromTable.IsEmpty());
    btnChangeCol->Enable(table != 0 && !lstColumns->GetText(pos, 6).IsEmpty());
}


void dlgTable::OnAddConstr(wxCommandEvent &ev)
{
    int sel=cbConstrType->GetSelection();
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
                lstConstraints->AppendItem(primaryKeyFactory.GetIconId(), pk.GetName(), pk.GetDefinition());
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
                wxString str=fk.GetDefinition();
                str.Replace(wxT("\n"), wxT(" "));
                lstConstraints->AppendItem(foreignKeyFactory.GetIconId(), fk.GetName(), str);
            }
            break;
        }
        case 2: // Unique
        {
            dlgUnique unq(&uniqueFactory, mainForm, lstColumns);
            unq.CenterOnParent();
            unq.SetDatabase(database);
            if (unq.Go(true) != wxID_CANCEL)
                lstConstraints->AppendItem(uniqueFactory.GetIconId(), unq.GetName(), unq.GetDefinition());
            break;
        }
        case 3: // Check
        {
            dlgCheck chk(&checkFactory, mainForm);
            chk.CenterOnParent();
            chk.SetDatabase(database);
            if (chk.Go(true) != wxID_CANCEL)
                lstConstraints->AppendItem(checkFactory.GetIconId(), chk.GetName(), chk.GetDefinition());
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


countRowsFactory::countRowsFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar) : contextActionFactory(list)
{
    mnu->Append(id, _("&Count"), _("Count rows in the selected object."));
}


wxWindow *countRowsFactory::StartDialog(frmMain *form, pgObject *obj)
{
    ((pgTable*)obj)->UpdateRows();
    
    wxTreeItemId item=form->GetBrowser()->GetSelection();
    if (obj == form->GetBrowser()->GetObject(item))
        obj->ShowTreeDetail(form->GetBrowser(), 0, form->GetProperties());

    return 0;
}


bool countRowsFactory::CheckEnable(pgObject *obj)
{
    return obj && obj->IsCreatedBy(tableFactory);
}
