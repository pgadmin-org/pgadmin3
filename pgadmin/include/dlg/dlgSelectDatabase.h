//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgSelectDatabase.h - Connect to a database
//
//////////////////////////////////////////////////////////////////////////

#ifndef DLGSELECTDATABASE_H
#define DLGSELECTDATABASE_H

#include <wx/wx.h>
#include <wx/image.h>
#include <wx/treectrl.h>


class pgServer;


class dlgSelDBNode : public wxTreeItemData
{

public:
	dlgSelDBNode(pgServer *server, const wxString &dbname = wxEmptyString);
	dlgSelDBNode *createChild(const wxString &dbName);

	wxString getDatabase()
	{
		return dbname;
	}
	wxString getConnectionString();

private:
	pgServer        *server;  // Do not remove it, not owned by this
	wxString         dbname;

	friend class dlgSelectDatabase;

};


class dlgSelectDatabase: public wxDialog
{

public:
	dlgSelectDatabase(wxWindow *parent, int id, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = wxCLOSE_BOX);

	wxString getConnInfo();
	static bool getValidConnectionString(wxString connStr, wxString &resultStr);

protected:
	void Initialize();
	void OnSelect(wxTreeEvent &ev);
	void OnSelActivate(wxTreeEvent &ev);

	wxTreeCtrl *tcServers;
	dlgSelDBNode *selectedConn;

	DECLARE_EVENT_TABLE()
};

#endif

