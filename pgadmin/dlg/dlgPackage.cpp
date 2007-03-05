//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: dlgPackage.cpp 5828 2007-01-04 16:41:08Z dpage $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgPackage.cpp - EnterpriseDB package Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"

#include "dlg/dlgPackage.h"
#include "schema/edbPackage.h"

// pointer to controls
#define txtName             CTRL_TEXT("txtName")
#define txtComment          CTRL_TEXT("txtComment")
#define txtHeader           CTRL_SQLBOX("txtHeader")
#define txtBody             CTRL_SQLBOX("txtBody")

dlgProperty *edbPackageFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
    return new dlgPackage(this, frame, (edbPackage*)node, (pgSchema *)parent);
}


BEGIN_EVENT_TABLE(dlgPackage, dlgSecurityProperty)
    EVT_STC_MODIFIED(XRCID("txtHeader"),            dlgProperty::OnChangeStc)
    EVT_STC_MODIFIED(XRCID("txtBody"),              dlgProperty::OnChangeStc)
END_EVENT_TABLE();


dlgPackage::dlgPackage(pgaFactory *f, frmMain *frame, edbPackage *node, pgSchema *sch)
: dlgSecurityProperty(f, frame, node, wxT("dlgPackage"), wxT("EXECUTE"), "X")
{
    schema=sch;
    package=node;

    txtHeader->SetMarginType(1, wxSTC_MARGIN_NUMBER);
    txtHeader->SetMarginWidth(1, ConvertDialogToPixels(wxPoint(16, 0)).x);

    txtBody->SetMarginType(1, wxSTC_MARGIN_NUMBER);
    txtBody->SetMarginWidth(1, ConvertDialogToPixels(wxPoint(16, 0)).x);
}



pgObject *dlgPackage::GetObject()
{
    return package;
}


int dlgPackage::Go(bool modal)
{
    txtComment->Disable();
    cbOwner->Disable();

    AddGroups();
    AddUsers(cbOwner);

    if (package)
    {
        // edit mode
        txtName->Disable();

        txtHeader->SetText(package->GetHeaderInner());
        txtBody->SetText(package->GetBodyInner());
    }
    else
    {
        // create mode

    }

    return dlgSecurityProperty::Go(modal);
}


pgObject *dlgPackage::CreateObject(pgCollection *collection)
{
    pgObject *obj=packageFactory.CreateObjects(collection, 0,
         wxT(" WHERE pkgname = '") + GetName() + wxT("'")
         wxT("\n   AND pkgnamespace=") + schema->GetOidStr());

    return obj;
}


void dlgPackage::CheckChange()
{
    bool enable=true;

    if (package)
    {
       if (!(txtBody->GetText() != package->GetBodyInner() ||
           txtHeader->GetText() != package->GetHeaderInner())) 
           enable = false;
    }
    else
    {
        CheckValid(enable, !txtName->GetValue().IsEmpty(), _("Please specify name."));
        CheckValid(enable, !txtHeader->GetText().IsEmpty(), _("Please specify package header."));
        CheckValid(enable, !txtBody->GetText().IsEmpty(), _("Please specify package body."));
    }
    EnableOK(enable);
}


wxString dlgPackage::GetSql()
{
    wxString sql;
    wxString qtName = schema->GetQuotedSchemaPrefix(schema->GetName()) + qtIdent(txtName->GetValue());

    if (!package || (package && txtHeader->GetText() != package->GetHeaderInner()))
    {
        if (package)
            sql = wxT("DROP PACKAGE BODY ") + qtName + wxT(";\n\n");

        sql += wxT("CREATE OR REPLACE PACKAGE ") + qtName + wxT("\nAS\n");
        sql += txtHeader->GetText();
        sql += wxT("\nEND ") + qtIdent(txtName->GetValue()) + wxT(";\n\n");
    }

    if (!package || (package && txtBody->GetText() != package->GetBodyInner())
                 || (package && txtHeader->GetText() != package->GetHeaderInner()))
    {
        sql += wxT("CREATE OR REPLACE PACKAGE BODY ") + qtName + wxT("\nAS\n");
        sql += txtBody->GetText();
        sql += wxT("\nEND ") + qtIdent(txtName->GetValue()) + wxT(";\n\n");
    }

    sql += GetGrant(wxT("X"), wxT("PACKAGE ") + qtName);

    return sql;
}
