// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgRepNode.cpp - PostgreSQL Slony-I Node Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"


#include "dlgRepNode.h"
#include "slCluster.h"
#include "slNode.h"

// Images
#include "images/slnode.xpm"


// pointer to controls
#define txtID               CTRL_TEXT("txtID")



BEGIN_EVENT_TABLE(dlgRepNode, dlgProperty)
END_EVENT_TABLE();


dlgRepNode::dlgRepNode(frmMain *frame, slNode *s, slCluster *c)
: dlgRepProperty(frame, c, wxT("dlgRepNode"))
{
    SetIcon(wxIcon(slnode_xpm));
    node=s;
}


pgObject *dlgRepNode::GetObject()
{
    return node;
}


int dlgRepNode::Go(bool modal)
{
    txtID->SetValidator(numericValidator);

    if (node)
    {
        // edit mode
        txtID->SetValue(NumToStr(node->GetSlId()));
        txtID->Disable();
    }
    else
    {
        // create mode
        EnableOK(true);
    }

    return dlgProperty::Go(modal);
}


pgObject *dlgRepNode::CreateObject(pgCollection *collection)
{
    wxString restriction;
    if (StrToLong(txtID->GetValue()) > 0)
        restriction = txtID->GetValue();
    else
        restriction = wxT("(SELECT MAX(no_id) FROM ") + cluster->GetSchemaPrefix() + wxT("sl_node)");

    pgObject *obj=slNode::ReadObjects((slCollection*)collection, 0,
         wxT(" WHERE no_id = ") + restriction);

    return obj;
}


void dlgRepNode::CheckChange()
{
    if (node)
    {
        EnableOK(txtComment->GetValue() != node->GetComment());
    }
    else
    {
        bool enable=true;

        EnableOK(enable);
    }
}



wxString dlgRepNode::GetSql()
{
    wxString sql;

    sql = wxT("-- Create replication node\n\n")
          wxT("SELECT ") + cluster->GetSchemaPrefix() + wxT("storenode(");

    if (StrToLong(txtID->GetValue()) > 0)
        sql += txtID->GetValue();
    else
        sql += wxT("(SELECT COALESCE(MAX(no_id), 0) + 1 FROM ") 
            +  cluster->GetSchemaPrefix() + wxT("sl_node)");

    sql += wxT(", ") + qtString(txtComment->GetValue())
        +  wxT(");\n");

    return sql;
}
