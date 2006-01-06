//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
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


// pointer to controls

BEGIN_EVENT_TABLE(dlgSchema, dlgSecurityProperty)
END_EVENT_TABLE();

dlgProperty *pgSchemaFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
    return new dlgSchema(this, frame, (pgSchema*)node);
}

dlgSchema::dlgSchema(pgaFactory *f, frmMain *frame, pgSchema *node)
: dlgSecurityProperty(f, frame, node, wxT("dlgSchema"), wxT("USAGE,CREATE"), "UC")
{
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

        if (!connection->BackendMinimumVersion(7, 5))
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

    pgObject *obj=schemaFactory.CreateObjects(collection, 0, wxT(" WHERE nspname=") + qtString(name) + wxT("\n"));
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
        AppendOwnerChange(sql, wxT("SCHEMA ") + qtIdent(name));
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


