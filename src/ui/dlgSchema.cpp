//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgSchema.cpp - PostgreSQL Schema Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "dlgSchema.h"
#include "pgSchema.h"

// Images
#include "images/namespace.xpm"


// pointer to controls
#define cbOwner         CTRL_COMBOBOX("cbOwner")


BEGIN_EVENT_TABLE(dlgSchema, dlgSecurityProperty)
    EVT_TEXT(XRCID("txtName"),                      dlgSchema::OnChange)
    EVT_TEXT(XRCID("txtComment"),                   dlgSchema::OnChange)
END_EVENT_TABLE();


dlgSchema::dlgSchema(frmMain *frame, pgSchema *node)
: dlgSecurityProperty(frame, node, wxT("dlgSchema"), wxT("USAGE,CREATE"), "UC")
{
    SetIcon(wxIcon(namespace_xpm));
    schema=node;

    txtOID->Disable();
}


pgObject *dlgSchema::GetObject()
{
    return schema;
}


int dlgSchema::Go(bool modal)
{
    if (!schema)
        cbOwner->Append(wxT(""));

    AddGroups();
    AddUsers(cbOwner);
    if (schema)
    {
        // edit mode
        txtName->SetValue(schema->GetName());
        txtOID->SetValue(NumToStr((long)schema->GetOid()));
        txtComment->SetValue(schema->GetComment());
        cbOwner->SetValue(schema->GetOwner());

        txtName->Disable();
        cbOwner->Disable();
    }
    else
    {
        // create mode
    }

    return dlgSecurityProperty::Go(modal);
}


pgObject *dlgSchema::CreateObject(pgCollection *collection)
{
    wxString name=GetName();

    pgObject *obj=pgSchema::ReadObjects(collection, 0, wxT(" WHERE nspname=") + qtString(name) + wxT("\n"));
    return obj;
}


void dlgSchema::OnChange(wxCommandEvent &ev)
{
    if (schema)
    {
        EnableOK(txtComment->GetValue() != schema->GetComment());
    }
    else
    {
        wxString name=GetName();

        bool enable=true;
        CheckValid(enable, !name.IsEmpty(), _("Please specify name."));

        EnableOK(enable);
    }
}



wxString dlgSchema::GetSql()
{
    wxString sql, name;
    name=GetName();

    if (schema)
    {
        // edit mode
    }
    else
    {
        // create mode
        sql = wxT("CREATE SCHEMA ") + qtIdent(name);
        AppendIfFilled(sql, wxT("\n       AUTHORIZATION "), qtIdent(cbOwner->GetValue()));
        sql += wxT(";\n");

    }
    AppendComment(sql, wxT("SCHEMA"), 0, schema);

    sql += GetGrant(wxT("UC"), wxT("SCHEMA ") + qtIdent(name));

    return sql;
}


