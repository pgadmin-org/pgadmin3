//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
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

class pgCollection;

class pgTable : public pgSchemaObject
{
public:
    pgTable(pgSchema *newSchema, const wxString& newName = wxT(""));
    ~pgTable();

    int GetIcon() { return PGICON_TABLE; }
    wxString GetAllConstraints(wxTreeCtrl *browser, wxTreeItemId collectionId, int type);
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    void ShowStatistics(frmMain *form, ctlListView *statistics);

    static pgObject *ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction=wxT(""));
    static void ShowStatistics(pgCollection *collection, ctlListView *statistics);

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
    bool DropObject(wxFrame *frame, wxTreeCtrl *browser);
    bool CanView() { return true; }
    bool CanMaintenance() { return true; }
    bool CanBackup() { return true; }
    bool CanRestore() { return true; }
    bool WantDummyChild() { return true; }

    wxMenu *GetNewMenu();
    wxString GetSql(wxTreeCtrl *browser);
    wxString GetHelpPage(bool forCreate) const;
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);

private:
    void UpdateInheritance();

    wxULongLong rows;
    double estimatedRows;
    bool hasOids, hasSubclass, rowsCounted;
    long inheritedTableCount;
    wxString quotedInheritedTables, inheritedTables, primaryKey, quotedPrimaryKey,
        primaryKeyName, primaryKeyColNumbers, tablespace;
    wxArrayString quotedInheritedTablesList;
};

#endif
