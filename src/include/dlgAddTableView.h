//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
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
class dlgAddTableView : public pgDialog
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
