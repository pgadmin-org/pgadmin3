//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmChildfrmChildTableViewFrame.h - The query builder child frames
//
//////////////////////////////////////////////////////////////////////////


// App headers
#include "pgAdmin3.h"
#include "frmQueryBuilder.h"
#include "frmQBJoin.h"
#include "menu.h"

// Icons
#ifndef __WIN32__
#include "wx/dnd.h"
#include "images/dnd_copy.xpm"
#include "images/dnd_move.xpm"
#include "images/dnd_none.xpm"
#endif

#include "images/closeup.xpm"
#include "images/closedown.xpm"

////////////////////////////////////////////////////////////////////////////////
// Event Table
////////////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(frmChildTableViewFrame, wxMDIChildFrame)

    EVT_LIST_ITEM_ACTIVATED(ID_TABLEVIEWLISTBOX, frmChildTableViewFrame::OnDoubleClick)

    EVT_MENU(MNU_ADDCOLUMN, frmChildTableViewFrame::OnAddColumn)
    EVT_MENU(MNU_CLOSE, frmChildTableViewFrame::OnClose)
    EVT_MENU_RANGE(MNU_JOINTO, MNU_JOINTO_N, frmChildTableViewFrame::OnJoinTo)
    EVT_MENU_RANGE(MNU_REMOVEJOIN, MNU_REMOVEJOIN_N, frmChildTableViewFrame::OnRemoveJoin)

    EVT_CLOSE(frmChildTableViewFrame::OnCloseWindow)  

#ifdef __WXMSW__
    EVT_CONTEXT_MENU(frmChildTableViewFrame::OnContextMenu)
#else
    EVT_RIGHT_UP(frmChildTableViewFrame::OnRightUp)
#endif

    EVT_MOVE(frmChildTableViewFrame::OnMove)
    EVT_SIZE(frmChildTableViewFrame::OnSize)

    EVT_LEFT_DOWN(frmChildTableViewFrame::OnLeftDown)
    EVT_MOTION(frmChildTableViewFrame::OnMotion)
    EVT_LEFT_UP(frmChildTableViewFrame::OnLeftUp)

	EVT_BUTTON(MNU_CLOSE, frmChildTableViewFrame::OnClose)

    EVT_PAINT(frmChildTableViewFrame::OnPaint)

END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////////////////
// Event Table
////////////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(myList, wxListCtrl)

    EVT_MOTION(myList::OnMotion)

END_EVENT_TABLE()

// We need system settings
extern sysSettings *settings;

////////////////////////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////////////////////////
frmChildTableViewFrame::frmChildTableViewFrame(wxMDIParentFrame* frame, 
		wxString table, wxString alias, pgDatabase *database)
{
	int rowct = 0;
	m_columnlist = NULL;
	m_close = NULL;
	m_oldpos.x = -1;
	m_oldpos.y = -1;

	// Indicate we built this dialog in the log
    wxLogInfo(wxT("Creating a Query Builder Child Table/View Frame"));

	// Set the database
	m_database = database;

	// Set the alias
	m_title = alias;
	int textwidth, textheight;
	int maxtextwidth = 0;
	m_titlewidth = 24;
	frame->GetTextExtent(m_title, &textwidth, &textheight);
	m_titlewidth += textwidth;
	int descent;
	int leading;

	// Create the frame
	this->Create(frame, -1, alias, wxDefaultPosition, 
		wxSize(200, 200), wxRESIZE_BORDER|wxSIMPLE_BORDER );

	// Create the table name
//	wxStaticText *tmpstatic = new wxStaticText(this, -1, 
//		m_title, wxPoint(2,1), wxSize(m_titlewidth, 15), wxNO_BORDER,
//		m_title);

	// Create the close icon bitmaps
    wxBitmap closeup = wxBitmap(closeup_xpm);
    wxBitmap closedown = wxBitmap(closedown_xpm);

	// Create the bitmap button
	m_close = new wxBitmapButton(this, MNU_CLOSE, closeup, wxPoint(87,3),
		wxSize(11,10));

	// Set the bitmaps options
	m_close->SetBitmapLabel(closeup);
	m_close->SetBitmapDisabled(closeup);
	m_close->SetBitmapFocus(closeup);
	m_close->SetBitmapSelected(closedown);

	// Create a column list
	m_columnlist = new myList(this, ID_TABLEVIEWLISTBOX);
		
	// We need to know if we're going to show system objects
	wxString sysobjstr;
	if (!settings->GetShowSystemObjects())
		sysobjstr = wxT(" AND attnum > 0 ");

	// Only do this if we have a database connection
	if (m_database->Connect() == PGCONN_OK) 
	{
		// Query the columns for the table
		// Currently does not process system columns
		pgSet *columns = m_database->ExecuteSet(
			wxT("SELECT quote_ident(attname) AS attname, c.typname\n")
			wxT(" FROM pg_attribute a\n")
			wxT(" JOIN (SELECT oid, typname FROM pg_type ) c ")
			wxT(" ON ( a.atttypid = c.oid )\n")
            wxT(" WHERE a.attrelid = '") + table + wxT("'::regclass\n")
            wxT(" AND a.attisdropped = FALSE\n")+
			sysobjstr +
			wxT("\n ORDER BY attnum"));

		// Add the star column
		int item = m_columnlist->InsertItem(0, wxT("*"));
		m_columnlist->InsertColumn(0,wxT(""));
		m_columnlist->SetItem(item, 0, wxT("*"));

		// Get the column count
		rowct = columns->NumRows();

		for (int si = 0; si < rowct; si++ )
		{
			wxString tmpcolname = columns->GetVal(wxT("attname"));
			item = m_columnlist->InsertItem(si + 1, tmpcolname);
			m_columnlist->SetItem(item, 0, tmpcolname);
			columns->MoveNext();
			GetTextExtent(tmpcolname, &textwidth, &textheight,
				&descent, &leading);
			if (textwidth > maxtextwidth)
				maxtextwidth = textwidth;
		}

		delete columns;
	}

	// ClientSize
	m_minheight = (textheight-descent+leading+1) * (rowct + 1) + 18;
	this->SetClientSize(maxtextwidth, m_minheight);

	// Set the drop target
	m_columnlist->SetDropTarget(new DnDJoin(this));
}

////////////////////////////////////////////////////////////////////////////////
// Destructor
////////////////////////////////////////////////////////////////////////////////
frmChildTableViewFrame::~frmChildTableViewFrame()
{
	// Indicate we're done with this frame
    wxLogInfo(wxT("Destroying a Query Builder Child Table/View frame"));
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmChildTableViewFrame::OnDoubleClick(wxCommandEvent& event)
{
	frmQueryBuilder *tmpparent = (frmQueryBuilder*)this->GetParent();

	int tmpitem = m_columnlist->GetSelection();
	tmpparent->AddColumn(this, tmpitem);
	m_columnlist->SetDblClickFlag(TRUE);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmChildTableViewFrame::ExecRightClick(wxPoint &point)
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
			tablemenu->Append(MNU_JOINTO + si, wxGetTranslation(tName));
			result++;
		}
   	}

    // Remove Join menu
    wxMenu *joinmenu = new wxMenu();

    int joins = tmpparent->m_joins.GetCount();

    for (int ti = 0; ti < joins; ti++) {
        JoinStruct *js = (JoinStruct *)tmpparent->m_joins[ti];
        joinmenu->Append(MNU_REMOVEJOIN + ti, js->left + wxT(" -> ") + js->right);
    }

	// Context Menu
    wxString text;
   	wxMenu contextmenu;

    text.Printf(_("&Add %s to query"), columnname.c_str());
    contextmenu.Append(MNU_ADDCOLUMN, text, 
		_("Connect to the selected server."));

	if (result)
	{
		contextmenu.AppendSeparator();
		contextmenu.Append(MNU_JOINTO, _("&Join To..."), tablemenu);
	}
    if (joins)
    {
		contextmenu.AppendSeparator();
		contextmenu.Append(MNU_REMOVEJOIN, _("&Remove Join..."), joinmenu);
    }

    contextmenu.AppendSeparator();
    contextmenu.Append(MNU_CLOSE, _("&Close Table/View"), 
		_("Connect to the selected server."));

	// Popup the menu
	PopupMenu(&contextmenu, point);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmChildTableViewFrame::OnCloseWindow(wxCloseEvent& event)
{
	frmQueryBuilder *tmpparent = (frmQueryBuilder*)this->GetParent();

	// Make sure the display updates
	tmpparent->GetClientWindow()->Refresh();

	// Delete itself from the child list
	tmpparent->DeleteChild(m_title);

    Destroy();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmChildTableViewFrame::OnClose(wxCommandEvent& event)
{
	Close(TRUE);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmChildTableViewFrame::OnAddColumn(wxCommandEvent& event)
{
	frmQueryBuilder *tmpparent = (frmQueryBuilder*)this->GetParent();

	int tmpitem = this->m_columnlist->GetSelection();

	// If nothing is selected, do nothing
	if ( tmpitem < 0 )
		return;

	tmpparent->AddColumn(this, tmpitem);
	event.Skip();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmChildTableViewFrame::OnLeftDown(wxMouseEvent &event)
{
	wxPoint clientmouse = event.GetPosition();
	wxPoint screenmouse = ClientToScreen(clientmouse);
	// wxPoint clientpos = GetPosition();

	if (clientmouse.y <= 16)
	{
		if (m_oldpos.x == -1 || m_oldpos.y == -1)
		{
			m_oldpos = screenmouse;
			CaptureMouse();
			SetCursor(wxCursor(wxCURSOR_SIZING));
		}
	}
	else
	{
		event.Skip();
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmChildTableViewFrame::OnMotion(wxMouseEvent &event)
{
	wxPoint clientmouse = event.GetPosition();
	wxPoint screenmouse = ClientToScreen(clientmouse);
	wxPoint clientpos = GetPosition();
	// wxSize clientsize = GetSize();

	frmQueryBuilder *tmpparent = (frmQueryBuilder*)this->GetParent();

	// wxSize parentsize = tmpparent->GetSize();
	// wxSize parentclientsize = tmpparent->GetClientSize();
	wxPoint parentoffset = tmpparent->GetClientAreaOrigin();

	if (m_oldpos.x != -1 && m_oldpos.y != -1)
	{
		wxPoint newpos;
		newpos.x = (screenmouse.x - m_oldpos.x);
		newpos.y = (screenmouse.y - m_oldpos.y);

		if (newpos.x || newpos.y)
		{
			m_oldpos = screenmouse;
			Move(clientpos.x + newpos.x - parentoffset.x, 
				clientpos.y + newpos.y - parentoffset.y);
		}
	}
	else
	{
		event.Skip();
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmChildTableViewFrame::OnLeftUp(wxMouseEvent &event)
{
	if (m_oldpos.x != -1 && m_oldpos.y != -1)
	{
		ReleaseMouse();
		SetCursor(wxNullCursor);
	}

	m_oldpos.x = -1;
	m_oldpos.y = -1;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void myList::OnMotion(wxMouseEvent &event)
{

	if (event.m_leftDown && !dblClick)
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
			case wxDragError:   pc = _("Error!");    break;
			case wxDragNone:    pc = _("Nothing");   break;
			case wxDragCopy:    pc = _("Copied");    break;
			case wxDragMove:    pc = _("Moved");     break;
			case wxDragCancel:  pc = _("Cancelled"); break;
			default:            pc = _("Huh?");      break;
		}

		return;

	}
	else
	{
		dblClick = FALSE;
		event.Skip();
	}

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool DnDJoin::OnDropText(wxCoord x, wxCoord y, const wxString& text)
{
	wxPoint hit(x,y);
	frmChildTableViewFrame *tmprightframe = 
		(frmChildTableViewFrame*)m_frame;

	int flags;
	int item = 
		tmprightframe->m_columnlist->HitTest(hit, flags);
	if (item == -1)
		return FALSE;

	tmprightframe->m_columnlist->Select(item);

	// Construct the Join dialog
	frmQBJoin dlgJoin(m_frame, text);

	// Extract the left table name/column name
	wxStringTokenizer tmptok(text, wxT("."));
	wxString lefttable = tmptok.GetNextToken();
    lefttable += wxT(".") + tmptok.GetNextToken();
	wxString column = tmptok.GetNextToken();

	// Fail if the column is the asterisk
	if (column == wxT("*"))
	{
		wxLogError(__("You cannot join on the asterisk."));
		return FALSE;
	}

	// Get the parent Query Builder
	frmQueryBuilder *tmpparent = (frmQueryBuilder*)m_frame->GetParent();

	// Populate the Join dialog
	dlgJoin.PopulateData(tmpparent->GetFrameFromAlias(lefttable), m_frame);

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
		tmpjoin->joinop = dlgJoin.GetJoinOperator();

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

	wxSize clientsize = this->GetClientSize();

	if (clientsize.y < m_minheight)
	{
		SetClientSize(clientsize.x, m_minheight);
		event.Skip();
		return;
	}

	if (clientsize.x < m_titlewidth)
	{
		SetClientSize(m_titlewidth, clientsize.y);
		event.Skip();
		return;
	}

	if (m_columnlist)
		m_columnlist->SetSize(clientsize.x, clientsize.y - 16);

	if (m_close)
		m_close->SetSize(clientsize.x-13, 3, 11, 10);

	if (m_columnlist)
		m_columnlist->SetColumnWidth(0, clientsize.x);

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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmChildTableViewFrame::OnRemoveJoin(wxCommandEvent& event)
{
	frmQueryBuilder *tmpparent = (frmQueryBuilder*)this->GetParent();

	int n = event.GetId() - MNU_REMOVEJOIN;
    JoinStruct *js = (JoinStruct *)tmpparent->m_joins[n];

    wxMessageDialog msg(this, wxString::Format(_("Are you sure you wish to remove the join %s %s %s ?"),
        js->left.c_str(), wxT(" -> "), js->right.c_str()),
        wxString::Format(_("Remove join?")), wxYES_NO | wxICON_QUESTION);

    if (msg.ShowModal() != wxID_YES) 
        return;

    tmpparent->m_joins.RemoveAt(n);
    tmpparent->GetClientWindow()->Refresh();
}

void frmChildTableViewFrame::OnPaint(wxPaintEvent &event)
{
	wxPaintDC dc(this);

	dc.SetFont(*wxNORMAL_FONT);
	dc.DrawText(m_title, 1, 1);
}

