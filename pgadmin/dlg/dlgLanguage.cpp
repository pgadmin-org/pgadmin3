//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgLanguage.cpp - PostgreSQL Language Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "utils/pgDefs.h"

#include "dlg/dlgLanguage.h"
#include "schema/pgLanguage.h"


// pointer to controls
#define cbName          CTRL_COMBOBOX("cbName")
#define chkTrusted      CTRL_CHECKBOX("chkTrusted")
#define cbHandler       CTRL_COMBOBOX("cbHandler")
#define cbValidator     CTRL_COMBOBOX("cbValidator")


dlgProperty *pgLanguageFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
    return new dlgLanguage(this, frame, (pgLanguage*)node);
}


BEGIN_EVENT_TABLE(dlgLanguage, dlgSecurityProperty)
    EVT_TEXT(XRCID("cbName"),                       dlgLanguage::OnChangeName)
    EVT_COMBOBOX(XRCID("cbName"),                   dlgLanguage::OnChangeName)
    EVT_TEXT(XRCID("cbHandler"),                    dlgProperty::OnChange)
    EVT_COMBOBOX(XRCID("cbHandler"),                dlgProperty::OnChange)
END_EVENT_TABLE();


dlgLanguage::dlgLanguage(pgaFactory *f, frmMain *frame, pgLanguage *node)
: dlgSecurityProperty(f, frame, node, wxT("dlgLanguage"), wxT("USAGE"), "U")
{
    language=node;
}


pgObject *dlgLanguage::GetObject()
{
    return language;
}


int dlgLanguage::Go(bool modal)
{
    if (!connection->BackendMinimumVersion(7, 5))
        txtComment->Disable();

    AddGroups();
    AddUsers();
    if (language)
    {
        // edit mode
        chkTrusted->SetValue(language->GetTrusted());
        cbHandler->Append(language->GetHandlerProc());
        cbHandler->SetSelection(0);
        wxString val=language->GetValidatorProc();
        if (!val.IsEmpty())
        {
            cbValidator->Append(val);
            cbValidator->SetSelection(0);
        }

        cbName->SetValue(language->GetName());
        if (!connection->BackendMinimumVersion(7, 4))
            cbName->Disable();
        cbHandler->Disable();
        chkTrusted->Disable();
        cbValidator->Disable();
    }
    else
    {
        // create mode
        if (connection->BackendMinimumVersion(8, 1))
        {
            pgSetIterator languages(connection, 
                wxT("SELECT tmplname FROM pg_pltemplate\n")
                wxT("  LEFT JOIN pg_language ON tmplname=lanname\n")
                wxT(" WHERE lanname IS NULL\n")
                wxT(" ORDER BY tmplname"));

            while (languages.RowsLeft())
                cbName->Append(languages.GetVal(wxT("tmplname")));
        }
        else
        {
            // to clear drop down list
            cbName->Append(wxT(" "));
            cbName->Delete(0);
        }
        cbValidator->Append(wxT(""));
        pgSet *set=connection->ExecuteSet(
            wxT("SELECT nspname, proname, prorettype\n")
            wxT("  FROM pg_proc p\n")
            wxT("  JOIN pg_namespace nsp ON nsp.oid=pronamespace\n")
            wxT(" WHERE prorettype=2280 OR (prorettype=") + NumToStr(PGOID_TYPE_VOID) +
            wxT(" AND proargtypes[0]=") + NumToStr(PGOID_TYPE_LANGUAGE_HANDLER) + wxT(")"));
        if (set)
        {
            while (!set->Eof())
            {
                wxString procname = database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("proname"));

                if (set->GetOid(wxT("prorettype")) == 2280)
                    cbHandler->Append(procname);
                else
                    cbValidator->Append(procname);
                set->MoveNext();
            }
            delete set;
        }
    }

    return dlgSecurityProperty::Go(modal);
}


pgObject *dlgLanguage::CreateObject(pgCollection *collection)
{
    wxString name=cbName->wxComboBox::GetValue();

    pgObject *obj=languageFactory.CreateObjects(collection, 0, wxT("\n   AND lanname ILIKE ") + qtDbString(name));
    return obj;
}


void dlgLanguage::OnChangeName(wxCommandEvent &ev)
{
    if (connection->BackendMinimumVersion(8, 1))
    {
        bool useTemplate = (cbName->FindString(cbName->wxComboBox::GetValue()) >= 0);
        chkTrusted->Enable(!useTemplate);
        cbHandler->Enable(!useTemplate);
        cbValidator->Enable(!useTemplate);
    }
    OnChange(ev);
}


void dlgLanguage::CheckChange()
{
    wxString name=cbName->wxComboBox::GetValue();
    if (language)
    {
        EnableOK(name != language->GetName() || txtComment->GetValue() != language->GetComment());
    }
    else
    {

        bool enable=true;
        bool useTemplate = (cbName->FindString(name) >= 0);

        CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
        CheckValid(enable, useTemplate || !cbHandler->GetValue().IsEmpty(), _("Please specify language handler."));
        EnableOK(enable);
    }
}



wxString dlgLanguage::GetSql()
{
    wxString sql, name;
    name=cbName->wxComboBox::GetValue();

    if (language)
    {
        // edit mode
        if (name != language->GetName())
            sql += wxT("ALTER LANGUAGE ") + qtIdent(language->GetName()) 
                +  wxT(" RENAME TO ") + qtIdent(name) + wxT(";\n");
    }
    else
    {
        // create mode
        if (connection->BackendMinimumVersion(8, 1) && cbName->FindString(name) >= 0)
        {
            sql = wxT("CREATE LANGUAGE ") + qtIdent(name) + wxT(";\n");
        }
        else
        {
            sql = wxT("CREATE ");
            if (chkTrusted->GetValue())
                sql += wxT("TRUSTED ");
            sql += wxT("LANGUAGE ") + qtIdent(name) + wxT("\n   HANDLER ") + qtIdent(cbHandler->GetValue());
            AppendIfFilled(sql, wxT("\n   VALIDATOR "), qtIdent(cbValidator->GetValue()));
            sql += wxT(";\n");
        }
    }

    sql += GetGrant(wxT("X"), wxT("LANGUAGE ") + qtIdent(name));
    AppendComment(sql, wxT("LANGUAGE ") + qtIdent(name), 0, language);

    return sql;
}


