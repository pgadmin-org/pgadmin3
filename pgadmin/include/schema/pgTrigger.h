//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgTrigger.h PostgreSQL Trigger
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGTRIGGER_H
#define PGTRIGGER_H

#include "pgTable.h"

class pgFunction;

class pgTriggerObject : public pgSchemaObject
{
public:
	pgTriggerObject(pgSchema *newSchema, pgaFactory &factory, const wxString &newName = wxEmptyString) : pgSchemaObject(newSchema, factory, newName) {}

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



class pgTriggerFactory : public pgSchemaObjFactory
{
public:
	pgTriggerFactory();
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
extern pgTriggerFactory triggerFactory;


class pgTrigger : public pgTriggerObject
{
public:
	pgTrigger(pgSchema *newSchema, const wxString &newName = wxT(""));
	~pgTrigger();

	int GetIconId();

	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	bool CanDropCascaded()
	{
		return !GetSystemObject() && pgSchemaObject::CanDrop();
	}

	wxString GetFireWhen() const;
	wxString GetEvent() const;
	wxString GetForEach() const;
	wxString GetFunction() const
	{
		return function;
	}
	void iSetFunction(const wxString &s)
	{
		function = s;
	}
	void iSetArguments(const wxString &s)
	{
		arguments = s;
	}
	wxString GetArguments() const
	{
		return arguments;
	}
	void iSetWhen(const wxString &s)
	{
		when = s;
	}
	wxString GetWhen() const
	{
		return when;
	}
	bool GetIsConstraint() const
	{
		return isconstraint;
	}
	void SetIsConstraint(const bool b)
	{
		isconstraint = b;
	}
	bool GetDeferrable() const
	{
		return deferrable;
	}
	void iSetDeferrable(const bool b)
	{
		deferrable = b;
	}
	bool GetDeferred() const
	{
		return deferred;
	}
	void iSetDeferred(const bool b)
	{
		deferred = b;
	}
	wxString GetLanguage() const
	{
		return language;
	}
	void iSetLanguage(const wxString &s)
	{
		language = s;
	}
	wxString GetSource() const
	{
		return source;
	}
	void iSetSource(const wxString &s)
	{
		source = s;
	}
	long GetTriggerType() const
	{
		return triggerType;
	}
	void iSetTriggerType(const long l)
	{
		triggerType = l;
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
	void iSetTriggerFunction(pgFunction *fkt)
	{
		triggerFunction = fkt;
	}
	wxString GetQuotedFullTable() const
	{
		return quotedFullTable;
	}
	void iSetQuotedFullTable(const wxString &s)
	{
		quotedFullTable = s;
	}
	OID GetFunctionOid() const
	{
		return functionOid;
	}
	void iSetFunctionOid(const OID d)
	{
		functionOid = d;
	}
	OID GetRelationOid() const
	{
		return relationOid;
	}
	void iSetRelationOid(const OID d)
	{
		relationOid = d;
	}
	wxString GetQuotedColumns() const
	{
		return quotedColumns;
	}
	wxString GetColumns() const
	{
		return columns;
	}
	wxArrayString GetColumnList() const
	{
		return columnList;
	}
	long GetColumnCount() const
	{
		return columnCount;
	}
	void iSetColumnCount(const long l)
	{
		columnCount = l;
	}
	void iSetParentIsTable(const bool b)
	{
		parentistable = b;
	}
	bool GetParentIsTable()
	{
		return parentistable;
	}

	void SetDirty();

	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
	wxString GetSql(ctlTree *browser);
	bool CanRestore()
	{
		return true;
	}
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

	bool IsUpToDate();

protected:
	void ReadColumnDetails();

private:
	wxString function, quotedFullTable, arguments, when, language, source, columns, quotedColumns;
	wxArrayString columnList;
	long columnCount;
	OID functionOid, relationOid;
	long triggerType;
	bool enabled, parentistable, isconstraint, deferrable, deferred;
	pgFunction *triggerFunction;
};


class pgTriggerCollection : public pgSchemaObjCollection
{
public:
	pgTriggerCollection(pgaFactory *factory, pgSchema *sch);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};


class enabledisableTriggerFactory : public contextActionFactory
{
public:
	enabledisableTriggerFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
	bool CheckChecked(pgObject *obj);
};

#endif
