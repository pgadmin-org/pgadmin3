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

class pgView : public pgSchemaObject
{
public:
    pgView(pgSchema *newSchema, const wxString& newName = wxString(""));
    ~pgView();

    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0);
    static void ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane);

    wxString GetDefinition() const { return definition; }
    void iSetDefinition(const wxString& s) { definition=s; }
    wxString GetSql(wxTreeCtrl *browser);

private:
    wxString definition;
};

#endif
