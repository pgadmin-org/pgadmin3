//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ctlMessageWindow.cpp - debugger
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "debugger/ctlMessageWindow.h"

IMPLEMENT_CLASS(ctlMessageWindow, wxTextCtrl)

BEGIN_EVENT_TABLE(ctlMessageWindow, wxTextCtrl)
	EVT_TIMER(wxID_ANY,       ctlMessageWindow::OnTimer)
END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////////////////
// ctlMessageWindow constructor
//
//  Initialize the grid control and clear it out....
//

ctlMessageWindow::ctlMessageWindow(wxWindow *parent, wxWindowID id)
	: wxTextCtrl(parent, wxID_ANY, wxT(""), wxPoint(0, 0), wxSize(0, 0),
	             wxTE_MULTILINE | wxTE_READONLY)
{
	SetFont(settings->GetSQLFont());
	m_timer.SetOwner(this);
}

////////////////////////////////////////////////////////////////////////////////
// AddMessage()
//
//    Adds the message in the 'DBMS Messages' window.
//

void ctlMessageWindow::AddMessage(wxString message)
{
	m_mutex.Lock();
	m_currMsg += message + wxT("\n");
	m_mutex.Unlock();

	if (!m_timer.IsRunning())
		m_timer.Start(100, true);
}

void ctlMessageWindow::OnTimer(wxTimerEvent &ev)
{
	m_mutex.Lock();
	AppendText(m_currMsg);
	m_currMsg = wxEmptyString;
	m_mutex.Unlock();
}

////////////////////////////////////////////////////////////////////////////////
// delMessage()
//
//    Removes the given message from the 'DBMS Messages' window.
//

void ctlMessageWindow::DelMessage(const char *name)
{
	SetValue(wxT(""));
}


wxString ctlMessageWindow::GetMessage(int row)
{
	return(GetValue());
}
