//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgJob.h - Job property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_JOBPROP
#define __DLG_JOBPROP

#include "dlg/dlgProperty.h"

class pgaJob;

class dlgJob : public dlgAgentProperty
{
public:
	dlgJob(pgaFactory *factory, frmMain *frame, pgaJob *j);

	void CheckChange();
	int Go(bool modal);

	wxString GetUpdateSql();
	wxString GetInsertSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();

	wxString GetHelpPage(bool forCreate) const
	{
		return wxT("pgagent-jobs");
	}

private:
	pgaJob *job;

	wxArrayString previousSteps, previousSchedules;

#ifdef __WXMAC__
	void OnChangeSize(wxSizeEvent &ev);
#endif

	void OnSelChangeStep(wxListEvent &ev);
	void OnChangeStep(wxCommandEvent &ev);
	void OnAddStep(wxCommandEvent &ev);
	void OnRemoveStep(wxCommandEvent &ev);

	void OnSelChangeSchedule(wxListEvent &ev);
	void OnChangeSchedule(wxCommandEvent &ev);
	void OnAddSchedule(wxCommandEvent &ev);
	void OnRemoveSchedule(wxCommandEvent &ev);

	DECLARE_EVENT_TABLE()
};


#endif
