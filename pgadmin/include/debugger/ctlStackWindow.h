//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ctlStackWindow.h - debugger
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//	class ctlStackWindow
//
//	This class implements the window that displays the current call stack at
//  bottom of the debugger window.  When we create a ctlStackWindow, the parent
//	is a ctlTabWindow (the ctlStackWindow becomes a tab in a tab control).
//
//	It is a simple grid control - the grid contains two columns:
//		the RowLabel column displays the stack level
//		column 0 displays the function name, line number and argument list
//
////////////////////////////////////////////////////////////////////////////////

#ifndef CTLSTACKWINDOW_H
#define CTLSTACKWINDOW_H

#include <wx/grid.h>
#include <wx/laywin.h>
#include <wx/listbox.h>
#include <wx/clntdata.h>

class dbgStackFrame : public wxClientData
{
public:
	dbgStackFrame(const wxString &_level, const wxString &_pkg,
	              const wxString &_func, const wxString &_desc)
		: m_level(_level), m_func(_func), m_pkg(_pkg), m_desc(_desc) {}

	dbgStackFrame(const dbgStackFrame &s)
		: m_level(s.m_level), m_func(s.m_func), m_pkg(s.m_pkg), m_desc(s.m_pkg) {}

	const wxString &GetLevel() const
	{
		return m_level;
	}
	const wxString &GetFunction() const
	{
		return m_func;
	}
	const wxString &GetPackage() const
	{
		return m_pkg;
	}
	const wxString &GetDescription() const
	{
		return m_desc;
	}

private:
	wxString m_level, m_func, m_pkg, m_desc;
};

WX_DECLARE_LIST(dbgStackFrame, dbgStackFrameList);


class ctlStackWindow : public wxListBox
{
	DECLARE_CLASS( ctlVarWindow )

public:
	ctlStackWindow(wxWindow *parent, wxWindowID id,
	               const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
	               long style = wxCLIP_CHILDREN | wxSW_3D,
	               const wxString &name = wxT("stackWindow"));

	// Remove all frames from the stack trace
	void ClearStack();
	// Add an array of frames to the stack trace
	void SetStack(const dbgStackFrameList &stacks, int selected = -1);
	void SelectFrame(const wxString &pkg, const wxString &frm);
};

#endif
