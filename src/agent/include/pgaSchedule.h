//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgaStep.h - PostgreSQL Agent Job Schedule
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGASCHEDULE_H
#define PGASCHEDULE_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgConn.h"
#include "pgObject.h"
#include "pgaJob.h"
// Class declarations


WX_DECLARE_OBJARRAY(wxTimeSpan, wxArrayTimeSpan);


class pgaSchedule : public pgaJobObject
{
public:
    pgaSchedule(pgaJob *job, const wxString& newName = wxT(""));
    ~pgaSchedule();

    int GetIcon() { return PGAICON_SCHEDULE; }
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    static pgObject *ReadObjects(pgaJob *job, wxTreeCtrl *browser, const wxString &restriction=wxEmptyString);
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);
    bool DropObject(wxFrame *frame, wxTreeCtrl *browser);

    bool GetEnabled() const { return enabled; }
    void iSetEnabled(const bool b) { enabled=b; }
    wxChar GetKindChar() const { return kindChar; }
    void iSetKindChar(const wxChar c) { kindChar=c; }
    wxString GetKind() const { return kind; }
    void iSetKind(const wxString &s) { kind=s; }
    wxDateTime GetStart() const { return start; }
    void iSetStart(const wxDateTime &d) { start=d; }
    wxDateTime GetEnd() const { return end; }
    void iSetEnd(const wxDateTime &d) { end=d; }
    wxDateTime GetSchedule() const { return schedule; }
    void iSetSchedule(const wxDateTime &d) { schedule=d; }
    wxString GetIntervalListString() const { return intervalListString; }
    void iSetIntervalList(const wxString &s);
    wxArrayTimeSpan GetIntervalList() const { return intervalList; }

private:
    bool enabled;
    wxString kind, intervalListString;
    wxArrayTimeSpan intervalList;
    wxDateTime start, end, schedule;
    wxChar kindChar;
};

#endif
