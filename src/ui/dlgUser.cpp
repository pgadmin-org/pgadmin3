//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// dlgUser.cpp - PostgreSQL User Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgUser.h"
#include "dlgUser.h"


// pointer to controls
#define txtUser         CTRL("txtUser", wxTextCtrl)
#define txtID           CTRL("txtID", wxTextCtrl)
#define txtPasswd       CTRL("txtPasswd", wxTextCtrl)
#define chkCreateDB     CTRL("chkCreateDB", wxCheckBox)
#define chkCreateUser   CTRL("chkCreateUser", wxCheckBox)

#define lbGroupsNotIn   CTRL("lbGroupsNotIn", wxListBox)
#define lbGroupsIn      CTRL("lbGroupsIn", wxListBox)
#define btnAddGroup     CTRL("btnAddGroup", wxButton)
#define btnDelGroup     CTRL("btnDelGroup", wxButton)

#define lstVariables    CTRL("lstVariables", wxListCtrl)
#define txtName         CTRL("txtName", wxTextCtrl)
#define txtValue        CTRL("txtValue", wxTextCtrl)

#define btnOK           CTRL("btnOK", wxButton)


BEGIN_EVENT_TABLE(dlgUser, dlgProperty)
    EVT_TEXT(XRCID("txtUser"),                      dlgUser::OnChange)
    
    EVT_LISTBOX_DCLICK(XRCID("lbGroupsNotIn"),      dlgUser::OnGroupAdd)
    EVT_LISTBOX_DCLICK(XRCID("lbGroupsIn"),         dlgUser::OnGroupRemove)
    EVT_BUTTON(XRCID("btnAddGroup"),                dlgUser::OnGroupAdd)
    EVT_BUTTON(XRCID("btnDelGroup"),                dlgUser::OnGroupRemove)

    EVT_BUTTON(XRCID("btnAdd"),                     dlgUser::OnVarAdd)
    EVT_BUTTON(XRCID("btnRemove"),                  dlgUser::OnVarRemove)
    EVT_LIST_ITEM_SELECTED(XRCID("lstVariables"),   dlgUser::OnVarSelChange)
END_EVENT_TABLE();



dlgUser::dlgUser(wxFrame *frame, pgUser *node)
: dlgProperty(frame, wxT("dlgUser"))
{
    user=node;
    lstVariables->InsertColumn(0, wxT("Variable"), wxLIST_FORMAT_LEFT, 100);
    lstVariables->InsertColumn(1, wxT("Value"), wxLIST_FORMAT_LEFT, 200);

    if (user)
    {
        // Edit Mode
        txtUser->SetValue(user->GetIdentifier());
        txtID->SetValue(NumToStr(user->GetUserId()));
        chkCreateDB->SetValue(user->GetCreateDatabase());
        chkCreateUser->SetValue(user->GetSuperuser());
        txtUser->Disable();
        txtID->Disable();

        int pos=0;
        wxStringTokenizer cfgTokens(user->GetConfigList(), ',');
        while (cfgTokens.HasMoreTokens())
        {
            wxString token=cfgTokens.GetNextToken();
            wxString name=token.BeforeFirst('=');
            lstVariables->InsertItem(pos, name, 0);
            lstVariables->SetItem(pos, 1, token.Mid(name.Length()+1));
            pos++;
        }
    }
    else
    {
        wxTextValidator numval(wxFILTER_NUMERIC);
        txtID->SetValidator(numval);
        btnOK->Disable();
    }
}


void dlgUser::Go()
{
    pgSet *set=connection->ExecuteSet(wxT("SELECT groname FROM pg_group"));
    if (set)
    {
        while (!set->Eof())
        {
            wxString groupName=set->GetVal(wxT("groname"));
            if (user && user->GetGroupsIn().Index(groupName) >= 0)
                lbGroupsIn->Append(groupName);
            else
                lbGroupsNotIn->Append(groupName);

            set->MoveNext();
        }
        delete set;
    }
    Show();
}

void dlgUser::OnChange(wxNotifyEvent &ev)
{
    if (!user)
    {
        wxString name=txtUser->GetValue();

        btnOK->Enable(!name.IsEmpty());
    }
}


void dlgUser::OnGroupAdd(wxNotifyEvent &ev)
{
    int pos=lbGroupsNotIn->GetSelection();
    if (pos >= 0)
    {
        lbGroupsIn->Append(lbGroupsNotIn->GetString(pos));
        lbGroupsNotIn->Delete(pos);
    }
}


void dlgUser::OnGroupRemove(wxNotifyEvent &ev)
{
    int pos=lbGroupsIn->GetSelection();
    if (pos >= 0)
    {
        lbGroupsNotIn->Append(lbGroupsIn->GetString(pos));
        lbGroupsIn->Delete(pos);
    }
}


void dlgUser::OnVarSelChange(wxListEvent &ev)
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


void dlgUser::OnVarAdd(wxNotifyEvent &ev)
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


void dlgUser::OnVarRemove(wxNotifyEvent &ev)
{
    lstVariables->DeleteItem(lstVariables->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED));
}


pgObject *dlgUser::CreateObject(pgCollection *collection)
{
    wxString name=txtUser->GetValue();

    pgObject *obj=pgUser::ReadObjects(collection, 0, wxT("\n WHERE usename=") + qtString(name));
    return obj;
}


wxString dlgUser::GetSql()
{
    wxString sql;
    
    wxString passwd=txtPasswd->GetValue();
    bool createDB=chkCreateDB->GetValue(),
         createUser=chkCreateUser->GetValue();

    if (user)
    {
        // Edit Mode
        if (!passwd.IsEmpty())
            sql += wxT(" PASSWORD ") + qtString(passwd);

        if (createDB != user->GetCreateDatabase() || createUser != user->GetSuperuser())
            sql += wxT("\n ");
        
        if (createDB != user->GetCreateDatabase())
        {
            if (createDB)
                sql += wxT(" CREATEDB");
            else
                sql += wxT(" NOCREATEDB");
        }
        if (createUser != user->GetSuperuser())
        {
            if (createUser)
                sql += wxT(" CREATEUSER");
            else
                sql += wxT(" NOCREATEUSER");
        }

        if (!sql.IsNull())
            sql = wxT("ALTER USER ") + user->GetQuotedFullIdentifier() + sql + wxT(";\n");


        wxArrayString vars;

        wxStringTokenizer cfgTokens(user->GetConfigList(), ',');
        while (cfgTokens.HasMoreTokens())
            vars.Add(cfgTokens.GetNextToken());

        int cnt=lstVariables->GetItemCount();
        int pos;

        // check for changed or added vars
        for (pos=0 ; pos < cnt ; pos++)
        {
            wxString newVar=lstVariables->GetItemText(pos);

            wxListItem item;
            item.SetId(pos);
            item.SetColumn(1);
            item.SetMask(wxLIST_MASK_TEXT);
            lstVariables->GetItem(item);
            

            wxString oldVal;

            int index;
            for (index=0 ; index < (int)vars.GetCount() ; index++)
            {
                wxString var=vars.Item(index);
                if (var.BeforeFirst('=').IsSameAs(newVar, false))
                {
                    oldVal = var.Mid(newVar.Length()+1);
                    vars.RemoveAt(index);
                    break;
                }
            }
            if (oldVal != item.GetText())
            {
                sql += wxT("ALTER USER ") + user->GetQuotedFullIdentifier()
                    +  wxT(" SET ") + newVar
                    +  wxT("=") + item.GetText()
                    +  wxT(";\n");
            }
        }
        
        // check for removed vars
        for (pos=0 ; pos < (int)vars.GetCount() ; pos++)
        {
            sql += wxT("ALTER USER ") + user->GetQuotedFullIdentifier()
                +  wxT(" RESET ") + vars.Item(pos).BeforeFirst('=')
                + wxT(";\n");
        }

    
        cnt=lbGroupsIn->GetCount();
        wxArrayString tmpGroups=user->GetGroupsIn();

        // check for added groups
        for (pos=0 ; pos < cnt ; pos++)
        {
            wxString groupName=lbGroupsIn->GetString(pos);

            int index=tmpGroups.Index(groupName);
            if (index >= 0)
                tmpGroups.RemoveAt(index);
            else
                sql += wxT("ALTER GROUP ") + qtIdent(groupName)
                    +  wxT(" ADD USER ") + user->GetQuotedFullIdentifier() + wxT(";\n");
        }
        
        // check for removed groups
        for (pos=0 ; pos < (int)tmpGroups.GetCount() ; pos++)
        {
            sql += wxT("ALTER GROUP ") + qtIdent(tmpGroups.Item(pos))
                +  wxT(" DROP USER ") + user->GetQuotedFullIdentifier() + wxT(";\n");
        }
    }
    else
    {
        // Create Mode
        wxString name=txtUser->GetValue();
        if (!name.IsEmpty())
        {
            long id=atol(txtID->GetValue());

            sql = wxT(
                "CREATE USER ") + qtIdent(name);
            if (id)
                sql += wxT("\n  WITH SYSID ") + NumToStr(id);
            if (!passwd.IsEmpty())
                sql += wxT(" PASSWORD ") + qtString(passwd);

            if (createDB || createUser)
                sql += wxT("\n ");
            if (createDB)
                sql += wxT(" CREATEDB");
            if (createUser)
                sql += wxT(" CREATEUSER");
            sql += wxT(";\n");

            int cnt=lstVariables->GetItemCount();
            int pos;
            for (pos=0 ; pos < cnt ; pos++)
            {
                wxListItem item;
                item.SetId(pos);
                item.SetColumn(1);
                item.SetMask(wxLIST_MASK_TEXT);
                lstVariables->GetItem(item);

                sql += wxT("ALTER USER ") + qtIdent(name) 
                    +  wxT(" SET ") + lstVariables->GetItemText(pos)
                    +  wxT("=") + item.GetText()
                    +  wxT(";\n");
            }

            cnt = lbGroupsIn->GetCount();
            for (pos=0 ; pos < cnt ; pos++)
                sql += wxT("ALTER GROUP ") + qtIdent(lbGroupsIn->GetString(pos))
                    +  wxT(" ADD USER ") + qtIdent(name) + wxT(";\n");
        }
    }
    return sql;
}

