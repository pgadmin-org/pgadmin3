//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
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

    bool CanDropCascaded() { return !GetSystemObject() && pgSchemaObject::CanDrop(); }
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
    OID GetTablespaceOid() const { return tablespaceOid; };
    void iSetTablespaceOid(const OID newVal) { tablespaceOid = newVal; }
    wxULongLong GetRows() const { return rows; }
    long GetInheritedTableCount() { if (inheritedTableCount < 0) UpdateInheritance(); return inheritedTableCount; }
    wxString GetInheritedTables() { GetInheritedTableCount(); return inheritedTables; }
    wxString GetQuotedInheritedTables() { GetInheritedTableCount(); return quotedInheritedTables; }
    wxArrayString GetQuotedInheritedTablesList() { GetInheritedTableCount(); return quotedInheritedTablesList; }
    wxString GetCoveringIndex(ctlTree *browser, const wxString &collist);
    pgCollection *GetColumnCollection(ctlTree *browser);
    pgCollection *GetConstraintCollection(ctlTree *browser);
    bool GetHasSubclass() const { return hasSubclass; }
    void iSetHasSubclass(bool b) { hasSubclass = b; }
    void iSetIsReplicated(bool b) { isReplicated = b; }
    bool GetIsReplicated() const { return isReplicated; }
    bool EnableTriggers(const bool b);
    void UpdateRows();
    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
    bool Truncate(bool cascaded);
    bool CanView() { return true; }
    bool CanMaintenance() { return true; }
    bool CanBackup() { return true; }
    bool CanRestore() { return true; }
    bool WantDummyChild() { return true; }
    bool GetCanHint();
    bool GetShowExtendedStatistics() { return showExtendedStatistics; }
    void iSetShowExtendedStatistics(bool b) { showExtendedStatistics = b; }
    wxString GetFillFactor() { return fillFactor; }
    void iSetFillFactor(const wxString& s) { fillFactor = s; }


    bool HasStats() { return true; }
    bool HasDepends() { return true; }
    bool HasReferences() { return true; }
    bool HasPgstattuple();

    wxMenu *GetNewMenu();
    wxString GetSql(ctlTree *browser);
    wxString GetSelectSql(ctlTree *browser);
    wxString GetInsertSql(ctlTree *browser);
    wxString GetUpdateSql(ctlTree *browser);
    wxString GetHelpPage(bool forCreate) const;
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);
	void iSetTriggersEnabled(ctlTree *browser, bool enable);

private:
    void UpdateInheritance();
    bool GetVacuumHint();
	wxString GetCols(ctlTree *browser, size_t indent, wxString &QMs, bool withQM);

    void AppendStuff(wxString &sql, ctlTree *browser, pgaFactory &factory);
    wxULongLong rows;
    double estimatedRows;
    wxString fillFactor;
    bool hasOids, hasSubclass, rowsCounted, isReplicated, showExtendedStatistics;
    long inheritedTableCount;
    wxString quotedInheritedTables, inheritedTables, primaryKey, quotedPrimaryKey,
        primaryKeyName, primaryKeyColNumbers, tablespace;
    wxArrayString quotedInheritedTablesList;
    slSet *replicationSet;
	OID tablespaceOid;
};


class pgTableObject : public pgSchemaObject
{
public:
    pgTableObject(pgTable *newTable, pgaFactory &factory, const wxString& newName = wxT(""))
        : pgSchemaObject(newTable->GetSchema(), factory, newName) { table = newTable; }
    virtual pgTable *GetTable() const { return table; }
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
    virtual pgTable *GetTable() const { return table; }
    bool CanCreate();

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

class countRowsFactory : public contextActionFactory
{
public:
    countRowsFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};


class executePgstattupleFactory : public contextActionFactory
{
public:
    executePgstattupleFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
	bool CheckChecked(pgObject *obj);
};

class disableAllTriggersFactory : public contextActionFactory
{
public:
    disableAllTriggersFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};

class enableAllTriggersFactory : public contextActionFactory
{
public:
    enableAllTriggersFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};

class truncateFactory : public contextActionFactory
{
public:
    truncateFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};


class truncateCascadedFactory : public contextActionFactory
{
public:
    truncateCascadedFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};

#endif
