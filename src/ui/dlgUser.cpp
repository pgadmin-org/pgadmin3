//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgUser.cpp - PostgreSQL User Property
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include "calbox.h"
#include "timespin.h"

// Images
#include "images/user.xpm"

// App headers
#include "misc.h"
#include "dlgUser.h"
#include "pgUser.h"


// pointer to controls
#define txtID           CTRL_TEXT("txtID")
#define txtPasswd       CTRL_TEXT("txtPasswd")
#define datValidUntil   CTRL_CALENDAR("datValidUntil")
#define timValidUntil   CTRL_TIME("timValidUntil")
#define chkCreateDB     CTRL_CHECKBOX("chkCreateDB")
#define chkCreateUser   CTRL_CHECKBOX("chkCreateUser")

#define lbGroupsNotIn   CTRL_LISTBOX("lbGroupsNotIn")
#define lbGroupsIn      CTRL_LISTBOX("lbGroupsIn")
#define btnAddGroup     CTRL_BUTTON("btnAddGroup")
#define btnDelGroup     CTRL_BUTTON("btnDelGroup")

#define lstVariables    CTRL_LISTVIEW("lstVariables")
#define btnAdd          CTRL_BUTTON("btnAdd")
#define btnRemove       CTRL_BUTTON("btnRemove")
#define cbVarname       CTRL_COMBOBOX("cbVarname")
#define txtValue        CTRL_TEXT("txtValue")
#define chkValue        CTRL_CHECKBOX("chkValue")


BEGIN_EVENT_TABLE(dlgUser, dlgProperty)
    EVT_TEXT(XRCID("txtName"),                      dlgUser::OnChange)
    EVT_CALENDAR_SEL_CHANGED(XRCID("datValidUntil"),dlgUser::OnChange)
    EVT_SPIN(XRCID("timValidUntil"),                dlgUser::OnChange)
    
    EVT_LISTBOX_DCLICK(XRCID("lbGroupsNotIn"),      dlgUser::OnGroupAdd)
    EVT_LISTBOX_DCLICK(XRCID("lbGroupsIn"),         dlgUser::OnGroupRemove)
    EVT_TEXT(XRCID("txtPasswd"),                    dlgUser::OnChange)
    EVT_CHECKBOX(XRCID("chkCreateDB"),              dlgUser::OnChange)
    EVT_CHECKBOX(XRCID("chkCreateUser"),            dlgUser::OnChange)

    EVT_BUTTON(XRCID("btnAddGroup"),                dlgUser::OnGroupAdd)
    EVT_BUTTON(XRCID("btnDelGroup"),                dlgUser::OnGroupRemove)

    EVT_LIST_ITEM_SELECTED(XRCID("lstVariables"),   dlgUser::OnVarSelChange)
    EVT_BUTTON(XRCID("btnAdd"),                     dlgUser::OnVarAdd)
    EVT_BUTTON(XRCID("btnRemove"),                  dlgUser::OnVarRemove)
    EVT_TEXT(XRCID("cbVarname"),                    dlgUser::OnVarnameSelChange)
END_EVENT_TABLE();



dlgUser::dlgUser(frmMain *frame, pgUser *node)
: dlgProperty(frame, wxT("dlgUser"))
{
    user=node;
    SetIcon(wxIcon(user_xpm));
    CreateListColumns(lstVariables, _("Variable"), _("Value"), -1);
    btnOK->Disable();
    chkValue->Hide();
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
        readOnly=!user->GetServer()->GetSuperUser();

        txtName->SetValue(user->GetIdentifier());
        txtID->SetValue(NumToStr(user->GetUserId()));
        chkCreateDB->SetValue(user->GetCreateDatabase());
        chkCreateUser->SetValue(user->GetSuperuser());
        datValidUntil->SetDate(user->GetAccountExpires());
        timValidUntil->SetTime(user->GetAccountExpires());
        txtName->Disable();
        txtID->Disable();

        wxStringTokenizer cfgTokens(user->GetConfigList(), wxT(","));
        while (cfgTokens.HasMoreTokens())
        {
            wxString token=cfgTokens.GetNextToken();
            AppendListItem(lstVariables, token.BeforeFirst('='), token.AfterFirst('='), 0);
        }

        timValidUntil->Enable(!readOnly && user->GetAccountExpires().IsValid());

        if (readOnly)
        {
            chkCreateDB->Disable();
            chkCreateUser->Disable();
            datValidUntil->Disable();
            timValidUntil->Disable();
            txtPasswd->Disable();
            btnAddGroup->Disable();
            btnDelGroup->Disable();
            cbVarname->Disable();
            txtValue->Disable();
            btnAdd->Disable();
            btnRemove->Disable();
        }
        else
        {
            pgSet *set;
            if (connection->BackendMinimumVersion(7, 4))
                set=connection->ExecuteSet(wxT("SELECT name, vartype, min_val, max_val\n")
                        wxT("  FROM pg_settings WHERE context='user'"));
            else
                set=connection->ExecuteSet(wxT("SELECT name, 'string' as vartype, '' as min_val, '' as max_val FROM pg_settings"));
            if (set)
            {
                while (!set->Eof())
                {
                    cbVarname->Append(set->GetVal(0));
                    varInfo.Add(set->GetVal(wxT("vartype")) + wxT(" ") + 
                                set->GetVal(wxT("min_val")) + wxT(" ") +
                                set->GetVal(wxT("max_val")));
                    set->MoveNext();
                }
                delete set;

                cbVarname->SetSelection(0);
            }
        }
    }
    else
    {
        txtID->SetValidator(numericValidator);
        timValidUntil->Disable();
    }

    return dlgProperty::Go(modal);
}


void dlgUser::OnChange(wxNotifyEvent &ev)
{
    bool timEn=datValidUntil->GetDate().IsValid();
    timValidUntil->Enable(timEn);
    if (!timEn)
        timValidUntil->SetTime(wxDefaultDateTime);

    if (!user)
    {
        wxString name=GetName();

        bool enable=true;
        CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
        EnableOK(enable);
    }
    else
    {
        btnOK->Enable(!GetSql().IsEmpty());
    }
}


void dlgUser::OnGroupAdd(wxNotifyEvent &ev)
{
    if (!readOnly)
    {
        int pos=lbGroupsNotIn->GetSelection();
        if (pos >= 0)
        {
            lbGroupsIn->Append(lbGroupsNotIn->GetString(pos));
            lbGroupsNotIn->Delete(pos);
        }
        OnChange(ev);
    }
}


void dlgUser::OnGroupRemove(wxNotifyEvent &ev)
{
    if (!readOnly)
    {
        int pos=lbGroupsIn->GetSelection();
        if (pos >= 0)
        {
            lbGroupsNotIn->Append(lbGroupsIn->GetString(pos));
            lbGroupsIn->Delete(pos);
        }
        OnChange(ev);
    }
}


void dlgUser::OnVarnameSelChange(wxNotifyEvent &ev)
{
    int sel=cbVarname->GetSelection();
    if (sel >= 0)
    {
        wxStringTokenizer vals(varInfo.Item(sel));
        wxString typ=vals.GetNextToken();

        if (typ == wxT("bool"))
        {
            txtValue->Hide();
            chkValue->Show();
        }
        else
        {
            chkValue->Hide();
            txtValue->Show();
            if (typ == wxT("string"))
                txtValue->SetValidator(wxTextValidator());
            else
                txtValue->SetValidator(numericValidator);
        }
    }
}


void dlgUser::OnVarSelChange(wxListEvent &ev)
{
    long pos=lstVariables->GetSelection();
    if (pos >= 0)
    {
        wxString value=lstVariables->GetText(pos, 1);
        cbVarname->SetValue(lstVariables->GetText(pos));
        wxNotifyEvent nullev;
        OnVarnameSelChange(nullev);

        txtValue->SetValue(value);
        chkValue->SetValue(value == wxT("on"));
    }
}


void dlgUser::OnVarAdd(wxNotifyEvent &ev)
{
    wxString name=cbVarname->GetValue();
    wxString value;
    if (chkValue->IsShown())
        value = chkValue->GetValue() ? wxT("on") : wxT("off");
    else
        value = txtValue->GetValue().Strip(wxString::both);

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
    lstVariables->DeleteCurrentItem();
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

        if (DateToStr(datValidUntil->GetDate()) != DateToStr(user->GetAccountExpires()))
        {
            if (datValidUntil->GetDate().IsValid())
                sql += wxT("\n   VALID UNTIL ") + qtString(DateToAnsiStr(datValidUntil->GetDate() + timValidUntil->GetValue())); 
            else
                sql += wxT("\n   VALID UNTIL 'infinity'");
        }

        if (!sql.IsNull())
            sql = wxT("ALTER USER ") + user->GetQuotedFullIdentifier() + sql + wxT(";\n");


        wxArrayString vars;

        wxStringTokenizer cfgTokens(user->GetConfigList(), wxT(","));
        while (cfgTokens.HasMoreTokens())
            vars.Add(cfgTokens.GetNextToken());

        int cnt=lstVariables->GetItemCount();
        int pos;

        // check for changed or added vars
        for (pos=0 ; pos < cnt ; pos++)
        {
            wxString newVar=lstVariables->GetText(pos);
            wxString newVal=lstVariables->GetText(pos, 1);

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

        long id=StrToLong(txtID->GetValue());

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
        if (datValidUntil->GetDate().IsValid())
            sql += wxT("\n   VALID UNTIL ") + qtString(DateToAnsiStr(datValidUntil->GetDate() + timValidUntil->GetValue())); 
        else
            sql += wxT("\n   VALID UNTIL 'infinity'");
        sql += wxT(";\n");

        int cnt=lstVariables->GetItemCount();
        int pos;
        for (pos=0 ; pos < cnt ; pos++)
        {
            sql += wxT("ALTER USER ") + qtIdent(name) 
                +  wxT(" SET ") + lstVariables->GetText(pos)
                +  wxT("=") + lstVariables->GetText(pos, 1)
                +  wxT(";\n");
        }

        cnt = lbGroupsIn->GetCount();
        for (pos=0 ; pos < cnt ; pos++)
            sql += wxT("ALTER GROUP ") + qtIdent(lbGroupsIn->GetString(pos))
                +  wxT(" ADD USER ") + qtIdent(name) + wxT(";\n");
    }
    return sql;
}

