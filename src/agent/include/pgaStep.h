//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgaStep.h - PostgreSQL Agent Job Step
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGASTEP_H
#define PGASTEP_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgConn.h"
#include "pgObject.h"
#include "pgaJob.h"

// Class declarations


class pgaStep : public pgaJobObject
{
public:
    pgaStep(pgaJob *job, const wxString& newName = wxT(""));
    ~pgaStep();

    int GetIcon() { return PGAICON_STEP; }
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
    wxString GetCode() const { return code; }
    void iSetCode(const wxString &s) { code=s; }
    wxString GetDbname() const { return dbname; }
    void iSetDbname(const wxString &s) { dbname=s; }
    wxString GetOnError() const { return onError; }
    void iSetOnError(const wxString &s) { onError=s; }
    wxChar GetOnErrorChar() const { return onErrorChar; }
    void iSetOnErrorChar(const wxChar c) { onErrorChar = c; }

private:
    bool enabled;
    wxString kind, code, dbname, onError;
    wxChar kindChar, onErrorChar;
};

#endif
