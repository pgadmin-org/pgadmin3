//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgDatabase.h - PostgreSQL Database
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGDATABASE_H
#define PGDATABASE_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgObject.h"
#include "pgServer.h"
#include "pgConn.h"

// Class declarations
class pgDatabase : public pgServerObject
{
public:
    pgDatabase(const wxString& newName = wxString(""));
    ~pgDatabase();

    int GetIcon() { return PGICON_DATABASE; }
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, wxListCtrl *properties=0, wxListCtrl *statistics=0, ctlSQLBox *sqlPane=0);
    static void ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane);

	// Keith 2002.03.04 --
	// pgServer::ExecuteSet executes on an arbitrary database on the server,
	// so I added this because it works on the correct database connection
    pgSet *ExecuteSet(const wxString& sql) { 
		return conn->ExecuteSet(sql); }

    wxString GetPath() const { return path; };
    void iSetPath(const wxString& newVal) { path = newVal; }
    wxString GetEncoding() const { return encoding; }
    void iSetEncoding(const wxString& newVal) { encoding = newVal; }
    wxString GetVariables() const { return variables; }
    void iSetVariables(const wxString& newVal) { variables = newVal; }
    bool GetAllowConnections() const { return allowConnections; }
    void iSetAllowConnections(bool newVal) { allowConnections = newVal; }
    bool GetConnected() const { return connected; }
    bool GetSystemObject() const;
    
    bool CanVacuum() { return true; }
    wxString GetSql(wxTreeCtrl *browser);
    pgConn *connection() { return conn; }

    int Connect();

private:
    pgConn *conn;
    wxString path, encoding, variables;
    bool allowConnections, connected;
};

#endif
