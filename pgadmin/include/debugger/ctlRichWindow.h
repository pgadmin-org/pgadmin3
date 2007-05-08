//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: ctlRichWindow.h 6215 2007-04-19 11:12:21Z dpage $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// ctlRichWindow.h - debugger 
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//  class ctlRichWindow
//
//	This class provides a few minor enhancements to the standard wxTextCtrl.
//
//  We use a ctlRichWindow object to display the source code for a PL function
//  when the debugger is stepping through that function. 
//
//	The wsQueryWindow class extends ctlRichWindow - the user enters commands
//  into a wsQueryWindow window
//
//	ctlRichWindow provides a few convenience functions for:
//		setting the foreground (text) color
//		setting the background color
//
////////////////////////////////////////////////////////////////////////////////

#ifndef CTLRICHWINDOW_H
#define CTLRICHWINDOW_H

#include <wx/stc/stc.h>

class frmDebugger;

class ctlRichWindow : public wxStyledTextCtrl
{
    DECLARE_DYNAMIC_CLASS( ctlRichWindow )

  public:
	ctlRichWindow(frmDebugger *parent = NULL, wxWindowID id = -1, const wxSize & size = wxDefaultSize);
	bool	isModified();		// Has text been modified since last save?
	bool	m_parentWantsKeys;	// Forward keyEvents to our parent window?

 private:

	ctlRichWindow( const ctlRichWindow & src );
	ctlRichWindow & operator=( const ctlRichWindow & src );

	void 	OnChar( wxKeyEvent & event );				// Called when the user types a character into this window
	void 	OnSTCUpdateUI( wxStyledTextEvent & event ); // Update the cut/copy/paste/... tools (and menu options)
	void 	OnEditCommand( wxCommandEvent & event );	// Make cut/copy/paste tools into STC functions

	frmDebugger *m_parent;

    DECLARE_EVENT_TABLE()
};

#endif
