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
#include "frmQBJoin.h"

////////////////////////////////////////////////////////////////////////////////
// Event Table
////////////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(frmChildTableViewFrame, wxMDIChildFrame)

	EVT_LISTBOX_DCLICK(ID_TABLEVIEWLISTBOX,OnDoubleClick)

    EVT_MENU(MNU_ADDCOLUMN, OnAddColumn)
    EVT_MENU(MNU_CLOSE, OnClose)
    EVT_MENU_RANGE(MNU_JOINTO, MNU_JOINTO_N, OnJoinTo)

	EVT_CLOSE(OnCloseWindow)  

#ifdef __WXMSW__
    EVT_CONTEXT_MENU(OnContextMenu)
#else
    EVT_RIGHT_UP(OnRightUp)
#endif

	EVT_MOVE(OnMove)
	EVT_SIZE(OnSize)

END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////////////////
// Event Table
////////////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(myList, wxListBox)

	EVT_MOTION(OnMotion)

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

	// Set the alias
	m_title = alias;

	// Create the frame
	this->Create(parent, -1, alias, wxDefaultPosition, wxSize(100, 200),
		wxCAPTION | wxRESIZE_BORDER );

	// Create a boxsizer for the frame to control layout
	m_sizer = new wxBoxSizer(wxVERTICAL);

	// Create a column list
	m_columnlist = new myList(this, ID_TABLEVIEWLISTBOX);
		
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
		sysobjstr = " WHERE attnum > 0 ";

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
		m_columnlist->GetCharHeight() * rowct + 4);

	// Set the drop target
	m_columnlist->SetDropTarget(new DnDJoin(this));
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
	frmQueryBuilder *tmpparent = (frmQueryBuilder*)this->GetParent();

	int tmpitem = this->m_columnlist->GetSelection();

   	// If nothing is selected, then select whatever is under the mouse
	if ( tmpitem < 0 )
	{
		tmpitem = 0;
		this->m_columnlist->Select(tmpitem);
	}

	// Get the name of the column selected
	wxString columnname = this->m_columnlist->GetString(tmpitem);

	// Table Menu
	wxMenu *tablemenu = new wxMenu();
	
	int count = tmpparent->m_aliases.GetCount();

	int result = 0;
	for (int si = 0; si < count; si++ )
	{
		wxString tName = tmpparent->m_aliases[si];

		if (tName!=m_title && !tName.IsEmpty())
		{
			tablemenu->Append(MNU_JOINTO + si, tName);
			result++;
		}
   	}

	// Context Menu
   	wxMenu contextmenu;
    contextmenu.Append(MNU_ADDCOLUMN, wxT("&Add " + columnname + " to Query"), 
		wxT("Connect to the selected server."));

	if (result)
	{
		contextmenu.AppendSeparator();
		contextmenu.Append(MNU_JOINTO, wxT("&Join To..."), tablemenu);
	}

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
	frmQueryBuilder *tmpparent = (frmQueryBuilder*)this->GetParent();

	tmpparent->DeleteChild(m_title);

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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void myList::OnMotion(wxMouseEvent &event)
{
	if (event.m_leftDown)
	{

		frmChildTableViewFrame *tmpparent = 
			(frmChildTableViewFrame*)this->GetParent();

		wxString tmpstr = tmpparent->m_title + wxT(".") + 
			this->GetStringSelection();

		// start drag operation
		wxTextDataObject textData(tmpstr);

		wxDropSource source(textData, NULL,
							wxDROP_ICON(dnd_copy),
							wxDROP_ICON(dnd_move),
							wxDROP_ICON(dnd_none));

		int flags = 0;
		//flags |= wxDrag_DefaultMove;
		flags |= wxDrag_AllowMove;

		const wxChar *pc;
		switch ( source.DoDragDrop(flags) )
		{
			case wxDragError:   pc = _T("Error!");    break;
			case wxDragNone:    pc = _T("Nothing");   break;
			case wxDragCopy:    pc = _T("Copied");    break;
			case wxDragMove:    pc = _T("Moved");     break;
			case wxDragCancel:  pc = _T("Cancelled"); break;
			default:            pc = _T("Huh?");      break;
		}

			return;

	}
	else
	{
		event.Skip();
	}

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool DnDJoin::OnDropText(wxCoord x, wxCoord y, const wxString& text)
{
	// Construct the Join dialog
	frmQBJoin dlgJoin(m_frame, text);

	// Extract the left table name/column name
	wxStringTokenizer tmptok(text, ".");
	wxString lefttable = tmptok.GetNextToken();
	wxString column = tmptok.GetNextToken();

	// Fail if the column is the asterisk
	if (column == wxT("*"))
	{
		wxLogError(wxT("You cannot join on the asterisk."));
		return FALSE;
	}

	// Get the parent Query Builder
	frmQueryBuilder *tmpparent = (frmQueryBuilder*)m_frame->GetParent();

	// Populate the Join dialog
	dlgJoin.PopulateData(tmpparent->GetFrameFromAlias(lefttable), m_frame);

	// Have to intialize prior to case statement
	
	// Show the Join dialog
	if (dlgJoin.ShowModal() == wxID_OK)
    {
		JoinStruct *tmpjoin = new JoinStruct();

		tmpjoin->left = dlgJoin.GetLeftTable();
		tmpjoin->right = dlgJoin.GetRightTable();
		tmpjoin->leftcolumn = dlgJoin.GetLeftColumn();
		tmpjoin->rightcolumn = dlgJoin.GetRightColumn();
		tmpjoin->jointype = dlgJoin.GetJoinType();
		tmpjoin->conditionct = 
			dlgJoin.GetConditions(tmpjoin->conditions);

		tmpparent->m_joins.Add(tmpjoin);

		tmpparent->GetClientWindow()->Refresh();
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmChildTableViewFrame::OnMove(wxMoveEvent& event)
{
	// Get the parent Query Builder
	frmQueryBuilder *tmpparent = (frmQueryBuilder*)this->GetParent();
	tmpparent->GetClientWindow()->Refresh();

	event.Skip();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmChildTableViewFrame::OnSize(wxSizeEvent& event)
{
	// Get the parent Query Builder
	frmQueryBuilder *tmpparent = (frmQueryBuilder*)this->GetParent();
	tmpparent->GetClientWindow()->Refresh();

	event.Skip();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmChildTableViewFrame::OnJoinTo(wxCommandEvent& event)
{
	frmQueryBuilder *tmpparent = (frmQueryBuilder*)this->GetParent();

	int n = event.GetId() - MNU_JOINTO;

	wxString tmprightname = tmpparent->m_aliases[n];

	frmChildTableViewFrame *tmpframe = 
		(frmChildTableViewFrame*)tmpparent->GetFrameFromAlias(tmprightname);

	wxString tmpcolumn = this->m_columnlist->GetStringSelection();

   	// If nothing is selected, then select the second item in the list
	// (since the first item is always the asterisk)
	if ( tmpcolumn.IsEmpty()  )
		tmpcolumn = this->m_columnlist->GetString(1);

	wxString tmpleftname = this->m_title;

	DnDJoin	tmpjoin(tmpframe);
	tmpjoin.OnDropText(0, 0, tmpleftname + wxT(".") + tmpcolumn);
}

