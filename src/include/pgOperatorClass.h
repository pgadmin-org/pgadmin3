//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgOperatorClass.h PostgreSQL OperatorClass
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGOperatorClass_H
#define PGOperatorClass_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgObject.h"
#include "pgServer.h"
#include "pgDatabase.h"

class pgCollection;

class pgOperatorClass : public pgSchemaObject
{
public:
    pgOperatorClass(pgSchema *newSchema, const wxString& newName = wxString(""));
    ~pgOperatorClass();

    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0);
    static void ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane);


    wxString GetInType() const {return inType; }
    void iSetInType(const wxString&s) { inType=s; }
    wxString GetKeyType() const {return keyType; }
    void iSetKeyType(const wxString&s) { keyType=s; }
    wxString GetSql(wxTreeCtrl *browser);
    bool GetOpcDefault() const { return opcDefault; }
    void iSetOpcDefault(const bool b) { opcDefault=b; }

private:
    wxString inType, keyType;
    bool opcDefault;
};

#endif