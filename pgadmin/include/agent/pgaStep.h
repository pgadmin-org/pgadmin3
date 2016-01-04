//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgaStep.h - PostgreSQL Agent Job Step
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGASTEP_H
#define PGASTEP_H

#include "agent/pgaJob.h"



class pgaStepFactory : public pgaJobObjFactory
{
public:
	pgaStepFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);
};
extern pgaStepFactory stepFactory;


class pgaStep : public pgaJobObject
{
public:
	pgaStep(pgCollection *collection, const wxString &newName = wxT(""));

	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	void ShowStatistics(frmMain *form, ctlListView *statistics);
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);
	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);

	bool GetEnabled() const
	{
		return enabled;
	}
	void iSetEnabled(const bool b)
	{
		enabled = b;
	}
	wxChar GetKindChar() const
	{
		return kindChar;
	}
	void iSetKindChar(const wxChar c)
	{
		kindChar = c;
	}
	wxString GetKind() const
	{
		return kind;
	}
	void iSetKind(const wxString &s)
	{
		kind = s;
	}
	wxString GetCode() const
	{
		return code;
	}
	void iSetCode(const wxString &s)
	{
		code = s;
	}
	wxString GetDbname() const
	{
		return dbname;
	}
	void iSetDbname(const wxString &s)
	{
		dbname = s;
	}
	wxString GetConnStr() const
	{
		return connstr;
	}
	void iSetConnStr(const wxString &s)
	{
		connstr = s;
	}
	wxString GetOnError() const
	{
		return onError;
	}
	void iSetOnError(const wxString &s)
	{
		onError = s;
	}
	wxChar GetOnErrorChar() const
	{
		return onErrorChar;
	}
	void iSetOnErrorChar(const wxChar c)
	{
		onErrorChar = c;
	}
	long GetRecId() const
	{
		return recId;
	}
	void iSetRecId(const long l)
	{
		recId = l;
	}

	bool HasConnectionString() const
	{
		return !connstr.IsEmpty();
	}

	bool IsUpToDate();

	wxString GetHelpPage(bool forCreate) const
	{
		return wxT("pgagent-steps");
	}

private:
	bool enabled;
	wxString kind, code, dbname, connstr, onError;
	wxChar kindChar, onErrorChar;
	long recId;
};

class pgaStepCollection : public pgaJobObjCollection
{
public:
	pgaStepCollection(pgaFactory *factory, pgaJob *job);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

#endif
