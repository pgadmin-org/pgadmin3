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
class myList : public wxListCtrl
{

public:

	// Constructor
	myList(wxWindow* parent, wxWindowID id) :
	wxListCtrl(parent, id, wxPoint(0,16), wxSize(100,100),
		wxLC_REPORT|wxLC_NO_HEADER|wxLC_SINGLE_SEL|wxNO_BORDER|wxLC_HRULES)
	{
		//
	}

	// Methods
	int GetSelection()
	{
		return GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	}

	void Select(int item)
	{
		SetItemState(item, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
	}

	wxString GetString(int item)
	{
		return GetItemText(item);
	}

	wxString GetStringSelection()
	{
		int item = GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
		
		if (item == 0)
			return wxT("");
		
		return GetItemText(item);
	}

	int GetCount()
	{
		return GetItemCount();
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

	// Construction
	frmChildTableViewFrame(wxMDIParentFrame* frame, 
		wxString table, wxString alias, pgDatabase *database);
	~frmChildTableViewFrame();

	// Controls
	myList *m_columnlist;

	// Data
	wxString m_title;
	pgDatabase *m_database;

private:

	// Data
	wxPoint m_oldpos;
	long m_titlewidth;
	long m_minheight;

	// Controls
	wxBitmapButton *m_close;

	// Events
	void OnMove(wxMoveEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnDoubleClick(wxCommandEvent& event);
	void OnClose();
	void OnCloseWindow(wxCloseEvent& event);
	void OnAddColumn();
	void OnJoinTo(wxCommandEvent& event);
    void OnLeftDown(wxMouseEvent& event);
	void OnMotion(wxMouseEvent& event);
    void OnLeftUp(wxMouseEvent& event);
	void OnPaint(wxPaintEvent& event);

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
		MNU_CLOSE,
		MNU_MIN
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
