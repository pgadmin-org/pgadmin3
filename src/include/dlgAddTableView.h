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
#include "pgDatabase.h"

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
class dlgAddTableView : public wxDialog
{
public:

	// Construction
	dlgAddTableView(wxWindow *frame, pgDatabase *database);
	dlgAddTableView::~dlgAddTableView();

	// Data
	pgDatabase *m_database;

	// Methods
	void InitLists();

	// Controls
	wxListBox *m_tablelist, *m_viewlist;
	wxNotebook *m_notebook;

private:

	void SelectItems(bool selectall);

    // Events 
	void OnAll(wxCommandEvent& event);
	void OnNone(wxCommandEvent& event);

	// Macros
	DECLARE_EVENT_TABLE()
};

#endif
