//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgSequence.h PostgreSQL Sequence
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGSEQUENCE_H
#define PGSEQUENCE_H


#include "pgSchema.h"

class pgSequenceFactory : public pgSchemaObjFactory
{
public:
    pgSequenceFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
    int GetReplicatedIconId() { return replicatedIconId; }
private:
    int replicatedIconId;
};
extern pgSequenceFactory sequenceFactory;

class pgSequence : public pgSchemaObject
{
public:
    pgSequence(pgSchema *newSchema, const wxString& newName = wxT(""));
    ~pgSequence();
    int GetIconId();

    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    void ShowStatistics(frmMain *form, ctlListView *statistics);
    bool CanDropCascaded() { return !GetSystemObject() && pgSchemaObject::CanDrop(); }

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

    bool HasStats() { return true; }
    bool HasDepends() { return true; }
    bool HasReferences() { return true; }

private:
    wxULongLong lastValue, minValue, maxValue, cacheValue, increment;
    bool cycled, isReplicated;
};

#endif
