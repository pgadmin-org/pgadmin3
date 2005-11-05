//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// ctlComboBox.cpp - enhanced combobox control
//
//////////////////////////////////////////////////////////////////////////



// App headers
#include "ctl/ctlComboBox.h"
#include "base/pgConnBase.h"
#include "base/pgSetBase.h"


class StringClientData : public wxClientData
{
public:
    StringClientData(const wxChar *c) { str=c; }
    wxString str;
};




int wxComboBoxFix::Append(const wxString& item, const wxString &str)
{
    return wxComboBox::Append(item, new StringClientData(str));
}


int wxComboBoxFix::Append(const wxString& item, long l)
{
    return wxComboBox::Append(item, (void*)l);
}


int wxComboBoxFix::Append(const wxString& item, OID oid)
{
    return wxComboBox::Append(item, (void*)oid);
}


int wxComboBoxFix::FillLongKey(pgConnBase *conn, const wxChar *qry)
{
    int cnt=0;
    pgSetIterator set(conn->ExecuteSet(qry));
    while (set.RowsLeft())
    {
        long l=set.GetLong(0);
        wxString txt=set.GetVal(1);
        Append(txt, l);
        cnt++;
    }
    return cnt;
}


int wxComboBoxFix::FillOidKey(pgConnBase *conn, const wxChar *qry)
{
    int cnt=0;
    pgSetIterator set(conn->ExecuteSet(qry));
    while (set.RowsLeft())
    {
        OID oid=set.GetOid(0);
        wxString txt=set.GetVal(1);
        Append(txt, oid);
        cnt++;
    }
    return cnt;
}


int wxComboBoxFix::FillStringKey(pgConnBase *conn, const wxChar *qry)
{
    int cnt=0;
    pgSetIterator set(conn->ExecuteSet(qry));
    while (set.RowsLeft())
    {
        wxString str=set.GetVal(0);
        wxString txt=set.GetVal(1);
        Append(txt, str);
        cnt++;
    }
    return cnt;
}

long wxComboBoxFix::GetLongKey(int sel)
{
    if (sel < 0)
        sel = GetSelection();
    return (long)GetClientData(sel);
}

OID wxComboBoxFix::GetOIDKey(int sel)
{
    if (sel < 0)
        sel = GetSelection();
    return (OID)GetClientData(sel);
}

wxString wxComboBoxFix::GetStringKey(int sel)
{
    if (sel < 0)
        sel = GetSelection();
    StringClientData *scd=(StringClientData*)GetClientObject(sel);
    if (scd)
        return scd->str;
    return wxEmptyString;
}


wxComboBoxFix::wxComboBoxFix(wxWindow *wnd, int id, wxPoint pos, wxSize siz, long attr)
: wxComboBox(wnd, id, wxEmptyString, pos, siz, 0, NULL, attr)
{
}


bool wxComboBoxFix::SetKey(long val)
{
    int i;
    for (i=0 ; i < GetCount() ; i++)
    {
        if (GetLongKey(i) == val)
        {
            SetSelection(i);
            return true;
        }
    }
    SetSelection(wxNOT_FOUND);
    return false;
}


bool wxComboBoxFix::SetKey(OID val)
{
    int i;
    for (i=0 ; i < GetCount() ; i++)
    {
        if (GetOIDKey(i) == val)
        {
            SetSelection(i);
            return true;
        }
    }
    SetSelection(wxNOT_FOUND);
    return false;
}


bool wxComboBoxFix::SetKey(const wxString &val)
{
    int i;
    for (i=0 ; i < GetCount() ; i++)
    {
        if (GetStringKey(i) == val)
        {
            SetSelection(i);
            return true;
        }
    }
    SetSelection(wxNOT_FOUND);
    return false;
}


////////////////////////////////////////////

ctlComboBox::ctlComboBox(wxWindow *wnd, int id, wxPoint pos, wxSize siz, long attr)
: wxComboBoxFix(wnd, id, pos, siz, attr)
{
#ifdef __WXGTK__
    SetEditable(false);
#endif
}


int ctlComboBox::GuessSelection(wxCommandEvent &ev)
{
    if (ev.GetEventType() != wxEVT_COMMAND_TEXT_UPDATED)
        return GetGuessedSelection();

    wxString str=wxComboBox::GetValue();
    if (str.Length())
    {
        long pos=GetInsertionPoint();
    
        long sel, count=GetCount();
        int len=str.Length();
        for (sel = 0 ; sel < count ; sel++)
        {
            if (str == GetString(sel).Left(len))
            {
                SetSelection(sel);
                wxString current = GetString(sel);
                SetSelection(pos, current.Length());
                return sel;
            }
        }
    }
    return -1;
}


int ctlComboBox::GetGuessedSelection() const
{
    int sel=wxComboBox::GetSelection();
    if (sel < 0)
        sel = FindString(GetValue());
    return sel;
}

int ctlComboBox::GetSelection() const
{
    int sel=wxComboBox::GetSelection();
    if (sel < 0)
        sel = FindString(GetValue());
    return sel;
}

wxString ctlComboBox::GetGuessedStringSelection() const
{
    int sel=GetGuessedSelection();
    if (sel < 0)
        return wxEmptyString;
    else
        return GetString(sel);
}
