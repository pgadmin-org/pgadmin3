//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsfuncview.cpp - debugger 
//
//////////////////////////////////////////////////////////////////////////

#include "wsFuncView.h"
#include "wsFuncdoc.h"
#include "wsMainFrame.h"
#include "debuggerMenu.h"
	
IMPLEMENT_DYNAMIC_CLASS( wsFuncView, wxView )

BEGIN_EVENT_TABLE( wsFuncView, wxView )
	EVT_SET_FOCUS( wsFuncView::OnGainFocus )
	EVT_TOOL( wxID_CUT,			wsFuncView::OnEditCommand)
	EVT_TOOL( wxID_COPY,		wsFuncView::OnEditCommand)
	EVT_TOOL( wxID_PASTE,		wsFuncView::OnEditCommand)
	EVT_TOOL( wxID_UNDO,		wsFuncView::OnEditCommand)
	EVT_TOOL( wxID_REDO,		wsFuncView::OnEditCommand)
END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////////////////
// wsFuncView constructor
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

wsFuncView::wsFuncView( )
	: wxView(),
	m_frame( NULL ),
	m_text( NULL )
{
}

////////////////////////////////////////////////////////////////////////////////
// wsFuncView destructor
//
//	Just tear down the frame (DocChild frame) and edit control

wsFuncView::~wsFuncView( )
{
	// Note: wxWidgets closes the text control in wxView::OnClose()
	// Note: wxWidgets closes the frame too
}

////////////////////////////////////////////////////////////////////////////////
// OnCreate()
//
//	This function is called when we create a view to display a wsFuncDoc.  
//  OnCreate() asks the main frame to create a new DocChild frame to hold a 
//  a wsRichWindow (which is derived from wxStyledTextCtrl).  

bool wsFuncView::OnCreate( wxDocument * doc, long flags )
{
	wsFuncDoc * funcDoc = (wsFuncDoc *)doc;

	// Create a new frame to live in
	m_frame = glMainFrame->makeFuncFrame( doc, this );

	// Create a new edit control (a wxStyledTextCtrl under the covers)
	m_text  = new wsRichWindow( m_frame );

	// Copy the source code into the edit control and mark it as "saved"
	m_text->SetText( funcDoc->getSourceCode());
	m_text->SetSavePoint();

	// Display the frame and activate this view
	m_frame->Show( true );
	Activate( true );

	// If you open many views onto the same document (that is, if you
	// open a number of windows to look at the same function), we want
	// to coordinate all of the views.  If you make a change in one view,
	// we want to update all of the other views.  To do that, we make sure
	// that all of the wxStyledTextCtrl's maintain the same (internal)
	// document.  That's what the following calls to GetDocPointer() and
	// SetDocPointer() are taking care of.
	void * p;

	if(( p = funcDoc->getDocHandle( m_text->GetDocPointer())) != NULL )
	{
		m_text->ReleaseDocument( m_text->GetDocPointer());
		m_text->SetDocPointer( p );
	}

	return( true );
}

////////////////////////////////////////////////////////////////////////////////
//  OnClose()
//
//	The document/view framework calls this function when the user closes a view.
//  We try to save the document first if it's been modified.

bool wsFuncView::OnClose( bool deleteWindow )
{ 
	// Make sure that any modifications have been written back to the server.
	if( !GetDocument()->Close())
		return( false );

	// Give the focus to some other window
	Activate( false );

	// And trash the DocChild frame that we used to live in
	if( deleteWindow )
	{
		m_frame->Destroy();
		m_frame = NULL;
	}

	return( true );
}

////////////////////////////////////////////////////////////////////////////////
//  IsModified()
//
//	Returns true if the user has modified the text inside of the edit control, 
//  otherwise returns false.

bool wsFuncView::IsModified() const 
{
	return( m_text->isModified());
}

////////////////////////////////////////////////////////////////////////////////
//  moveTo()
//
//	This function moves the insertion point (i.e. the caret/cursor) to the given
//  position and then scrolls back to column 0.

void wsFuncView::moveTo( int pos )
{
	m_text->GotoPos( pos );
	m_text->ScrollToColumn( 0 );
}

////////////////////////////////////////////////////////////////////////////////
// OnGainFocus()
//
//	This function forwards the input focus (i.e. the keyboard focus) to the edit
//  control whenever this view gains the focus.

void wsFuncView::OnGainFocus( wxFocusEvent & event )
{
	m_text->SetFocus();
}

////////////////////////////////////////////////////////////////////////////////
// OnEditCommand()
//
//	This event handler forwards edit commands (cut, copy, paste, ...) to the edit
//  control.

void wsFuncView::OnEditCommand( wxCommandEvent & event )
{
	m_text->ProcessEvent( event );
}

////////////////////////////////////////////////////////////////////////////////
// Modify()
//
//	This function is used to mark the document/view as "saved".  We call this
//  function whenever you save the underlying document.  That way, we know 
//  whether we should hassle the user to save the document when he chooses to
//  close the last view.  
//
//	Note that the SetSavePoint() function coordinates the Undo/Redo mechanism
//  too.  That is, you can't undo past a SavePoint.

void wsFuncView::Modify( bool mod )
{
	if( mod == false )
		m_text->SetSavePoint();
}

void wsFuncView::OnUpdate( wxView * sender, wxObject * hint )
{
	wsFuncDoc * funcDoc = (wsFuncDoc *)GetDocument();

	m_text->SetText( funcDoc->getSourceCode());
	m_text->SetSavePoint();
}

void wsFuncView::OnActivateView( bool activate, wxView *activeView, wxView *deactiveView )
{
	wxToolBar * t = glMainFrame->m_toolBar;

	t->EnableTool( MENU_ID_EXECUTE, ! activate );

}
