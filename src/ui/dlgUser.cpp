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


BEGIN_EVENT_TABLE(dlgUser, dlgProperty)
    EVT_TEXT(XRCID("txtUser"), dlgUser::OnChange)
END_EVENT_TABLE();


dlgUser::dlgUser(wxFrame *frame, pgUser *node)
: dlgProperty(frame, wxT("dlgUser"))
{
    user=node;
    if (user)
    {
        // Edit Mode
        CTRL("txtUser", wxTextCtrl)->SetValue(user->GetIdentifier());
        CTRL("txtID", wxTextCtrl)->SetValue(NumToStr(user->GetUserId()));
        CTRL("chkCreateDB", wxCheckBox)->SetValue(user->GetCreateDatabase());
        CTRL("chkCreateUser", wxCheckBox)->SetValue(user->GetSuperuser());
        CTRL("txtUser", wxTextCtrl)->Disable();
        CTRL("txtID", wxTextCtrl)->Disable();
    }
    else
    {
        wxTextValidator numval(wxFILTER_NUMERIC);
        CTRL("txtID", wxTextCtrl)->SetValidator(numval);
        CTRL("btnOK", wxButton)->Disable();
    }

    Show();
}



void dlgUser::OnChange(wxNotifyEvent &ev)
{
    if (!user)
    {
        wxString name=CTRL("txtUser", wxTextCtrl)->GetValue();

        CTRL("btnOK", wxButton)->Enable(!name.IsEmpty());
    }
}



pgObject *dlgUser::CreateObject(pgCollection *collection)
{
    wxString name=CTRL("txtUser", wxTextCtrl)->GetValue();

    pgObject *obj=pgUser::ReadObjects(collection, 0, wxT("\n WHERE usename=") + qtString(name));
    return obj;
}


wxString dlgUser::GetSql()
{
    wxString sql;
    
    wxString passwd=CTRL("txtPasswd", wxTextCtrl)->GetValue();
    bool createDB=CTRL("chkCreateDB", wxCheckBox)->GetValue(),
         createUser=CTRL("chkCreateUser", wxCheckBox)->GetValue();

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
            return wxT("ALTER USER ") + user->GetQuotedFullIdentifier() + sql + wxT(";\n");
    }
    else
    {
        // Create Mode
        wxString name=CTRL("txtUser", wxTextCtrl)->GetValue();
        if (!name.IsEmpty())
        {
            long id=atol(CTRL("txtID", wxTextCtrl)->GetValue());

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
        }
    }
    return sql;
}

