//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// dlgAddTableView.h - The query builder dialog to add tables/views
//
//////////////////////////////////////////////////////////////////////////

#ifndef dlgAddTableView_h
#define dlgAddTableView_h

// App headers
#include "pgConn.h"
#include "pgDatabase.h"
#include "pgSet.h"
#include "pgServer.h"
#include "pgObject.h"
#include "pgCollection.h"

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
class dlgAddTableView : public wxDialog
{
public:

	// Construction
	dlgAddTableView(wxWindow *parent, pgDatabase *database);
	dlgAddTableView::~dlgAddTableView();

	// Data
	pgDatabase *m_database;

	// Methods
	void InitLists();

private:

	// Events 
	void OnClose(wxCommandEvent& event);
	void OnOK(wxCommandEvent& event);
	void OnKeyDown(wxKeyEvent& event);
	void OnChar(wxKeyEvent& event);

	// Controls
	wxListBox *m_tablelist, *m_viewlist;
	wxNotebook *m_notebook;
	wxButton *m_OK, *m_Close;

	// Buttons
	enum
	{
		BTN_CLOSE = 1000,
		BTN_OK = 1001
	};

	// Control Enumeration
	enum
	{
		ID_TABLELISTBOX = 2000,
		ID_VIEWLISTBOX = 2001
	};

	// Macros
	DECLARE_EVENT_TABLE()
};

#endif
