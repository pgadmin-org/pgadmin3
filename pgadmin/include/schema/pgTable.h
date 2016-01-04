//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgTable.h PostgreSQL Table
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGTABLE_H
#define PGTABLE_H

#include "pgSchema.h"


enum
{
	REPLICATIONSTATUS_NONE = 0,
	REPLICATIONSTATUS_SUBSCRIBED,
	REPLICATIONSTATUS_REPLICATED,
	REPLICATIONSTATUS_MULTIPLY_PUBLISHED
};


class pgTableFactory : public pgSchemaObjFactory
{
public:
	pgTableFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);
	int GetReplicatedIconId()
	{
		return replicatedIconId;
	}
private:
	int replicatedIconId;
};
extern pgTableFactory tableFactory;

class slSet;
class pgTable : public pgSchemaObject
{
public:
	pgTable(pgSchema *newSchema, const wxString &newName = wxT(""));
	pgTable(pgSchema *newSchema, pgaFactory &factory, const wxString &newName = wxT(""));
	~pgTable();
	wxString GetTranslatedMessage(int kindOfMessage) const;
	int GetIconId();

	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	void ShowHint(frmMain *form, bool force);
	void ShowStatistics(frmMain *form, ctlListView *statistics);

	bool CanDropCascaded()
	{
		return !GetSystemObject() && pgSchemaObject::CanDrop();
	}
	int GetReplicationStatus(ctlTree *browser, wxString *clusterName = 0, long *setId = 0);

	bool GetHasOids() const
	{
		return hasOids;
	}
	void iSetHasOids(bool b)
	{
		hasOids = b;
	}
	bool GetUnlogged() const
	{
		return unlogged;
	}
	void iSetUnlogged(bool b)
	{
		unlogged = b;
	}
	wxString GetPrimaryKey() const
	{
		return primaryKey;
	}
	void iSetPrimaryKey(const wxString &s)
	{
		primaryKey = s;
	}
	wxString GetQuotedPrimaryKey() const
	{
		return quotedPrimaryKey;
	}
	void iSetQuotedPrimaryKey(const wxString &s)
	{
		quotedPrimaryKey = s;
	}
	wxString GetPrimaryKeyColNumbers() const
	{
		return primaryKeyColNumbers;
	}
	void iSetPrimaryKeyColNumbers(const wxString &s)
	{
		primaryKeyColNumbers = s;
	}
	wxString GetPrimaryKeyName() const
	{
		return primaryKeyName;
	}
	void iSetPrimaryKeyName(const wxString &s)
	{
		primaryKeyName = s;
	}
	wxString GetDistributionColNumbers() const
	{
		return distributionColNumbers;    // for Greenplum
	}
	void iSetDistributionColNumbers(const wxString &s)
	{
		distributionColNumbers = s;    // for Greenplum
		if (s.Length() > 0) distributionIsRandom = false;
	}
	void iSetDistributionIsRandom()
	{
		distributionIsRandom = true;
	}
	double GetEstimatedRows() const
	{
		return estimatedRows;
	}
	void iSetEstimatedRows(const double d)
	{
		estimatedRows = d;
	}
	wxString GetTablespace() const
	{
		return tablespace;
	};
	void iSetTablespace(const wxString &newVal)
	{
		tablespace = newVal;
	}
	OID GetTablespaceOid() const
	{
		return tablespaceOid;
	};
	void iSetTablespaceOid(const OID newVal)
	{
		tablespaceOid = newVal;
	}
	wxString GetOfType() const
	{
		return ofType;
	};
	void iSetOfType(const wxString &newVal)
	{
		ofType = newVal;
	}
	OID GetOfTypeOid() const
	{
		return ofTypeOid;
	};
	void iSetOfTypeOid(const OID newVal)
	{
		ofTypeOid = newVal;
	}
	wxULongLong GetRows() const
	{
		return rows;
	}
	long GetInheritedTableCount()
	{
		if (inheritedTableCount < 0) UpdateInheritance();
		return inheritedTableCount;
	}
	wxString GetInheritedTables()
	{
		GetInheritedTableCount();
		return inheritedTables;
	}
	wxString GetQuotedInheritedTables()
	{
		GetInheritedTableCount();
		return quotedInheritedTables;
	}
	wxArrayString GetInheritedTablesOidList()
	{
		GetInheritedTableCount();
		return inheritedTablesOidList;
	}
	wxArrayString GetQuotedInheritedTablesList()
	{
		GetInheritedTableCount();
		return quotedInheritedTablesList;
	}
	wxString GetCoveringIndex(ctlTree *browser, const wxString &collist);
	pgCollection *GetColumnCollection(ctlTree *browser);
	pgCollection *GetConstraintCollection(ctlTree *browser);
	bool GetHasSubclass() const
	{
		return hasSubclass;
	}
	void iSetHasSubclass(bool b)
	{
		hasSubclass = b;
	}
	void iSetIsReplicated(bool b)
	{
		isReplicated = b;
	}
	bool GetIsReplicated() const
	{
		return isReplicated;
	}
	void iSetTriggerCount(long l)
	{
		triggerCount = l;
	}
	int GetTriggerCount() const
	{
		return triggerCount;
	}
	bool EnableTriggers(const bool b);
	void UpdateRows();
	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
	bool Truncate(bool cascaded);
	bool ResetStats();
	bool CanView()
	{
		return true;
	}
	bool CanMaintenance()
	{
		return true;
	}
	bool CanBackup()
	{
		return true;
	}
	bool CanRestore()
	{
		return true;
	}
	bool WantDummyChild()
	{
		return true;
	}
	bool GetCanHint();
	bool GetShowExtendedStatistics()
	{
		return showExtendedStatistics;
	}
	void iSetShowExtendedStatistics(bool b)
	{
		showExtendedStatistics = b;
	}
	wxString GetFillFactor()
	{
		return fillFactor;
	}
	void iSetFillFactor(const wxString &s)
	{
		fillFactor = s;
	}
	wxString GetAppendOnly()
	{
		return appendOnly;
	}
	void iSetAppendOnly(const wxString &s)
	{
		appendOnly = s;
	}
	wxString GetCompressLevel()
	{
		return compressLevel;
	}
	void iSetCompressLevel(const wxString &s)
	{
		compressLevel = s;
	}
	wxString GetOrientation()
	{
		return orientation;
	}
	void iSetOrientation(const wxString &s)
	{
		orientation = s;
	}
	wxString GetCompressType()
	{
		return compresstype;
	}
	void iSetCompressType(const wxString &s)
	{
		compresstype = s;
	};
	wxString GetBlocksize()
	{
		return blocksize;
	}
	void iSetBlocksize(const wxString &s)
	{
		blocksize = s;
	};
	wxString GetChecksum()
	{
		return checksum;
	}
	void iSetChecksum(const wxString &s)
	{
		checksum = s;
	};
	wxString GetPartitionDef()
	{
		return partitionDef;
	}
	void iSetPartitionDef(const wxString &s)
	{
		partitionDef = s;
	}
	bool GetIsPartitioned() const
	{
		return isPartitioned || partitionDef.Length() > 0;
	}
	void iSetIsPartitioned(bool b)
	{
		isPartitioned = b;
	}

	bool GetCustomAutoVacuumEnabled()
	{
		return !reloptions.IsEmpty();
	}
	wxString GetRelOptions()
	{
		return reloptions;
	}
	void iSetRelOptions(const wxString &s)
	{
		reloptions = s;
	}
	int GetAutoVacuumEnabled()
	{
		return autovacuum_enabled;
	}
	void iSetAutoVacuumEnabled(int i)
	{
		autovacuum_enabled = i;
	}
	wxString GetAutoVacuumVacuumThreshold()
	{
		return autovacuum_vacuum_threshold;
	}
	void iSetAutoVacuumVacuumThreshold(const wxString &s)
	{
		autovacuum_vacuum_threshold = s;
	}
	wxString GetAutoVacuumVacuumScaleFactor()
	{
		return autovacuum_vacuum_scale_factor;
	}
	void iSetAutoVacuumVacuumScaleFactor(const wxString &s)
	{
		autovacuum_vacuum_scale_factor = s;
	}
	wxString GetAutoVacuumAnalyzeThreshold()
	{
		return autovacuum_analyze_threshold;
	}
	void iSetAutoVacuumAnalyzeThreshold(const wxString &s)
	{
		autovacuum_analyze_threshold = s;
	}
	wxString GetAutoVacuumAnalyzeScaleFactor()
	{
		return autovacuum_analyze_scale_factor;
	}
	void iSetAutoVacuumAnalyzeScaleFactor(const wxString &s)
	{
		autovacuum_analyze_scale_factor = s;
	}
	wxString GetAutoVacuumVacuumCostDelay()
	{
		return autovacuum_vacuum_cost_delay;
	}
	void iSetAutoVacuumVacuumCostDelay(const wxString &s)
	{
		autovacuum_vacuum_cost_delay = s;
	}
	wxString GetAutoVacuumVacuumCostLimit()
	{
		return autovacuum_vacuum_cost_limit;
	}
	void iSetAutoVacuumVacuumCostLimit(const wxString &s)
	{
		autovacuum_vacuum_cost_limit = s;
	}
	wxString GetAutoVacuumFreezeMinAge()
	{
		return autovacuum_freeze_min_age;
	}
	void iSetAutoVacuumFreezeMinAge(const wxString &s)
	{
		autovacuum_freeze_min_age = s;
	}
	wxString GetAutoVacuumFreezeMaxAge()
	{
		return autovacuum_freeze_max_age;
	}
	void iSetAutoVacuumFreezeMaxAge(const wxString &s)
	{
		autovacuum_freeze_max_age = s;
	}
	wxString GetAutoVacuumFreezeTableAge()
	{
		return autovacuum_freeze_table_age;
	}
	void iSetAutoVacuumFreezeTableAge(const wxString &s)
	{
		autovacuum_freeze_table_age = s;
	}
	bool GetHasToastTable()
	{
		return hasToastTable;
	}
	void iSetHasToastTable(bool b)
	{
		hasToastTable = b;
	}

	/* TOAST TABLE autovacuum settings */
	bool GetToastCustomAutoVacuumEnabled()
	{
		return !toast_reloptions.IsEmpty();
	}
	wxString GetToastRelOptions()
	{
		return toast_reloptions;
	}
	void iSetToastRelOptions(const wxString &s)
	{
		toast_reloptions = s;
	}
	int GetToastAutoVacuumEnabled()
	{
		return toast_autovacuum_enabled;
	}
	void iSetToastAutoVacuumEnabled(int i)
	{
		toast_autovacuum_enabled = i;
	}
	wxString GetToastAutoVacuumVacuumThreshold()
	{
		return toast_autovacuum_vacuum_threshold;
	}
	void iSetToastAutoVacuumVacuumThreshold(const wxString &s)
	{
		toast_autovacuum_vacuum_threshold = s;
	}
	wxString GetToastAutoVacuumVacuumScaleFactor()
	{
		return toast_autovacuum_vacuum_scale_factor;
	}
	void iSetToastAutoVacuumVacuumScaleFactor(const wxString &s)
	{
		toast_autovacuum_vacuum_scale_factor = s;
	}
	wxString GetToastAutoVacuumVacuumCostDelay()
	{
		return toast_autovacuum_vacuum_cost_delay;
	}
	void iSetToastAutoVacuumVacuumCostDelay(const wxString &s)
	{
		toast_autovacuum_vacuum_cost_delay = s;
	}
	wxString GetToastAutoVacuumVacuumCostLimit()
	{
		return toast_autovacuum_vacuum_cost_limit;
	}
	void iSetToastAutoVacuumVacuumCostLimit(const wxString &s)
	{
		toast_autovacuum_vacuum_cost_limit = s;
	}
	wxString GetToastAutoVacuumFreezeMinAge()
	{
		return toast_autovacuum_freeze_min_age;
	}
	void iSetToastAutoVacuumFreezeMinAge(const wxString &s)
	{
		toast_autovacuum_freeze_min_age = s;
	}
	wxString GetToastAutoVacuumFreezeMaxAge()
	{
		return toast_autovacuum_freeze_max_age;
	}
	void iSetToastAutoVacuumFreezeMaxAge(const wxString &s)
	{
		toast_autovacuum_freeze_max_age = s;
	}
	wxString GetToastAutoVacuumFreezeTableAge()
	{
		return toast_autovacuum_freeze_table_age;
	}
	void iSetToastAutoVacuumFreezeTableAge(const wxString &s)
	{
		toast_autovacuum_freeze_table_age = s;
	}

	bool HasStats()
	{
		return true;
	}
	bool HasDepends()
	{
		return true;
	}
	bool HasReferences()
	{
		return true;
	}
	bool HasPgstattuple();

	virtual wxMenu *GetNewMenu();
	virtual wxString GetSql(ctlTree *browser);
	wxString GetSelectSql(ctlTree *browser);
	wxString GetInsertSql(ctlTree *browser);
	wxString GetUpdateSql(ctlTree *browser);
	wxString GetDeleteSql(ctlTree *browser);
	wxString GetHelpPage(bool forCreate) const;
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);
	void iSetTriggersEnabled(ctlTree *browser, bool enable);

private:
	void UpdateInheritance();
	bool GetVacuumHint();
	wxString GetCols(ctlTree *browser, size_t indent, wxString &QMs, bool withQM);
	void AppendStuff(wxString &sql, ctlTree *browser, pgaFactory &factory);
	void AppendStuffNoSql(wxString &sql, ctlTree *browser, pgaFactory &factory);

	void Init();

	wxULongLong rows;
	double estimatedRows;

	bool hasToastTable;
	/*
	* Three possible values:
	* 0 - Disabled
	* 1 - Enabled
	* 2 - GUC Setting
	*/
	int autovacuum_enabled, toast_autovacuum_enabled;
	wxString reloptions, toast_reloptions;

	wxString fillFactor, autovacuum_vacuum_threshold,
	         autovacuum_vacuum_scale_factor, autovacuum_analyze_threshold,
	         autovacuum_analyze_scale_factor, autovacuum_vacuum_cost_delay,
	         autovacuum_vacuum_cost_limit, autovacuum_freeze_min_age,
	         autovacuum_freeze_max_age, autovacuum_freeze_table_age;
	wxString appendOnly;
	wxString compressLevel;
	wxString orientation;  // "row" or "column"
	wxString compresstype;
	wxString blocksize;
	wxString checksum;
	wxString partitionDef;
	bool isPartitioned;
	bool hasOids, unlogged, hasSubclass, rowsCounted, isReplicated, showExtendedStatistics, distributionIsRandom;

	wxString toast_fillFactor, toast_autovacuum_vacuum_threshold,
	         toast_autovacuum_vacuum_scale_factor, toast_autovacuum_vacuum_cost_delay,
	         toast_autovacuum_vacuum_cost_limit, toast_autovacuum_freeze_min_age,
	         toast_autovacuum_freeze_max_age, toast_autovacuum_freeze_table_age;

	long inheritedTableCount, triggerCount;
	wxString quotedInheritedTables, inheritedTables, primaryKey, quotedPrimaryKey,
	         primaryKeyName, primaryKeyColNumbers, tablespace,
	         distributionColNumbers, ofType;
	wxArrayString quotedInheritedTablesList, inheritedTablesOidList;

	slSet *replicationSet;
	OID tablespaceOid;
	OID ofTypeOid;
};


class pgTableObject : public pgSchemaObject
{
public:
	pgTableObject(pgTable *newTable, pgaFactory &factory, const wxString &newName = wxT(""))
		: pgSchemaObject(newTable->GetSchema(), factory, newName)
	{
		table = newTable;
	}
	virtual pgTable *GetTable() const
	{
		return table;
	}
	OID GetTableOid() const
	{
		return table->GetOid();
	}
	wxString GetTableOidStr() const
	{
		return NumToStr(table->GetOid()) + wxT("::oid");
	}

protected:
	pgTable *table;
};


class pgTableCollection : public pgSchemaObjCollection
{
public:
	pgTableCollection(pgaFactory *factory, pgSchema *sch);
	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowStatistics(frmMain *form, ctlListView *statistics);
};

class pgTableObjCollection : public pgSchemaObjCollection
{
public:
	pgTableObjCollection(pgaFactory *factory, pgTable *_table)
		: pgSchemaObjCollection(factory, _table->GetSchema())
	{
		iSetOid(_table->GetOid());
		table = _table;
	}
	virtual pgTable *GetTable() const
	{
		return table;
	}
	bool CanCreate();

protected:
	pgTable *table;
};

class pgTableObjFactory : public pgSchemaObjFactory
{
public:
	pgTableObjFactory(const wxChar *tn, const wxChar *ns, const wxChar *nls, wxImage *img, wxImage *imgSm = 0)
		: pgSchemaObjFactory(tn, ns, nls, img, imgSm) {}
	virtual pgCollection *CreateCollection(pgObject *obj);
};

class countRowsFactory : public contextActionFactory
{
public:
	countRowsFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};


class executePgstattupleFactory : public contextActionFactory
{
public:
	executePgstattupleFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
	bool CheckChecked(pgObject *obj);
};

class disableAllTriggersFactory : public contextActionFactory
{
public:
	disableAllTriggersFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};

class enableAllTriggersFactory : public contextActionFactory
{
public:
	enableAllTriggersFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};

class truncateFactory : public contextActionFactory
{
public:
	truncateFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};


class truncateCascadedFactory : public contextActionFactory
{
public:
	truncateCascadedFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};


class resetTableStatsFactory : public contextActionFactory
{
public:
	resetTableStatsFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};


#endif
