//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
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
#include <wx/fdrepdlg.h>

// App headers
#include "pgAdmin3.h"


// Class declarations
class ctlSQLBox : public wxStyledTextCtrl
{
public:
    ctlSQLBox(wxWindow *parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
    ctlSQLBox();
    ~ctlSQLBox();

    void Create(wxWindow *parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
    wxFindReplaceData m_findData;
    wxFindReplaceDialog* m_dlgFind;

    void OnKeyDown(wxKeyEvent& event);
    void OnFind(wxCommandEvent& event);
    void OnFindDialog(wxFindDialogEvent& event);
    
    DECLARE_DYNAMIC_CLASS(ctlSQLBox)
    DECLARE_EVENT_TABLE()
};


#endif
