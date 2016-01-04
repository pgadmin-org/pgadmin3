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

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/aui/auibook.h>

// App headers
#include <ctl/ctlAuiNotebook.h>

BEGIN_EVENT_TABLE(ctlAuiNotebook, wxAuiNotebook)
	EVT_CHILD_FOCUS(ctlAuiNotebook::OnChildFocus)
END_EVENT_TABLE()

// Handle, and pass up child focus events
void ctlAuiNotebook::OnChildFocus(wxChildFocusEvent &event)
{

#if wxCHECK_VERSION(2, 9, 0)
	wxAuiNotebook::OnChildFocusNotebook(event);
	GetParent()->GetEventHandler()->AddPendingEvent(event);
#else
	wxAuiNotebook::OnChildFocus(event);
	GetParent()->AddPendingEvent(event);
#endif

}
