//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgUser.cpp - PostgreSQL User Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// Images
#include "images/user.xpm"

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "dlgUser.h"
#include "pgUser.h"


// pointer to controls
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


BEGIN_EVENT_TABLE(dlgUser, dlgProperty)
    EVT_TEXT(XRCID("txtName"),                      dlgUser::OnChange)
    
    EVT_LISTBOX_DCLICK(XRCID("lbGroupsNotIn"),      dlgUser::OnGroupAdd)
    EVT_LISTBOX_DCLICK(XRCID("lbGroupsIn"),         dlgUser::OnGroupRemove)
    EVT_BUTTON(XRCID("btnAddGroup"),                dlgUser::OnGroupAdd)
    EVT_BUTTON(XRCID("btnDelGroup"),                dlgUser::OnGroupRemove)

    EVT_BUTTON(XRCID("btnAdd"),                     dlgUser::OnVarAdd)
    EVT_BUTTON(XRCID("btnRemove"),                  dlgUser::OnVarRemove)
    EVT_LIST_ITEM_SELECTED(XRCID("lstVariables"),   dlgUser::OnVarSelChange)
END_EVENT_TABLE();



dlgUser::dlgUser(frmMain *frame, pgUser *node)
: dlgProperty(frame, wxT("dlgUser"))
{
    user=node;
    SetIcon(wxIcon(user_xpm));
    CreateListColumns(lstVariables, wxT("Variable"), wxT("Value"), -1);
}


pgObject *dlgUser::GetObject()
{
    return user;
}


int dlgUser::Go(bool modal)
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

    if (user)
    {
        // Edit Mode
        txtName->SetValue(user->GetIdentifier());
        txtID->SetValue(NumToStr(user->GetUserId()));
        chkCreateDB->SetValue(user->GetCreateDatabase());
        chkCreateUser->SetValue(user->GetSuperuser());
        txtName->Disable();
        txtID->Disable();

        wxStringTokenizer cfgTokens(user->GetConfigList(), ',');
        while (cfgTokens.HasMoreTokens())
        {
            wxString token=cfgTokens.GetNextToken();
            AppendListItem(lstVariables, token.BeforeFirst('='), token.AfterFirst('='), 0);
        }
    }
    else
    {
        txtID->SetValidator(numericValidator);
    }

    return dlgProperty::Go(modal);
}


void dlgUser::OnChange(wxNotifyEvent &ev)
{
    if (!user)
    {
        wxString name=GetName();

        bool enable=true;
        CheckValid(enable, !name.IsEmpty(), wxT("Please specify name."));
        EnableOK(enable);
    }
    else
    {
        btnOK->Enable(!GetSql().IsEmpty());
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
    OnChange(ev);
}


void dlgUser::OnGroupRemove(wxNotifyEvent &ev)
{
    int pos=lbGroupsIn->GetSelection();
    if (pos >= 0)
    {
        lbGroupsNotIn->Append(lbGroupsIn->GetString(pos));
        lbGroupsIn->Delete(pos);
    }
    OnChange(ev);
}


void dlgUser::OnVarSelChange(wxListEvent &ev)
{
    long pos=GetListSelected(lstVariables);
    if (pos >= 0)
    {
        txtName->SetValue(lstVariables->GetItemText(pos));
        txtValue->SetValue(GetListText(lstVariables, pos, 1));
    }
}


void dlgUser::OnVarAdd(wxNotifyEvent &ev)
{
    wxString name=GetName().Strip(wxString::both);
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
    OnChange(ev);
}


void dlgUser::OnVarRemove(wxNotifyEvent &ev)
{
    lstVariables->DeleteItem(GetListSelected(lstVariables));
    OnChange(ev);
}


pgObject *dlgUser::CreateObject(pgCollection *collection)
{
    wxString name=GetName();

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
            wxString newVal=GetListText(lstVariables, pos, 1);

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
            if (oldVal != newVal)
            {
                sql += wxT("ALTER USER ") + user->GetQuotedFullIdentifier()
                    +  wxT(" SET ") + newVar
                    +  wxT("=") + newVal
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
        wxString name=GetName();

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
            sql += wxT("ALTER USER ") + qtIdent(name) 
                +  wxT(" SET ") + lstVariables->GetItemText(pos)
                +  wxT("=") + GetListText(lstVariables, pos, 1)
                +  wxT(";\n");
        }

        cnt = lbGroupsIn->GetCount();
        for (pos=0 ; pos < cnt ; pos++)
            sql += wxT("ALTER GROUP ") + qtIdent(lbGroupsIn->GetString(pos))
                +  wxT(" ADD USER ") + qtIdent(name) + wxT(";\n");
    }
    return sql;
}

