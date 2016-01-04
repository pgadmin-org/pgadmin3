//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the BSD Licence
//
// ctlColourPicker.cpp - TreeView with Checkboxes
//
//////////////////////////////////////////////////////////////////////////


#ifndef _CTLCOLOURPICKER_H
#define _CTLCOLOURPICKER_H

// wxWindows headers
#include <wx/wx.h>
#include "utils/misc.h"



class ctlColourPicker : public wxBitmapButton
{
public:
	ctlColourPicker(wxWindow *parent, wxWindowID id, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize)
	{
		Create(parent, id, pos, size);
	}

	void DoProcessLeftClick(wxMouseEvent &event);

	wxColour GetColour();
	wxString GetColourString();

	void SetColour(const wxColour &colour);
	void SetColour(const wxString &colour);

	void SetTitle(const wxString &title);
	void UpdateColour();

private:
	wxString m_title;
	wxColour m_colour_clr;

	void Create(wxWindow *parent, wxWindowID id, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize);
};

#endif
