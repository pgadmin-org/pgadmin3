//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmChildfrmChildTableViewFrame.h - The query builder child frames
//
//////////////////////////////////////////////////////////////////////////

#include "frmQueryBuilder.h"

////////////////////////////////////////////////////////////////////////////////
// Event Table
////////////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(frmChildTableViewFrame, wxMDIChildFrame)

	EVT_LISTBOX_DCLICK(ID_TABLEVIEWLISTBOX,OnDoubleClick)
    EVT_MENU(MNU_ADDCOLUMN, OnAddColumn)
    EVT_MENU(MNU_CLOSE, OnClose)
	EVT_CLOSE(OnCloseWindow)  

#ifdef __WXMSW__
    EVT_CONTEXT_MENU(OnContextMenu)
#else
    EVT_RIGHT_UP(OnRightUp)
#endif

END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////////////////////////
frmChildTableViewFrame::frmChildTableViewFrame(wxMDIParentFrame *parent, 
											   wxString table,
											   wxString alias,
											   pgDatabase *database)
{
	int rowct = 0;

	// Set the database
	m_database = database;

	// Create the frame
	this->Create(parent, -1, alias, wxDefaultPosition, wxSize(100, 200));

	// Create a boxsizer for the frame to control layout
	m_sizer = new wxBoxSizer(wxVERTICAL);

	// Create a column list
	m_columnlist = new wxListBox(this, ID_TABLEVIEWLISTBOX, 
		wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_NEEDED_SB );

	// Add the column list to the sizer
	m_sizer->Add(m_columnlist, 1, wxEXPAND, 0 );

	// Size hints (because the window is sizable)
	m_sizer->SetSizeHints( this );

	// Set the sizer for the frame
	SetSizer( m_sizer );

	// We need system settings
    extern sysSettings *settings;

	// We need to know if we're going to show system objects
	wxString sysobjstr;
	if (!settings->GetShowSystemObjects())
		sysobjstr = " WHERE attnum >= 0 ";

	// Only do this if we have a database connection
	if (m_database->Connect() == PGCONN_OK) 
	{
		// Query the columns for the table
		// Currently does not process system columns
		pgSet *columns = m_database->ExecuteSet(
			wxT("SELECT attname, c.typname " 
				"FROM pg_attribute a "
				"JOIN "
				"( SELECT oid FROM pg_class "
				"WHERE lower(relname) = lower('" + table + "') ) b "
				"ON ( a.attrelid = b.oid ) "
				"JOIN "
				"( SELECT oid, typname FROM pg_type ) c "
				"ON ( a.atttypid = c.oid ) " + 
				sysobjstr +
				"ORDER BY attnum"));

		// Add the star column
		m_columnlist->Append(wxT("*"));

		// Get the column count
		rowct = columns->NumRows();

		for (int si = 0; si < rowct; si++ )
		{
			m_columnlist->Append(columns->GetVal(wxT("attname")));
			columns->MoveNext();
		}

		delete columns;
	}

	// GetFont
	this->SetClientSize( GetClientSize().GetWidth(), 
		int( m_columnlist->GetFont().GetPointSize() * (rowct + 1) * 2.0f) );
}

////////////////////////////////////////////////////////////////////////////////
// Destructor
////////////////////////////////////////////////////////////////////////////////
frmChildTableViewFrame::~frmChildTableViewFrame()
{
	frmQueryBuilder *tmpparent = (frmQueryBuilder*)this->GetParent();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmChildTableViewFrame::OnDoubleClick(wxCommandEvent& event)
{
	frmQueryBuilder *tmpparent = (frmQueryBuilder*)this->GetParent();

	int tmpitem = event.GetInt();
	tmpparent->AddColumn(this, tmpitem);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmChildTableViewFrame::OnRightClick(wxPoint &point)
{
	int tmpitem = this->m_columnlist->GetSelection();

   	// If nothing is selected, then select whatever is under the mouse
	if ( tmpitem < 0 )
	{
		tmpitem = 0;
		this->m_columnlist->Select(tmpitem);
	}

	// Get the name of the column selected
	wxString columnname = this->m_columnlist->GetString(tmpitem);

	// Context Menu
   	wxMenu contextmenu;
    contextmenu.Append(MNU_ADDCOLUMN, wxT("&Add " + columnname + " to Query"), 
		wxT("Connect to the selected server."));
    contextmenu.AppendSeparator();
    contextmenu.Append(MNU_CLOSE, wxT("&Close Table/View"), 
		wxT("Connect to the selected server."));

	// Popup the menu
	PopupMenu(&contextmenu, point);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmChildTableViewFrame::OnCloseWindow(wxCloseEvent& event)
{
    Destroy();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmChildTableViewFrame::OnClose()
{
	Close(TRUE);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmChildTableViewFrame::OnAddColumn()
{
	frmQueryBuilder *tmpparent = (frmQueryBuilder*)this->GetParent();

	int tmpitem = this->m_columnlist->GetSelection();

	// If nothing is selected, do nothing
	if ( tmpitem < 0 )
		return;

	tmpparent->AddColumn(this, tmpitem);
}
