//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgRepSet.cpp - PostgreSQL Slony-I Set Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"

#include "dlgRepSet.h"
#include "slCluster.h"
#include "slSet.h"

// Images
#include "images/slset.xpm"


// pointer to controls
#define txtOrigin           CTRL_TEXT("txtOrigin")
#define txtID               CTRL_TEXT("txtID")



BEGIN_EVENT_TABLE(dlgRepSet, dlgProperty)
END_EVENT_TABLE();


dlgRepSet::dlgRepSet(frmMain *frame, slSet *s, slCluster *c)
: dlgRepProperty(frame, c, wxT("dlgRepSet"))
{
    SetIcon(wxIcon(slset_xpm));
    set=s;
}


pgObject *dlgRepSet::GetObject()
{
    return set;
}


int dlgRepSet::Go(bool modal)
{
    txtID->SetValidator(numericValidator);


    if (set)
    {
        // edit mode
        txtID->SetValue(NumToStr(set->GetSlId()));
        txtID->Disable();
        txtOrigin->SetValue(IdAndName(set->GetOriginId(), set->GetOriginNode()));
    }
    else
    {
        // create mode
        txtOrigin->SetValue(IdAndName(cluster->GetLocalNodeID(), cluster->GetLocalNodeName()));
        EnableOK(true);
    }

    txtOrigin->Disable();
        
    return dlgProperty::Go(modal);
}


pgObject *dlgRepSet::CreateObject(pgCollection *collection)
{
    wxString restriction;
    if (StrToLong(txtID->GetValue()) > 0)
        restriction = txtID->GetValue();
    else
        restriction = wxT("(SELECT MAX(set_id) FROM ") + cluster->GetSchemaPrefix() + wxT("sl_set)");

    pgObject *obj=slSet::ReadObjects((slCollection*)collection, 0,
         wxT(" WHERE set_id = ") + restriction);

    return obj;
}


void dlgRepSet::CheckChange()
{
    if (set)
    {
        EnableOK(txtComment->GetValue() != set->GetComment());
    }
    else
    {
        bool enable=true;

        EnableOK(enable);
    }
}



wxString dlgRepSet::GetSql()
{
    wxString sql;

    sql = wxT("-- Create replication set\n\n")
          wxT("SELECT ") + cluster->GetSchemaPrefix() + wxT("storeset(");

    if (StrToLong(txtID->GetValue()) > 0)
        sql += txtID->GetValue();
    else
        sql += wxT("(SELECT COALESCE(MAX(set_id), 0) + 1 FROM ") 
            +  cluster->GetSchemaPrefix() + wxT("sl_set)");

    sql += wxT(", ") + qtString(txtComment->GetValue())
        +  wxT(");\n");

    return sql;
}
