//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgaInstance.h - PostgreSQL Agent Instance
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGAINSTANCE_H
#define PGAInstance_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgConn.h"
#include "pgObject.h"
// Class declarations


class pgaInstance : public pgDatabaseObject
{
public:
    pgaInstance(const wxString& newName = wxT(""));
    ~pgaInstance();

    int GetIcon() { return PGAICON_INSTANCE; }
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0);
    static pgObject *ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction=wxEmptyString);
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);

    bool CanCreate() { return false; }
    bool CanView() { return false; }
    bool CanEdit() { return false; }
    bool CanDrop() { return false; }

    long GetPid() const { return pid; }
    void iSetPid(const long l) { pid=l; }
    wxDateTime GetLogintime() const { return logintime; }
    void iSetLogintime(const wxDateTime &d) { logintime=d; }
    wxString GetStation() const { return station; }
    void iSetStation(const wxString &s) { station=s; }

private:
    long pid;
    wxString station;
    wxDateTime logintime;
};

#endif
