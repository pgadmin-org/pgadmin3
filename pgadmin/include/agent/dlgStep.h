//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgStep.h - Job property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_STEPPROP
#define __DLG_STEPPROP

#include "dlg/dlgProperty.h"

class pgaStep;
class pgaJob;

class dlgStep : public dlgAgentProperty
{
public:
	dlgStep(pgaFactory *factory, frmMain *frame, pgaStep *s, pgaJob *j);

	void CheckChange();
	int Go(bool modal);

	wxString GetUpdateSql();
	wxString GetInsertSql();
	wxString GetComment();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();

	void SetObject(pgObject *obj)
	{
		step = (pgaStep *)obj;
	}

	void SetJobId(long id)
	{
		jobId = id;
	}

	wxString GetHelpPage(bool forCreate) const
	{
		return wxT("pgagent-steps");
	}

private:
	void OnSelRemoteConn(wxCommandEvent &ev);
	void OnSelLocalConn(wxCommandEvent &ev);
	void OnSelectDatabase(wxCommandEvent &ev);

	virtual bool IsUpToDate();

	long jobId;
	ctlSQLBox *sqlBox;
	pgaStep *step;
	pgaJob *job;
	bool hasConnStrSupport;

	DECLARE_EVENT_TABLE()
};


#endif
