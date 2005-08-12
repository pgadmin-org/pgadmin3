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
#include "pgSchema.h"

class pgCollection;
class pgSequenceFactory : public pgSchemaObjFactory
{
public:
    pgSequenceFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern pgSequenceFactory sequenceFactory;

class pgSequence : public pgSchemaObject
{
public:
    pgSequence(pgSchema *newSchema, const wxString& newName = wxT(""));
    ~pgSequence();

    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    void ShowStatistics(frmMain *form, ctlListView *statistics);
    bool CanDropCascaded() { return true; }

    void UpdateValues();
    wxULongLong GetLastValue() const { return lastValue; }
    wxULongLong GetMinValue() const { return minValue; }
    wxULongLong GetMaxValue() const { return maxValue; }
    wxULongLong GetCacheValue() const { return cacheValue; }
    wxULongLong GetIncrement() const { return increment; }
    bool GetCycled() const { return cycled; }

    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
    wxString GetSql(ctlTree *browser);
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

private:
    wxULongLong lastValue, minValue, maxValue, cacheValue, increment;
    bool cycled;
};

#endif
