//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsRichWindow.h - debugger 
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//  class wsRichWindow
//
//	This class provides a few minor enhancements to the standard wxTextCtrl.
//
//  We use a wsRichWindow object to display the source code for a PL function
//  when the debugger is stepping through that function. 
//
//	The wsQueryWindow class extends wsRichWindow - the user enters commands
//  into a wsQueryWindow window
//
//	wsRichWindow provides a few convenience functions for:
//		setting the foreground (text) color
//		setting the background color
//
////////////////////////////////////////////////////////////////////////////////

#ifndef WSRICHWINDOWH
#define WSRICHWINDOWH

#include <wx/stc/stc.h>

class wsRichWindow : public wxStyledTextCtrl
{
    DECLARE_DYNAMIC_CLASS( wsRichWindow )

  public:
	wsRichWindow( wxWindow * parent = NULL, wxWindowID id = -1, const wxSize & size = wxDefaultSize );
	bool	isModified();		// Has text been modified since last save?
	bool	m_parentWantsKeys;	// Forward keyEvents to our parent window?

 private:

	wsRichWindow( const wsRichWindow & src );
	wsRichWindow & operator=( const wsRichWindow & src );

	void 	OnChar( wxKeyEvent & event );				// Called when the user types a character into this window
	void 	OnSTCUpdateUI( wxStyledTextEvent & event ); // Update the cut/copy/paste/... tools (and menu options)
	void 	OnEditCommand( wxCommandEvent & event );	// Make cut/copy/paste tools into STC functions

    DECLARE_EVENT_TABLE()
};

#endif //WSRICHWINDOWH
