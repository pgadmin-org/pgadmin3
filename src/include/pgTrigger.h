//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
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
    pgTrigger(pgSchema *newSchema, const wxString& newName = wxString(""));
    ~pgTrigger();

    int GetIcon() { return PGICON_TRIGGER; }
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0);
    static void ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane);

    wxString GetFireWhen() const;
    wxString GetEvent() const;
    wxString GetForEach() const;
    wxString GetFunction() const { return function; }
    void iSetFunction(const wxString& s) { function=s; }
    long GetTriggerType() const {return triggerType; }
    void iSetTriggerType(const long l) { triggerType=l; }
    bool GetEnabled() const { return enabled; }
    void iSetEnabled(const bool b) {enabled=b; }
    void iSetTriggerFunction(pgFunction *fkt) { triggerFunction=fkt; }
    wxString GetQuotedFullTable() const { return quotedFullTable; }
    void iSetQuotedFullTable(const wxString &s) { quotedFullTable=s; }
    double GetFunctionOid() const { return functionOid; }
    void iSetFunctionOid(const double d) { functionOid=d; }

    void SetDirty();

    bool CanDrop() { return true; }
    wxString GetSql(wxTreeCtrl *browser);
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);
    static pgObject *ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction=wxT(""));

private:
    wxString function, quotedFullTable;
    double functionOid;
    long triggerType;
    bool enabled;
    pgFunction *triggerFunction;
};

#endif
