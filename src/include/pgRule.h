//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgRule.h PostgreSQL Rule
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGRule_H
#define PGRule_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgObject.h"
#include "pgServer.h"
#include "pgDatabase.h"

class pgCollection;

class pgRule : public pgRuleObject
{
public:
    pgRule(pgSchema *newSchema, const wxString& newName = wxT(""));
    ~pgRule();

    int GetIcon() { return PGICON_RULE; }
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0);
    static pgObject *ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction=wxT(""));

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
    bool CanDrop() { return !GetSystemObject(); }
    bool CanEdit() { return true; }
    bool CanCreate() { return true; }
    bool DropObject(wxFrame *frame, wxTreeCtrl *browser);
    wxString GetSql(wxTreeCtrl *browser);
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);

private:
    wxString event, condition, action, quotedFullTable;
    bool doInstead;
};

#endif
