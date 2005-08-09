//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgTable.h PostgreSQL Table
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGTable_H
#define PGTable_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgObject.h"
#include "pgServer.h"
#include "pgDatabase.h"
#include "pgSchema.h"

class pgCollection;
class pgaTableFactory : public pgaFactory
{
public:
    pgaTableFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, wxTreeCtrl *browser, const wxString &restr=wxEmptyString);
};
extern pgaTableFactory tableFactory;

class pgTable : public pgSchemaObject
{
public:
    pgTable(pgSchema *newSchema, const wxString& newName = wxT(""));
    ~pgTable();

    wxString GetAllConstraints(wxTreeCtrl *browser, wxTreeItemId collectionId, int type);
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    void ShowHint(frmMain *form, bool force);
    void ShowStatistics(frmMain *form, ctlListView *statistics);

    bool CanDropCascaded() { return true; }

    bool GetHasOids() const { return hasOids; }
    void iSetHasOids(bool b) { hasOids=b; }
    wxString GetPrimaryKey() const { return primaryKey; }
    void iSetPrimaryKey(const wxString& s) {primaryKey = s; }
    wxString GetQuotedPrimaryKey() const { return quotedPrimaryKey; }
    void iSetQuotedPrimaryKey(const wxString& s) {quotedPrimaryKey = s; }
    wxString GetPrimaryKeyColNumbers() const { return primaryKeyColNumbers; }
    void iSetPrimaryKeyColNumbers(const wxString& s) {primaryKeyColNumbers = s; }
    wxString GetPrimaryKeyName() const { return primaryKeyName; }
    void iSetPrimaryKeyName(const wxString& s) {primaryKeyName = s; }
    double GetEstimatedRows() const { return estimatedRows; }
    void iSetEstimatedRows(const double d) { estimatedRows=d; }
    wxString GetTablespace() const { return tablespace; };
    void iSetTablespace(const wxString& newVal) { tablespace = newVal; }
    wxULongLong GetRows() const { return rows; }
    long GetInheritedTableCount() { if (inheritedTableCount < 0) UpdateInheritance(); return inheritedTableCount; }
    wxString GetInheritedTables() { GetInheritedTableCount(); return inheritedTables; }
    wxString GetQuotedInheritedTables() { GetInheritedTableCount(); return quotedInheritedTables; }
    wxArrayString GetQuotedInheritedTablesList() { GetInheritedTableCount(); return quotedInheritedTablesList; }
    wxString GetCoveringIndex(wxTreeCtrl *browser, const wxString &collist);
    bool GetHasSubclass() const { return hasSubclass; }
    void iSetHasSubclass(bool b) { hasSubclass = b; }
    void UpdateRows();
    bool DropObject(wxFrame *frame, wxTreeCtrl *browser, bool cascaded);
    bool CanView() { return true; }
    bool CanMaintenance() { return true; }
    bool CanBackup() { return true; }
    bool CanRestore() { return true; }
    bool WantDummyChild() { return true; }
    bool GetCanHint();

    wxMenu *GetNewMenu();
    wxString GetSql(wxTreeCtrl *browser);
    wxString GetHelpPage(bool forCreate) const;
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);

private:
    void UpdateInheritance();
    bool GetVacuumHint();

    wxULongLong rows;
    double estimatedRows;
    bool hasOids, hasSubclass, rowsCounted;
    long inheritedTableCount;
    wxString quotedInheritedTables, inheritedTables, primaryKey, quotedPrimaryKey,
        primaryKeyName, primaryKeyColNumbers, tablespace;
    wxArrayString quotedInheritedTablesList;
};


class pgTableCollection : public pgSchemaCollection
{
public:
    pgTableCollection(pgaFactory &factory, pgSchema *sch);
    void ShowStatistics(frmMain *form, ctlListView *statistics);
};

#endif
