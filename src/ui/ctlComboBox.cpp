//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// ctlComboBox.cpp - enhanced combobox control
//
//////////////////////////////////////////////////////////////////////////



// App headers
#include "pgAdmin3.h"
#include "ctlComboBox.h"


ctlComboBox::ctlComboBox(wxWindow *wnd, int id, wxPoint pos, wxSize siz, long attr)
: wxComboBox(wnd, id, wxEmptyString, pos, siz, attr)
{
}


int ctlComboBox::GuessSelection()
{
    wxString str=GetValue();
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


int ctlComboBox::GetSelection() const
{
    int sel=wxComboBox::GetSelection();
    if (sel < 0)
        sel = FindString(GetValue());
    return sel;
}


wxString ctlComboBox::GetStringSelection() const
{
    int sel=GetSelection();
    if (sel < 0)
        return wxEmptyString;
    else
        return GetString(sel);
}
