//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgaAgent.h - PostgreSQL Server Agent
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGAAGENT_H
#define PGAAGENT_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgConn.h"
#include "pgObject.h"
// Class declarations


class pgaAgent : public pgDatabaseObject
{
public:
    pgaAgent(const wxString& newName = wxT(""));
    ~pgaAgent();

    int GetIcon() { return PGAICON_AGENT; }
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0);
    static pgObject *ReadObjects(pgDatabase *database, wxTreeCtrl *browser);
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);

    bool CanCreate() { return false; }
    bool CanView() { return false; }
    bool CanEdit() { return false; }
    bool CanDrop() { return false; }
    bool WantDummyChild() { return true; }

private:
};

#endif
