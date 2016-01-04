//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgReassignDropOwned.h - Reassign or drop owned objects
//
//////////////////////////////////////////////////////////////////////////

#ifndef dlgReassignDropOwned_H
#define dlgReassignDropOwned_H

// wxWindows headers
#include <wx/wx.h>
#include <wx/settings.h>


#include "dlg/dlgClasses.h"
class pgConn;
class pgRole;

// Class declarations
class dlgReassignDropOwned : public pgDialog
{
public:
	dlgReassignDropOwned(frmMain *win, pgConn *conn, pgRole *role, wxString dbrestriction);
	~dlgReassignDropOwned();
	wxString GetDatabase();
	wxString GetRole();
	bool IsReassign();

private:
	pgConn *connection;
	frmMain *parent;

	void OnOK(wxCommandEvent &ev);
	void OnCancel(wxCommandEvent &ev);
	void OnChange(wxCommandEvent &ev);
	DECLARE_EVENT_TABLE()
};

#endif
