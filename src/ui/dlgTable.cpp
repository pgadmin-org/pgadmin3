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

#include "pgTable.h"
#include "pgColumn.h"
#include "pgCheck.h"
#include "pgForeignKey.h"
#include "pgIndexConstraint.h"


// Images
#include "images/table.xpm"

#define cbSchema        CTRL("cbSchema",        wxComboBox)
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


dlgTable::dlgTable(frmMain *frame, pgTable *node)
: dlgSecurityProperty(frame, node, wxT("dlgTable"), wxT("INSERT,SELECT,UPDATE,DELETE,RULE,REFERENCE,TRIGGER"), "arwdRxt")
{
    SetIcon(wxIcon(table_xpm));
    table=node;
    column=0;

    txtOID->Disable();

    CreateListColumns(lstColumns, wxT("Column name"), wxT("Definition"));
    CreateListColumns(lstConstraints, wxT("Constraint name"), wxT("Definition"));

}


pgObject *dlgTable::GetObject()
{
    return table;
}


int dlgTable::Go(bool modal)
{
    AddGroups();
    AddUsers(cbOwner);

    hasPK=false;

    if (table)
    {
        // edit mode
        txtName->SetValue(table->GetName());
        txtOID->SetValue(NumToStr(table->GetOid()));
        chkHasOids->SetValue(table->GetHasOids());

        cbSchema->Append(table->GetSchema()->GetName());
        cbSchema->SetSelection(0);
        cbOwner->SetValue(table->GetOwner());

        txtOID->Disable();
        stHasOids->Disable();
        chkHasOids->Disable();
        cbSchema->Disable();

        long cookie;
        pgObject *data;
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
        btnOK->Disable();

        wxString systemRestriction;
        if (!settings->GetShowSystemObjects())
            systemRestriction = wxT("   AND oid >= 100\n");

        pgSet *set=connection->ExecuteSet(wxT(
            "SELECT nspname FROM pg_namespace nsp\n"
            " WHERE nspname NOT LIKE 'pg\\_temp\\_%'\n") + systemRestriction);

        if (set)
        {
            int publicPos=0;
            while (!set->Eof())
            {
                cbSchema->Append(set->GetVal(0));
                if (set->GetVal(0) == "public")
                    publicPos = cbSchema->GetCount() -1;
                set->MoveNext();
            }
            delete set;

            cbSchema->SetSelection(publicPos);
        }

    }

    FillConstraint();

    btnChangeCol->Disable();
    btnRemoveCol->Disable();
    btnRemoveConstr->Disable();

    return dlgSecurityProperty::Go();
}


wxString dlgTable::GetSql()
{
    wxString sql;

    wxString schema=cbSchema->GetValue();
    if (!schema.IsEmpty())
        schema = qtIdent(schema) + wxT(".");

    if (table)
    {
        int pos;
        unsigned index;

        wxString coldef;
        wxArrayString tmpCols=previousColumns;

        for (pos=0; pos < lstColumns->GetItemCount() ; pos++)
        {
            coldef=lstColumns->GetItemText(pos) + wxT(" ") + GetListText(lstConstraints, pos, 1);
            index=tmpCols.Index(coldef);
            if (index >= 0)
                tmpCols.RemoveAt(index);
            else
                sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                    +  wxT(" ADD COLUMN ") + coldef + wxT(";\n");
        }

        for (index=0 ; index < tmpCols.GetCount() ; index++)
        {
            coldef = tmpCols.Item(index);
            if (coldef[0U] == '"')
                coldef = coldef.Mid(1).BeforeFirst('"');
            else
                coldef = coldef.BeforeFirst(' ');
            sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                +  wxT(" DROP COLUMN ") + coldef + wxT(";\n");
        }


        wxString condef;
        wxArrayString tmpCons=previousConstraints;

        for (pos=0; pos < lstConstraints->GetItemCount() ; pos++)
        {
            condef=lstConstraints->GetItemText(pos) + wxT(" ") + GetListText(lstConstraints, pos, 1);
            index=tmpCons.Index(coldef);
            if (index >= 0)
                tmpCons.RemoveAt(index);
            else
                sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                    +  wxT(" ADD CONSTRAINT ") + coldef + wxT(";\n");
        }

        for (index=0 ; index < tmpCons.GetCount() ; index++)
        {
            condef = tmpCons.Item(index);
            if (condef[0U] == '"')
                condef = condef.Mid(1).BeforeFirst('"');
            else
                condef = condef.BeforeFirst(' ');
            sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                +  wxT(" DROP CONSTRAINT ") + qtIdent(condef) + wxT(";\n");
        }

        
        wxString name=txtName->GetValue();
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
        sql = wxT("CREATE TABLE ") + schema + qtIdent(txtName->GetValue())
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
            wxListItem item;
            item.SetId(pos);
            item.SetColumn(0);
            item.SetMask(wxLIST_MASK_IMAGE);
            lstConstraints->GetItem(item);

            wxString name=lstConstraints->GetItemText(pos);
            wxString definition = GetListText(lstConstraints, pos, 1);

            sql += wxT(",\n   ");
            AppendIfFilled(sql, wxT("CONSTRAINT "), qtIdent(name));
            switch (item.GetImage())
            {
                case PGICON_PRIMARYKEY:
                    sql += wxT(" PRIMARY KEY ");
                    break;
                case PGICON_FOREIGNKEY:
                    sql += wxT(" FOREIGN KEY ");
                    break;
                case PGICON_UNIQUE:
                    sql += wxT(" UNIQUE ");
                    break;
                case PGICON_CHECK:
                    sql += wxT(" CHECK ");
                    break;
            }
            sql += definition;
        }

        sql += wxT("\n);\n");
    }
    sql += GetGrant(wxT("arwdRxt"), wxT("TABLE ") + schema + qtIdent(txtName->GetValue()));

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
    wxString name=txtName->GetValue();
    wxString schema=cbSchema->GetValue();
    if (schema.IsEmpty())
        schema = wxT("public");

    pgObject *obj=pgTable::ReadObjects(collection, 0, wxT(
        "\n   AND rel.relname=") + qtString(name) + wxT(
        "\n   AND rek.relnamespace=(SELECT oid FROM pg_namespace WHERE nspname=") + 
        qtString(schema) + wxT(")"));

    return obj;
}



void dlgTable::OnChange(wxNotifyEvent &ev)
{
    if (table)
    {
        bool changed=false;
        if (txtName->GetValue() != table->GetName() ||
            txtComment->GetValue() != table->GetComment() ||
            cbOwner->GetValue() != table->GetOwner())
            changed=true;
        else
            changed = !GetSql().IsEmpty();

        btnOK->Enable(changed);
    }
    else
        btnOK->Enable(!txtName->GetValue().IsEmpty() && lstColumns->GetItemCount() > 0);
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

    if (!lstColumns->GetItemCount())
        btnOK->Disable();
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
}

void dlgTable::OnSelChangeConstr(wxListEvent &ev)
{
    btnRemoveConstr->Enable();
}
