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

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/notebook.h>
#include <wx/xrc/xmlres.h>

// App headers
#include "dlgAddTableView.h"

// Icons
#include "images/pgAdmin3.xpm"

////////////////////////////////////////////////////////////////////////////////
// Event Table
////////////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(dlgAddTableView, wxDialog)

    EVT_LISTBOX_DCLICK(-1,dlgAddTableView::OnOK)

    EVT_BUTTON(XRCID("btnOK"), wxDialog::OnOK)
    EVT_BUTTON(XRCID("btnClose"), wxDialog::OnCancel)
    EVT_BUTTON(XRCID("btnAll"), dlgAddTableView::OnAll)
    EVT_BUTTON(XRCID("btnNone"), dlgAddTableView::OnNone)

END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////////////////////////
dlgAddTableView::dlgAddTableView(wxWindow *frame, pgDatabase *database)
{
	// Indicate we built this dialog in the log
    wxLogInfo(wxT("Creating the Query Builder Add Table/View dialogue"));

	// Load the XML resource for this dialog
    wxXmlResource::Get()->LoadDialog(this, frame, wxT("frmAddTableView")); 

    // Set the Icon
    SetIcon(wxIcon(pgAdmin3_xpm));

	// Store the database for later use
	this->m_database = database;

	// Store the lists for later use
	m_tablelist = XRCCTRL(*this, "listTables", wxListBox);
	m_viewlist = XRCCTRL(*this, "listViews", wxListBox);

	// Store the notebook for later use
	m_notebook = XRCCTRL(*this, "notebookMain", wxNotebook);
	
	// Set the accelerator table
    wxAcceleratorEntry entries[2];
    entries[0].Set(wxACCEL_CTRL, (int)'A', XRCID("btnAll"));
    entries[1].Set(wxACCEL_CTRL, (int)'D', XRCID("btnNone"));
    wxAcceleratorTable accel(2, entries);
    SetAcceleratorTable(accel);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
dlgAddTableView::~dlgAddTableView()
{
	// Indicate we're done with this dialog
    wxLogInfo(wxT("Destroying a Query Builder Add Table/View dialogue"));
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void dlgAddTableView::OnAll(wxCommandEvent& event)
{
    SelectItems(true);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void dlgAddTableView::SelectItems(bool selectall)
{
	wxListBox *tmplistbox = NULL;
	int si;

	// Page 0 = tablelist, Page 1 = viewlist
	if (m_notebook->GetSelection() == 0)
		tmplistbox = m_tablelist;
	else
		tmplistbox = m_viewlist;

	// How many columns are in the list?
	int count = tmplistbox->GetCount();

	// Iterate through all the columns and select them
	for (si = 0; si < count; si++)
		tmplistbox->SetSelection(si, selectall);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void dlgAddTableView::OnNone(wxCommandEvent& event)
{
	SelectItems(FALSE);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void dlgAddTableView::InitLists()
{
	// We need system settings
    extern sysSettings *settings;

	// We need to know if we're going to show system objects
	wxString sysobjstr;
	if (!settings->GetShowSystemObjects())
		sysobjstr = wxT("AND nspname <> 'pg_catalog' ")
				wxT("AND nspname <> 'pg_toast' ")
				wxT("AND nspname NOT LIKE 'pg_temp_%' ");

	// Clear the lists
	m_tablelist->Clear();
	m_viewlist->Clear();

    if (m_database->Connect() == PGCONN_OK) {

		wxString querystr = 
			wxT("SELECT quote_ident(nspname) || '.' || quote_ident(a.relname) AS tablename FROM pg_class a, pg_namespace b ")
				wxT("WHERE a.relnamespace = b.oid AND a.relkind='r' ") +
                sysobjstr +
				wxT("ORDER BY lower(a.relname)");

		// tables
		pgSet *tables = m_database->ExecuteSet(querystr);

		while (!tables->Eof()) {
			m_tablelist->Append(tables->GetVal(wxT("tablename")));
			tables->MoveNext();
		}

		delete tables;

		querystr = 
			wxT("SELECT quote_ident(nspname) || '.' || quote_ident(a.relname) AS viewname FROM pg_class a, pg_namespace b ")
				wxT("WHERE a.relnamespace = b.oid AND a.relkind='v' ") +
                sysobjstr + 
				wxT("ORDER BY lower(a.relname)");

		// views
		pgSet *views = m_database->ExecuteSet(querystr);

		while (!views->Eof()) {
			m_viewlist->Append(views->GetVal(wxT("viewname")));
			views->MoveNext();
		}

		delete views;
    }
}

