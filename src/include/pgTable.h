//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
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
    pgTable(pgSchema *newSchema, const wxString& newName = wxString(""));
    ~pgTable();
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0);
    static void ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane);
    bool GetHasOids() const { return hasOids; }
    void iSetHasOids(bool b) { hasOids=b; }
    wxString GetPrimaryKey() const { return primaryKey; }
    void iSetPrimaryKey(const wxString& s) {primaryKey = s; }
    wxString GetPrimaryKeyColNumbers() const { return primaryKeyColNumbers; }
    void iSetPrimaryKeyColNumbers(const wxString& s) {primaryKeyColNumbers = s; }
    wxString GetPrimaryKeyName() const { return primaryKeyName; }
    void iSetPrimaryKeyName(const wxString& s) {primaryKeyName = s; }
    double GetEstimatedRows() const { return estimatedRows; }
    void iSetEstimatedRows(double d) { estimatedRows=d; }
    double GetRows() const { return rows; }
    long GetInheritedTableCount() { if (inheritedTableCount < 0) UpdateInheritance(); return inheritedTableCount; }
    wxString GetInheritedTables() { GetInheritedTableCount(); return inheritedTables; }
    wxString GetQuotedInheritedTables() { GetInheritedTableCount(); return quotedInheritedTables; }
    bool GetHasSubclass() const { return hasSubclass; }
    void iSetHasSubclass(bool b) { hasSubclass = b; }
    void UpdateRows();

    wxString GetSql(wxTreeCtrl *browser);

private:
    void UpdateInheritance();

    double rows, estimatedRows;
    bool hasOids, hasSubclass;
    long inheritedTableCount;
    wxString inheritedTables, quotedInheritedTables, primaryKey, primaryKeyName, primaryKeyColNumbers;
};

#endif
