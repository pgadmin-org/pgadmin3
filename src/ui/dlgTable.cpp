//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
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

    EVT_BUTTON(XRCID("btnAddCol"),                  dlgTable::OnAddCol)
    EVT_BUTTON(XRCID("btnRemoveCol"),               dlgTable::OnRemoveCol)
    EVT_LIST_ITEM_SELECTED(XRCID("lstColumns"),     dlgTable::OnSelChangeCol)

    EVT_BUTTON(XRCID("btnAddConstr"),               dlgTable::OnAddConstr)
    EVT_BUTTON(XRCID("btnRemoveConstr"),            dlgTable::OnRemoveConstr)
    EVT_LIST_ITEM_SELECTED(XRCID("lstConstraints"), dlgTable::OnSelChangeConstr)
END_EVENT_TABLE();

dlgTable::dlgTable(frmMain *frame, pgTable *node, pgSchema *sch)
: dlgSecurityProperty(frame, node, wxT("dlgTable"), wxT("INSERT,SELECT,UPDATE,DELETE,RULE,REFERENCE,TRIGGER"), "arwdRxt")
{
    SetIcon(wxIcon(table_xpm));
    schema=sch;
    table=node;
    column=0;

    txtOID->Disable();

    CreateListColumns(lstColumns, wxT("Column name"), wxT("Definition"), 150);
    CreateListColumns(lstConstraints, wxT("Constraint name"), wxT("Definition"), 150);

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

        cbOwner->SetValue(table->GetOwner());

        txtOID->Disable();
        stHasOids->Disable();
        chkHasOids->Disable();

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
                        AppendListItem(lstColumns, column->GetName(), column->GetDefinition(), column->GetIcon());
                        previousColumns.Add(column->GetQuotedIdentifier() 
                            + wxT(" ") + column->GetDefinition());
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
                
                item=mainForm->GetBrowser()->GetNextChild(columnsItem, cookie);
            }
        }
    }
    else
    {
        // create mode
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

    if (table)
    {
        int pos;
        int index;

        wxString definition;
        wxArrayString tmpDef=previousColumns;

        for (pos=0; pos < lstColumns->GetItemCount() ; pos++)
        {
            definition=lstColumns->GetItemText(pos) + wxT(" ") + GetListText(lstColumns, pos, 1);
            index=tmpDef.Index(definition);
            if (index >= 0)
                tmpDef.RemoveAt(index);
            else
                sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                    +  wxT(" ADD COLUMN ") + definition + wxT(";\n");
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


        tmpDef=previousConstraints;

        for (pos=0; pos < lstConstraints->GetItemCount() ; pos++)
        {
            definition=lstConstraints->GetItemText(pos) 
                        + wxT(" ") + GetItemConstraintType(lstConstraints, pos) 
                        + wxT(" ") + GetListText(lstConstraints, pos, 1);
            index=tmpDef.Index(definition);
            if (index >= 0)
                tmpDef.RemoveAt(index);
            else
                sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                    +  wxT(" ADD CONSTRAINT ") + definition + wxT(";\n");
        }

        for (index=0 ; index < (int)tmpDef.GetCount() ; index++)
        {
            definition = tmpDef.Item(index);
            if (definition[0U] == '"')
                definition = definition.Mid(1).BeforeFirst('"');
            else
                definition = definition.BeforeFirst(' ');
            sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                +  wxT(" DROP CONSTRAINT ") + qtIdent(definition) + wxT(";\n");
        }

        
        wxString name=GetName();
        if (name != table->GetName())
            sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                +  wxT(" RENAME TO ") + qtIdent(name) + wxT(";\n");
        wxString owner=cbOwner->GetValue();
        if (owner != table->GetOwner())
            sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                +  wxT(" OWNER TO ") + owner + wxT(";\n");
    }
    else
    {
        sql = wxT("CREATE TABLE ") + schema->GetQuotedFullIdentifier() + wxT(".") + qtIdent(GetName())
            + wxT("\n(");

        int pos;
        for (pos=0 ; pos < lstColumns->GetItemCount() ; pos++)
        {
            if (pos)
                sql += wxT(",");
            wxString name=lstColumns->GetItemText(pos);
            wxString definition = GetListText(lstColumns, pos, 1);

            sql += wxT("\n   ") + qtIdent(name)
                + wxT(" ") + definition;
        }

        for (pos=0 ; pos < lstConstraints->GetItemCount() ; pos++)
        {
            wxString name=lstConstraints->GetItemText(pos);
            wxString definition = GetListText(lstConstraints, pos, 1);

            sql += wxT(",\n   ");
            AppendIfFilled(sql, wxT("CONSTRAINT "), qtIdent(name));

            sql += wxT(" ") + GetItemConstraintType(lstConstraints, pos) + wxT(" ") + definition;
        }

        sql += wxT("\n);\n");
    }
    sql +=  GetGrant(wxT("arwdRxt"), wxT("TABLE ") + schema->GetQuotedFullIdentifier() + wxT(".") + qtIdent(GetName()));

    return sql;
}


void dlgTable::FillConstraint()
{
    cbConstrType->Clear();
    if (!hasPK)
        cbConstrType->Append(wxT("Primary Key"));
    cbConstrType->Append(wxT("Foreign Key"));
    cbConstrType->Append(wxT("Unique"));
    cbConstrType->Append(wxT("Check"));
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
        if (lstColumns->GetItemCount() > 0)
        {
            if (GetName() != table->GetName() ||
                txtComment->GetValue() != table->GetComment() ||
                cbOwner->GetValue() != table->GetOwner())
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
        CheckValid(enable, !name.IsEmpty(), wxT("Please specify name."));
        CheckValid(enable, lstColumns->GetItemCount() > 0, wxT("Please specify columns."));
        EnableOK(enable);
    }
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
    btnRemoveCol->Enable();
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
                AppendListItem(lstConstraints, pk.GetName(), pk.GetDefinition(), PGICON_PRIMARYKEY);
            break;
        }
        case 1: // Foreign Key
        {
            dlgForeignKey fk(mainForm, lstColumns);
            fk.CenterOnParent();
            fk.SetConnection(connection);
            if (fk.Go(true) >= 0)
                AppendListItem(lstConstraints, fk.GetName(), fk.GetDefinition(), PGICON_FOREIGNKEY);
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
