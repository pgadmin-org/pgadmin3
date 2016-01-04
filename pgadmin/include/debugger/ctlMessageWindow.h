//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ctlMessageWindow.h - debugger
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//	class ctlMessageWindow
//
//	This class implements the window that displays DBMS messages at the
//  bottom of the debugger window.  When we create a ctlMessageWindow, the parent
//	is a ctlTabWindow (the ctlMessageWindow becomes a tab in a tab control).
//
////////////////////////////////////////////////////////////////////////////////

#ifndef CTLMESSAGEWINDOW_H
#define CTLMESSAGEWINDOW_H

class ctlMessageWindow : public wxTextCtrl
{
	DECLARE_CLASS(ctlMessageWindow)
	DECLARE_EVENT_TABLE()

public:
	ctlMessageWindow(wxWindow *parent, wxWindowID id);

	void     AddMessage(wxString message);	// Add a message to the window
	void     DelMessage(const char *name = NULL);								     // Remove a message from the window
	wxString GetMessage(int row);

protected:
	wxString m_currMsg;
	wxTimer  m_timer;
	wxMutex  m_mutex;

	void OnTimer(wxTimerEvent &);
};

#endif
