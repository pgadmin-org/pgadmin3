//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// ctlComboBox.h - enhanced combobox control
//
//////////////////////////////////////////////////////////////////////////


#ifndef __COMBOBOX_H
#define __COMBOBOX_H

// wxWindows headers
#include <wx/wx.h>
#include "base/base.h"

class pgConnBase;
class wxComboBoxFix : public wxComboBox
{
public:
    wxComboBoxFix(wxWindow *wnd, int id, wxPoint pos, wxSize siz, long attr);
    int FillLongKey(pgConnBase *conn, const wxChar *qry);
    int FillOidKey(pgConnBase *conn, const wxChar *qry);
    int FillStringKey(pgConnBase *conn, const wxChar *qry);
    long GetLongKey(int sel);
    OID GetOIDKey(int sel);
    wxString GetStringKey(int sel);
    bool SetKey(long val);
    bool SetKey(OID val);
    bool SetKey(const wxString &val);

#ifdef __WXMSW__
    wxString GetValue() const { return wxGetWindowText(GetHwnd()); }
#endif
};

class ctlComboBox : public wxComboBoxFix
{
public:
    ctlComboBox(wxWindow *wnd, int id, wxPoint pos, wxSize siz, long attr=0);
    int GuessSelection(wxCommandEvent &ev);
    int GetGuessedSelection() const;
    wxString GetGuessedStringSelection() const;
    int GetSelection() const;
};

#endif
