//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgTablespace.cpp - Tablespace property 
//
//////////////////////////////////////////////////////////////////////////



#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>


// App headers
#include "utils/misc.h"
#include "dlg/dlgTablespace.h"
#include "schema/pgTablespace.h"


// pointer to controls
#define txtLocation     CTRL_TEXT("txtLocation")

dlgProperty *pgTablespaceFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
    return new dlgTablespace(this, frame, (pgTablespace*)node);
}


BEGIN_EVENT_TABLE(dlgTablespace, dlgSecurityProperty)
    EVT_TEXT(XRCID("txtLocation"), dlgProperty::OnChange)
END_EVENT_TABLE();



dlgTablespace::dlgTablespace(pgaFactory *f, frmMain *frame, pgTablespace *node)
: dlgSecurityProperty(f, frame, node, wxT("dlgTablespace"), wxT("CREATE"), "C")
{
    tablespace=node;
    btnOK->Disable();
}


pgObject *dlgTablespace::GetObject()
{
    return tablespace;
}


int dlgTablespace::Go(bool modal)
{
    if (!tablespace)
        cbOwner->Append(wxEmptyString);
    AddGroups();
    AddUsers(cbOwner);
    txtComment->Disable();

    if (tablespace)
    {
        // Edit Mode
        txtName->SetValue(tablespace->GetIdentifier());
        txtLocation->SetValue(tablespace->GetLocation());

        txtLocation->Disable();
    }
    else
    {
    }

    return dlgSecurityProperty::Go(modal);
}


void dlgTablespace::CheckChange()
{
    if (tablespace)
    {
        EnableOK(txtComment->GetValue() != tablespace->GetComment()
            || GetName() != tablespace->GetName()
            || cbOwner->GetValue() != tablespace->GetOwner());
    }
    else
    {
        wxString name=GetName();

        bool enable=true;
        CheckValid(enable, !GetName().IsEmpty(), _("Please specify name."));
        CheckValid(enable, !txtLocation->GetValue().IsEmpty(), _("Please specify location."));
        EnableOK(enable);
    }
}


pgObject *dlgTablespace::CreateObject(pgCollection *collection)
{
    wxString name=GetName();

    pgObject *obj=tablespaceFactory.CreateObjects(collection, 0, wxT("\n WHERE spcname=") + qtDbString(name));
    return obj;
}


wxString dlgTablespace::GetSql()
{
    wxString sql;
    wxString name=GetName();
    

    if (tablespace)
    {
        // Edit Mode

        AppendNameChange(sql);
        AppendOwnerChange(sql, wxT("TABLESPACE ") + qtIdent(name));
    }
    else
    {
        // Create Mode
        sql = wxT("CREATE TABLESPACE ") + qtIdent(name);
        AppendIfFilled(sql, wxT(" OWNER "), qtIdent(cbOwner->GetValue()));
        sql += wxT(" LOCATION ") + qtDbString(txtLocation->GetValue())
            +  wxT(";\n");
    }
    sql += GetGrant(wxT("C"), wxT("TABLESPACE ") + qtIdent(name));
    return sql;
}

