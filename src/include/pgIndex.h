//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgIndex.h PostgreSQL Index
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGIndex_H
#define PGIndex_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgObject.h"
#include "pgServer.h"
#include "pgDatabase.h"

class pgCollection;

class pgIndex : public pgSchemaObject
{
public:
    pgIndex(pgSchema *newSchema, const wxString& newName = wxString(""));
    ~pgIndex();

    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0);
    static void ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane);

    wxString GetQuotedColumns() const { return quotedColumns; }
    wxString GetColumns() const { return columns; }

    wxString GetColumnNumbers() const { return columnNumbers; }
    void iSetColumnNumbers(const wxString& s) { columnNumbers=s; }
    wxString GetConstraint() const { return constraint; }
    void iSetConstraint(const wxString& s) { constraint=s; }
    wxString GetIndexType() const { return indexType; }
    void iSetIndexType(const wxString& s) { indexType=s; }
    long GetColumnCount() const { return columnCount; }
    void iSetColumnCount(const long l) { columnCount=l; }
    bool GetIsUnique() const { return isUnique; }
    void iSetIsUnique(const bool b) { isUnique=b; }
    bool GetIsPrimary() const { return isPrimary; }
    void iSetIsPrimary(const bool b) { isPrimary=b; }
    bool GetIsClustered() const { return isClustered; }
    void iSetIsClustered(const bool b) { isClustered=b; }
    wxString GetIdxTable() const { return idxTable; }
    void iSetIdxTable(const wxString& s) { idxTable=s; }
    wxString GetIdxSchema() const { return idxSchema; }
    void iSetIdxSchema(const wxString& s) { idxSchema=s; }
    double GetRelTableOid() const { return relTableOid; }
    void iSetRelTableOid(const double d) { relTableOid=d; }
    wxString GetProcName() const { return procName; }
    void iSetProcName(const wxString& s) { procName=s; }
    wxString GetProcNamespace() const { return procNamespace; }
    void iSetProcNamespace(const wxString& s) { procNamespace=s; }
    wxString GetProcArgs() const { return procArgs; }
    void iSetProcArgs(const wxString& s) { procArgs=s; }
    wxString GetOperatorClass() const { return operatorClass; }
    void iSetOperatorClass(const wxString& s) { operatorClass=s; }

    wxString GetSql(wxTreeCtrl *browser);
    wxString GetCreate();

private:
    wxString columnNumbers, columns, quotedColumns, constraint, indexType, idxTable, idxSchema;
    wxString procName, procNamespace, procArgs, operatorClass;
    long columnCount;
    bool isUnique, isPrimary, isClustered;
    double relTableOid;
};

#endif