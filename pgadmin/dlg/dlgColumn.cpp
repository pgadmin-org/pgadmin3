//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2009, The pgAdmin Development Team
// This software is released under the BSD Licence
//
// dlgColumns.cpp - PostgreSQL Columns Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "utils/pgDefs.h"

#include "dlg/dlgColumn.h"
#include "schema/pgSchema.h"
#include "schema/pgColumn.h"
#include "schema/pgTable.h"
#include "schema/pgDatatype.h"
#include "frm/frmMain.h"
#include "schema/pgUser.h"
#include "schema/pgGroup.h"


// pointer to controls
#define txtDefault          CTRL_TEXT("txtDefault")
#define chkNotNull          CTRL_CHECKBOX("chkNotNull")
#define txtAttstattarget    CTRL_TEXT("txtAttstattarget")
#define txtAttdistinct      CTRL_TEXT("txtAttdistinct")

BEGIN_EVENT_TABLE(dlgColumn, dlgTypeProperty)
    EVT_TEXT(XRCID("txtLength"),                    dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtPrecision"),                 dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtDefault"),                   dlgProperty::OnChange)
    EVT_CHECKBOX(XRCID("chkNotNull"),               dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtAttstattarget"),             dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtAttdistinct"),             dlgProperty::OnChange)
    EVT_TEXT(XRCID("cbDatatype"),                   dlgColumn::OnSelChangeTyp)
    EVT_COMBOBOX(XRCID("cbDatatype"),               dlgColumn::OnSelChangeTyp)
    EVT_BUTTON(CTL_ADDPRIV,                         dlgColumn::OnAddPriv)
    EVT_BUTTON(CTL_DELPRIV,                         dlgColumn::OnDelPriv)
#ifdef __WXMAC__
    EVT_SIZE(                                       dlgColumn::OnChangeSize)
#endif
END_EVENT_TABLE();


dlgProperty *pgColumnFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
    return new dlgColumn(this, frame, (pgColumn*)node, (pgTable*)parent);
}


dlgColumn::dlgColumn(pgaFactory *f, frmMain *frame, pgColumn *node, pgTable *parentNode)
: dlgTypeProperty(f, frame, wxT("dlgColumn"))
{
    column=node;
    table=parentNode;
    wxASSERT(!table || (table->GetMetaType() == PGM_TABLE || table->GetMetaType() == PGM_VIEW || table->GetMetaType() == GP_EXTTABLE || table->GetMetaType() == GP_PARTITION));

    txtAttstattarget->SetValidator(numericValidator);
    if (connection && connection->BackendMinimumVersion(8, 5))
        txtAttdistinct->SetValidator(numericValidator);

    /* Column Level Privileges */
    securityChanged=false;
    if (node)
        connection = node->GetConnection();
    securityPage = new ctlSecurityPanel(nbNotebook, wxT("INSERT,SELECT,UPDATE,REFERENCES"), "arwx", frame->GetImageList());
    if (connection && connection->BackendMinimumVersion(8, 4) && (!node || node->CanCreate()))
    {
        // Fetch Groups Information
        pgSet *setGrp = connection->ExecuteSet(wxT("SELECT groname FROM pg_group ORDER BY groname"));

        if (setGrp)
        {
            while (!setGrp->Eof())
            {
                groups.Add(setGrp->GetVal(0));
                setGrp->MoveNext();
            }
            delete setGrp;
        }

        if (node)
        {
            wxString strAcl = node->GetAcl();
            if (!strAcl.IsEmpty())
            {
                wxArrayString aclArray;
                strAcl = strAcl.Mid(1, strAcl.Length()-2);
                getArrayFromCommaSeparatedList(strAcl, aclArray);
                wxString roleName;
                for (unsigned int index = 0; index < aclArray.Count(); index++)
                {
                    wxString strCurrAcl = aclArray[index];

                    /*
                    * In rare case, we can have ',' (comma) in the user name.
                    * But, we need to handle them also
                    */ 
                    if (strCurrAcl.Find(wxChar('=')) == wxNOT_FOUND)
                    {
                        // Check it is start of the ACL
                        if (strCurrAcl[0U] == (wxChar)'"')
                            roleName = strCurrAcl + wxT(",");
                        continue;
                    }
                    else
                        strCurrAcl = roleName + strCurrAcl;

                    if (strCurrAcl[0U] == (wxChar)'"')
                        strCurrAcl = strCurrAcl.Mid(1, strCurrAcl.Length()-1);
                    roleName = strCurrAcl.BeforeLast('=');

                    wxString value=strCurrAcl.Mid(roleName.Length()+1).BeforeLast('/');

                    int icon = userFactory.GetIconId();

                    if (roleName.Left(6).IsSameAs(wxT("group ")), false)
                    {
                        icon = groupFactory.GetIconId();
                        roleName = wxT("group ") + qtStrip(roleName.Mid(6));
                    }
                    else if (roleName.IsEmpty())
                    {
                        icon = PGICON_PUBLIC;
                        roleName = wxT("public");
                    }
                    else
                    {
                        roleName = qtStrip(roleName);
                        for (unsigned int index=0; index < groups.Count(); index++)
                            if (roleName == groups[index])
                            {
                                roleName = wxT("group ") + roleName;
                                icon = groupFactory.GetIconId();
                                break;
                            }
                    }

                    securityPage->lbPrivileges->AppendItem(icon, roleName, value);
                    currentAcl.Add(roleName + wxT("=") + value);

                    // Reset roleName
                    roleName.Empty();
                }
            }
        }
    }
    else
        securityPage->Disable();

}


#ifdef __WXMAC__
void dlgColumn::OnChangeSize(wxSizeEvent &ev)
{
    securityPage->lbPrivileges->SetSize(wxDefaultCoord, wxDefaultCoord,
        ev.GetSize().GetWidth(), ev.GetSize().GetHeight() - 550);
    if (GetAutoLayout())
    {
        Layout();
    }
}
#endif

pgObject *dlgColumn::GetObject()
{
    return column;
}


int dlgColumn::Go(bool modal)
{
    if (connection->BackendMinimumVersion(8, 4))
    {
        securityPage->SetConnection(connection);
        
        if (securityPage->cbGroups)
        {
            // Fetch Groups Information
            for ( unsigned int index=0; index < groups.Count();)
                securityPage->cbGroups->Append(wxT("group ") + groups[index++]);

            // Fetch Users Information
            if (settings->GetShowUsersForPrivileges())
            {
                securityPage->stGroup->SetLabel(_("Group/User"));
                dlgProperty::AddUsers(securityPage->cbGroups);
            }
        }
        securityPage->lbPrivileges->GetParent()->Layout();
    }
    

    if (column)
    {
        // edit mode
        if (column->GetLength() > 0)
            txtLength->SetValue(NumToStr(column->GetLength()));
        if (column->GetPrecision() >= 0)
            txtPrecision->SetValue(NumToStr(column->GetPrecision()));
        txtDefault->SetValue(column->GetDefault());
        chkNotNull->SetValue(column->GetNotNull());
        txtAttstattarget->SetValue(NumToStr(column->GetAttstattarget()));
        if (connection && connection->BackendMinimumVersion(8, 5))
            txtAttdistinct->SetValue(NumToStr(column->GetAttdistinct()));

        wxString fullType = column->GetRawTypename();
        if (column->GetIsArray())
            fullType += wxT("[]");
        cbDatatype->Append(fullType);
        AddType(wxT("?"), column->GetAttTypId(), fullType);

        if (!column->IsReferenced())
        {
            wxString typeSql=
                wxT("SELECT tt.oid, format_type(tt.oid,NULL) AS typname\n")
                wxT("  FROM pg_cast\n")
                wxT("  JOIN pg_type tt ON tt.oid=casttarget\n")
                wxT(" WHERE castsource=") + NumToStr(column->GetAttTypId()) + wxT("\n");

            if (connection->BackendMinimumVersion(8, 0))
                typeSql += wxT("   AND castcontext IN ('i', 'a')");
            else
                typeSql += wxT("   AND castfunc=0");

            pgSetIterator set(connection, typeSql);

            while (set.RowsLeft())
            {
                if (set.GetVal(wxT("typname")) != column->GetRawTypename())
                {
                    cbDatatype->Append(set.GetVal(wxT("typname")));
                    AddType(wxT("?"), set.GetOid(wxT("oid")), set.GetVal(wxT("typname")));
                }
            }
        }
        if (cbDatatype->GetCount() <= 1)
            cbDatatype->Disable();

        cbDatatype->SetSelection(0);
        wxNotifyEvent ev;
        OnSelChangeTyp(ev);

        previousDefinition=GetDefinition();
        if (column->GetColNumber() < 0)  // Disable controls not valid for system columns
        {
            txtName->Disable();
            txtDefault->Disable();
            chkNotNull->Disable();
            txtLength->Disable();
            cbDatatype->Disable();
            txtAttstattarget->Disable();
            txtAttdistinct->Disable();
        }
        else if (column->GetTable()->GetMetaType() == PGM_VIEW) // Disable controls not valid for view columns
        {
            txtName->Disable();
            chkNotNull->Disable();
            txtLength->Disable();
            cbDatatype->Disable();
            txtAttstattarget->Disable();
            txtAttdistinct->Disable();
        }
         else if (column->GetTable()->GetMetaType() == GP_EXTTABLE) // Disable controls not valid for external table columns
        {
            txtName->Disable();
            chkNotNull->Disable();
            txtLength->Disable();
            cbDatatype->Disable();
            txtAttstattarget->Disable();
            txtAttdistinct->Disable();
            txtDefault->Disable();
        }
    }
    else
    {
        // create mode
        FillDatatype(cbDatatype);
        cbDatatype->Append(wxT("serial"));
        cbDatatype->Append(wxT("bigserial"));
        AddType(wxT(" "), 0, wxT("serial"));
        AddType(wxT(" "), 0, wxT("bigserial"));

        if (!table)
        {
            cbClusterSet->Disable();
            cbClusterSet = 0;
        }

        txtAttstattarget->Disable();
        txtAttdistinct->Disable();
        txtComment->Disable();
    }
    return dlgTypeProperty::Go(modal);
}


wxString dlgColumn::GetSql()
{
    wxString sql;
    wxString name=GetName();

    bool isSerial = (cbDatatype->GetValue() == wxT("serial") || cbDatatype->GetValue() == wxT("bigserial"));

    if (table)
    {
        if (column)
        {
            if (name != column->GetName())
                sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                    +  wxT(" RENAME ") + qtIdent(column->GetName())
                    +  wxT("  TO ") + qtIdent(name)
                    +  wxT(";\n");

            wxString len;
            if (txtLength->IsEnabled())
                len = txtLength->GetValue();

            wxString prec;
            if (txtPrecision->IsEnabled())
                prec = txtPrecision->GetValue();

            if (connection->BackendMinimumVersion(7, 5))
            {
                if (cbDatatype->GetValue() != column->GetRawTypename() || 
                    (isVarLen && txtLength->IsEnabled() && StrToLong(len) != column->GetLength()) ||
                    (isVarPrec && txtPrecision->IsEnabled() && StrToLong(prec) != column->GetPrecision()))
                {
                    sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                        +  wxT(" ALTER ") + qtIdent(name) + wxT(" TYPE ")
                        +  GetQuotedTypename(cbDatatype->GetGuessedSelection())
                        +  wxT(";\n");
                }
            }
            else
            {
                wxString sqlPart;
                if (cbDatatype->GetCount() > 1 && cbDatatype->GetValue() != column->GetRawTypename())
                    sqlPart = wxT("atttypid=") + GetTypeOid(cbDatatype->GetGuessedSelection());


                if (!sqlPart.IsEmpty() || 
                    (isVarLen && txtLength->IsEnabled() && StrToLong(prec) != column->GetLength()) ||
                    (isVarPrec && txtPrecision->IsEnabled() && StrToLong(prec) != column->GetPrecision()))
                {
                    long typmod = pgDatatype::GetTypmod(column->GetRawTypename(), len, prec);

                    if (!sqlPart.IsEmpty())
                        sqlPart += wxT(", ");
                    sqlPart += wxT("atttypmod=") + NumToStr(typmod);
                }
                if (!sqlPart.IsEmpty())
                {
                    sql += wxT("UPDATE pg_attribute\n")
                           wxT("   SET ") + sqlPart + wxT("\n")
                           wxT(" WHERE attrelid=") + table->GetOidStr() +
                           wxT(" AND attnum=") + NumToStr(column->GetColNumber()) + wxT(";\n");
                }
            }

            if (txtDefault->GetValue() != column->GetDefault())
            {
                sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                    +  wxT("\n   ALTER COLUMN ") + qtIdent(name);
                if (txtDefault->GetValue().IsEmpty())
                    sql += wxT(" DROP DEFAULT");
                else
                    sql += wxT(" SET DEFAULT ") + txtDefault->GetValue();

                sql += wxT(";\n");
            }
            if (chkNotNull->GetValue() != column->GetNotNull())
            {
                sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                    +  wxT("\n   ALTER COLUMN ") + qtIdent(name);
                if (chkNotNull->GetValue())
                    sql += wxT(" SET");
                else
                    sql += wxT(" DROP");

                sql += wxT(" NOT NULL;\n");
            }
            if (txtAttstattarget->GetValue() != NumToStr(column->GetAttstattarget()))
            {
                sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                    +  wxT("\n   ALTER COLUMN ") + qtIdent(name);
                if (txtAttstattarget->GetValue().IsEmpty())
                    sql += wxT(" SET STATISTICS -1");
                else
                    sql += wxT(" SET STATISTICS ") + txtAttstattarget->GetValue();
                sql += wxT(";\n");
            }
            if (connection->BackendMinimumVersion(8, 5))
            {
                if (txtAttdistinct->GetValue() != NumToStr(column->GetAttdistinct()))
                {
                    sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                        +  wxT("\n   ALTER COLUMN ") + qtIdent(name);
                    if (txtAttdistinct->GetValue().IsEmpty())
                        sql += wxT(" SET STATISTICS DISTINCT 0");
                    else
                        sql += wxT(" SET STATISTICS DISTINCT ") + txtAttdistinct->GetValue();
                    sql += wxT(";\n");
                }
            }
        }
        else
        {
            sql = wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                + wxT("\n   ADD COLUMN ") + qtIdent(name)
                + wxT(" ") + GetQuotedTypename(cbDatatype->GetGuessedSelection());

            if (chkNotNull->GetValue())
                sql += wxT(" NOT NULL");
        
            if (!isSerial && !txtDefault->GetValue().IsEmpty())
                sql += wxT(" DEFAULT ") + txtDefault->GetValue(); 
            
            sql += wxT(";\n");

            if (!txtAttstattarget->GetValue().IsEmpty())
                sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                    + wxT("\n   ALTER COLUMN ") + qtIdent(name)
                    + wxT(" SET STATISTICS ") + txtAttstattarget->GetValue()
                    + wxT(";\n");

            if (connection->BackendMinimumVersion(8, 5))
            {
                if (!txtAttdistinct->GetValue().IsEmpty())
                    sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                        + wxT("\n   ALTER COLUMN ") + qtIdent(name)
                        + wxT(" SET DISTINCT ") + txtAttdistinct->GetValue()
                        + wxT(";\n");
            }
        }

        AppendComment(sql, wxT("COLUMN ") + table->GetQuotedFullIdentifier() 
                + wxT(".") + qtIdent(name), column);

        // securityPage will exists only for PG 8.4 and later
        if (connection->BackendMinimumVersion(8, 4))
            sql += securityPage->GetGrant(wxT("arwx"), table->GetQuotedFullIdentifier(), &currentAcl, qtIdent(name));
    }
    return sql;
}


wxString dlgColumn::GetDefinition()
{
    wxString sql, col;
    sql = GetQuotedTypename(cbDatatype->GetGuessedSelection());
    if (chkNotNull->GetValue())
        sql += wxT(" NOT NULL");

    AppendIfFilled(sql, wxT(" DEFAULT "), txtDefault->GetValue());

    return sql;
}


pgObject *dlgColumn::CreateObject(pgCollection *collection)
{
    pgObject *obj;
    obj=columnFactory.CreateObjects(collection, 0, 
        wxT("\n   AND attname=") + qtDbString(GetName()) +
        wxT("\n   AND cl.relname=") + qtDbString(table->GetName()) +
        wxT("\n   AND cl.relnamespace=") + table->GetSchema()->GetOidStr() +
        wxT("\n"));
    return obj;
}


void dlgColumn::OnSelChangeTyp(wxCommandEvent &ev)
{
    cbDatatype->GuessSelection(ev);

    CheckLenEnable();
    txtLength->Enable(isVarLen);

    bool isSerial = (cbDatatype->GetValue() == wxT("serial") || cbDatatype->GetValue() == wxT("bigserial"));
    txtDefault->Enable(!isSerial);

    CheckChange();
}


void dlgColumn::CheckChange()
{
    long varlen=StrToLong(txtLength->GetValue()), 
         varprec=StrToLong(txtPrecision->GetValue());

    if (column)
    {
        txtPrecision->Enable(column->GetTable()->GetMetaType() != PGM_VIEW && isVarPrec && varlen > 0);

        bool enable=true;
        EnableOK(enable);   // to get rid of old messages

        CheckValid(enable, cbDatatype->GetGuessedSelection() >= 0, _("Please select a datatype."));
        if (!connection->BackendMinimumVersion(7, 5))
        {
            CheckValid(enable, !isVarLen || !txtLength->GetValue().IsEmpty() || varlen >= column->GetLength(), 
                    _("New length must not be less than old length."));

            CheckValid(enable, !txtPrecision->IsEnabled() || varprec >= column->GetPrecision(), 
                    _("New precision must not be less than old precision."));
            CheckValid(enable, !txtPrecision->IsEnabled() || varlen-varprec >= column->GetLength()-column->GetPrecision(), 
                    _("New total digits must not be less than old total digits."));
        }

        
        if (enable)
            enable = GetName() != column->GetName()
                    || txtDefault->GetValue() != column->GetDefault()
                    || txtComment->GetValue() != column->GetComment()
                    || chkNotNull->GetValue() != column->GetNotNull()
                    || (cbDatatype->GetCount() > 1 && cbDatatype->GetGuessedStringSelection() != column->GetRawTypename())
                    || (isVarLen && varlen != column->GetLength())
                    || (isVarPrec && varprec != column->GetPrecision())
                    || txtAttstattarget->GetValue() != NumToStr(column->GetAttstattarget())
                    || txtAttdistinct->GetValue() != NumToStr(column->GetAttdistinct());

        EnableOK(enable | securityChanged);
    }
    else
    {
        txtPrecision->Enable(isVarPrec && varlen > 0);

        wxString name=GetName();

        bool enable=true;
        CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
        CheckValid(enable, cbDatatype->GetGuessedSelection() >= 0, _("Please select a datatype."));
        CheckValid(enable, !isVarLen || txtLength->GetValue().IsEmpty() 
            || (varlen >= minVarLen && varlen <= maxVarLen && NumToStr(varlen) == txtLength->GetValue()),
            _("Please specify valid length."));
        CheckValid(enable, !txtPrecision->IsEnabled() 
            || (varprec >= 0 && varprec <= varlen && NumToStr(varprec) == txtPrecision->GetValue()),
            _("Please specify valid numeric precision (0..") + NumToStr(varlen) + wxT(")."));

        EnableOK(enable);
    }
}


void dlgColumn::OnAddPriv(wxCommandEvent &ev)
{
    securityChanged=true;
    CheckChange();
}


void dlgColumn::OnDelPriv(wxCommandEvent &ev)
{
    securityChanged=true;
    CheckChange();
}

