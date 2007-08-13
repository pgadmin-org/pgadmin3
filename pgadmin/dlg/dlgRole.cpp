//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgRole.cpp - PostgreSQL Role Property
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "utils/misc.h"
#include "dlg/dlgRole.h"
#include "schema/pgRole.h"


// pointer to controls
#define txtPasswd       CTRL_TEXT("txtPasswd")
#define txtRePasswd     CTRL_TEXT("txtRePasswd")
#define datValidUntil   CTRL_CALENDAR("datValidUntil")
#define timValidUntil   CTRL_TIME("timValidUntil")
#define chkCanLogin     CTRL_CHECKBOX("chkCanLogin")
#define chkSuperuser    CTRL_CHECKBOX("chkSuperuser")
#define chkInherits     CTRL_CHECKBOX("chkInherits")
#define chkCreateDB     CTRL_CHECKBOX("chkCreateDB")
#define chkCreateRole   CTRL_CHECKBOX("chkCreateRole")
#define chkUpdateCat    CTRL_CHECKBOX("chkUpdateCat")

#define lbRolesNotIn    CTRL_LISTBOX("lbRolesNotIn")
#define lbRolesIn       CTRL_LISTBOX("lbRolesIn")
#define btnAddRole      CTRL_BUTTON("btnAddRole")
#define btnDelRole      CTRL_BUTTON("btnDelRole")
#define chkAdminOption  CTRL_CHECKBOX("chkAdminOption")

#define lstVariables    CTRL_LISTVIEW("lstVariables")
#define btnAdd          CTRL_BUTTON("wxID_ADD")
#define btnRemove       CTRL_BUTTON("wxID_REMOVE")
#define cbVarname       CTRL_COMBOBOX2("cbVarname")
#define txtValue        CTRL_TEXT("txtValue")
#define chkValue        CTRL_CHECKBOX("chkValue")



dlgProperty *pgLoginRoleFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
    return new dlgRole(this, frame, (pgRole*)node, true);
}

dlgProperty *pgGroupRoleFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
    return new dlgRole(this, frame, (pgRole*)node, false);
}

BEGIN_EVENT_TABLE(dlgRole, dlgProperty)
    EVT_CALENDAR_SEL_CHANGED(XRCID("datValidUntil"),dlgRole::OnChangeCal)
    EVT_DATE_CHANGED(XRCID("datValidUntil"),        dlgRole::OnChangeDate)
    EVT_SPIN(XRCID("timValidUntil"),                dlgRole::OnChangeSpin)
    
    EVT_LISTBOX_DCLICK(XRCID("lbRolesNotIn"),       dlgRole::OnRoleAdd)
    EVT_LISTBOX_DCLICK(XRCID("lbRolesIn"),          dlgRole::OnRoleRemove)
    EVT_TEXT(XRCID("txtPasswd"),                    dlgRole::OnChangePasswd)
    EVT_TEXT(XRCID("txtRePasswd"),                  dlgRole::OnChangePasswd)
    EVT_CHECKBOX(XRCID("chkCanLogin"),              dlgRole::OnChange)
    EVT_CHECKBOX(XRCID("chkInherits"),              dlgRole::OnChange)
    EVT_CHECKBOX(XRCID("chkCreateDB"),              dlgRole::OnChange)
    EVT_CHECKBOX(XRCID("chkUpdateCat"),             dlgRole::OnChange)
    EVT_CHECKBOX(XRCID("chkSuperuser"),             dlgRole::OnChangeSuperuser)
    EVT_CHECKBOX(XRCID("chkCreateRole"),            dlgRole::OnChange)

    EVT_BUTTON(XRCID("btnAddRole"),                 dlgRole::OnRoleAdd)
    EVT_BUTTON(XRCID("btnDelRole"),                 dlgRole::OnRoleRemove)

    EVT_LIST_ITEM_SELECTED(XRCID("lstVariables"),   dlgRole::OnVarSelChange)
    EVT_BUTTON(wxID_ADD,                            dlgRole::OnVarAdd)
    EVT_BUTTON(wxID_REMOVE,                         dlgRole::OnVarRemove)
    EVT_TEXT(XRCID("cbVarname"),                    dlgRole::OnVarnameSelChange)
    EVT_COMBOBOX(XRCID("cbVarname"),                dlgRole::OnVarnameSelChange)

    EVT_BUTTON(wxID_OK,                             dlgRole::OnOK)
END_EVENT_TABLE();



dlgRole::dlgRole(pgaFactory *f, frmMain *frame, pgRole *node, bool chkLogin)
: dlgProperty(f, frame, wxT("dlgRole"))
{
    role=node;
    lstVariables->CreateColumns(0, _("Variable"), _("Value"), -1);
    btnOK->Disable();
    chkValue->Hide();
    if (chkLogin)
        chkCanLogin->SetValue(true);
}


pgObject *dlgRole::GetObject()
{
    return role;
}


int dlgRole::Go(bool modal)
{
    wxString roleSql=
        wxT("SELECT rolname\n")
        wxT("  FROM pg_roles r\n");

    if (role)
    {
        wxArrayString roles=role->GetRolesIn();
        size_t i;
        for (i=0 ; i < roles.GetCount() ; i++)
            lbRolesIn->Append(roles.Item(i));

        roleSql += 
            wxT("  LEFT JOIN pg_auth_members ON r.oid=roleid AND member = ") + role->GetOidStr() + wxT("\n")
            wxT(" WHERE r.oid <> ") + role->GetOidStr() + wxT("\n")
            wxT("   AND roleid IS NULL");

        // Edit Mode
        if (role->GetServer()->GetSuperUser() || role->GetServer()->GetCreateRole()) 
			readOnly=false;
		else
			readOnly=true;

        chkCreateDB->SetValue(role->GetCreateDatabase());
        chkCreateRole->SetValue(role->GetCreateRole());
        chkSuperuser->SetValue(role->GetSuperuser());
        chkInherits->SetValue(role->GetInherits());
        chkUpdateCat->SetValue(role->GetUpdateCatalog());
        chkCanLogin->SetValue(role->GetCanLogin());
        datValidUntil->SetValue(role->GetAccountExpires());
        timValidUntil->SetTime(role->GetAccountExpires());
        txtComment->SetValue(role->GetComment());

        size_t index;
        for (index = 0 ; index < role->GetConfigList().GetCount() ; index++)
        {
            wxString item=role->GetConfigList().Item(index);
            lstVariables->AppendItem(0, item.BeforeFirst('='), item.AfterFirst('='));
        }

        timValidUntil->Enable(!readOnly && role->GetAccountExpires().IsValid());

        if (readOnly)
        {
            chkCanLogin->Disable();
            chkCreateDB->Disable();
            chkCreateRole->Disable();
            chkSuperuser->Disable();
            chkInherits->Disable();
            chkUpdateCat->Disable();
            datValidUntil->Disable();
            timValidUntil->Disable();
            txtPasswd->Disable();
            txtRePasswd->Disable();
            btnAddRole->Disable();
            btnDelRole->Disable();
            cbVarname->Disable();
            txtValue->Disable();
            btnAdd->Disable();
            btnRemove->Disable();
        }
        else
        {
            varInfo.Add(wxT("role"));
            cbVarname->Append(wxT("role"));

            pgSet *set;
            set=connection->ExecuteSet(wxT("SELECT name, vartype, min_val, max_val\n")
                    wxT("  FROM pg_settings WHERE context in ('user', 'superuser')"));

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
        chkCanLogin->Disable();
        wxDateTime empty;
        datValidUntil->SetValue(empty);
        timValidUntil->Disable();
    }

    // Role comments are only appropriate in 8.2+
    if (!connection->BackendMinimumVersion(8, 2))
        txtComment->Disable();

    if (!settings->GetShowUsersForPrivileges())
    {
        if (role)
            roleSql += wxT("\n   AND NOT rolcanlogin");
        else
            roleSql += wxT("\n WHERE NOT rolcanlogin");
    }
    roleSql += wxT("\n ORDER BY rolname");

    pgSetIterator roles(connection, roleSql);

    while (roles.RowsLeft())
        lbRolesNotIn->Append(roles.GetVal(wxT("rolname")));

    return dlgProperty::Go(modal);
}


wxString dlgRole::GetHelpPage() const
{
    if (nbNotebook->GetSelection() == 2)
        return wxT("pg/runtime-config");
    else
        return wxT("pg/sql-createrole");
}


void dlgRole::OnOK(wxCommandEvent &ev)
{
    dlgProperty::OnOK(ev);

    if (role && role->GetCanLogin() != chkCanLogin->GetValue() == !btnOK->IsEnabled())
    {
        // LOGIN attribute changed successfully; need to put object under different collection
    }
}


void dlgRole::OnChangeCal(wxCalendarEvent &ev)
{
	CheckChange();
}


void dlgRole::OnChangeDate(wxDateEvent &ev)
{
	CheckChange();

    bool timEn=ev.GetDate().IsValid();
    timValidUntil->Enable(timEn);
    if (!timEn)
        timValidUntil->SetTime(wxDefaultDateTime);
}

void dlgRole::OnChangeSpin(wxSpinEvent &ev)
{
	CheckChange();
}


void dlgRole::OnChangeSuperuser(wxCommandEvent &ev)
{
    if (role && role->GetSuperuser() && !chkSuperuser->GetValue())
    {
        wxMessageDialog dlg(this,
            _("Deleting a superuser might result in unwanted behaviour (e.g. when restoring the database).\nAre you sure?"),
            _("Confirm superuser deletion"),
                     wxICON_EXCLAMATION | wxYES_NO |wxNO_DEFAULT);
        if (dlg.ShowModal() != wxID_YES)
        {
            chkSuperuser->SetValue(true);
            return;
        }
    }
    CheckChange();
}

void dlgRole::OnChangePasswd(wxCommandEvent &ev)
{
	wxString passwd=txtPasswd->GetValue();
	wxString repasswd=txtRePasswd->GetValue();
	if ((!repasswd.IsEmpty())||(!repasswd.IsEmpty()))
	{
           bool enable=true;
           CheckValid(enable, !passwd.compare(repasswd), _("The passwords entered do not match!"));
           EnableOK(enable);
	}
}

void dlgRole::CheckChange()
{
    bool timEn=datValidUntil->GetValue().IsValid();
    timValidUntil->Enable(timEn);
    if (!timEn)
        timValidUntil->SetTime(wxDefaultDateTime);

    if (!readOnly)
        chkUpdateCat->Enable(chkSuperuser->GetValue());

    if (!role)
    {
        wxString name=GetName();

        bool enable=true;
        CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
        EnableOK(enable);
    }
    else
    {
        EnableOK(!GetSql().IsEmpty());
    }
}


void dlgRole::OnRoleAdd(wxCommandEvent &ev)
{
    if (!readOnly)
    {
        int pos=lbRolesNotIn->GetSelection();
        if (pos >= 0)
        {
            wxString roleName=lbRolesNotIn->GetString(pos);
            if (chkAdminOption->GetValue())
                roleName += PGROLE_ADMINOPTION;
            lbRolesIn->Append(roleName);
            lbRolesNotIn->Delete(pos);
        }
        CheckChange();
    }
}


void dlgRole::OnRoleRemove(wxCommandEvent &ev)
{
    if (!readOnly)
    {
        int pos=lbRolesIn->GetSelection();
        if (pos >= 0)
        {
            wxString role=lbRolesIn->GetString(pos);
            if (role.Right(PGROLE_ADMINOPTION_LEN) == PGROLE_ADMINOPTION)
                role = role.Left(role.Length()-PGROLE_ADMINOPTION_LEN);

            lbRolesNotIn->Append(role);
            lbRolesIn->Delete(pos);
        }
        CheckChange();
    }
}


void dlgRole::OnVarnameSelChange(wxCommandEvent &ev)
{
    int sel=cbVarname->GuessSelection(ev);

    SetupVarEditor(sel);
}

void dlgRole::SetupVarEditor(int var)
{
    if (var >= 0)
    {
        wxStringTokenizer vals(varInfo.Item(var));
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

void dlgRole::OnVarSelChange(wxListEvent &ev)
{
    long pos=lstVariables->GetSelection();
    if (pos >= 0)
    {
        wxString value=lstVariables->GetText(pos, 1);
        cbVarname->SetValue(lstVariables->GetText(pos));


        // We used to raise an OnVarnameSelChange() event here, but
        // at this point the combo box hasn't necessarily updated.
        int sel = cbVarname->FindString(lstVariables->GetText(pos));
        SetupVarEditor(sel);

        txtValue->SetValue(value);
        chkValue->SetValue(value == wxT("on"));
    }
}


void dlgRole::OnVarAdd(wxCommandEvent &ev)
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
    CheckChange();
}


void dlgRole::OnVarRemove(wxCommandEvent &ev)
{
    lstVariables->DeleteCurrentItem();
    CheckChange();
}


pgObject *dlgRole::CreateObject(pgCollection *collection)
{
    wxString name=GetName();

    pgObject *obj=loginRoleFactory.CreateObjects(collection, 0, wxT("\n WHERE rolname=") + qtDbString(name));
    return obj;
}


wxString dlgRole::GetSql()
{
    wxString sql;
    wxString name=GetName();
    
    wxString passwd=txtPasswd->GetValue();
    bool createDB=chkCreateDB->GetValue(),
         createRole=chkCreateRole->GetValue(),
         superuser=chkSuperuser->GetValue(),
         inherits=chkInherits->GetValue(),
         canLogin=chkCanLogin->GetValue();

    if (role)
    {
        // Edit Mode

        AppendNameChange(sql, wxT("ROLE ") + role->GetQuotedFullIdentifier());


        wxString options;
        if (canLogin != role->GetCanLogin())
        {
            if (canLogin)
                options = wxT(" LOGIN");
            else
                options = wxT(" NOLOGIN");
        }
        if (canLogin && !passwd.IsEmpty())
            options += wxT(" ENCRYPTED PASSWORD ") + qtDbString(connection->EncryptPassword(name, passwd));

        if (createDB != role->GetCreateDatabase() || createRole != role->GetCreateRole() 
            || superuser != role->GetSuperuser() || inherits != role->GetInherits())
        {
            options += wxT("\n ");
        
            if (superuser != role->GetSuperuser())
            {
                if (superuser)
                    options += wxT(" SUPERUSER");
                else
                    options += wxT(" NOSUPERUSER");
            }
            if (inherits != role->GetInherits())
            {
                if (inherits)
                    options += wxT(" INHERIT");
                else
                    options += wxT(" NOINHERIT");
            }
            if (createDB != role->GetCreateDatabase())
            {
                if (createDB)
                    options += wxT(" CREATEDB");
                else
                    options += wxT(" NOCREATEDB");
            }
            if (createRole != role->GetCreateRole())
            {
                if (createRole)
                    options += wxT(" CREATEROLE");
                else
                    options += wxT(" NOCREATEROLE");
            }
        }
        if (DateToStr(datValidUntil->GetValue()) != DateToStr(role->GetAccountExpires()))
        {
            if (datValidUntil->GetValue().IsValid())
                options += wxT("\n   VALID UNTIL ") + qtDbString(DateToAnsiStr(datValidUntil->GetValue() + timValidUntil->GetValue())); 
            else
                options += wxT("\n   VALID UNTIL 'infinity'");
        }

        if (!options.IsNull())
            sql += wxT("ALTER Role ") + qtIdent(name) + options + wxT(";\n");



        wxArrayString vars;
        size_t index;
        for (index = 0 ; index < role->GetConfigList().GetCount() ; index++)
            vars.Add(role->GetConfigList().Item(index));

        int cnt=lstVariables->GetItemCount();
        int pos;

        // check for changed or added vars
        for (pos=0 ; pos < cnt ; pos++)
        {
            wxString newVar=lstVariables->GetText(pos);
            wxString newVal=lstVariables->GetText(pos, 1);

            wxString oldVal;

            for (index=0 ; index < vars.GetCount() ; index++)
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
                sql += wxT("ALTER ROLE ") + qtIdent(name)
                    +  wxT(" SET ") + newVar
                    +  wxT("=") + newVal
                    +  wxT(";\n");
            }
        }
        
        // check for removed vars
        for (pos=0 ; pos < (int)vars.GetCount() ; pos++)
        {
            sql += wxT("ALTER Role ") + qtIdent(name)
                +  wxT(" RESET ") + vars.Item(pos).BeforeFirst('=')
                + wxT(";\n");
        }

        if (chkUpdateCat->GetValue() != role->GetUpdateCatalog())
        {
            if (!connection->HasPrivilege(wxT("Table"), wxT("pg_authid"), wxT("update")))
                sql += wxT(" -- Can't update 'UpdateCatalog privilege: can't write to pg_authid.\n")
                       wxT("-- ");

            sql += wxT("UPDATE pg_authid SET rolcatupdate=") + BoolToStr(chkUpdateCat->GetValue())
                    + wxT(" WHERE OID=") + role->GetOidStr() + wxT(";\n");
        }
        cnt=lbRolesIn->GetCount();
        wxArrayString tmpRoles=role->GetRolesIn();

        // check for added roles
        for (pos=0 ; pos < cnt ; pos++)
        {
            wxString roleName=lbRolesIn->GetString(pos);

            int index=tmpRoles.Index(roleName);
            if (index >= 0)
            {
                // role membership unchanged
                tmpRoles.RemoveAt(index);
            }
            else
            {
                bool admin=false;
                if (roleName.Right(PGROLE_ADMINOPTION_LEN) == PGROLE_ADMINOPTION)
                {
                    admin=true;
                    roleName=roleName.Left(roleName.Length()-PGROLE_ADMINOPTION_LEN);
                }
                else
                {
                    // new role membership without admin option
                    index=tmpRoles.Index(roleName + PGROLE_ADMINOPTION);
                    if (index >= 0)
                    {
                        // old membership with admin option
                        tmpRoles.RemoveAt(index);
                        sql += wxT("REVOKE ADMIN OPTION FOR ") + qtIdent(roleName)
                            + wxT(" FROM ") + qtIdent(name) + wxT(";\n");
                        continue;
                    }
                }

                index=tmpRoles.Index(roleName);
                if (index >= 0)
                {
                    // admin option added to existing membership
                    tmpRoles.RemoveAt(index);
                }

                sql += wxT("GRANT ") + qtIdent(roleName)
                    +  wxT(" TO ") + qtIdent(name);

                if (admin)
                    sql += wxT(" WITH ADMIN OPTION");

                sql += wxT(";\n");
            }
        }
        
        // check for removed roles
        for (pos=0 ; pos < (int)tmpRoles.GetCount() ; pos++)
        {
            sql += wxT("REVOKE ") + qtIdent(tmpRoles.Item(pos))
                +  wxT(" FROM ") + qtIdent(name) + wxT(";\n");
        }
    }
    else
    {
        // Create Mode
        sql = wxT(
            "CREATE ROLE ") + qtIdent(name);
        if (canLogin)
        {
            sql += wxT(" LOGIN");
            if (!passwd.IsEmpty())
                sql += wxT(" ENCRYPTED PASSWORD ") + qtDbString(connection->EncryptPassword(name, passwd));
        }

        if (createDB || createRole ||!inherits || superuser)
            sql += wxT("\n ");
        if (superuser)
            sql += wxT(" SUPERUSER");
        if (!inherits)
            sql += wxT(" NOINHERIT");
        if (createDB)
            sql += wxT(" CREATEDB");
        if (createRole)
            sql += wxT(" CREATEROLE");
        if (datValidUntil->GetValue().IsValid())
            sql += wxT("\n   VALID UNTIL ") + qtDbString(DateToAnsiStr(datValidUntil->GetValue() + timValidUntil->GetValue())); 
        else
            sql += wxT("\n   VALID UNTIL 'infinity'");
        
        int cnt = lbRolesIn->GetCount();
        int pos;

        if (cnt)
        {
            sql += wxT("\n   IN ROLE ");
            for (pos=0 ; pos < cnt ; pos++)
            {
                if (pos)
                    sql += wxT(", ");
                sql += qtIdent(lbRolesIn->GetString(pos));
            }
        }
        sql += wxT(";\n");

        if (chkUpdateCat->GetValue())
            sql += wxT("UPDATE pg_authid SET rolcatupdate=true WHERE rolname=") + qtDbString(name) + wxT(";\n");

        cnt=lstVariables->GetItemCount();
        for (pos=0 ; pos < cnt ; pos++)
        {
            sql += wxT("ALTER ROLE ") + qtIdent(name) 
                +  wxT(" SET ") + lstVariables->GetText(pos)
                +  wxT("=") + lstVariables->GetText(pos, 1)
                +  wxT(";\n");
        }
    }

    AppendComment(sql, wxT("ROLE"), 0, role);

    return sql;
}

