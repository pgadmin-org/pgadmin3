//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgMoveTablespace.h - Reassign or drop owned objects
//
//////////////////////////////////////////////////////////////////////////

#ifndef dlgMoveTablespace_H
#define dlgMoveTablespace_H

// wxWindows headers
#include <wx/wx.h>
#include <wx/settings.h>


#include "dlg/dlgClasses.h"
class pgConn;
class pgTablespace;

// Class declarations
class dlgMoveTablespace : public pgDialog
{
public:
	dlgMoveTablespace(frmMain *win, pgConn *conn, pgTablespace *tblspc);
	~dlgMoveTablespace();
	wxString GetTablespace();
	wxArrayString GetKind();
	wxString GetOwner();
private:
	pgConn *connection;
	frmMain *parent;

	void OnOK(wxCommandEvent &ev);
	void OnCancel(wxCommandEvent &ev);
	void OnChange(wxCommandEvent &ev);
	DECLARE_EVENT_TABLE()
};

#endif
