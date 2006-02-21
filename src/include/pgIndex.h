//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: pgIndex.h 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgIndex.h PostgreSQL Index
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGINDEX_H
#define PGINDEX_H

#include "pgTable.h"

class pgCollection;



class pgIndexBase : public pgTableObject
{
protected:
    pgIndexBase(pgTable *newTable, pgaFactory &factory, const wxString& newName = wxT(""));

public:
    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    void ShowStatistics(frmMain *form, ctlListView *statistics);
    bool CanDropCascaded() { return true; }

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
    wxString GetTablespace() const { return tablespace; };
    void iSetTablespace(const wxString& newVal) { tablespace = newVal; }

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

    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
    wxString GetCreate();
    bool CanRestore() { return true; }
    wxString GetSql(ctlTree *browser);
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);
    bool CanMaintenance() { return true; }

protected:
    void ReadColumnDetails();

private:
    wxString columnNumbers, columns, quotedColumns, indexType, idxTable, idxSchema, constraint, tablespace;
    wxString procName, procNamespace, procArgs, procArgTypeList, typedColumns, quotedTypedColumns, operatorClasses, operatorClassList;
    long columnCount;
    bool isUnique, isPrimary, isClustered;
    bool deferrable, deferred;
    OID relTableOid;
};


class pgIndex : public pgIndexBase
{
public:
    pgIndex(pgTable *newTable, const wxString& newName = wxT(""));
    ~pgIndex();
};


class pgIndexBaseFactory : public pgTableObjFactory
{
public:
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
protected:
    pgIndexBaseFactory(const wxChar *tn, const wxChar *ns, const wxChar *nls, char **img) : pgTableObjFactory(tn, ns, nls, img) {}
};

class pgIndexFactory : public pgIndexBaseFactory
{
public:
    pgIndexFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern pgIndexFactory indexFactory;


#endif
