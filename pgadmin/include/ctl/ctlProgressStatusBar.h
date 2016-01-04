//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ctlProgressStatusBar.h - Status bar indicating the current progress
//
//////////////////////////////////////////////////////////////////////////
//
#ifndef CTLPROGRESS_STATUSBAR_H
#define CTLPROGRESS_STATUSBAR_H

// wxWindows headers
#include <wx/wx.h>
#include <wx/gauge.h>
#include <wx/timer.h>

class ctlProgressStatusBar : public wxStatusBar
{
public:
	ctlProgressStatusBar(wxWindow *parent, bool showProgressInitially = true, bool autoProgressive = true, int max = -1);
	virtual ~ctlProgressStatusBar();

	void ShowProgress(bool restart = true);
	void StopProgress();
	void SetProgress(int val);
	virtual void SetFieldsCount(int number = 1, const int *widths = NULL);
	virtual void SetStatusWidths(int n, const int widths_field[]);

	static const unsigned short ms_increment,
	       ms_progressbar_width,
	       ms_progressstatus_width;

protected:
	void OnTimer(wxTimerEvent &WXUNUSED(event));
	void OnSize(wxSizeEvent &ev);

	wxGauge *m_progress;
	wxTimer  m_timer;

	bool     m_progressStopped;
	bool     m_autoProgressive;
	bool     m_autoValIncrementing;

	int      m_hr, m_min, m_sec, m_mil;
	int      m_val;

	enum
	{
		Status_field,
		ProgressBar_field,
		ProgressStatus_field,
		Max_Field
	};

	DECLARE_EVENT_TABLE()
};

#endif // CTLPROGRESS_STATUSBAR_H
