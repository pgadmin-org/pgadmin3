//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// dlgGroup.cpp - PostgreSQL Group Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// Images
#include "images/group.xpm"

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "dlgGroup.h"
#include "pgGroup.h"


// pointer to controls
#define txtID           CTRL("txtID", wxTextCtrl)

#define lbUsersNotIn    CTRL("lbUsersNotIn", wxListBox)
#define lbUsersIn       CTRL("lbUsersIn", wxListBox)
#define btnAddUser      CTRL("btnAddUser", wxButton)
#define btnDelUser      CTRL("btnDelUser", wxButton)



BEGIN_EVENT_TABLE(dlgGroup, dlgProperty)
    EVT_TEXT(XRCID("txtName"),                      dlgGroup::OnChange)
    
    EVT_LISTBOX_DCLICK(XRCID("lbUsersNotIn"),       dlgGroup::OnUserAdd)
    EVT_LISTBOX_DCLICK(XRCID("lbUsersIn"),          dlgGroup::OnUserRemove)
    EVT_BUTTON(XRCID("btnAddUser"),                 dlgGroup::OnUserAdd)
    EVT_BUTTON(XRCID("btnDelUser"),                 dlgGroup::OnUserRemove)
END_EVENT_TABLE();



dlgGroup::dlgGroup(frmMain *frame, pgGroup *node)
: dlgProperty(frame, wxT("dlgGroup"))
{
    group=node;
    SetIcon(wxIcon(group_xpm));
}


pgObject *dlgGroup::GetObject()
{
    return group;
}


int dlgGroup::Go(bool modal)
{
    pgSet *set=connection->ExecuteSet(wxT("SELECT usename FROM pg_shadow"));
    if (set)
    {
        while (!set->Eof())
        {
            wxString userName=set->GetVal(wxT("usename"));

            if (group && group->GetUsersIn().Index(userName) >= 0)
                lbUsersIn->Append(userName);
            else
                lbUsersNotIn->Append(userName);

            set->MoveNext();
        }
        delete set;
    }

    if (group)
    {
        // Edit Mode
        txtName->SetValue(group->GetIdentifier());
        txtID->SetValue(NumToStr(group->GetGroupId()));
        txtName->Disable();
        txtID->Disable();
    }
    else
    {
        txtID->SetValidator(numericValidator);
    }

    return dlgProperty::Go(modal);
}


void dlgGroup::OnChange(wxNotifyEvent &ev)
{
    if (group)
    {
        btnOK->Enable(!GetSql().IsEmpty());
    }
    else
    {
        wxString name=GetName();

        bool enable=true;
        CheckValid(enable, !name.IsEmpty(), wxT("Please specify name."));

        EnableOK(enable);
    }
}


void dlgGroup::OnUserAdd(wxNotifyEvent &ev)
{
    int pos=lbUsersNotIn->GetSelection();
    if (pos >= 0)
    {
        lbUsersIn->Append(lbUsersNotIn->GetString(pos));
        lbUsersNotIn->Delete(pos);
    }
    OnChange(ev);
}


void dlgGroup::OnUserRemove(wxNotifyEvent &ev)
{
    int pos=lbUsersIn->GetSelection();
    if (pos >= 0)
    {
        lbUsersNotIn->Append(lbUsersIn->GetString(pos));
        lbUsersIn->Delete(pos);
    }
    OnChange(ev);
}




pgObject *dlgGroup::CreateObject(pgCollection *collection)
{
    wxString name=GetName();

    pgObject *obj=pgGroup::ReadObjects(collection, 0, wxT("\n WHERE groname=") + qtString(name));
    return obj;
}


wxString dlgGroup::GetSql()
{
    wxString sql;
    wxString name=GetName();
    int cnt, pos;

    if (group)
    {
        // Edit Mode
    
        cnt=lbUsersIn->GetCount();
        wxArrayString tmpUsers=group->GetUsersIn();

        // check for added users
        for (pos=0 ; pos < cnt ; pos++)
        {
            wxString userName=lbUsersIn->GetString(pos);

            int index=tmpUsers.Index(userName);
            if (index >= 0)
                tmpUsers.RemoveAt(index);
            else
                sql += wxT("ALTER GROUP ") + qtIdent(name)
                    +  wxT(" ADD USER ") + qtIdent(userName) + wxT(";\n");
        }
        
        // check for removed users
        for (pos=0 ; pos < (int)tmpUsers.GetCount() ; pos++)
        {
            sql += wxT("ALTER GROUP ") + qtIdent(name)
                +  wxT(" DROP USER ") + qtIdent(tmpUsers.Item(pos)) + wxT(";\n");
        }
    }
    else
    {
        // Create Mode
        wxString name=GetName();

        long id=atol(txtID->GetValue());

        sql = wxT(
            "CREATE GROUP ") + qtIdent(name);
        if (id)
            sql += wxT("\n  WITH SYSID ") + NumToStr(id);
        cnt = lbUsersIn->GetCount();
        if (cnt)
            sql += wxT("\n   USER ");
        for (pos=0 ; pos < cnt ; pos++)
        {
            if (pos)
                sql += wxT(", ");
            sql += qtIdent(lbUsersIn->GetString(pos));
        }
        sql += wxT(";\n");

    }
    return sql;
}

