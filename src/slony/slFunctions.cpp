//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// slFunctions.cpp PostgreSQL Slony-I Functions
//
//////////////////////////////////////////////////////////////////////////


// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "slCluster.h"
#include "slSet.h"
#include "slFunctions.h"
#include "dlgRepCluster.h"
#include "dlgRepSet.h"



bool slFunctions::Show(dlgProperty *dlg, frmMain *frame, pgObject *obj)
{
    dlg->InitDialog(frame, obj);
    dlg->CreateAdditionalPages();
    dlg->Go(false);
    dlg->CheckChange();
    return true;
}


bool slFunctions::MergeSet(frmMain *frame, pgObject *obj)
{
    return Show(new dlgRepSetMerge(frame, (slSet*)obj), frame, obj);
}


bool slFunctions::MoveSet(frmMain *frame, pgObject *obj)
{
    return Show(new dlgRepSetMove(frame, (slSet*)obj), frame, obj);
}


bool slFunctions::Failover(frmMain *frame, pgObject *obj)
{
    return true;
}


bool slFunctions::UpgradeNode(frmMain *frame, pgObject *obj)
{
    return Show(new dlgRepClusterUpgrade(frame, (slCluster*)obj), frame, obj);
}


bool slFunctions::RestartNode(wxFrame *frame, pgObject *obj)
{
    slCluster *cluster=(slCluster*)obj;

    wxString notifyName=cluster->GetDatabase()->ExecuteScalar(
        wxT("SELECT relname FROM pg_listener")
        wxT(" WHERE relname=") + qtString(wxT("_") + cluster->GetName() + wxT("_Restart")));

    if (notifyName.IsEmpty())
    {
        wxMessageDialog dlg(frame, wxString::Format(_("Node \"%s\" not running"), cluster->GetLocalNodeName().c_str()),
              _("Can't restart node"), wxICON_EXCLAMATION|wxOK);
        dlg.ShowModal();
        return true;
    }

    wxMessageDialog dlg(frame, wxString::Format(_("Restart node \"%s\"?"), 
        cluster->GetLocalNodeName().c_str()), _("Restart node"), wxICON_EXCLAMATION|wxYES_NO|wxNO_DEFAULT);

    if (dlg.ShowModal() != wxID_YES)
        return true;

    return cluster->GetDatabase()->ExecuteVoid(
        wxT("NOTIFY ") + qtIdent(notifyName));
}
