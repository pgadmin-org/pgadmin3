//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// ctlComboBox.h - enhanced combobox control
//
//////////////////////////////////////////////////////////////////////////


#ifndef __COMBOBOX_H
#define __COMBOBOX_H

// wxWindows headers
#include <wx/wx.h>


class ctlComboBox : public wxComboBox
{
public:
    ctlComboBox(wxWindow *wnd, int id, wxPoint pos, wxSize siz, long attr=0);
    int GuessSelection();
    int GetSelection() const;
    wxString GetStringSelection() const;
};

#endif
