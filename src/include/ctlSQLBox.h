//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// ctlSQLBox.h - SQL syntax highlighting textbox
//
//////////////////////////////////////////////////////////////////////////

#ifndef CTLSQLBOX_H
#define CTLSQLBOX_H

// wxWindows headers
#include <wx/wx.h>
#include <wx/stc/stc.h>

// App headers
#include "pgAdmin3.h"


// Class declarations
class ctlSQLBox : public wxStyledTextCtrl
{
public:
    ctlSQLBox(wxWindow *parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
    ~ctlSQLBox();
    void OnKeyDown(wxKeyEvent& event);
    DECLARE_EVENT_TABLE();
};


#endif
