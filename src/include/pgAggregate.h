//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgAggregate.h PostgreSQL Aggregate
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGAGGREGATE_H
#define PGAGGREGATE_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgObject.h"
#include "pgServer.h"
#include "pgDatabase.h"

class pgCollection;

class pgAggregate : public pgSchemaObject
{
public:
    pgAggregate(pgSchema *newSchema, const wxString& newName = wxString(""));
    ~pgAggregate();
    void SetSchema(pgSchema *newSchema) { schema = newSchema; }
    pgSchema *GetSchema() const {return schema; }
    int GetIcon() { return PGICON_AGGREGATE; }

    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0);
    static void ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane);
    wxString GetFullName() const;
    wxString GetInputType() const { return inputType; }
    void iSetInputType(const wxString& s) { inputType=s; }
    wxString GetStateType() { return stateType; }
    void iSetStateType(const wxString& s) { stateType=s; }
    wxString GetFinalType() { return finalType; }
    void iSetFinalType(const wxString& s) { finalType=s; }
    wxString GetStateFunction() { return stateFunction; }
    void iSetStateFunction(const wxString& s) { stateFunction=s; }
    wxString GetFinalFunction() { return finalFunction; }
    void iSetFinalFunction(const wxString& s) { finalFunction=s; }
    wxString GetInitialCondition() { return initialCondition; }
    void iSetInitialCondition(const wxString& s) { initialCondition=s; }

    wxString GetSql(wxTreeCtrl *browser);
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);
    static pgObject *ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction=wxT(""));

private:
    pgSchema *schema;
    wxString inputType, stateType, finalType,
             stateFunction, finalFunction, initialCondition;
};

#endif
