//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// dlgAddTableView.h - The query builder dialog to add tables/views
//
//////////////////////////////////////////////////////////////////////////

// App headers
#include "frmQueryBuilder.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(dlgAddTableView, wxDialog)

	EVT_KEY_DOWN(dlgAddTableView::OnKeyDown)
	EVT_KEY_UP(dlgAddTableView::OnChar)
	EVT_CHAR(dlgAddTableView::OnChar) 
	EVT_BUTTON(dlgAddTableView::BTN_CLOSE, 
		dlgAddTableView::OnClose)
	EVT_BUTTON(dlgAddTableView::BTN_OK, 
		dlgAddTableView::OnOK)
	EVT_LISTBOX_DCLICK(-1,OnOK)

END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
dlgAddTableView::dlgAddTableView(wxWindow *parent, pgDatabase *database)
{
	this->m_database = database;

	this->Create(parent, -1, wxT("Add Tables/Views"),
		wxDefaultPosition, wxSize(198, 314));

	// Setup the m_notebook
    m_notebook = new wxNotebook(this, -1, wxPoint(6,6), 
		wxSize(180,240), wxCLIP_CHILDREN );

	// Setup the table list and the view list
	m_tablelist = new wxListBox(m_notebook, -1, wxDefaultPosition,
		wxDefaultSize, 0, NULL, wxLB_MULTIPLE  | wxLB_NEEDED_SB );
	m_viewlist = new wxListBox(m_notebook, -1, wxDefaultPosition,
		wxDefaultSize, 0, NULL, wxLB_MULTIPLE  | wxLB_NEEDED_SB );

	// Add table/view to the m_notebook
	m_notebook->AddPage(m_tablelist, wxT("Tables"));
	m_notebook->AddPage(m_viewlist, wxT("Views"));

	// Add buttons
	m_OK = new wxButton(this, dlgAddTableView::BTN_OK, 
		"&Add", wxPoint(12, 252), wxSize(80, 20));
	m_Close = new wxButton(this, dlgAddTableView::BTN_CLOSE, 
		"&Close", wxPoint(102, 252), wxSize(80, 20));
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
dlgAddTableView::~dlgAddTableView()
{
	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void dlgAddTableView::OnChar(wxKeyEvent& event)
{
    long keycode = event.KeyCode();
    switch ( keycode )
    {
		case 1: 
				wxMessageBox( "got ctrl-a" );

        default:
				wxMessageBox( "gothere" );
    }

	wxMessageBox( "gothere2" );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void dlgAddTableView::OnOK(wxCommandEvent& event)
{
	frmQueryBuilder *tmpparent = (frmQueryBuilder*)this->GetParent();

	// Grab the selected items
	wxArrayInt tableselections, viewselections;
	m_tablelist->GetSelections( tableselections );
	m_viewlist->GetSelections( viewselections );

	// Find out how many tables there are to add
	int tblcount = tableselections.GetCount();
	int viewcount = viewselections.GetCount();

	// Add the tables to the MDI Client Window
	for ( int si = 0; si < tblcount; si++ )
	{
		// Grab the item number and the name of the table
		int itemno = tableselections.Item( si );
		wxString tmpname = m_tablelist->GetString( itemno );

		// Check to see if that table already exists, and if it does
		// then we need to get the correct alias for it
		wxString tmpalias = tmpparent->GetTableViewAlias(tmpname);

		// Create the child frames
		frmChildTableViewFrame *tmpframe = 
			new frmChildTableViewFrame(tmpparent, tmpname, 
			tmpalias, m_database);
		tmpparent->m_children.Add(tmpframe);
		tmpparent->m_names.Add(tmpname);
		tmpparent->m_aliases.Add(tmpalias);
		tmpparent->UpdateGridTables(NULL);
	}

	// Add the views to the MDI Client Window
	for ( si = 0; si < viewcount; si++ )
	{
		// Grab the item number and the name of the view
		int itemno = viewselections.Item( si );
		wxString tmpname = m_viewlist->GetString( itemno );

		// Check to see if that view already exists, and if it does
		// then we need to get the correct alias for it
		wxString tmpalias = tmpparent->GetTableViewAlias(tmpname);

		// Create the child frames
		frmChildTableViewFrame *tmpframe = 
			new frmChildTableViewFrame(tmpparent, tmpname, 
			tmpalias, m_database);
		tmpparent->m_children.Add(tmpframe);
		tmpparent->m_names.Add(tmpname);
		tmpparent->m_aliases.Add(tmpalias);
		tmpparent->UpdateGridTables(NULL);
	}

	// Hide the add/view window 
	this->Hide();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void dlgAddTableView::OnClose(wxCommandEvent& event)
{
	this->Hide();
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
		sysobjstr = "JOIN (SELECT oid,nspname FROM pg_namespace "
				"WHERE nspname <> 'pg_catalog' AND "
				"nspname <> 'pg_toast' AND "
				"nspname NOT LIKE 'pg_temp_%' ) b "
				"ON (a.relnamespace = b.oid) ";

	// Clear the lists
	m_tablelist->Clear();
	m_viewlist->Clear();

    if (m_database->Connect() == PGCONN_OK) {

		wxString querystr = 
			wxT("SELECT a.relname FROM pg_class a " +
				sysobjstr + 
				"WHERE a.relkind='r' " 
				"ORDER BY lower(a.relname)");

		// tables
		pgSet *tables = m_database->ExecuteSet(querystr);

		while (!tables->Eof()) {
			m_tablelist->Append(tables->GetVal(wxT("relname")));
			tables->MoveNext();
		}

		delete tables;

		querystr = 
			wxT("SELECT a.relname FROM pg_class a " +
				sysobjstr + 
				"WHERE a.relkind='v' " 
				"ORDER BY lower(a.relname)");

		// views
		pgSet *views = m_database->ExecuteSet(querystr);

		while (!views->Eof()) {
			m_viewlist->Append(views->GetVal(wxT("relname")));
			views->MoveNext();
		}

		delete views;
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void dlgAddTableView::OnKeyDown(wxKeyEvent& event)
{
	wxMessageBox("gothere");
}