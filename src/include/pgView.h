//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgView.h PostgreSQL View
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGView_H
#define PGView_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgObject.h"
#include "pgServer.h"
#include "pgDatabase.h"

class pgCollection;

class pgView : public pgRuleObject
{
public:
    pgView(pgSchema *newSchema, const wxString& newName = wxT(""));
    ~pgView();

    int GetIcon() { return PGICON_VIEW; }
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, ctlListView *properties=0, ctlListView *statistics=0, ctlSQLBox *sqlPane=0);
    static pgObject *ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction=wxT(""));

    bool DropObject(wxFrame *frame, wxTreeCtrl *browser);
    bool CanView() { return true; }
    bool WantDummyChild() { return true; }

    wxMenu *GetNewMenu();
    wxString GetSql(wxTreeCtrl *browser);
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);
};

#endif
