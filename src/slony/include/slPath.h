//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// slPath.h PostgreSQL Slony-I Node
//
//////////////////////////////////////////////////////////////////////////

#ifndef SLPATH_H
#define SLPATH_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgObject.h"
#include "pgServer.h"
#include "pgDatabase.h"
#include "slObject.h"


class slPath : public slNodeObject
{
public:
    slPath(slNode *n, const wxString& newName = wxT(""));
    ~slPath();

    int GetIconId() { return SLICON_PATH; }
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    static pgObject *ReadObjects(slNodeCollection *coll, wxTreeCtrl *browser, const wxString &restriction);
    static pgObject *ReadObjects(slNodeCollection *coll, wxTreeCtrl *browser);

    void iSetConnInfo(const wxString &s) { connInfo = s;}
    wxString GetConnInfo() const { return connInfo; }
    void iSetConnRetry(long l) { connRetry = l; }
    long GetConnRetry() { return connRetry; }

    bool DropObject(wxFrame *frame, wxTreeCtrl *browser, bool cascaded);
    wxString GetSql(wxTreeCtrl *browser);
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);

private:
    long connRetry;
    wxString connInfo;
};

#endif

