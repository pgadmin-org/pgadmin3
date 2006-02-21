//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: pgRule.h 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgRule.h PostgreSQL Rule
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGRULE_H
#define PGRULE_H


#include "pgSchema.h"

class pgRuleObject : public pgSchemaObject
{
public:
    pgRuleObject(pgSchema *newSchema, pgaFactory &factory, const wxString& newName=wxEmptyString) : pgSchemaObject(newSchema, factory, newName) {}

    wxString GetFormattedDefinition();
    wxString GetDefinition() const { return definition; }
    void iSetDefinition(const wxString& s) { definition=s; }

protected:
    wxString definition;
};



//////////////////////////////////////////////////////////7


class pgRuleFactory : public pgSchemaObjFactory
{
public:
    pgRuleFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern pgRuleFactory ruleFactory;


class pgRule : public pgRuleObject
{
public:
    pgRule(pgSchema *newSchema, const wxString& newName = wxT(""));
    ~pgRule();

    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    bool CanDropCascaded() { return true; }

    wxString GetEvent() const { return event; }
    void iSetEvent(const wxString& s) { event=s; }
    wxString GetCondition() const { return condition; }
    void iSetCondition(const wxString& s) { condition=s; }
    wxString GetAction() const { return action; }
    void iSetAction(const wxString& s) { action=s; }
    bool GetDoInstead() const { return doInstead; }
    void iSetDoInstead(const bool b) { doInstead=b; }
    wxString GetQuotedFullTable() const { return quotedFullTable; }
    void iSetQuotedFullTable(const wxString &s) { quotedFullTable=s; }

    bool GetSystemObject() const { return GetName() == wxT("_RETURN"); }
    bool CanDrop() { return !GetSystemObject() && pgSchemaObject::CanDrop(); }
    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
    wxString GetSql(ctlTree *browser);
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

private:
    wxString event, condition, action, quotedFullTable;
    bool doInstead;
};

#endif
