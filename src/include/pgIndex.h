//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
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
    pgIndex(pgSchema *newSchema, const wxString& newName = wxT(""), int type=PG_INDEX);
    ~pgIndex();

    int GetIcon() { return PGICON_INDEX; }
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, ctlListView *properties=0, ctlListView
        *statistics=0, ctlSQLBox *sqlPane=0);
    static pgObject *ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction);
    static pgObject *ReadObjects(pgCollection *collection, wxTreeCtrl *browser);

    wxString GetProcArgs() const { return procArgs; }
    wxString GetQuotedTypedColumns() const { return quotedTypedColumns; }
    wxString GetTypedColumns() const { return typedColumns; }
    wxString GetOperatorClasses() const { return operatorClasses; }
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
    OID GetRelTableOid() const { return relTableOid; }
    void iSetRelTableOid(const OID d) { relTableOid=d; }

    wxString GetProcName() const { return procName; }
    void iSetProcName(const wxString& s) { procName=s; }
    wxString GetProcNamespace() const { return procNamespace; }
    void iSetProcNamespace(const wxString& s) { procNamespace=s; }

    bool GetDeferrable() const { return deferrable; }
    void iSetDeferrable(const bool b) { deferrable=b; }
    bool GetDeferred() const { return deferred; }
    void iSetDeferred(const bool b) { deferred=b; }

    void iSetOperatorClassList(const wxString& s) { operatorClassList=s; }
    void iSetProcArgTypeList(const wxString& s) { procArgTypeList=s; }

    bool DropObject(wxFrame *frame, wxTreeCtrl *browser);
    wxString GetCreate();
    wxString GetSql(wxTreeCtrl *browser);
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);
    bool CanMaintenance() { return true; }

protected:
    void ReadColumnDetails();

private:
    wxString columnNumbers, columns, quotedColumns, indexType, idxTable, idxSchema, constraint;
    wxString procName, procNamespace, procArgs, procArgTypeList, typedColumns, quotedTypedColumns, operatorClasses, operatorClassList;
    long columnCount;
    bool isUnique, isPrimary, isClustered;
    bool deferrable, deferred;
    OID relTableOid;
};



#endif
