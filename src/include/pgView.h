//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
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
    pgView(pgSchema *newSchema, const wxString& newName = wxString(""));
    ~pgView();

    int GetIcon() { return PGICON_VIEW; }
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0);
    static void ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane);

    bool CanView() { return true; }
    wxString GetSql(wxTreeCtrl *browser);
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);
    static pgObject *ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction=wxT(""));

};

#endif
