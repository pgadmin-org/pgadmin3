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

#ifndef PGTABLE_H
#define PGTABLE_H

#include "pgSchema.h"


enum
{
    REPLICATIONSTATUS_NONE=0,
    REPLICATIONSTATUS_SUBSCRIBED,
    REPLICATIONSTATUS_REPLICATED,
    REPLICATIONSTATUS_MULTIPLY_PUBLISHED
};


class pgTableFactory : public pgSchemaObjFactory
{
public:
    pgTableFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
    virtual pgCollection *CreateCollection(pgObject *obj);
    int GetReplicatedIconId() { return replicatedIconId; }
private:
    int replicatedIconId;
};
extern pgTableFactory tableFactory;

class slSet;
class pgTable : public pgSchemaObject
{
public:
    pgTable(pgSchema *newSchema, const wxString& newName = wxT(""));
    ~pgTable();
    int GetIconId();

    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    void ShowHint(frmMain *form, bool force);
    void ShowStatistics(frmMain *form, ctlListView *statistics);

    bool CanDropCascaded() { return true; }
    int GetReplicationStatus(ctlTree *browser, wxString *clusterName=0, long *setId=0);

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
    wxString GetCoveringIndex(ctlTree *browser, const wxString &collist);
    bool GetHasSubclass() const { return hasSubclass; }
    void iSetHasSubclass(bool b) { hasSubclass = b; }
    void iSetIsReplicated(bool b) { isReplicated = b; }
    bool GetIsReplicated() const { return isReplicated; }
    void UpdateRows();
    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
    bool CanView() { return true; }
    bool CanMaintenance() { return true; }
    bool CanBackup() { return true; }
    bool CanRestore() { return true; }
    bool WantDummyChild() { return true; }
    bool GetCanHint();

    wxMenu *GetNewMenu();
    wxString GetSql(ctlTree *browser);
    wxString GetHelpPage(bool forCreate) const;
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

private:
    void UpdateInheritance();
    bool GetVacuumHint();

    void AppendStuff(wxString &sql, ctlTree *browser, pgaFactory &factory);
    wxULongLong rows;
    double estimatedRows;
    bool hasOids, hasSubclass, rowsCounted, isReplicated;
    long inheritedTableCount;
    wxString quotedInheritedTables, inheritedTables, primaryKey, quotedPrimaryKey,
        primaryKeyName, primaryKeyColNumbers, tablespace;
    wxArrayString quotedInheritedTablesList;
    slSet *replicationSet;
};


class pgTableObject : public pgSchemaObject
{
public:
    pgTableObject(pgTable *newTable, pgaFactory &factory, const wxString& newName = wxT(""))
        : pgSchemaObject(newTable->GetSchema(), factory, newName) { table = newTable; }
    pgTable *GetTable() { return table; }
    OID GetTableOid() const {return table->GetOid(); }
    wxString GetTableOidStr() const {return NumToStr(table->GetOid()) + wxT("::oid"); }

protected:
    pgTable *table;
};


class pgTableCollection : public pgSchemaObjCollection
{
public:
    pgTableCollection(pgaFactory *factory, pgSchema *sch);
    void ShowStatistics(frmMain *form, ctlListView *statistics);
};

class pgTableObjCollection : public pgSchemaObjCollection
{
public:
    pgTableObjCollection(pgaFactory *factory, pgTable *_table)
    : pgSchemaObjCollection(factory, _table->GetSchema()) { iSetOid(_table->GetOid()); table=_table; }
    pgTable *GetTable() { return table; }

protected:
    pgTable *table;
};

class pgTableObjFactory : public pgSchemaObjFactory
{
public:
    pgTableObjFactory(const wxChar *tn, const wxChar *ns, const wxChar *nls, char **img, char **imgSm=0) 
        : pgSchemaObjFactory(tn, ns, nls, img, imgSm) {}
    virtual pgCollection *CreateCollection(pgObject *obj);
};


#endif
