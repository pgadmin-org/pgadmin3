//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgEventTrigger.h PostgreSQL Event Trigger
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGEVENTTRIGGER_H
#define PGEVENTTRIGGER_H

#include "pgDatabase.h"

class pgCollection;
class pgFunction;

class pgEventTriggerFactory : public pgDatabaseObjFactory
{
public:
	pgEventTriggerFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);
	virtual ~pgEventTriggerFactory() {};

	int GetClosedIconId()
	{
		return closedId;
	}

protected:
	int closedId;
};

extern pgEventTriggerFactory eventTriggerFactory;

class pgEventTrigger : public pgDatabaseObject
{
public:
	pgEventTrigger(const wxString &newName = wxT(""));
	~pgEventTrigger();
	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
	bool IsUpToDate();
	wxString GetSql(ctlTree *browser);
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

	int GetIconId();

	wxString GetFunction() const
	{
		return function;
	}
	void iSetFunction(const wxString &s)
	{
		function = s ;
	}
	wxString GetWhen() const
	{
		return when;
	}
	void iSetWhen(const wxString &s)
	{
		when = s;
	}
	OID GetFunctionOid() const
	{
		return functionOid;
	}
	void iSetFunctionOid(const OID d)
	{
		functionOid = d;
	}
	OID GetSchemaOid() const
	{
		return schemaOid;
	}
	void iSetSchemaOid(const OID d)
	{
		schemaOid = d;
	}
	wxString GetSource() const
	{
		return source;
	}
	void iSetSource(const wxString &s)
	{
		source = s;
	}
	wxString GetLanguage() const
	{
		return language;
	}
	void iSetLanguage(const wxString &s)
	{
		language = s;
	}
	wxString GetEventName() const
	{
		return eventName;
	}
	wxString GetEnableStatus() const
	{
		return enableStatus;
	}
	void iSetEnableStatus(const wxString &s)
	{
		enableStatus = s;
	}
	void iSetEventName(const wxString &s)
	{
		eventName = s;
	}

	bool GetEnabled() const
	{
		return enabled;
	}
	void iSetEnabled(const bool b)
	{
		enabled = b;
	}
	void SetEnabled(ctlTree *browser, const bool b);
	void SetDirty();

private:
	wxString function, when, language, source, eventName, enableStatus;
	OID functionOid, schemaOid;
	bool enabled;
	pgFunction *eventTriggerFunction;
	pgSchema *eventTriggerFunctionSchema;
};

class pgEventTriggerCollection : public pgDatabaseObjCollection
{
public:
	pgEventTriggerCollection(pgaFactory *factory, pgDatabase *db);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

class enabledisableEventTriggerFactory : public contextActionFactory
{
public:
	enabledisableEventTriggerFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
	bool CheckChecked(pgObject *obj);
};

#endif