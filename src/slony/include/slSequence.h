//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// slSequence.h PostgreSQL Slony-I Node
//
//////////////////////////////////////////////////////////////////////////

#ifndef SLSEQUENCE_H
#define SLSEQUENCE_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgObject.h"
#include "pgServer.h"
#include "pgDatabase.h"
#include "slObject.h"


class slSequence : public slSetObject
{
public:
    slSequence(slSet *set, const wxString& newName = wxT(""));
    ~slSequence();

    int GetIconId() { return -1; }
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    static pgObject *ReadObjects(slSetCollection *coll, wxTreeCtrl *browser, const wxString &restriction);
    static pgObject *ReadObjects(slSetCollection *coll, wxTreeCtrl *browser);

    bool GetActive() const { return active; }
    void iSetActive(bool b) { active=b; }

    bool DropObject(wxFrame *frame, wxTreeCtrl *browser, bool cascaded);
    wxString GetSql(wxTreeCtrl *browser);
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);

private:
    bool active;
};

#endif

