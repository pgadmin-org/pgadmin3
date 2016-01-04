//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgView.h PostgreSQL View
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGVIEW_H
#define PGVIEW_H

#include "pgRule.h"

class pgCollection;

class pgViewFactory : public pgSchemaObjFactory
{
public:
	pgViewFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);
	int GetMaterializedIconId()
	{
		return WantSmallIcon() ? smallMaterializedId : materializedId;
	}

protected:
	int materializedId, smallMaterializedId;

};
extern pgViewFactory viewFactory;


class pgView : public pgRuleObject
{
public:
	pgView(pgSchema *newSchema, const wxString &newName = wxT(""));
	~pgView();

	int GetIconId();

	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	bool CanDropCascaded()
	{
		return !GetSystemObject() && pgSchemaObject::CanDrop();
	}

	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
	bool CanCreate()
	{
		return GetSchema()->CanCreate();
	}
	bool CanView()
	{
		return true;
	}
	bool WantDummyChild()
	{
		return true;
	}

	bool HasInsertRule()
	{
		return hasInsertRule;
	}
	bool HasUpdateRule()
	{
		return hasUpdateRule;
	}
	bool HasDeleteRule()
	{
		return hasDeleteRule;
	}
	wxString GetSecurityBarrier()
	{
		return security_barrier;
	}
	void iSetSecurityBarrier(const wxString &s)
	{
		security_barrier = s;
	}

	wxString GetTablespace() const
	{
		return tablespace;
	}
	void iSetTablespace(const wxString &newVal)
	{
		tablespace = newVal;
	}
	OID GetTablespaceOid() const
	{
		return tablespaceOid;
	}
	void iSetTablespaceOid(const OID newVal)
	{
		tablespaceOid = newVal;
	}

	wxMenu *GetNewMenu();
	wxString GetSql(ctlTree *browser);
	wxString GetSelectSql(ctlTree *browser);
	wxString GetInsertSql(ctlTree *browser);
	wxString GetUpdateSql(ctlTree *browser);
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);
	void RefreshMatView(bool concurrently);

	bool HasStats()
	{
		return false;
	}
	bool HasDepends()
	{
		return true;
	}
	bool HasReferences()
	{
		return true;
	}

	void ShowHint(frmMain *form, bool force);
	bool GetCanHint()
	{
		return true;
	};

	bool IsUpToDate();

	wxString GetFillFactor()
	{
		return fillFactor;
	}
	void iSetFillFactor(const wxString &s)
	{
		fillFactor = s;
	}

	wxString GetIsPopulated()
	{
		return isPopulated;
	}
	void iSetIsPopulated(const wxString &s)
	{
		isPopulated = s;
	}

	wxString GetCheckOption()
	{
		return check_option;
	}
	void iSetCheckOption(const wxString &s)
	{
		check_option = s;
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

	void iSetMaterializedView(const bool matView)
	{
		materializedView = matView;
	}

	bool GetMaterializedView() const
	{
		return materializedView;
	}

private:
	wxString GetCols(ctlTree *browser, size_t indent, wxString &QMs, bool withQM);
	void AppendStuff(wxString &sql, ctlTree *browser, pgaFactory &factory);
	bool IsMaterializedView(ctlTree *browser);
	bool hasInsertRule, hasUpdateRule, hasDeleteRule, materializedView;
	wxString security_barrier;

	int autovacuum_enabled, toast_autovacuum_enabled;
	wxString reloptions, toast_reloptions;

	wxString fillFactor, autovacuum_vacuum_threshold,
	         autovacuum_vacuum_scale_factor, autovacuum_analyze_threshold,
	         autovacuum_analyze_scale_factor, autovacuum_vacuum_cost_delay,
	         autovacuum_vacuum_cost_limit, autovacuum_freeze_min_age,
	         autovacuum_freeze_max_age, autovacuum_freeze_table_age;

	wxString toast_fillFactor, toast_autovacuum_vacuum_threshold,
	         toast_autovacuum_vacuum_scale_factor, toast_autovacuum_vacuum_cost_delay,
	         toast_autovacuum_vacuum_cost_limit, toast_autovacuum_freeze_min_age,
	         toast_autovacuum_freeze_max_age, toast_autovacuum_freeze_table_age;

	wxString tablespace, isPopulated, check_option;
	bool hasToastTable;
	OID tablespaceOid;

};

class pgViewCollection : public pgSchemaObjCollection
{
public:
	pgViewCollection(pgaFactory *factory, pgSchema *sch);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

class refreshMatViewFactory : public contextActionFactory
{
public:
	refreshMatViewFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};

class refreshConcurrentlyMatViewFactory : public contextActionFactory
{
public:
	refreshConcurrentlyMatViewFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};


#endif
