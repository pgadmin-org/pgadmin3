//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgRule.h PostgreSQL Rule
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGRULE_H
#define PGRULE_H


#include "pgSchema.h"

class pgRuleObject : public pgSchemaObject
{
public:
	pgRuleObject(pgSchema *newSchema, pgaFactory &factory, const wxString &newName = wxEmptyString) : pgSchemaObject(newSchema, factory, newName) {}

	wxString GetFormattedDefinition();
	wxString GetDefinition() const
	{
		return definition;
	}
	void iSetDefinition(const wxString &s)
	{
		definition = s;
	}

protected:
	wxString definition;
};



//////////////////////////////////////////////////////////7


class pgRuleFactory : public pgSchemaObjFactory
{
public:
	pgRuleFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);

	int GetClosedIconId()
	{
		return closedId;
	}

protected:
	int closedId;
};
extern pgRuleFactory ruleFactory;


class pgRule : public pgRuleObject
{
public:
	pgRule(pgSchema *newSchema, const wxString &newName = wxT(""));
	~pgRule();

	int GetIconId();

	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	bool CanDropCascaded()
	{
		return GetSchema()->GetMetaType() != PGM_CATALOG;
	}

	wxString GetEvent() const
	{
		return event;
	}
	void iSetEvent(const wxString &s)
	{
		event = s;
	}
	wxString GetCondition() const
	{
		return condition;
	}
	void iSetCondition(const wxString &s)
	{
		condition = s;
	}
	wxString GetAction() const
	{
		return action;
	}
	void iSetAction(const wxString &s)
	{
		action = s;
	}
	bool GetDoInstead() const
	{
		return doInstead;
	}
	void iSetDoInstead(const bool b)
	{
		doInstead = b;
	}
	bool GetEnabled() const
	{
		return enabled;
	}
	void SetEnabled(ctlTree *browser, const bool b);
	void iSetEnabled(const bool b)
	{
		enabled = b;
	}
	wxString GetQuotedFullTable() const
	{
		return quotedFullTable;
	}
	void iSetQuotedFullTable(const wxString &s)
	{
		quotedFullTable = s;
	}
	void iSetParentIsTable(const bool b)
	{
		parentistable = b;
	}
	bool GetParentIsTable()
	{
		return parentistable;
	}

	bool GetSystemObject() const
	{
		return GetName() == wxT("_RETURN");
	}
	bool CanDrop()
	{
		return !GetSystemObject() && GetSchema()->CanDrop();
	}
	bool CanCreate()
	{
		return GetSchema()->CanCreate();
	}
	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
	wxString GetSql(ctlTree *browser);
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

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
private:
	wxString event, condition, action, quotedFullTable;
	bool doInstead, enabled, parentistable;
};

class pgRuleCollection : public pgSchemaObjCollection
{
public:
	pgRuleCollection(pgaFactory *factory, pgSchema *sch);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

class enabledisableRuleFactory : public contextActionFactory
{
public:
	enabledisableRuleFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
	bool CheckChecked(pgObject *obj);
};

#endif
