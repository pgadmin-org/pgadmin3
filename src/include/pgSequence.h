//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgSequence.h PostgreSQL Sequence
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGSequence_H
#define PGSequence_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgObject.h"
#include "pgServer.h"
#include "pgDatabase.h"

class pgCollection;

class pgSequence : public pgSchemaObject
{
public:
    pgSequence(pgSchema *newSchema, const wxString& newName = wxString(""));
    ~pgSequence();

    int GetIcon() { return PGICON_SEQUENCE; }
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0);
    static void ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane);

    void UpdateValues();
    wxULongLong GetLastValue() const { return lastValue; }
    wxULongLong GetMinValue() const { return minValue; }
    wxULongLong GetMaxValue() const { return maxValue; }
    wxULongLong GetCacheValue() const { return cacheValue; }
    wxULongLong GetIncrement() const { return increment; }
    bool GetCycled() const { return cycled; }

    bool CanDrop() { return true; }
    bool DropObject(wxFrame *frame, wxTreeCtrl *browser);
    wxString GetSql(wxTreeCtrl *browser);
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);

private:
    static pgObject *ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction=wxT(""));

    wxULongLong lastValue, minValue, maxValue, cacheValue, increment;
    bool cycled;
};

#endif
