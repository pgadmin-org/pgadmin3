//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgDatabase.cpp - PostgreSQL Database Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "dlg/dlgDatabase.h"
#include "schema/pgDatabase.h"


// pointer to controls
#define cbEncoding      CTRL_COMBOBOX("cbEncoding")
#define cbTemplate      CTRL_COMBOBOX("cbTemplate")
#define stPath          CTRL_STATIC("stPath")
#define txtPath         CTRL_TEXT("txtPath")
#define cbTablespace    CTRL_COMBOBOX("cbTablespace")
#define txtSchemaRestr  CTRL_TEXT("txtSchemaRestr")

#define lstVariables    CTRL_LISTVIEW("lstVariables")
#define cbVarname       CTRL_COMBOBOX2("cbVarname")
#define txtValue        CTRL_TEXT("txtValue")
#define chkValue        CTRL_CHECKBOX("chkValue")
#define btnAdd          CTRL_BUTTON("wxID_ADD")
#define btnRemove       CTRL_BUTTON("wxID_REMOVE")


dlgProperty *pgDatabaseFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
    dlgDatabase *dlg=new dlgDatabase(this, frame, (pgDatabase*)node);
    if (dlg && !node)
    {
        // use the server's connection to avoid "template1 in use"
        dlg->connection=parent->GetConnection();
    }
    return dlg;
}


BEGIN_EVENT_TABLE(dlgDatabase, dlgSecurityProperty)
    EVT_TEXT(XRCID("txtPath"),                      dlgProperty::OnChange)
    EVT_TEXT(XRCID("cbTablespace"),                 dlgProperty::OnChange)
    EVT_COMBOBOX(XRCID("cbTablespace"),             dlgProperty::OnChange)
    EVT_TEXT(XRCID("cbEncoding"),                   dlgProperty::OnChange)
    EVT_COMBOBOX(XRCID("cbEncoding"),               dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtSchemaRestr"),               dlgDatabase::OnChangeRestr)
    EVT_LIST_ITEM_SELECTED(XRCID("lstVariables"),   dlgDatabase::OnVarSelChange)
    EVT_BUTTON(wxID_ADD,                            dlgDatabase::OnVarAdd)
    EVT_BUTTON(wxID_REMOVE,                         dlgDatabase::OnVarRemove)
    EVT_TEXT(XRCID("cbVarname"),                    dlgDatabase::OnVarnameSelChange)
    EVT_COMBOBOX(XRCID("cbVarname"),                dlgDatabase::OnVarnameSelChange)
    EVT_BUTTON(wxID_OK,                             dlgDatabase::OnOK)
END_EVENT_TABLE();


dlgDatabase::dlgDatabase(pgaFactory *f, frmMain *frame, pgDatabase *node)
: dlgSecurityProperty(f, frame, node, wxT("dlgDatabase"), wxT("CREATE,TEMP,CONNECT"), "CTc")
{
    database=node;
    schemaRestrictionOk=true;
    lstVariables->CreateColumns(0, _("Variable"), _("Value"));

    chkValue->Hide();

	dirtyVars = false;
}

pgObject *dlgDatabase::GetObject()
{
    return database;
}


wxString dlgDatabase::GetHelpPage() const
{
    if (nbNotebook->GetSelection() == 1)
        return wxT("pg/runtime-config");
    return dlgSecurityProperty::GetHelpPage();
}


int dlgDatabase::Go(bool modal)
{
    if (!database)
        cbOwner->Append(wxT(""));

    AddGroups();
    AddUsers(cbOwner);


    if (connection->BackendMinimumVersion(8, 0))
    {
        stPath->SetLabel(_("Tablespace"));
        txtPath->Hide();
    }
    else
    {
        cbTablespace->Hide();
    }

    pgSet *set;
    if (connection->BackendMinimumVersion(7, 4))
        set=connection->ExecuteSet(wxT("SELECT name, vartype, min_val, max_val\n")
                wxT("  FROM pg_settings WHERE context in ('user', 'superuser')"));
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

    if (database)
    {
        // edit mode

		// Even with 7.4+, we don't currently have a way to change the database name
		// as it must be done from a different database.
        // if (!connection->BackendMinimumVersion(7, 4))
            txtName->Disable();

        if (!connection->BackendMinimumVersion(8, 0))
            cbOwner->Disable();

        readOnly = !database->GetServer()->GetCreatePrivilege();

        if (readOnly)
        {
            cbVarname->Disable();
            txtValue->Disable();
            btnAdd->Disable();
            btnRemove->Disable();
        }

        size_t i;
        for (i=0 ; i < database->GetVariables().GetCount() ; i++)
        {
            wxString item=database->GetVariables().Item(i);
            lstVariables->AppendItem(0, item.BeforeFirst('='), item.AfterFirst('='));
        }

        PrepareTablespace(cbTablespace, database->GetTablespaceOid());
		cbTablespace->Disable();
        txtPath->SetValue(database->GetPath());
        txtPath->Disable();

        cbEncoding->Append(database->GetEncoding());
        cbEncoding->SetSelection(0);

        cbTemplate->Disable();
        cbEncoding->Disable();

        txtSchemaRestr->SetValue(database->GetSchemaRestriction());
    }
    else
    {
        // create mode
        if (!connection->BackendMinimumVersion(8, 2))
            txtComment->Disable();

        PrepareTablespace(cbTablespace);

        // Add the default tablespace (note the hack to 
        // avoid a string change close to release - this 
        // can be removed from SVN-Trunk at some point)
        wxString dt = wxString::Format(wxT("<%s>"), _("Default tablespace"));
        dt.LowerCase();
        cbTablespace->Insert(dt, 0, (void *)0);
        cbTablespace->SetSelection(0);

        cbTemplate->Append(wxEmptyString);
        FillCombobox(wxT("SELECT datname FROM pg_database ORDER BY datname"), cbTemplate);

        long encNo=0;
        wxString encStr;
        do
        {
            encStr=connection->ExecuteScalar(
                wxT("SELECT pg_encoding_to_char(") + NumToStr(encNo) + wxT(")"));
            if (!encStr.IsEmpty())
                cbEncoding->Append(encStr);

            encNo++;
        }
        while (!encStr.IsEmpty());

        encStr=connection->ExecuteScalar(wxT("SELECT pg_encoding_to_char(encoding) FROM pg_database WHERE datname = 'template0'"));
        encNo=cbEncoding->FindString(encStr);

        if (encNo < 0) 
        {
            encNo=cbEncoding->FindString(wxT("UNICODE"));
            if (encNo < 0)
                encNo=cbEncoding->FindString(wxT("UTF8"));
        }

        if (encNo >= 0)
            cbEncoding->SetSelection(encNo);

    }

    // Find, and disable the CONNECT ACL option if we're on pre 8.2
    if (!connection->BackendMinimumVersion(8, 2))
    {
        // Disable the checkbox
        if (!DisablePrivilege(wxT("CONNECT")))
            wxLogError(_("Failed to disable the CONNECT privilege checkbox!"));
    }

    SetupVarEditor(1);
    return dlgSecurityProperty::Go(modal);
}


pgObject *dlgDatabase::CreateObject(pgCollection *collection)
{
    wxString name=GetName();

    pgObject *obj=databaseFactory.CreateObjects(collection, 0, wxT(" WHERE datname=") + qtDbString(name) + wxT("\n"));
    return obj;
}


void dlgDatabase::OnChangeRestr(wxCommandEvent &ev)
{
    if (txtSchemaRestr->GetValue().IsEmpty())
        schemaRestrictionOk = true;
    else
    {
        wxString sql=wxT("EXPLAIN SELECT 1 FROM pg_namespace nsp\n")
                wxT("  JOIN pg_description des ON des.objoid=nsp.oid\n")
                wxT(" WHERE (") + txtSchemaRestr->GetValue() + wxT(")");

        wxLogNull nix;
        wxString result=connection->ExecuteScalar(sql);

        schemaRestrictionOk = !result.IsEmpty();
    }
    OnChange(ev);
}


void dlgDatabase::OnOK(wxCommandEvent &ev)
{
    if (database)
    {
        database->iSetSchemaRestriction(txtSchemaRestr->GetValue().Trim());
        settings->Write(wxString::Format(wxT("Servers/%d/Databases/%s/SchemaRestriction"), database->GetServer()->GetServerIndex(), database->GetName().c_str()), txtSchemaRestr->GetValue().Trim());
    }
    dlgSecurityProperty::OnOK(ev);
}


void dlgDatabase::CheckChange()
{
    bool enable=true;
    
    if (database)
    {
        enable = txtSchemaRestr->GetValue() != database->GetSchemaRestriction()
               || txtComment->GetValue() != database->GetComment()
			   || txtName->GetValue() != database->GetName()
               || cbOwner->GetValue() != database->GetOwner()
			   || dirtyVars;
    }

    CheckValid(enable, !GetName().IsEmpty(), _("Please specify name."));
    CheckValid(enable, schemaRestrictionOk, _("Restriction not valid."));

    EnableOK(enable);
}


void dlgDatabase::OnVarnameSelChange(wxCommandEvent &ev)
{
    int sel=cbVarname->GuessSelection(ev);

    SetupVarEditor(sel);
}

void dlgDatabase::SetupVarEditor(int var)
{
    if (var >= 0 && varInfo.Count() > 0)
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

void dlgDatabase::OnVarSelChange(wxListEvent &ev)
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


void dlgDatabase::OnVarAdd(wxCommandEvent &ev)
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
	dirtyVars = true;
    CheckChange();
}


void dlgDatabase::OnVarRemove(wxCommandEvent &ev)
{
    lstVariables->DeleteCurrentItem();
	dirtyVars = true;
    CheckChange();
}


// Note: CREATE DATABASE cannot be part of a multi-statement query as of 
//       PG83, and never actually would have been transaction-safe prior
//       to then. Therefore, when creating a new database, only the CREATE
//       statement comes from GetSql(), subsequent ALTERs come from GetSql2()
wxString dlgDatabase::GetSql()
{
    wxString sql, name;
    name=GetName();

    if (database)
    {
        // edit mode

        AppendNameChange(sql);
        AppendOwnerChange(sql, wxT("DATABASE ") + qtIdent(name));

        AppendComment(sql, wxT("DATABASE"), 0, database);

        if (!connection->BackendMinimumVersion(8, 2))
            sql += GetGrant(wxT("CT"), wxT("DATABASE ") + qtIdent(name));
        else
            sql += GetGrant(wxT("CTc"), wxT("DATABASE ") + qtIdent(name));

        wxArrayString vars;

        size_t index;

        for (index = 0 ; index < database->GetVariables().GetCount() ; index++)
            vars.Add(database->GetVariables().Item(index));

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
                sql += wxT("ALTER DATABASE ") + qtIdent(name)
                    +  wxT(" SET ") + newVar
                    +  wxT("=") + newVal
                    +  wxT(";\n");
            }
        }
        
        // check for removed vars
        for (pos=0 ; pos < (int)vars.GetCount() ; pos++)
        {
            sql += wxT("ALTER DATABASE ") + qtIdent(name)
                +  wxT(" RESET ") + vars.Item(pos).BeforeFirst('=')
                + wxT(";\n");
        }
    }
    else
    {
        // create mode
        sql = wxT("CREATE DATABASE ") + qtIdent(name) 
            + wxT("\n  WITH ENCODING=") + qtDbString(cbEncoding->GetValue());

        AppendIfFilled(sql, wxT("\n       OWNER="), qtIdent(cbOwner->GetValue()));
        AppendIfFilled(sql, wxT("\n       TEMPLATE="), qtIdent(cbTemplate->GetValue()));
        AppendIfFilled(sql, wxT("\n       LOCATION="), txtPath->GetValue());
        if (cbTablespace->GetCurrentSelection() > 0 && cbTablespace->GetOIDKey() > 0)
            sql += wxT("\n       TABLESPACE=") + qtIdent(cbTablespace->GetValue());

        sql += wxT(";\n");
    }

    return sql;
}

wxString dlgDatabase::GetSql2()
{
    wxString sql, name;
    name=GetName();

    // We only use GetSql2() in the CREATE case
    if (!database)
    {
        if (connection->BackendMinimumVersion(8, 2))
            AppendComment(sql, wxT("DATABASE"), 0, database);

        if (!connection->BackendMinimumVersion(8, 2))
            sql += GetGrant(wxT("CT"), wxT("DATABASE ") + qtIdent(name));
        else
            sql += GetGrant(wxT("CTc"), wxT("DATABASE ") + qtIdent(name));

        int cnt=lstVariables->GetItemCount();
        int pos;

        // check for changed or added vars
        for (pos=0 ; pos < cnt ; pos++)
        {
            sql += wxT("ALTER DATABASE ") + qtIdent(name)
                +  wxT(" SET ") + lstVariables->GetText(pos)
                +  wxT("=") + lstVariables->GetText(pos, 1)
                +  wxT(";\n");
        }
    }

    return sql;
}


