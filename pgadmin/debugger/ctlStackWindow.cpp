//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ctlStackWindow.cpp - debugger
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/listimpl.cpp>

// App headers
#include "debugger/ctlStackWindow.h"

WX_DEFINE_LIST(dbgStackFrameList);

IMPLEMENT_CLASS(ctlStackWindow, wxListBox)

////////////////////////////////////////////////////////////////////////////////
// ctlStackWindow constructor
//
//  Initialize the grid control and clear it out....
//

ctlStackWindow::ctlStackWindow(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, const wxString &name )
	: wxListBox(parent , id, pos, size, 0, NULL, style | wxLB_HSCROLL | wxLB_NEEDED_SB )
{
	SetFont(settings->GetSystemFont());
}

////////////////////////////////////////////////////////////////////////////////
// ClearStack()
//
//    Remove all stack frames from the display
//
void ctlStackWindow::ClearStack()
{
	Set(0, NULL);
}

////////////////////////////////////////////////////////////////////////////////
// SetStack()
//
//    Add an array of stack frames to the display
//
void ctlStackWindow::SetStack(const dbgStackFrameList &stacks, int selected)
{
	Set(0, NULL);

	for (dbgStackFrameList::Node *node = stacks.GetFirst(); node;
	        node = node->GetNext())
	{
		dbgStackFrame *frame = node->GetData();
		Append(frame->GetDescription(), (wxClientData *)frame);
	}
	if (selected != -1)
	{
		SetSelection(selected);
	}
}

////////////////////////////////////////////////////////////////////////////////
// SelectFrame()
//
//    Select the frame based on the input function and package
//
void ctlStackWindow::SelectFrame(const wxString &pkg, const wxString &func)
{
	int cnt = GetCount();

	for (int idx = 0; idx < cnt; idx++)
	{
		dbgStackFrame *frame = (dbgStackFrame *)GetClientObject(idx);

		if (frame && frame->GetFunction() == func && frame->GetPackage() == pkg)
		{
			SetSelection(idx);

			return;
		}
	}
}
