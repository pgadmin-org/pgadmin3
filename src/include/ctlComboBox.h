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


#ifdef __WXMSW__
class wxComboBoxFix : public wxComboBox
{
public:
    wxString GetValue() const { return wxGetWindowText(GetHwnd()); }
};
#else
#define wxComboBoxFix wxComboBox
#endif

class ctlComboBox : public wxComboBox
{
public:
    ctlComboBox(wxWindow *wnd, int id, wxPoint pos, wxSize siz, long attr=0);
    int GuessSelection(wxCommandEvent &ev);
    int GetGuessedSelection() const;
    wxString GetGuessedStringSelection() const;
#ifdef __WXMSW__
    wxString GetValue() const { return wxGetWindowText(GetHwnd()); }
#endif
    int GetSelection() const;
};

#endif
