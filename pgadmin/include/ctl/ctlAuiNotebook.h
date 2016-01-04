//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ctlAuiNotebook.cpp - Custom AUI Notebook class
//
//////////////////////////////////////////////////////////////////////////

// The primary purpose of this class is to pass child focus events from
// the notebook to the parent window. This is the only way we can grab
// focus events from the page controls.

#ifndef CTLAUINOTEBOOK_H
#define CTLAUINOTEBOOK_H

// wxWindows headers
#include <wx/wx.h>
#include <wx/aui/auibook.h>

class ctlAuiNotebook : public wxAuiNotebook
{
public:
	ctlAuiNotebook(wxWindow *parent, wxWindowID id = wxID_ANY, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = wxAUI_NB_DEFAULT_STYLE) :
		wxAuiNotebook(parent, id, pos, size, style) { }

protected:
	void OnChildFocus(wxChildFocusEvent &evt);

	DECLARE_EVENT_TABLE()
};

#endif

