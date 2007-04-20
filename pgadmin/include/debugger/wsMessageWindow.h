//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: wsMessageWindow.h 6136 2007-03-29 10:38:10Z hiroshi $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsMessageWindow.h - debugger 
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//	class wsMessageWindow
//
//	This class implements the window that displays DBMS messages at the 
//  bottom of the debugger window.  When we create a wsMessageWindow, the parent
//	is a wsTabWindow (the wsMessageWindow becomes a tab in a tab control).
//
////////////////////////////////////////////////////////////////////////////////

#ifndef WSMESSAGEWINDOWH
#define WSMESSAGEWINDOWH

class wsMessageWindow : public wxTextCtrl
{
    DECLARE_CLASS( wsMessageWindow )

public:
	wsMessageWindow( wxWindow * parent, wxWindowID id );

	void	addMessage( wxString message );	// Add a message to the window
	void	delMessage( const char * name = NULL );								    // Remove a message from the window
	wxString	getMessage( int row );

private:

    typedef struct
    {
	int	m_row;		// Row number for this variable/grid cell
    } gridCell;
};

#endif
