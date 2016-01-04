//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgaJob.h - PostgreSQL Agent Job
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGAJOB_H
#define PGAJOB_H

#include "schema/pgServer.h"


class pgaJobFactory : public pgServerObjFactory
{
public:
	pgaJobFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);
	int GetDisabledId()
	{
		return disabledId;
	}

protected:
	int disabledId;
};
extern pgaJobFactory jobFactory;

class pgaJob : public pgServerObject
{
public:
	pgaJob(const wxString &newName = wxT(""));

	int GetIconId();
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	void ShowStatistics(frmMain *form, ctlListView *statistics);
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);
	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);

	wxString GetTranslatedMessage(int kindOfMessage) const;
	wxString GetJobclass() const
	{
		return jobclass;
	}
	void iSetJobclass(const wxString &s)
	{
		jobclass = s;
	}
	bool GetEnabled() const
	{
		return enabled;
	}
	void iSetEnabled(const bool b)
	{
		enabled = b;
	}
	wxDateTime GetCreated() const
	{
		return created;
	}
	void iSetCreated(const wxDateTime &d)
	{
		created = d;
	}
	wxDateTime GetChanged() const
	{
		return changed;
	}
	void iSetChanged(const wxDateTime &d)
	{
		changed = d;
	}
	wxDateTime GetNextrun() const
	{
		return nextrun;
	}
	void iSetNextrun(const wxDateTime &d)
	{
		nextrun = d;
	}
	wxDateTime GetLastrun() const
	{
		return lastrun;
	}
	void iSetLastrun(const wxDateTime &d)
	{
		lastrun = d;
	}
	wxString GetLastresult() const
	{
		return lastresult;
	}
	void iSetLastresult(const wxString &s)
	{
		lastresult = s;
	}
	wxString GetCurrentAgent() const
	{
		return currentAgent;
	}
	void iSetCurrentAgent(const wxString &s)
	{
		currentAgent = s;
	}
	wxString GetHostAgent() const
	{
		return hostAgent;
	}
	void iSetHostAgent(const wxString &s)
	{
		hostAgent = s;
	}
	long GetRecId() const
	{
		return recId;
	}
	void iSetRecId(const long l)
	{
		recId = l;
	}
	bool RunNow();

	wxMenu *GetNewMenu();
	bool CanCreate()
	{
		return true;
	}
	bool CanView()
	{
		return false;
	}
	bool CanEdit()
	{
		return true;
	}
	bool CanDrop()
	{
		return true;
	}
	bool WantDummyChild()
	{
		return true;
	}

	wxString GetHelpPage(bool forCreate) const
	{
		return wxT("pgagent-jobs");
	}

private:
	bool enabled;
	wxDateTime created, changed, nextrun, lastrun;
	wxString lastresult, jobclass, currentAgent, hostAgent;
	long recId;
};


class pgaJobObject : public pgServerObject
{
public:
	pgaJobObject(pgaJob *job, pgaFactory &factory, const wxString &newName);
	virtual pgaJob *GetJob()
	{
		return job;
	}

	bool CanCreate()
	{
		return job->CanCreate();
	}
	bool CanView()
	{
		return false;
	}
	bool CanEdit()
	{
		return job->CanEdit();
	}
	bool CanDrop()
	{
		return job->CanDrop();
	}

protected:
	pgaJob *job;
};


class pgaJobCollection : public pgServerObjCollection
{
public:
	pgaJobCollection(pgaFactory *factory, pgServer *sv);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};


class pgaJobObjCollection : public pgServerObjCollection
{
public:
	pgaJobObjCollection(pgaFactory *factory, pgaJob *job);
	bool CanCreate();
};

class pgaJobObjFactory : public pgServerObjFactory
{
public:
	pgaJobObjFactory(const wxChar *tn, const wxChar *ns, const wxChar *nls, wxImage *img, wxImage *imgSm = 0)
		: pgServerObjFactory(tn, ns, nls, img, imgSm) {}
	virtual pgCollection *CreateCollection(pgObject *obj);
};

class runNowFactory : public contextActionFactory
{
public:
	runNowFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};

#endif
