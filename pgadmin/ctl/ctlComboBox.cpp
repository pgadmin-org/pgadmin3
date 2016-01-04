//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ctlComboBox.cpp - enhanced combobox control
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// App headers
#include "ctl/ctlComboBox.h"
#include "db/pgConn.h"
#include "db/pgSet.h"


class StringClientData : public wxClientData
{
public:
	StringClientData(const wxChar *c)
	{
		str = c;
	}
	wxString str;
};




int ctlComboBoxFix::Append(const wxString &item, const wxString &str)
{
	return wxComboBox::Append(item, new StringClientData(str));
}


int ctlComboBoxFix::Append(const wxString &item, long l)
{
	return wxComboBox::Append(item, (void *)l);
}


int ctlComboBoxFix::Append(const wxString &item, OID oid)
{
	return wxComboBox::Append(item, (void *)oid);
}


int ctlComboBoxFix::FillLongKey(pgConn *conn, const wxChar *qry)
{
	int cnt = 0;
	pgSetIterator set(conn->ExecuteSet(qry));
	while (set.RowsLeft())
	{
		long l = set.GetLong(0);
		wxString txt = set.GetVal(1);
		Append(txt, l);
		cnt++;
	}
	return cnt;
}


int ctlComboBoxFix::FillOidKey(pgConn *conn, const wxChar *qry)
{
	int cnt = 0;
	pgSetIterator set(conn->ExecuteSet(qry));
	while (set.RowsLeft())
	{
		OID oid = set.GetOid(0);
		wxString txt = set.GetVal(1);
		Append(txt, oid);
		cnt++;
	}
	return cnt;
}


int ctlComboBoxFix::FillStringKey(pgConn *conn, const wxChar *qry)
{
	int cnt = 0;
	pgSetIterator set(conn->ExecuteSet(qry));
	while (set.RowsLeft())
	{
		wxString str = set.GetVal(0);
		wxString txt = set.GetVal(1);
		Append(txt, str);
		cnt++;
	}
	return cnt;
}

long ctlComboBoxFix::GetLongKey(int sel)
{
	if (sel < 0)
		sel = GetSelection();
	return (long)wxItemContainer::GetClientData(sel);
}

OID ctlComboBoxFix::GetOIDKey(int sel)
{
	if (sel < 0)
		sel = GetSelection();
	return (OID)wxItemContainer::GetClientData(sel);
}

wxString ctlComboBoxFix::GetStringKey(int sel)
{
	if (sel < 0)
		sel = GetSelection();
	StringClientData *scd = (StringClientData *)wxItemContainer::GetClientObject(sel);
	if (scd)
		return scd->str;
	return wxEmptyString;
}


ctlComboBoxFix::ctlComboBoxFix(wxWindow *wnd, int id, wxPoint pos, wxSize siz, long attr)
	: wxComboBox(wnd, id, wxEmptyString, pos, siz, 0, NULL, attr)
{
}


bool ctlComboBoxFix::SetKey(long val)
{
	unsigned int i;
	for (i = 0 ; i < GetCount() ; i++)
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


bool ctlComboBoxFix::SetKey(OID val)
{
	unsigned int i;
	for (i = 0 ; i < GetCount() ; i++)
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


bool ctlComboBoxFix::SetKey(const wxString &val)
{
	unsigned int i;
	for (i = 0 ; i < GetCount() ; i++)
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
	: ctlComboBoxFix(wnd, id, pos, siz, attr)
{
#ifdef __WXGTK__
	SetEditable(false);
#endif
}


int ctlComboBox::GuessSelection(wxCommandEvent &ev)
{
	if (ev.GetEventType() != wxEVT_COMMAND_TEXT_UPDATED)
		return GetGuessedSelection();

	wxString str = wxComboBox::GetValue();
	if (str.Length())
	{
		long pos = GetInsertionPoint();

		long sel, count = GetCount();
		int len = str.Length();
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
	int sel = GetCurrentSelection();

	if (sel < 0)
		sel = FindString(GetValue());
	return sel;
}

int ctlComboBox::GetSelection() const
{
	int sel = 0;
#ifdef __WXMSW__
	sel = GetCurrentSelection();

	if (sel < 0)
#endif
		sel = FindString(GetValue());
	return sel;
}

wxString ctlComboBox::GetGuessedStringSelection() const
{
	int sel = GetGuessedSelection();
	if (sel < 0)
		return wxEmptyString;
	else
		return GetString(sel);
}
