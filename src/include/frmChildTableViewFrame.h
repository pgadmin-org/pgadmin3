//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmChildTableViewFrame.h - The query builder child frames
//
//////////////////////////////////////////////////////////////////////////

#ifndef frmChildfrmChildTableViewFrame_H
#define frmChildfrmChildTableViewFrame_H
		
////////////////////////////////////////////////////////////////////////////////
// Class Definition
////////////////////////////////////////////////////////////////////////////////
class myList : public wxListBox
{

public:

	// Constructor
	myList(wxWindow* parent, wxWindowID id) :
	wxListBox(parent, id, wxDefaultPosition, 
		wxDefaultSize, 0, NULL, wxLB_NEEDED_SB)
	{
		//
	}

private:

	// Events
    void OnMotion(wxMouseEvent& event);

	// Macros
	DECLARE_EVENT_TABLE()
};

////////////////////////////////////////////////////////////////////////////////
// Class Definition
////////////////////////////////////////////////////////////////////////////////
class frmChildTableViewFrame : public wxMDIChildFrame
{
public:
	frmChildTableViewFrame(wxMDIParentFrame* parent, 
		wxString table, wxString alias, pgDatabase *database);
	~frmChildTableViewFrame();

	// Controls
	myList *m_columnlist;

	// Data
	wxString m_title;
	pgDatabase *m_database;

private:

	// Controls
	wxBoxSizer *m_sizer;

	// Events
	void OnMove(wxMoveEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnDoubleClick(wxCommandEvent& event);
	void OnClose();
	void OnCloseWindow(wxCloseEvent& event);
	void OnAddColumn();
	void OnJoinTo(wxCommandEvent& event);

#ifdef __WXMSW__
    void OnContextMenu(wxContextMenuEvent& event)
        { OnRightClick(ScreenToClient(event.GetPosition())); }
#else
    void OnRightUp(wxMouseEvent& event)
        { wxPoint pt=event.GetPosition(); OnRightClick(pt); }
#endif

	void OnRightClick(wxPoint& point);

	// Control Enumeration
	enum
	{
		ID_TABLEVIEWLISTBOX = 1000
	};

	// Menu options
	enum
	{
		MNU_ADDCOLUMN = 2000,
		MNU_CLOSE
	};

	// "Reserve" these IDs for dynamic menus
	enum
	{
		MNU_JOINTO = 3000,
		MNU_JOINTO_N = 3999
	};

	// Macros
	DECLARE_EVENT_TABLE()
};

////////////////////////////////////////////////////////////////////////////////
// Class Definition
////////////////////////////////////////////////////////////////////////////////
class DnDJoin : public wxTextDropTarget
{

public:

	DnDJoin(wxFrame *frame) 
	{ 
		m_frame = frame; 
	}

    virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& text);
	
private:

    wxFrame *m_frame;
};

#endif // frmChildfrmChildTableViewFrame_H
