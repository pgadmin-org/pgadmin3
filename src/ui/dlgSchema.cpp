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
#define cbTablespace    CTRL_COMBOBOX("cbTablespace")

BEGIN_EVENT_TABLE(dlgSchema, dlgSecurityProperty)
    EVT_TEXT(XRCID("cbTablespace"),                 dlgProperty::OnChange)
    EVT_COMBOBOX(XRCID("cbTablespace"),             dlgProperty::OnChange)
END_EVENT_TABLE();


dlgSchema::dlgSchema(frmMain *frame, pgSchema *node)
: dlgSecurityProperty(frame, node, wxT("dlgSchema"), wxT("USAGE,CREATE"), "UC")
{
    SetIcon(wxIcon(namespace_xpm));
    schema=node;
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
        PrepareTablespace(cbTablespace, schema->GetTablespace());

        if (!connection->BackendMinimumVersion(7, 5))
            cbOwner->Disable();
        if (!connection->BackendMinimumVersion(7, 6))
            cbTablespace->Disable();
    }
    else
    {
        // create mode
        PrepareTablespace(cbTablespace);
    }

    return dlgSecurityProperty::Go(modal);
}


pgObject *dlgSchema::CreateObject(pgCollection *collection)
{
    wxString name=GetName();

    pgObject *obj=pgSchema::ReadObjects(collection, 0, wxT(" WHERE nspname=") + qtString(name) + wxT("\n"));
    return obj;
}


void dlgSchema::CheckChange()
{
    wxString name=GetName();
    if (schema)
    {
        EnableOK(name != schema->GetName() || txtComment->GetValue() != schema->GetComment()
            || cbOwner->GetValue() != schema->GetOwner());
    }
    else
    {

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
        AppendNameChange(sql);
        AppendOwnerChange(sql);
    }
    else
    {
        // create mode
        sql = wxT("CREATE SCHEMA ") + qtIdent(name);
        AppendIfFilled(sql, wxT("\n       AUTHORIZATION "), qtIdent(cbOwner->GetValue()));
        AppendIfFilled(sql, wxT("\n       TABLESPACE "), qtIdent(cbTablespace->GetValue()));
        sql += wxT(";\n");

    }
    AppendComment(sql, wxT("SCHEMA"), 0, schema);

    sql += GetGrant(wxT("UC"), wxT("SCHEMA ") + qtIdent(name));

    return sql;
}


