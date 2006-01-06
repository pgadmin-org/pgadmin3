//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgAggregate.h PostgreSQL Aggregate
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGAGGREGATE_H
#define PGAGGREGATE_H

// App headers
#include "pgSchema.h"

class pgCollection;
class pgAggregateFactory : public pgaFactory
{
public:
    pgAggregateFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
    pgCollection *CreateCollection(pgObject *obj);
};
extern pgAggregateFactory aggregateFactory;

class pgAggregate : public pgSchemaObject
{
public:
    pgAggregate(pgSchema *newSchema, const wxString& newName = wxT(""));
    ~pgAggregate();
    void SetSchema(pgSchema *newSchema) { schema = newSchema; }
    pgSchema *GetSchema() const {return schema; }
    bool CanDropCascaded() { return true; }

    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
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
    wxString GetSortOp() { return sortOp; }
    void iSetSortOp(const wxString &s) { sortOp=s; }
    wxString GetQuotedSortOp() { return quotedSortOp; }
    void iSetQuotedSortOp(const wxString &s) { quotedSortOp=s; }


    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
    wxString GetSql(ctlTree *browser);
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

private:
    pgSchema *schema;
    wxString inputType, stateType, finalType, sortOp, quotedSortOp,
             stateFunction, finalFunction, initialCondition;
};

#endif
