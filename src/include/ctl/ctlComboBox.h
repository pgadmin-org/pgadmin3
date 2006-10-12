//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
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
class ctlComboBoxFix : public wxComboBox
{
public:
    ctlComboBoxFix(wxWindow *wnd, int id, wxPoint pos, wxSize siz, long attr);

#if wxCHECK_VERSION(2,6,2) && !defined(__WXMAC__)
    // we have GetCurrentSelection() in wxChoice, implementing the old GetSelection() semantics
#else
    int GetCurrentSelection() const { return wxComboBox::GetSelection(); }
#endif

    int FillLongKey(pgConnBase *conn, const wxChar *qry);
    int FillOidKey(pgConnBase *conn, const wxChar *qry);
    int FillStringKey(pgConnBase *conn, const wxChar *qry);
    long GetLongKey(int sel=-1);
    OID GetOIDKey(int sel=-1);
    wxString GetStringKey(int sel=-1);
    bool SetKey(long val);
    bool SetKey(OID val);
    bool SetKey(const wxString &val);

    int Append(const wxString& item) { return wxComboBox::Append(item); }
    int Append(const wxString& item, void *data) { return wxComboBox::Append(item, data); }
    int Append(const wxString& item, const wxString &str);
    int Append(const wxString& item, long l);
    int Append(const wxString& item, OID oid);



#ifdef __WXMSW__
    wxString GetValue() const { return wxGetWindowText(GetHwnd()); }
#endif
};

class ctlComboBox : public ctlComboBoxFix
{
public:
    ctlComboBox(wxWindow *wnd, int id, wxPoint pos, wxSize siz, long attr=0);
    int GuessSelection(wxCommandEvent &ev);
    int GetGuessedSelection() const;
    wxString GetGuessedStringSelection() const;
    int GetSelection() const;
};

#endif
