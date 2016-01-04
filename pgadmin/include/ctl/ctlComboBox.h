//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ctlComboBox.h - enhanced combobox control
//
//////////////////////////////////////////////////////////////////////////


#ifndef __COMBOBOX_H
#define __COMBOBOX_H

// wxWindows headers
#include <wx/wx.h>
#include "utils/misc.h"



class pgConn;
class ctlComboBoxFix : public wxComboBox
{
public:
	ctlComboBoxFix(wxWindow *wnd, int id, wxPoint pos, wxSize siz, long attr);

	int FillLongKey(pgConn *conn, const wxChar *qry);
	int FillOidKey(pgConn *conn, const wxChar *qry);
	int FillStringKey(pgConn *conn, const wxChar *qry);
	long GetLongKey(int sel = -1);
	OID GetOIDKey(int sel = -1);
	wxString GetStringKey(int sel = -1);
	bool SetKey(long val);
	bool SetKey(OID val);
	bool SetKey(const wxString &val);

	int Append(const wxString &item)
	{
		return wxComboBox::Append(item);
	}
	int Append(const wxString &item, void *data)
	{
		return wxComboBox::Append(item, data);
	}
	int Append(const wxString &item, const wxString &str);
	int Append(const wxString &item, long l);
	int Append(const wxString &item, OID oid);
};

class ctlComboBox : public ctlComboBoxFix
{
public:
	ctlComboBox(wxWindow *wnd, int id, wxPoint pos, wxSize siz, long attr = 0);
	int GuessSelection(wxCommandEvent &ev);
	int GetGuessedSelection() const;
	wxString GetGuessedStringSelection() const;
	int GetSelection() const;
};

#endif
