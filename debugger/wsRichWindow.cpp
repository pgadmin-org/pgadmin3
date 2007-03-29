//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsrichwindow.cpp - debugger 
//
//////////////////////////////////////////////////////////////////////////

#include "wsRichWindow.h"
#include "wsMainFrame.h"
#include "debugger.h"

IMPLEMENT_DYNAMIC_CLASS( wsRichWindow, wxStyledTextCtrl )

BEGIN_EVENT_TABLE( wsRichWindow, wxStyledTextCtrl )
    EVT_CHAR( wsRichWindow::OnChar )
    EVT_STC_UPDATEUI( wxID_ANY, wsRichWindow::OnSTCUpdateUI)
    EVT_TOOL( wxID_CUT,			wsRichWindow::OnEditCommand)
    EVT_TOOL( wxID_COPY,		wsRichWindow::OnEditCommand)
    EVT_TOOL( wxID_PASTE,		wsRichWindow::OnEditCommand)
    EVT_TOOL( wxID_UNDO,		wsRichWindow::OnEditCommand)
    EVT_TOOL( wxID_REDO,		wsRichWindow::OnEditCommand)

END_EVENT_TABLE()

static const char * keywords1 = 
"abort absolute access action add after aggregate all also alter analyse  British ng analyze and any array as asc assertion assignment at authorization backward before begin between bigint binary bit boolean both by cache called cascade case cast chain char character characteristics check checkpoint class close cluster coalesce collate column comment commit committed constraint";
static const char * keywords2 = 
" constraints conversion convert copy create createdb createuser cross csv currentcursor cycle database day deallocate dec decimal declare default defaults deferrable deferred definer delete delimiter delimiters desc distinct do domain double drop each else encoding encrypted end escape except excluding exclusive execute exists explain external extract false fetch first float for";
static const char * keywords3 = 
" force foreign forward freeze from full function global grant group handler having hold hour ilike immediate immutable implicit in including increment index inherits initially inner inout input insensitive insert instead int integer intersect interval into invoker is isnull isolation join key lancompiler language large last leading left level like limit listen load local localtime";
static const char * keywords4 = 
" localtimestamp location lock match maxvalue minute minvalue mode month move names national natural nchar new next no nocreatedb nocreateuser none not nothing notify notnull nowait null nullif numeric object of off offset oids old on only operator option or order out outer overlaps overlay owner partial password placing position precision prepare preserve primary prior privileges";
static const char * keywords5 = 
" procedural procedure quote read real recheck references reindex relative release rename repeatable replace reset restart restrict returns revoke right rollback row rows rule savepoint schema scroll second security select sequence serializable session session set setof share show similar simple smallint some stable start statement statistics stdin stdout storage strict substring";
static const char * keywords6 = 
" sysid table tablespace temp template temporary then time timestamp to toast trailing transaction treat trigger trim true truncate trusted raise return exception";
static const char * keywords7 = 
" date type uncommitted unencrypted union unique unknown unlisten until update usage user using vacuum valid validator values varchar varying verbose view volatile when where with without work write year zone";

////////////////////////////////////////////////////////////////////////////////
// wsRichWindow constructor
//
//	This constructor initializes our base class (a wxTextCtrl), selects an 
//	upright (roman) and italic font, and sets the default style for any text
//  that gets added.
// 

wsRichWindow::wsRichWindow( wxWindow * parent, wxWindowID id, const wxSize & size )
	: wxStyledTextCtrl( parent, id, wxDefaultPosition, size ),
	m_parentWantsKeys( false )
{

	// Set the default font - FIXME: should get this from the .pgAdmin3 settings file

	wxString fontName;

	if( glApp->getSettings().Read( wxT( "Font" ), &fontName ))
	{
		wxFont	font( fontName );

		StyleSetFont( wxSTC_STYLE_DEFAULT, font );

		for( int i = 0; i <= 12; ++i )
			StyleSetFont( i, font );
	}
	else
	{
#if 0
		StyleSetSpec( wxSTC_STYLE_DEFAULT, wxT( "size:10,face:Courier" ));

		for( int i = 0; i <= 12; ++i )
			StyleSetSpec( i , wxT( "size:10,face:Courier" ));
#else
		wxFont mainFont( 12 , wxDEFAULT , wxNORMAL , wxNORMAL );
		wxFont otherFont( 10 , wxDEFAULT , wxNORMAL , wxNORMAL );

		StyleSetFont( wxSTC_STYLE_DEFAULT, mainFont );

		for( int i = 0; i <= 12; ++i )
			StyleSetFont( i , otherFont );
#endif
	}

	// Initialize the style-specific fonts

	// No margins required by default
	SetMarginWidth(1, 0);

	// Setup the different highlight colours
	StyleSetForeground( 0,  wxColour( 0x80, 0x80, 0x80 ));
	StyleSetForeground( 1,  wxColour( 0x00, 0x7f, 0x00 ));
	StyleSetForeground( 2,  wxColour( 0x00, 0x7f, 0x00 ));
	StyleSetForeground( 3,  wxColour( 0x7f, 0x7f, 0x7f ));
	StyleSetForeground( 4,  wxColour( 0x00, 0x7f, 0x7f ));
	StyleSetForeground( 5,  wxColour( 0x00, 0x00, 0x7f ));
	StyleSetForeground( 6,  wxColour( 0x7f, 0x00, 0x7f ));
	StyleSetForeground( 7,  wxColour( 0x7f, 0x00, 0x7f ));
	StyleSetForeground( 8,  wxColour( 0x00, 0x7f, 0x7f ));
	StyleSetForeground( 9,  wxColour( 0x7f, 0x7f, 0x7f ));
	StyleSetForeground( 10, wxColour( 0x00, 0x00, 0x00 ));
	StyleSetForeground( 11, wxColour( 0x00, 0x00, 0x00 ));

	// Select the SQL lexer (the lexer handles colorizing and fontification)
	SetLexer( wxSTC_LEX_SQL );
	SetKeyWords( 0, wxString( wxString( keywords1, wxConvUTF8 )
			+ wxString( keywords2, wxConvUTF8 )
			+ wxString( keywords3, wxConvUTF8 )
			+ wxString( keywords4, wxConvUTF8 )
			+ wxString( keywords5, wxConvUTF8 )
			+ wxString( keywords6, wxConvUTF8 )
			+ wxString( keywords7, wxConvUTF8 )));

	SetTabWidth( 4 );
}


////////////////////////////////////////////////////////////////////////////////
// OnChar()
//
// 	This event handler is called when the user presses (and 
// 	releases) a key - if the parent wants to handle the 
// 	keystrokes, give this event to our parent.

void wsRichWindow::OnChar( wxKeyEvent & event )
{
	if( m_parentWantsKeys )
		GetParent()->AddPendingEvent( event );
	
	event.Skip();	// Make sure that a base class handles this event
}

////////////////////////////////////////////////////////////////////////////////
// isModified()
//
//	This function returns true if the user has modified any of the text in the
//  edit control.

bool wsRichWindow::isModified( )
{
	return( GetModify());
}

////////////////////////////////////////////////////////////////////////////////
// OnSTCUpdateUI()
//
//	This event handler is called during idle periods - we use this opportunity
//	to update the toolbars (and menu choices) to match the state of the edit
//  control.  For example, if you have selected a chunk of text in the edit 
//  control, we enable the Cut and Copy tools - if we find any text in the 
//  clipboard, we enable the Paste tool.

void wsRichWindow::OnSTCUpdateUI( wxStyledTextEvent & event )
{
	event.Skip();

	wxToolBar  * t = glMainFrame->m_toolBar;

	t->EnableTool( wxID_PASTE, CanPaste());
	t->EnableTool( wxID_COPY, ( GetSelectionEnd() != GetSelectionStart()));
	t->EnableTool( wxID_CUT,  ( GetSelectionEnd() != GetSelectionStart()) && !GetReadOnly());
	t->EnableTool( wxID_UNDO, CanUndo());
	t->EnableTool( wxID_REDO, CanRedo());

}

////////////////////////////////////////////////////////////////////////////////
// OnEditCommand()
//
//	This event handler forwards edit commands (like cut, copy, paste, ...) from
//  the toolbar/menubar to the edit control.

void wsRichWindow::OnEditCommand( wxCommandEvent & event )
{
	switch( event.GetId())
	{
		case wxID_CUT:   Cut(); break;
		case wxID_COPY:  Copy(); break;
		case wxID_PASTE: Paste(); break;
		case wxID_UNDO:  Undo(); break;
		case wxID_REDO:	 Redo(); break;
	}
}
