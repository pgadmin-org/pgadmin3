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


bool slFunctions::MergeSet(wxFrame *frame, pgObject *obj)
{
    return true;
}


bool slFunctions::MoveSet(wxFrame *frame, pgObject *obj)
{
    return true;
}


bool slFunctions::Failover(wxFrame *frame, pgObject *obj)
{
    return true;
}


bool slFunctions::UpgradeNode(wxFrame *frame, pgObject *obj)
{
    return true;
}


bool slFunctions::RestartNode(wxFrame *frame, pgObject *obj)
{
    slCluster *cluster=(slCluster*)obj;

    wxMessageDialog dlg(frame, wxString::Format(_("Restart node %s?"), 
        cluster->GetLocalNodeName()), _("Restart node"), wxICON_EXCLAMATION|wxYES_NO|wxNO_DEFAULT);

    if (dlg.ShowModal() != wxID_YES)
        return true;

    return cluster->GetDatabase()->ExecuteVoid(
        wxT("NOTIFY ") + cluster->GetSchemaPrefix() + wxT("_Restart"));
}
