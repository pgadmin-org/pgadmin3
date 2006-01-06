//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgTrigger.h PostgreSQL Trigger
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGTRIGGER_H
#define PGTRIGGER_H

#include "pgTable.h"

class pgFunction;

class pgTriggerFactory : public pgTableObjFactory
{
public:
    pgTriggerFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern pgTriggerFactory triggerFactory;


class pgTrigger : public pgTableObject
{
public:
    pgTrigger(pgTable *newTable, const wxString& newName = wxT(""));
    ~pgTrigger();

    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    bool CanDropCascaded() { return true; }

    wxString GetFireWhen() const;
    wxString GetEvent() const;
    wxString GetForEach() const;
    wxString GetFunction() const { return function; }
    void iSetFunction(const wxString& s) { function=s; }
    void iSetArguments(const wxString& s) { arguments=s; }
    wxString GetArguments() const { return arguments; }
    long GetTriggerType() const {return triggerType; }
    void iSetTriggerType(const long l) { triggerType=l; }
    bool GetEnabled() const { return enabled; }
    void iSetEnabled(const bool b) {enabled=b; }
    void iSetTriggerFunction(pgFunction *fkt) { triggerFunction=fkt; }
    wxString GetQuotedFullTable() const { return quotedFullTable; }
    void iSetQuotedFullTable(const wxString &s) { quotedFullTable=s; }
    OID GetFunctionOid() const { return functionOid; }
    void iSetFunctionOid(const OID d) { functionOid=d; }

    void SetDirty();

    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
    wxString GetSql(ctlTree *browser);
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

private:
    wxString function, quotedFullTable, arguments;
    OID functionOid;
    long triggerType;
    bool enabled;
    pgFunction *triggerFunction;
};

#endif
