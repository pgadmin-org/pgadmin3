//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// dlgDatabase.cpp - PostgreSQL Database Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "dlgDatabase.h"


// pointer to controls
#define txtName         CTRL("txtName", wxTextCtrl)
#define txtOID          CTRL("txtOID", wxTextCtrl)
#define cbOwner         CTRL("cbOwner", wxComboBox)
#define cbEncoding      CTRL("cbEncoding", wxComboBox)
#define cbTemplate      CTRL("cbTemplate", wxComboBox)
#define txtPath         CTRL("txtPath", wxTextCtrl)
#define txtComment      CTRL("txtComment", wxTextCtrl)

#define lstVariables    CTRL("lstVariables", wxListCtrl)
#define txtName         CTRL("txtName", wxTextCtrl)
#define txtValue        CTRL("txtValue", wxTextCtrl)

#define btnOK           CTRL("btnOK", wxButton)


BEGIN_EVENT_TABLE(dlgDatabase, dlgSecurityProperty)
    EVT_TEXT(XRCID("txtName"),                      dlgDatabase::OnChange)

    EVT_BUTTON(XRCID("btnAdd"),                     dlgDatabase::OnVarAdd)
    EVT_BUTTON(XRCID("btnRemove"),                  dlgDatabase::OnVarRemove)
    EVT_LIST_ITEM_SELECTED(XRCID("lstVariables"),   dlgDatabase::OnVarSelChange)
END_EVENT_TABLE();



dlgDatabase::dlgDatabase(wxFrame *frame, pgDatabase *node)
: dlgSecurityProperty(frame, node, wxT("dlgDatabase"), wxT("CREATE,TEMP"), "CT")
{
    database=node;
    lstVariables->InsertColumn(0, wxT("Variable"), wxLIST_FORMAT_LEFT, 100);
    lstVariables->InsertColumn(1, wxT("Value"), wxLIST_FORMAT_LEFT, 200);

    txtOID->Disable();

    if (database)
    {
        // edit mode
        txtName->SetValue(database->GetName());
        txtOID->SetValue(NumToStr((long)database->GetOid()));
        txtPath->SetValue(database->GetPath());
        cbEncoding->SetValue(database->GetEncoding());
        txtComment->SetValue(database->GetComment());

        txtName->Disable();
        cbOwner->Disable();
        txtPath->Disable();
        cbTemplate->Disable();
        cbEncoding->Disable();
    }
    else
    {
        // create mode
    }
}


void dlgDatabase::Go()
{
    AddGroups();
    AddUsers(cbOwner);
    if (database)
        cbOwner->SetValue(database->GetOwner());
    else
    {
        pgSet *set=connection->ExecuteSet(wxT(
            "SELECT datname FROM pg_database ORDER BY datname"));
        if (set)
        {
            while (!set->Eof())
            {
                cbTemplate->Append(set->GetVal(0));
                set->MoveNext();
            }
            delete set;
        }
    }

    dlgSecurityProperty::Go();
}


pgObject *dlgDatabase::CreateObject(pgCollection *collection)
{
    wxString name=txtName->GetValue();

    pgObject *obj=pgDatabase::ReadObjects(collection, 0, wxT(" WHERE datname=") + qtString(name) + wxT("\n"));
    return obj;
}



void dlgDatabase::OnChange(wxNotifyEvent &ev)
{
    if (!database)
    {
        wxString name=txtName->GetValue();

        btnOK->Enable(!name.IsEmpty());
    }
}


void dlgDatabase::OnVarSelChange(wxListEvent &ev)
{
    long pos=lstVariables->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (pos >= 0)
    {
        txtName->SetValue(lstVariables->GetItemText(pos));
        wxListItem item;
        item.SetId(pos);
        item.SetColumn(1);
        item.SetMask(wxLIST_MASK_TEXT);
        lstVariables->GetItem(item);
        txtValue->SetValue(item.GetText());
    }
}


void dlgDatabase::OnVarAdd(wxNotifyEvent &ev)
{
    wxString name=txtName->GetValue().Strip(wxString::both);
    wxString value=txtValue->GetValue().Strip(wxString::both);
    if (value.IsEmpty())
        value = wxT("DEFAULT");

    if (!name.IsEmpty())
    {
        long pos=lstVariables->FindItem(-1, name);
        if (pos < 0)
        {
            pos = lstVariables->GetItemCount();
            lstVariables->InsertItem(pos, name, 0);
        }
        lstVariables->SetItem(pos, 1, value);
    }
}


void dlgDatabase::OnVarRemove(wxNotifyEvent &ev)
{
    lstVariables->DeleteItem(lstVariables->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED));
}




wxString dlgDatabase::GetSql()
{
    wxString sql, name;
    name=txtName->GetValue();
    if (!name.IsEmpty())
    {
        if (database)
        {
            // edit mode
        }
        else
        {
            // create mode
            sql = wxT("CREATE DATABASE ") + qtIdent(name) 
                + wxT("\n  WITH ENCODING=") + qtString(cbEncoding->GetValue());

            AppendIfFilled(sql, wxT("\n       OWNER="), qtIdent(cbOwner->GetValue()));
            AppendIfFilled(sql, wxT("\n       TEMPLATE="), cbTemplate->GetValue());
            AppendIfFilled(sql, wxT("\n       LOCATION="), txtPath->GetValue());

            sql += wxT(";\n");
            
        }
        sql += GetGrant(wxT("CT"), wxT("DATABASE ") + qtIdent(name));
        wxString comment=txtComment->GetValue();
        if (!database || database->GetComment() != comment)
            sql += wxT("COMMENT ON DATABASE ") + qtIdent(name)
                +  wxT(" IS '") + comment + wxT("';\n");

    }
    return sql;
}


