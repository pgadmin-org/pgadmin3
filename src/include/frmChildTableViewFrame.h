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
class frmChildTableViewFrame : public wxMDIChildFrame
{
public:
	frmChildTableViewFrame(wxMDIParentFrame* parent, 
		wxString table, wxString alias, pgDatabase *database);
	~frmChildTableViewFrame();

	// Controls
	wxListBox *m_columnlist;

private:

	// Data
	pgDatabase *m_database;

	// Controls
	wxBoxSizer *m_sizer;

	// Events
	void OnDoubleClick(wxCommandEvent& event);
	void OnClose();
	void OnCloseWindow(wxCloseEvent& event);
	void OnAddColumn();

#ifdef __WXMSW__
    void OnContextMenu(wxContextMenuEvent& event)
        { OnRightClick(ScreenToClient(event.GetPosition())); }
#else
    void OnRightUp(wxMouseEvent& event)
        { OnRightClick(event.GetPosition()); }
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
		MNU_CLOSE = 2001
	};

	// Macros
	DECLARE_EVENT_TABLE()
};

#endif
