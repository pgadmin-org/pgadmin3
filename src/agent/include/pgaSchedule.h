//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgaSchedule.h - PostgreSQL Agent Job Schedule
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGASCHEDULE_H
#define PGASCHEDULE_H

#include "pgaJob.h"


class pgaScheduleFactory : public pgServerObjFactory
{
public:
    pgaScheduleFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern pgaScheduleFactory scheduleFactory;


class pgaSchedule : public pgaJobObject
{
public:
    pgaSchedule(pgCollection *collection, const wxString& newName = wxT(""));
    ~pgaSchedule();

    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);
    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);

    bool GetEnabled() const { return enabled; }
    void iSetEnabled(const bool b) { enabled=b; }
    wxDateTime GetStart() const { return start; }
    void iSetStart(const wxDateTime &d) { start=d; }
    wxDateTime GetEnd() const { return end; }
    void iSetEnd(const wxDateTime &d) { end=d; }
    long GetRecId() const { return recId; }
    void iSetRecId(const long l) { recId=l; }

	wxString GetMinutes() const { return minutes; }
	wxString GetMinutesString();
	void iSetMinutes(const wxString &s) { minutes = s; }

	wxString GetHours() const { return hours; }
	wxString GetHoursString();
	void iSetHours(const wxString &s) { hours = s; }

	wxString GetWeekdays() const { return weekdays; }
	wxString GetWeekdaysString();
	void iSetWeekdays(const wxString &s) { weekdays = s; }

	wxString GetMonthdays() const { return monthdays; }
	wxString GetMonthdaysString();
	void iSetMonthdays(const wxString &s) { monthdays = s; }

	wxString GetMonths() const { return months; }
	wxString GetMonthsString();
	void iSetMonths(const wxString &s) { months = s; }

	wxString GetExceptions() const { return exceptions; }
	wxString GetExceptionsString();
	void iSetExceptions(const wxString &s) { exceptions = s; }

    wxString GetHelpPage(bool forCreate) const { return wxT("pgagent-schedules"); }

private:
    bool enabled;
    wxDateTime start, end;
	long recId;
	wxString minutes, hours, weekdays, monthdays, months, exceptions;
};

#endif
