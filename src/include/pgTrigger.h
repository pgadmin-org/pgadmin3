//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgTrigger.h PostgreSQL Trigger
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGTrigger_H
#define PGTrigger_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgObject.h"
#include "pgServer.h"
#include "pgDatabase.h"

class pgCollection;
class pgFunction;


class pgTrigger : public pgSchemaObject
{
public:
    pgTrigger(pgSchema *newSchema, const wxString& newName = wxT(""));
    ~pgTrigger();

    int GetIcon() { return PGICON_TRIGGER; }
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    static pgObject *ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction=wxT(""));

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

    bool DropObject(wxFrame *frame, wxTreeCtrl *browser);
    wxString GetSql(wxTreeCtrl *browser);
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);

private:
    wxString function, quotedFullTable, arguments;
    OID functionOid;
    long triggerType;
    bool enabled;
    pgFunction *triggerFunction;
};

#endif
