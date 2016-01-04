//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgSchedule.h - Job property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_SCHEDULEPROP
#define __DLG_SCHEDULEPROP

#include "dlg/dlgProperty.h"
#include "ctl/timespin.h"
#include "ctl/calbox.h"

class pgaSchedule;
class pgaJob;

class dlgSchedule : public dlgAgentProperty
{
public:
	dlgSchedule(pgaFactory *factory, frmMain *frame, pgaSchedule *s, pgaJob *j);

	void CheckChange();
	int Go(bool modal);

	wxString GetComment();
	wxString GetUpdateSql();
	wxString GetInsertSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();
	void SetJobId(long id)
	{
		jobId = id;
	}

	wxString GetHelpPage(bool forCreate) const
	{
		return wxT("pgagent-schedules");
	}

private:
	long jobId;
	pgaSchedule *schedule;
	pgaJob *job;

	wxArrayString deleteExceptions;

#ifdef __WXMAC__
	void OnChangeSize(wxSizeEvent &ev);
#endif

	void OnChangeCom(wxCommandEvent &ev);
	void OnChangeCal(wxCalendarEvent &ev);
	void OnSelChangeException(wxListEvent &ev);
	void OnAddException(wxCommandEvent &ev);
	void OnChangeException(wxCommandEvent &ev);
	void OnRemoveException(wxCommandEvent &ev);
	const wxString ChkListBox2PgArray(wxCheckListBox *lb);
	const wxString ChkListBox2StrArray(wxCheckListBox *lb);
	void OnSelectAll(wxCommandEvent &ev, int origin);
	void OnSelectAllWeekdays(wxCommandEvent &ev)
	{
		OnSelectAll(ev, 1);
	};
	void OnSelectAllMonthdays(wxCommandEvent &ev)
	{
		OnSelectAll(ev, 2);
	};
	void OnSelectAllMonths(wxCommandEvent &ev)
	{
		OnSelectAll(ev, 3);
	};
	void OnSelectAllHours(wxCommandEvent &ev)
	{
		OnSelectAll(ev, 4);
	};
	void OnSelectAllMinutes(wxCommandEvent &ev)
	{
		OnSelectAll(ev, 5);
	};
	void InitSelectAll();

	DECLARE_EVENT_TABLE()
};


#endif
