//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
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



// Images
#include "images/table.xpm"

#define cbOwner         CTRL("cbOwner",         wxComboBox)
#define stHasOids       CTRL("stHasOids",       wxStaticText)
#define chkHasOids      CTRL("chkHasOids",      wxCheckBox)
#define lbTables        CTRL("lbTables",        wxListBox)
#define btnAddTable     CTRL("btnAddTable",     wxButton)
#define btnRemoveTable  CTRL("btnRemoveTable",  wxButton)
#define cbTables        CTRL("cbTables",        wxComboBox)

#define btnAddCol       CTRL("btnAddCol"        wxButton)
#define btnChangeCol    CTRL("btnChangeCol",     wxButton)
#define btnRemoveCol    CTRL("btnRemoveCol",    wxButton)

#define lstConstraints  CTRL("lstConstraints",  wxListCtrl)
#define btnAddConstr    CTRL("btnAddConstr"     wxButton)
#define cbConstrType    CTRL("cbConstrType",    wxComboBox)
#define btnRemoveConstr CTRL("btnRemoveConstr", wxButton)


BEGIN_EVENT_TABLE(dlgTable, dlgSecurityProperty)
    EVT_TEXT(XRCID("txtName"),                      dlgTable::OnChange)
    EVT_TEXT(XRCID("txtComment"),                   dlgTable::OnChange)
    EVT_CHECKBOX(XRCID("chkHasOids"),               dlgTable::OnChange)
    EVT_COMBOBOX(XRCID("cbOwner"),                  dlgTable::OnChange)
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
END_EVENT_TABLE();

dlgTable::dlgTable(frmMain *frame, pgTable *node, pgSchema *sch)
: dlgSecurityProperty(frame, node, wxT("dlgTable"), wxT("INSERT,SELECT,UPDATE,DELETE,RULE,REFERENCES,TRIGGER"), "arwdRxt")
{
    SetIcon(wxIcon(table_xpm));
    schema=sch;
    table=node;

    txtOID->Disable();
    btnRemoveTable->Disable();

    CreateListColumns(lstColumns, _("Column name"), _("Definition"), 150);
    lstColumns->InsertColumn(2, wxT("Inherited from table"), wxLIST_FORMAT_LEFT, 0);
    lstColumns->InsertColumn(3, wxT("Column definition"), wxLIST_FORMAT_LEFT, 0);
    lstColumns->InsertColumn(4, wxT("Column"), wxLIST_FORMAT_LEFT, 0);

    CreateListColumns(lstConstraints, _("Constraint name"), _("Definition"), 150);
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

    hasPK=false;

    if (table)
    {
        // edit mode
        txtName->SetValue(table->GetName());
        txtOID->SetValue(NumToStr(table->GetOid()));
        chkHasOids->SetValue(table->GetHasOids());
        txtComment->SetValue(table->GetComment());

        cbOwner->SetValue(table->GetOwner());

        wxArrayString qitl=table->GetQuotedInheritedTablesList();
        size_t i;
        for (i=0 ; i < qitl.GetCount() ; i++)
            lbTables->Append(qitl.Item(i));

        btnAddTable->Disable();
        lbTables->Disable();
        cbTables->Disable();
        chkHasOids->Disable();

        txtOID->Disable();

        long cookie;
        pgObject *data=0;
        wxTreeItemId item=mainForm->GetBrowser()->GetFirstChild(table->GetId(), cookie);
        while (item)
        {
            data=(pgObject*)mainForm->GetBrowser()->GetItemData(item);
            if (data->GetType() == PG_COLUMNS)
                columnsItem = item;
            else if (data->GetType() == PG_CONSTRAINTS)
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
                data=(pgObject*)mainForm->GetBrowser()->GetItemData(item);
                if (data->GetType() == PG_COLUMN)
                {
                    pgColumn *column=(pgColumn*)data;
                    // make sure column details are read
                    column->ShowTreeDetail(mainForm->GetBrowser());

                    if (column->GetColNumber() > 0)
                    {
                        bool inherited = (column->GetInheritedCount() != 0);
                        int pos=AppendListItem(lstColumns, column->GetName(), column->GetDefinition(), 
                            (inherited ? PGICON_TABLE : column->GetIcon()));
                        previousColumns.Add(column->GetQuotedIdentifier() 
                            + wxT(" ") + column->GetDefinition());
                        lstColumns->SetItem(pos, 4, NumToStr((long)column));
                        if (inherited)
                            lstColumns->SetItem(pos, 2, _("Inherited"));
                    }
                }
                
                item=mainForm->GetBrowser()->GetNextChild(columnsItem, cookie);
            }
        }
        if (constraintsItem)
        {
            pgCollection *coll=(pgCollection*)mainForm->GetBrowser()->GetItemData(constraintsItem);
            // make sure all constraints are appended
            coll->ShowTreeDetail(mainForm->GetBrowser());
            // this is the constraints collection
            item=mainForm->GetBrowser()->GetFirstChild(constraintsItem, cookie);

            // add constraints
            while (item)
            {
                data=(pgObject*)mainForm->GetBrowser()->GetItemData(item);
                data->ShowTreeDetail(mainForm->GetBrowser());
                switch (data->GetType())
                {
                    case PG_PRIMARYKEY:
                        hasPK = true;
                    case PG_UNIQUE:
                    {
                        pgIndexConstraint *obj=(pgIndexConstraint*)data;

                        AppendListItem(lstConstraints, obj->GetName(), obj->GetDefinition(), data->GetIcon());
                        previousConstraints.Add(obj->GetQuotedIdentifier() 
                            + wxT(" ") + obj->GetTypeName().Upper() + wxT(" ") + obj->GetDefinition());
                        break;
                    }
                    case PG_FOREIGNKEY:
                    {
                        pgForeignKey *obj=(pgForeignKey*)data;

                        AppendListItem(lstConstraints, obj->GetName(), obj->GetDefinition(), data->GetIcon());
                        previousConstraints.Add(obj->GetQuotedIdentifier() 
                            + wxT(" ") + obj->GetTypeName().Upper() + wxT(" ") + obj->GetDefinition());
                        break;
                    }
                    case PG_CHECK:
                    {
                        pgCheck *obj=(pgCheck*)data;

                        AppendListItem(lstConstraints, obj->GetName(), obj->GetDefinition(), data->GetIcon());
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
            wxT("   AND (n.oid = 2200 OR n.oid >= ") + NumToStr(connection->GetLastSystemOID()) + wxT(")\n")
            wxT("   AND n.nspname NOT LIKE 'pg\\_temp\\_%'\n");

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
                cbTables->Append(qtIdent(set->GetVal(wxT("nspname")))
                        +wxT(".")+qtIdent(set->GetVal(wxT("relname"))));

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

    return dlgSecurityProperty::Go();
}



wxString dlgTable::GetItemConstraintType(wxListCtrl *list, long pos)
{
    wxString con;
    wxListItem item;
    item.SetId(pos);
    item.SetColumn(0);
    item.SetMask(wxLIST_MASK_IMAGE);
    list->GetItem(item);
    switch (item.GetImage())
    {
        case PGICON_PRIMARYKEY:
            con = wxT("PRIMARY KEY");
            break;
        case PGICON_FOREIGNKEY:
            con = wxT("FOREIGN KEY");
            break;
        case PGICON_UNIQUE:
            con = wxT("UNIQUE");
            break;
        case PGICON_CHECK:
            con = wxT("CHECK");
            break;
    }
    return con;
}


wxString dlgTable::GetSql()
{
    wxString sql;
    wxString tabname=schema->GetQuotedFullIdentifier() + wxT(".") + qtIdent(GetName());

    if (table)
    {
        int pos;
        int index;

        wxString definition;
        wxArrayString tmpDef=previousColumns;

        if (GetName() != table->GetName())
            sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                +  wxT(" RENAME TO ") + qtIdent(GetName())
                +  wxT(";\n");

        if (cbOwner->GetValue() != table->GetOwner())
            sql += wxT("ALTER TABLE ") + tabname 
                +  wxT(" OWNER TO ") + qtIdent(cbOwner->GetValue())
                + wxT(";\n");


        for (pos=0; pos < lstColumns->GetItemCount() ; pos++)
        {
            definition = GetListText(lstColumns, pos, 3);
            if (definition.IsEmpty())
            {
                definition=qtIdent(lstColumns->GetItemText(pos)) + wxT(" ") + GetListText(lstColumns, pos, 1);
                index=tmpDef.Index(definition);
                if (index < 0)
                    sql += wxT("ALTER TABLE ") + tabname
                        +  wxT(" ADD COLUMN ") + definition + wxT(";\n");
            }
            else
            {
                sql += definition;

                pgColumn *column=(pgColumn*) StrToLong(GetListText(lstColumns, pos, 4));
                index=tmpDef.Index(column->GetQuotedIdentifier() 
                            + wxT(" ") + column->GetDefinition());
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
            sql += wxT("ALTER TABLE ") + tabname
                +  wxT(" DROP COLUMN ") + qtIdent(definition) + wxT(";\n");
        }



        tmpDef=previousConstraints;

        for (pos=0; pos < lstConstraints->GetItemCount() ; pos++)
        {
            wxString conname= qtIdent(lstConstraints->GetItemText(pos));
            definition = conname;
            definition += wxT(" ") + GetItemConstraintType(lstConstraints, pos) 
                        + wxT(" ") + GetListText(lstConstraints, pos, 1);
            index=tmpDef.Index(definition);
            if (index >= 0)
                tmpDef.RemoveAt(index);
            else
            {
                sql += wxT("ALTER TABLE ") + tabname
                    +  wxT(" ADD");
                if (!conname.IsEmpty())
                    sql += wxT(" CONSTRAINT ") + conname;

                sql += definition + wxT(";\n");
            }
        }

        for (index=0 ; index < (int)tmpDef.GetCount() ; index++)
        {
            definition = tmpDef.Item(index);
            if (definition[0U] == '"')
                definition = definition.Mid(1).BeforeFirst('"');
            else
                definition = definition.BeforeFirst(' ');
            sql += wxT("ALTER TABLE ") + tabname
                +  wxT(" DROP CONSTRAINT ") + qtIdent(definition) + wxT(";\n");
        }
        if (hasPK && chkHasOids->GetValue() != table->GetHasOids())
        {
            sql += wxT("ALTER TABLE ") + tabname 
                +  wxT(" WITHOUT OIDS;\n");
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
            if (GetListText(lstColumns, pos, 2).IsEmpty())
            {
                // standard definition, not inherited
                if (needComma)
                    sql += wxT(", ");
                else
                    needComma=true;

                wxString name=lstColumns->GetItemText(pos);
                wxString definition = GetListText(lstColumns, pos, 1);

                sql += wxT("\n   ") + qtIdent(name)
                    + wxT(" ") + definition;
            }
        }

        for (pos=0 ; pos < lstConstraints->GetItemCount() ; pos++)
        {
            wxString name=lstConstraints->GetItemText(pos);
            wxString definition = GetListText(lstConstraints, pos, 1);

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
        sql += (chkHasOids->GetValue() ? wxT("WITH OIDS;\n") : wxT("WITHOUT OIDS;\n"));

        if (cbOwner->GetSelection() > 0)
            sql += wxT("ALTER TABLE ") + tabname 
                +  wxT(" OWNER TO ") + qtIdent(cbOwner->GetValue())
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

    pgObject *obj=pgTable::ReadObjects(collection, 0, wxT(
        "\n   AND rel.relname=") + qtString(name) + wxT(
        "\n   AND rel.relnamespace=") + schema->GetOidStr());

    return obj;
}


void dlgTable::OnChange(wxNotifyEvent &ev)
{
    if (table)
    {
        bool changed=false;
        if (connection->BackendMinimumVersion(7, 4) || lstColumns->GetItemCount() > 0)
        {
            if (GetName() != table->GetName() ||
                txtComment->GetValue() != table->GetComment() ||
                cbOwner->GetValue() != table->GetOwner())
//                 (hasPK && chkHasOids->GetValue() != table->GetHasOids())
                changed=true;
            else
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


void dlgTable::OnAddTable(wxNotifyEvent &ev)
{
    int sel=cbTables->GetSelection();
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
                row=AppendListItem(lstColumns, set->GetVal(wxT("attname")), 
                    wxString::Format(_("Inherited from table %s"), tabname.c_str()), PGICON_TABLE);
                lstColumns->SetItem(row, 2, tabname);
                set->MoveNext();
            }
            delete set;
        }        
        OnChange(ev);
    }
}


void dlgTable::OnRemoveTable(wxNotifyEvent &ev)
{
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
            if (tabname == GetListText(lstColumns, row, 2))
                lstColumns->DeleteItem(row);
        }
        OnChange(ev);
    }
    btnRemoveTable->Disable();
}


void dlgTable::OnSelChangeTable(wxListEvent &ev)
{
    btnRemoveTable->Enable();
}


void dlgTable::OnChangeCol(wxNotifyEvent &ev)
{
    long pos=GetListSelected(lstColumns);
    pgColumn *column=(pgColumn*) StrToLong(GetListText(lstColumns, pos, 4));

    dlgColumn col(mainForm, column, table);
    col.CenterOnParent();
    col.SetConnection(connection);
    if (col.Go(true) >= 0)
    {
        lstColumns->SetItem(pos, 0, col.GetName());
        lstColumns->SetItem(pos, 1, col.GetDefinition());
        lstColumns->SetItem(pos, 3, col.GetSql());
    }
    wxNotifyEvent event;
    OnChange(event);
}


void dlgTable::OnAddCol(wxNotifyEvent &ev)
{
    dlgColumn col(mainForm, NULL, table);
    col.CenterOnParent();
    col.SetConnection(connection);
    if (col.Go(true) >= 0)
        AppendListItem(lstColumns, col.GetName(), col.GetDefinition(), PGICON_COLUMN);
    wxNotifyEvent event;
    OnChange(event);
}


void dlgTable::OnRemoveCol(wxNotifyEvent &ev)
{
    lstColumns->DeleteItem(GetListSelected(lstColumns));

    btnRemoveCol->Disable();

    wxNotifyEvent event;
    OnChange(event);
}


void dlgTable::OnSelChangeCol(wxListEvent &ev)
{
    long pos=GetListSelected(lstColumns);
    wxString inheritedFromTable=GetListText(lstColumns, pos, 2);
    
    btnRemoveCol->Enable(inheritedFromTable.IsEmpty());
    btnChangeCol->Enable(table != 0 && !GetListText(lstColumns, pos, 4).IsEmpty());
}


void dlgTable::OnAddConstr(wxNotifyEvent &ev)
{
    int sel=cbConstrType->GetSelection();
    if (hasPK)
        sel++;

    switch (sel)
    {
        case 0: // Primary Key
        {
            dlgPrimaryKey pk(mainForm, lstColumns);
            pk.CenterOnParent();
            pk.SetConnection(connection);
            if (pk.Go(true) >= 0)
            {
                AppendListItem(lstConstraints, pk.GetName(), pk.GetDefinition(), PGICON_PRIMARYKEY);
                hasPK=true;
                FillConstraint();
            }
            break;
        }
        case 1: // Foreign Key
        {
            dlgForeignKey fk(mainForm, lstColumns);
            fk.CenterOnParent();
            fk.SetConnection(connection);
            if (fk.Go(true) >= 0)
            {
                wxString str=fk.GetDefinition();
                str.Replace(wxT("\n"), wxT(" "));
                AppendListItem(lstConstraints, fk.GetName(), str, PGICON_FOREIGNKEY);
            }
            break;
        }
        case 2: // Unique
        {
            dlgUnique unq(mainForm, lstColumns);
            unq.CenterOnParent();
            unq.SetConnection(connection);
            if (unq.Go(true) >= 0)
                AppendListItem(lstConstraints, unq.GetName(), unq.GetDefinition(), PGICON_UNIQUE);
            break;
        }
        case 3: // Check
        {
            dlgCheck chk(mainForm);
            chk.CenterOnParent();
            chk.SetConnection(connection);
            if (chk.Go(true) >= 0)
                AppendListItem(lstConstraints, chk.GetName(), chk.GetDefinition(), PGICON_CHECK);
            break;
        }
    }
    wxNotifyEvent event;
    OnChange(event);
}


void dlgTable::OnRemoveConstr(wxNotifyEvent &ev)
{
    int pos=GetListSelected(lstConstraints);
    if (pos < 0)
        return;

    wxListItem item;
    item.SetId(pos);
    item.SetColumn(0);
    item.SetMask(wxLIST_MASK_IMAGE);
    lstConstraints->GetItem(item);
    if (item.GetImage() == PGICON_PRIMARYKEY)
    {
        hasPK=false;
        FillConstraint();
    }
    
    lstConstraints->DeleteItem(pos);
    btnRemoveConstr->Disable();
    wxNotifyEvent event;
    OnChange(event);
}


void dlgTable::OnSelChangeConstr(wxListEvent &ev)
{
    btnRemoveConstr->Enable();
}
