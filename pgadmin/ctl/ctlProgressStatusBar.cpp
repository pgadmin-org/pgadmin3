//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ctlProgressStatusBar.cpp - A status bar with progress indicator
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/statusbr.h>
#include <wx/gauge.h>

// App headers
#include "ctl/ctlProgressStatusBar.h"

BEGIN_EVENT_TABLE(ctlProgressStatusBar, wxStatusBar)
	EVT_SIZE(ctlProgressStatusBar::OnSize)
	EVT_TIMER(wxID_ANY, ctlProgressStatusBar::OnTimer)
END_EVENT_TABLE()

const unsigned short ctlProgressStatusBar::ms_increment = 100;
const unsigned short ctlProgressStatusBar::ms_progressbar_width = 70;
const unsigned short ctlProgressStatusBar::ms_progressstatus_width = 130;

ctlProgressStatusBar::ctlProgressStatusBar(wxWindow *parent, bool showProgressInitially, bool autoProgressive, int max)
	: wxStatusBar(parent, wxID_ANY), m_progressStopped(!showProgressInitially),
	  m_autoProgressive(autoProgressive), m_autoValIncrementing(true),
	  m_hr(0), m_min(0), m_sec(0), m_mil(0), m_val(0)
{
	static int widths[] = { -1, ms_progressbar_width, ms_progressstatus_width};
	int fields = 0;

	if (max <= 0)
		max = 100;

	if (m_autoProgressive)
		fields = 3;
	else
		fields = 2;

	m_progress = new wxGauge(this, -1, max);

	wxStatusBar::SetFieldsCount(fields);
	wxStatusBar::SetStatusWidths(fields, widths);

	m_timer.SetOwner(this->GetEventHandler());

	if (!showProgressInitially)
	{
		m_progressStopped = true;
	}
	else
	{
		m_progressStopped = false;
		if (m_autoProgressive)
		{
			m_timer.Start(ms_increment);

			m_hr = 0;
			m_min = 0;
			m_sec = 0;
			m_mil = 0;
		}
	}
	// Dummy event to place the progressbar at right place
	wxSizeEvent ev;
	this->OnSize(ev);
}

ctlProgressStatusBar::~ctlProgressStatusBar()
{
	if (m_timer.IsRunning())
	{
		m_timer.Stop();
	}
}


void ctlProgressStatusBar::OnSize(wxSizeEvent &ev)
{
	// We should have hide the progress bar
	if (GetFieldsCount() <= ProgressBar_field)
		return;

	wxRect r;
	GetFieldRect(ProgressBar_field, r);

	m_progress->SetSize(r.x + 1, r.y + 1, r.width - 2, r.height - 2);

	ev.Skip();
}


void ctlProgressStatusBar::OnTimer(wxTimerEvent &ev)
{
	if (m_progressStopped || !m_autoProgressive)
	{
		m_timer.Stop();
		return;
	}

	m_val = m_progress->GetValue();
	int max = m_progress->GetRange();

	if (m_val == max)
	{
		m_val -= 1;
		m_autoValIncrementing = false;
	}
	else if (m_val == 0)
	{
		m_val = 1;
		m_autoValIncrementing = true;
	}
	else if (m_autoValIncrementing)
	{
		m_val += 1;
	}
	else
	{
		m_val -= 1;
	}

	m_progress->SetValue(m_val);

	m_mil += ms_increment;

	if (m_mil >= 1000)
	{
		m_mil = m_mil - 1000;
		m_sec += 1;

		if (m_sec == 60)
		{
			m_sec = 0;
			m_min += 1;

			if (m_min == 60)
			{
				m_min = 0;
				m_hr += 1;
			}
		}
	}

	wxStatusBar::SetStatusText(wxString::Format(_("%d:%02d:%02d.%03d"), m_hr, m_min, m_sec, m_mil), ProgressStatus_field);
}


void ctlProgressStatusBar::ShowProgress(bool restart)
{
	if (m_progressStopped)
	{
		m_progressStopped = false;

		if (!m_timer.IsRunning())
		{
			if (restart)
			{
				m_val = 0;
				m_hr = m_min = m_sec = m_mil = 0;
			}

			m_progress->SetValue(m_val);
			m_timer.Start(ms_increment);
		}
	}
}

void ctlProgressStatusBar::StopProgress()
{
	if (m_timer.IsRunning())
		m_timer.Stop();

	m_progress->SetValue(0);
	m_progressStopped = true;
}

void ctlProgressStatusBar::SetFieldsCount(int number, const int *widths)
{
	m_progress->Show(number > ProgressBar_field);
	wxStatusBar::SetFieldsCount( number, widths);

	// Dummy Size event (to reposition the progress bar)
	wxSizeEvent ev;
	this->OnSize(ev);
}

void ctlProgressStatusBar::SetStatusWidths(int n, const int widths_field[])
{
	wxStatusBar::SetStatusWidths( n, widths_field);

	// Dummy Size event (to reposition the progress bar)
	wxSizeEvent ev;
	this->OnSize(ev);
}