//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgRepListen.cpp - PostgreSQL Slony-I Listen Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"

#include "dlgRepListen.h"
#include "slCluster.h"
#include "slNode.h"
#include "slListen.h"
#include "pgDatatype.h"

// pointer to controls

#define cbOrigin        CTRL_COMBOBOX("cbOrigin")
#define txtReceiver     CTRL_TEXT("txtReceiver")
#define cbProvider      CTRL_COMBOBOX("cbProvider")




BEGIN_EVENT_TABLE(dlgRepListen, dlgProperty)
    EVT_COMBOBOX(XRCID("cbOrigin"),       dlgRepListen::OnChange)
    EVT_COMBOBOX(XRCID("cbProvider"),     dlgRepListen::OnChange)
END_EVENT_TABLE();


dlgProperty *slListenFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
    return new dlgRepListen(frame, (slListen*)node, (slNode*)parent);
}

dlgRepListen::dlgRepListen(frmMain *frame, slListen *l, slNode *n)
: dlgRepProperty(frame, n->GetCluster(), wxT("dlgRepListen"))
{
    listen=l;
    node=n;
}


pgObject *dlgRepListen::GetObject()
{
    return listen;
}


int dlgRepListen::Go(bool modal)
{
    txtReceiver->SetValue(IdAndName(node->GetSlId(), node->GetName()));
    txtReceiver->Disable();

    if (listen)
    {
        // edit mode
        cbOrigin->Append(IdAndName(listen->GetOriginId(), listen->GetOriginName()));
        cbOrigin->SetSelection(0);
        cbProvider->Append(IdAndName(listen->GetSlId(), listen->GetName()));
        cbProvider->SetSelection(0);
        cbOrigin->Disable();
        cbProvider->Disable();
    }
    else
    {
        // create mode

        pgSet *nodes=connection->ExecuteSet(
            wxT("SELECT no_id, no_comment, pa_server\n")
            wxT("  FROM ") + cluster->GetSchemaPrefix() + wxT("sl_node\n")
            wxT("  LEFT JOIN ") + cluster->GetSchemaPrefix() + wxT("sl_path ON pa_server=no_id")
                                    wxT(" AND pa_client=") + NumToStr(node->GetSlId()) + wxT("\n")
            wxT(" WHERE no_id <> ") + NumToStr(node->GetSlId()) + wxT("\n")
            wxT(" ORDER BY no_id")
            );

        if (nodes)
        {
            while (!nodes->Eof())
            {
                long id=nodes->GetLong(wxT("no_id"));
                wxString name=IdAndName(id, nodes->GetVal(wxT("no_comment")));
                
                cbOrigin->Append(name, (void*)id);
                
                if (nodes->GetLong(wxT("pa_server")) > 0)
                    cbProvider->Append(name, (void*)id);

                nodes->MoveNext();
            }
            delete nodes;
        }
    }

    return dlgProperty::Go(modal);
}


pgObject *dlgRepListen::CreateObject(pgCollection *collection)
{
    pgObject *obj=listenFactory.CreateObjects(collection, 0,
         wxT(" WHERE li_origin = ") + NumToStr((OID)cbOrigin->GetClientData(cbOrigin->GetSelection())) +
         wxT("   AND li_receiver = ") + NumToStr(node->GetSlId()) +
         wxT("   AND li_provider = ") + NumToStr((OID)cbProvider->GetClientData(cbProvider->GetSelection()))
         );

    return obj;
}




void dlgRepListen::CheckChange()
{
    if (listen)
    {
    }
    else
    {
        bool enable=true;
        CheckValid(enable, cbProvider->GetCount() > 0, _("No path to any provider node; add proper path."));
        CheckValid(enable, cbOrigin->GetSelection() >= 0, _("Please select origin node of replication events."));
        CheckValid(enable, cbProvider->GetSelection() >= 0, _("Please select provider node for replication events."));

        EnableOK(enable);
    }
}



wxString dlgRepListen::GetSql()
{
    wxString sql;

    if (listen)
    {
        // edit mode
    }
    else
    {
        // create mode

        sql = wxT("SELECT ") + cluster->GetSchemaPrefix() + wxT("storelisten(")
                + NumToStr((OID)cbOrigin->GetClientData(cbOrigin->GetSelection())) + wxT(", ")
                + NumToStr((OID)cbProvider->GetClientData(cbProvider->GetSelection())) + wxT(", ")
                + NumToStr(node->GetSlId()) + wxT(");");
    }

    return sql;
}
