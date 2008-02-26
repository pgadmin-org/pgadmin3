//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: ctlMessageWindow.h 6136 2007-03-29 10:38:10Z hiroshi $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// ctlMessageWindow.h - debugger 
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//	class ctlMessageWindow
//
//	This class implements the window that displays DBMS messages at the 
//  bottom of the debugger window.  When we create a ctlMessageWindow, the parent
//	is a ctlTabWindow (the ctlMessageWindow becomes a tab in a tab control).
//
////////////////////////////////////////////////////////////////////////////////

#ifndef CTLMESSAGEWINDOW_H
#define CTLMESSAGEWINDOW_H

class ctlMessageWindow : public wxTextCtrl
{
    DECLARE_CLASS( ctlMessageWindow )

public:
	ctlMessageWindow( wxWindow * parent, wxWindowID id );

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
