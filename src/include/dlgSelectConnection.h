//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgSelectConnection.h - Connect to a database
//
//////////////////////////////////////////////////////////////////////////

#ifndef DLGSELECTCONNECTION_H
#define DLGSELECTCONNECTION_H

#include "dlgClasses.h"

class pgServer;
// Class declarations
class dlgSelectConnection : public DialogWithHelp
{
public:
    dlgSelectConnection(wxWindow *parent, frmMain *form);
    ~dlgSelectConnection();
    wxString GetHelpPage() const;
    pgServer *GetServer() { return remoteServer; }
	pgConn *CreateConn();
	pgConn *CreateConn(wxString& server, wxString& dbname, wxString& username, int port, int sslmode, bool writeMRU = false);
	wxString GetServerName();
    wxString GetDatabase();

    int Go(pgConn *conn, ctlComboBoxFix *cb);
    
private:
    void OnChangeServer(wxCommandEvent& ev);
    void OnChangeDatabase(wxCommandEvent& ev);
	void OnTextChange(wxCommandEvent& ev);
    void OnOK(wxCommandEvent& ev);
    void OnCancel(wxCommandEvent& ev);

    pgServer *remoteServer;
    ctlComboBoxFix *cbConnection;
	ctlComboBoxFix *cbServer;
	ctlComboBoxFix *cbDatabase;

    DECLARE_EVENT_TABLE()
};

#endif
