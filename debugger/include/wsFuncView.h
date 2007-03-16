//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsFuncView.h - debugger 
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// 	class wsFuncView
//
//	This class implements the "function view" in the document/view architecture.
//  A wsFuncView object is a "view" into a document (specifically, a wsFuncDoc
//  document).  
//  
//	Each wsFuncView object is a view into a wsFuncDoc document - you can open
//  many views for the same document so that you can edit different parts of 
//  of a long function.  Each wsFuncView object contains a wsRichWindow (which
//  is derived from a STC - styled-text-control).  All views for a given document
//  share the same document and the same STC document handle.  When you type in 
//  a change in one view, all views (for that document) are automatically modified
//  to match.

#ifndef WSFUNCVIEWH
#define WSFUNCVIEWH

#include <wx/docview.h>
#include "wsRichWindow.h"

class wsFuncView: public wxView
{

public:

    wsFuncView();
    ~wsFuncView();

	bool 	 IsModified() const;						// Returns true if view has been modified
	void	 Modify( bool mod );						// Marks this view as "saved"
	wxString getText() { return( m_text->GetText()); }	// Return document content
	void	 moveTo( int position );				  	// Move caret to given position

private:
    wxMDIChildFrame *	m_frame;						// Pointer to our parent frame
    wsRichWindow 	*	m_text;
  
    bool 	OnCreate( wxDocument *doc, long flags );	// Create the STC text control and a new frame
    bool 	OnClose( bool deleteWindow = true );		// Close this view (and the underlying document)
	void 	OnGainFocus( wxFocusEvent & event );		// Move the focus to the edit control when we gain focus
	void 	OnDraw( wxDC *) {}							// Render view (handled by m_text)
	void 	OnEditCommand( wxCommandEvent & event );	// Handle cut/copy/paste/...
	void    OnUpdate( wxView * sender, wxObject * hint );	// Handle document modification
	void    OnActivateView( bool activate, wxView *activeView, wxView *deactiveView );
    DECLARE_EVENT_TABLE()
	DECLARE_DYNAMIC_CLASS( wsFuncView )
};

#endif
