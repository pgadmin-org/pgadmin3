//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgSelectConnection.h - Connect to a database
//
//////////////////////////////////////////////////////////////////////////

#ifndef DLGSELECTCONNECTION_H
#define DLGSELECTCONNECTION_H

#include <wx/bmpcbox.h>
#include "dlg/dlgClasses.h"

class pgServer;
// Class declarations
class dlgSelectConnection : public DialogWithHelp
{
public:
	dlgSelectConnection(wxWindow *parent, frmMain *form);
	~dlgSelectConnection();
	wxString GetHelpPage() const;
	pgServer *GetServer()
	{
		return remoteServer;
	}
	pgConn *CreateConn(wxString &applicationame, bool &createdNew);
	pgConn *CreateConn(wxString &server, wxString &dbname, wxString &username, int port, wxString &rolename, int sslmode, wxString &applicationame, bool writeMRU = false);
	wxString GetServerName();
	wxString GetDatabase();

	int Go(pgConn *conn, wxBitmapComboBox *cb);

private:
	void OnChangeServer(wxCommandEvent &ev);
	void OnChangeDatabase(wxCommandEvent &ev);
	void OnTextChange(wxCommandEvent &ev);
	void OnOK(wxCommandEvent &ev);
	void OnCancel(wxCommandEvent &ev);

	pgServer *remoteServer;
	wxBitmapComboBox *cbConnection;
	ctlComboBoxFix *cbServer;
	wxComboBox     *cbDatabase, *cbUsername, *cbRolename;

	DECLARE_EVENT_TABLE()
};

#endif
