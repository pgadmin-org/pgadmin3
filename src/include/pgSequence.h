//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
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
class pgaSequenceFactory : public pgaFactory
{
public:
    pgaSequenceFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, wxTreeCtrl *browser, const wxString &restr=wxEmptyString);
};
extern pgaSequenceFactory sequenceFactory;

class pgSequence : public pgSchemaObject
{
public:
    pgSequence(pgSchema *newSchema, const wxString& newName = wxT(""));
    ~pgSequence();

    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    void ShowStatistics(frmMain *form, ctlListView *statistics);
    bool CanDropCascaded() { return true; }

    void UpdateValues();
    wxULongLong GetLastValue() const { return lastValue; }
    wxULongLong GetMinValue() const { return minValue; }
    wxULongLong GetMaxValue() const { return maxValue; }
    wxULongLong GetCacheValue() const { return cacheValue; }
    wxULongLong GetIncrement() const { return increment; }
    bool GetCycled() const { return cycled; }

    bool DropObject(wxFrame *frame, wxTreeCtrl *browser, bool cascaded);
    wxString GetSql(wxTreeCtrl *browser);
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);

private:
    wxULongLong lastValue, minValue, maxValue, cacheValue, increment;
    bool cycled;
};

#endif
