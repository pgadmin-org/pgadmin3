//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsfuncdoc.cpp - debugger 
//
//////////////////////////////////////////////////////////////////////////

#include "wsFuncdoc.h"
#include "wsFuncView.h"
#include "wsConnProp.h"
#include "debugger.h"
#include "wsMainFrame.h"
#include "debuggerMenu.h"

#include <wx/filesys.h>
#include <wx/tokenzr.h>

#include "wx/wfstream.h"
#include "wx/txtstrm.h"

IMPLEMENT_DYNAMIC_CLASS( wsFuncDoc, wxDocument )

BEGIN_EVENT_TABLE(  wsFuncDoc, wxDocument )
	EVT_MENU( RESULT_ID_RESULT_SET_READY, wsFuncDoc::OnResultReady )
END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////////////////
// wsFuncDoc constructors
//
// 	This class implements the "function document" in the document/view 
//	architecture.  A wsFuncDoc object represents an editable document that
//	happens to be the source code for a function/procedure.  We create a 
//  new wsFuncDoc object whenever the user opens a function for editing.
//
//	Each wsFuncDoc object corresponds to a single function/procedure.  For 
//  each function/procedure, we keep track of the function signature (name+
//	argument types), the function source code, result type, schema, owner,
//  and other function-related attributes.
//
//	A wsFuncDoc object knows how to retreive it's definition from the server
//  (see readFunction()) and knows how to save itself back to the server (see
//   OnSaveDocument()).

wsFuncDoc::wsFuncDoc( const wxString & sourceCode, const wxString & title, const wxString & funcOID )
  : m_sourceCode( _( "-- Loading" )),
	m_docHandle( NULL ),
	m_oid( funcOID ),
	m_conn( NULL )
{
	// SetDocumentName is a bit of a misnomer - we're really setting the document type.
	SetDocumentName( _( "SQL Document" ));

	SetTitle( title );
	SetFilename( wxT( "::" ) + title );
	SetDocumentTemplate((wxDocTemplate *) glApp->getDocMgr()->GetTemplates().GetFirst()->GetData());

	SetDocumentSaved( true );	// Make sure 'Save' doesn't prompt user for a file name

	// Read the rest of the function definition from the server.
	readFunction( funcOID );
}

wsFuncDoc::wsFuncDoc()
  : m_docHandle( NULL ),
	m_conn( NULL ),
	m_result( NULL )
{
}

////////////////////////////////////////////////////////////////////////////////
// OpenDoc()
//
//	This static function creates a new wsFuncDoc object (that is, OpenDoc() 
//  creates a new function document).  You should never create a wsFuncDoc
//  directly (that's why the two constructors are private).  Instead, call 
//  OpenDoc() and it will search for an existing document with the same name.
//  If you already have the requested document, OpenDoc() returns a pointer
//  to that document, otherwise, OpenDoc() creates a new document.
//
//  The caller provides the source code for the function, the title (the name 
//  of the function), the OID of the function, and a starting position.  

wsFuncDoc * wsFuncDoc::OpenDoc( const wxString & sourceCode, const wxString & title, const wxString & funcOid, int startingPos )
{
	wxDocManager * docMgr    = glApp->getDocMgr();
	wxList       & documents = docMgr->GetDocuments();

	// Search for an existing document with the same name...

	for( size_t i = 0; i < documents.GetCount(); ++i )
	{
		wxDocument * doc = (wxDocument*)( documents.Item( i )->GetData());

		if( title.CmpNoCase( doc->GetFilename()) == 0 )
		{
			// We already have a document for this function, create a new 
			// view and return a pointer to the existing document.

#if 0
			if( doc->GetFirstView())
			{
				// Try to bring the view for this document to the foreground...

				// FIXME: this doesn't seem to work very well and there doesn't
				// 	      seem to be a portable way to fix it.

				docMgr->ActivateView( doc->GetFirstView(), true );

				glMainFrame->ActivateNext();

				if( doc->GetDocumentWindow())
					doc->GetDocumentWindow()->SetFocus();

				return((wsFuncDoc *) doc );
			}
			else
#endif
			{
				// Create a new view for this document

				docMgr->CreateView( doc, 0 );

				return((wsFuncDoc *) doc );
			}
		}
	}

	// We don't know about this document yet... make a new one
	wsFuncDoc * doc = new wsFuncDoc( sourceCode, title, funcOid );

	// Tell the document manager about this document...
	docMgr->AddDocument( doc );

	// Create a new view for this document 
	wsFuncView * view = (wsFuncView *)docMgr->CreateView( doc, 0 );

	// Move the insertion point (cursor/caret) to the given position
	view->moveTo( startingPos );

	return((wsFuncDoc *) doc );
}

////////////////////////////////////////////////////////////////////////////////
// getDocHandle()
//
//	This function manages the "document handle" for this document. We use edit
//  controls (wxStyledTextCtrl's) to display the source code for each document.
//  Since you can open multiple views of the same document, we want to make sure
//  that each edit control uses the same underlying document.  Fortunately, the
//  wxStyledTextCtrl knows how to manage multiple views.  When we create the 
//  first view for a given document, we ask the wxStyledTextCtrl for a handle
//  to it's internal document object - when we create subsequent views, we 
//  give that handle to each new wxStyledTextCtrl.
//
//  We store the wxStyledTextCtrl handle in m_docHandle.

void * wsFuncDoc::getDocHandle( void * defaultHandle )
{
	if( m_docHandle == NULL )
	{
		m_docHandle = defaultHandle;
		return( NULL );
	}
	else
	{
		return( m_docHandle );
	}
}

////////////////////////////////////////////////////////////////////////////////
// makeSignature()
//
//  This function builds a human-friendly string out of the function name, 
//  argument data types, and (optional) argument names.

wxString wsFuncDoc::makeSignature()
{
	wxStringTokenizer	argNames( m_rawArgNames, wxT( "{}, " ), wxTOKEN_STRTOK );
	wxStringTokenizer	argTypes( m_rawArgTypes, wxT( ", " ), wxTOKEN_STRTOK );
	wxString			signature( m_rawSchema + wxT( "." ) +  m_rawName + wxT( "(" ));
	wxString			delimiter = wxT( " " );

	while( argTypes.HasMoreTokens())
	{
		signature += delimiter;

		if( argNames.HasMoreTokens())
		{
			signature += argNames.GetNextToken();
			signature += wxT( " " );
		}

		signature += argTypes.GetNextToken();

		delimiter = wxT( ", " );
	}

	signature += wxT( " )" );

	return( signature );
}

////////////////////////////////////////////////////////////////////////////////
// OnSaveDocument()
//
//	The document/view framework calls this function whenever the user closes the
//  last view for a given document or when the user clicks the Save button.
//
//	This function writes any modifications back to the PostgreSQL server.

bool wsFuncDoc::OnSaveDocument( const wxString & fileName )
{
	wsFuncView * view = (wsFuncView *)GetFirstView();
        
	// If we have a view, grab the source code from there just in case
	// the user has made any changes
	if( view )
	{
		m_sourceCode = view->getText();
	}

	if( fileName.StartsWith( wxT( "::" )))
		return( saveToServer( fileName, m_sourceCode ));
	else
		return( saveToFile( fileName, m_sourceCode ));
}

////////////////////////////////////////////////////////////////////////////////
//  saveToServer()
//
//	This function saves the source code for this function to the PostgreSQL 
//  server (actually, this function simply initiates the command, we don't 
//  hang around to see if it completes - see OnResultReady() for result handling)

bool wsFuncDoc::saveToServer( const wxString & fileName, const wxString & command )
{
	if( getPgConn() != NULL )
	{
		// Build the CREATE FUNCTION command...
                                                            
		glApp->getStatusBar()->SetStatusText( _( "Sending changes to server..." ), 1 );	

		// Send the command to the server - note, we don't wait for completion: the 
		// connection object (m_conn) calls OnResultReady() when the command completes.
		m_conn->startCommand( command, this );
            
		return( true );
	}
	else
	{
		wxMessageBox( _( "Sorry, can't save your changes to the server\nTry 'Save As'" ), _( "Can't connect to server" ), wxOK|wxICON_EXCLAMATION );
		return( false );
	}
}

////////////////////////////////////////////////////////////////////////////////
//  saveToFile()
//
//	This function saves the source code for this function (along with the 'CREATE
//  OR REPLACE FUNCTION verbiage) to the given file.

bool wsFuncDoc::saveToFile( const wxString & fileName, const wxString & command )
{
	wxFile	dst;

	if( !dst.Create( fileName, true ))
		return( false );

	if( dst.Write( command ))
	{
		Modify( false );    // Function saved...
	}
	else
	{
		dst.Close();
		return( false );
	}
	

	dst.Close();

	return( true );
}	

////////////////////////////////////////////////////////////////////////////////
// getSourceCode()
//
//	Returns a reference to the source code for this function

wxString & wsFuncDoc::getSourceCode()
{
	return( m_sourceCode );
}

////////////////////////////////////////////////////////////////////////////////
// IsModified()
//
//	Returns true if the source code for this document has been modified.

bool wsFuncDoc::IsModified() const
{
	wsFuncView * view = (wsFuncView *)GetFirstView();

	if( view )
		return( wxDocument::IsModified() || view->IsModified());
	else
		return( wxDocument::IsModified());
}

////////////////////////////////////////////////////////////////////////////////
// Modify()
//
//	Sets the modified/saved state for this document.  If 'mod' is true, we mark
//  this document as modified (that is, it needs to be saved before closing). 
//  If 'mod' is false, we mark this document as unmodified (doesn't need to be 
//  saved).

void wsFuncDoc::Modify( bool mod )
{
  wsFuncView * view = (wsFuncView *)GetFirstView();

  view->Modify( mod );
  wxDocument::Modify( mod );

}
////////////////////////////////////////////////////////////////////////////////
// getPgConn()
//
//	This function connects to the PostgreSQL server (or returns a handle to the
//  existing connection if we've already made the connection).  
//
//  To find the server, we ask the application (glApp) for the connection
//  properties specified on the command line.  If we can't make a connection
//  using those properties, we ask the user to intervene.  

wsPgConn * wsFuncDoc::getPgConn()
{
    // If we've already connected to the server, just return that connection handle
    if( m_conn )
        return( m_conn );
        
    // We haven't connected to the server yet - grab any connection properties that
    // appear in the command-line
    wsConnProp connectProperties( glApp->getConnProp());
    
    do
    {
        // Try to connect using the current set of connection properties        
        m_conn = new wsPgConn( connectProperties );
        
        if( m_conn->isConnected())
            return( m_conn );
            
        // That didn't work... throw out this connection object 
        delete( m_conn );
        m_conn = NULL;
            
        // Ask the user to adjust the connection properties        
        wsConnPropDlg * connDlg = new wsConnPropDlg( NULL );

        connDlg->setDefaults( connectProperties );
        
        if( connDlg->ShowModal() != wxID_OK )
            return( NULL );
        else
            connDlg->getChoices( connectProperties );                
            
        // If the user didn't cancel this dialog, loop back and try to connect to the server again
        
     } while( true );
}
////////////////////////////////////////////////////////////////////////////////
// readFunction()
//
//	This function reads all a number of function attributes from the server.  We
//  already have the source code (the caller gave that to us), but we need to 
//  read the owner, argument names, comment, and such so we can build a CREATE
//  OR REPLACE FUNCTION command when we save any changes back to the server.

void wsFuncDoc::readFunction( const wxString & funcOID )
{
	wsPgConn * conn = getPgConn();
        
	if( conn )
	{
		wxString   command;
		
		command += wxT( "SELECT quote_ident( n.nspname ) as \"schema\",\n" );
		command += wxT( "  quote_ident( p.proname ) as \"name\",\n" );
		command += wxT( "  CASE WHEN p.proretset THEN 'setof ' ELSE '' END || pg_catalog.format_type( p.prorettype, NULL ) as \"resulttype\",\n" );
		command += wxT( "  pg_catalog.oidvectortypes(p.proargtypes) as \"argtypes\",\n" );
		command += wxT( "  p.proargnames as \"argnames\",\n" );
		command += wxT( "  quote_ident( u.usename ) as \"owner\",\n" );
		command += wxT( "  l.lanname as \"language\",\n" );
		command += wxT( "  quote_literal( pg_catalog.obj_description( p.oid, 'pg_proc' )) as \"comment\",\n" );
		command += wxT( "  p.prosrc as \"sourcecode\",\n" );
		command += wxT( "  CASE WHEN p.proisstrict THEN 'STRICT' ELSE '' END as \"isstrict\",\n" );
		command += wxT( "  CASE WHEN p.prosecdef THEN 'SECURITY DEFINER' ELSE '' END as \"securitydefiner\"\n" );
		command += wxT( "FROM pg_catalog.pg_proc p\n" );
		command += wxT( "LEFT JOIN pg_catalog.pg_namespace n ON n.oid = p.pronamespace\n" );
		command += wxT( "LEFT JOIN pg_catalog.pg_language l ON l.oid = p.prolang\n" );
		command += wxT( "LEFT JOIN pg_catalog.pg_user u ON u.usesysid = p.proowner\n" );
		command += wxT( "WHERE p.oid = " ) + funcOID;

		m_conn->startCommand( command, this );

		glApp->getStatusBar()->SetStatusText( _( "Retrieving function details..." ), 1 );
	}
}

////////////////////////////////////////////////////////////////////////////////
// OnResultReady()
//
//	This event handler is called when a complete result set arrives from the 
//  PostgreSQL server.  
//
//	At the moment, we expect two different result sets.  The first result set
//  is generated by the query that we execute in readFunction().  The second
//  result set is generated by the CREATE OR REPLACE FUNCTION command that we
//  execute in OnSaveDocument().

void wsFuncDoc::OnResultReady( wxCommandEvent & event )
{
	m_result = (PGresult *)event.GetClientData();

	if( PQnfields( m_result ))
	{
		glApp->getStatusBar()->SetStatusText( _( "Retrieving function details...complete" ), 1 );

		// This is a result for our query (see readFunction())
		m_rawArgNames     = wxString( PQgetvalue( m_result, 0, PQfnumber( m_result, "argnames" )), wxConvUTF8 );
		m_rawArgTypes     = wxString( PQgetvalue( m_result, 0, PQfnumber( m_result, "argtypes" )), wxConvUTF8 );
		m_rawSchema       = wxString( PQgetvalue( m_result, 0, PQfnumber( m_result, "schema" )), wxConvUTF8 );
		m_rawName         = wxString( PQgetvalue( m_result, 0, PQfnumber( m_result, "name" )), wxConvUTF8 );
		m_rawResultType   = wxString( PQgetvalue( m_result, 0, PQfnumber( m_result, "resulttype" )), wxConvUTF8 );
		m_rawOwner        = wxString( PQgetvalue( m_result, 0, PQfnumber( m_result, "owner" )), wxConvUTF8 );
		m_rawLanguage     = wxString( PQgetvalue( m_result, 0, PQfnumber( m_result, "language" )), wxConvUTF8 );
		m_comment         = wxString( PQgetvalue( m_result, 0, PQfnumber( m_result, "comment" )), wxConvUTF8 );
		m_isStrict        = wxString( PQgetvalue( m_result, 0, PQfnumber( m_result, "isstrict" )), wxConvUTF8 );
		m_securityDefiner = wxString( PQgetvalue( m_result, 0, PQfnumber( m_result, "securitydefiner" )), wxConvUTF8 );
		m_sourceCode      = wxString( PQgetvalue( m_result, 0, PQfnumber( m_result, "sourcecode" )), wxConvUTF8 );

		wxString command( wxT( "CREATE OR REPLACE FUNCTION " ));
        
		wxString signature = makeSignature();
        
		command.Printf( wxT( "CREATE OR REPLACE FUNCTION %s RETURNS %s AS $BODY$\n%s\n$BODY$\nLANGUAGE '%s' %s %s\n" ),
						makeSignature().c_str(),
						m_rawResultType.c_str(), 
						m_sourceCode.c_str(),
						m_rawLanguage.c_str(), 
						m_isStrict.c_str(),
						m_securityDefiner.c_str());

		m_sourceCode = command;
		UpdateAllViews();


	}
	else
	{
		// This is a result set for the CREATE OR REPLACE FUNCTION command
		if( PQresultStatus( m_result ) == PGRES_COMMAND_OK )
		{
			Modify( false );    // Function saved...
			glApp->getStatusBar()->SetStatusText( _( "Sending changes to server...complete" ), 1 );	
		}
		else
		{
			glApp->getStatusBar()->SetStatusText( _( "Sending changes to server...failed" ), 1 );	
			wxMessageBox( wxString( PQresultErrorMessage( m_result ), wxConvUTF8 ), _( "Can't save changes" ), wxOK | wxICON_EXCLAMATION );
		}
	}	
}

bool wsFuncDoc::OnOpenDocument( const wxString & name )
{
	if( name.StartsWith( wxT( "::" )))
		;
	else
	{
		bool success = false;
		
		wxFile file( name, wxFile::read );

		if( file.IsOpened())
		{
			size_t len = (ssize_t)file.Length();

			if( len > 0 )
			{
				wxChar * buf = m_sourceCode.GetWriteBuf( len );

				if( file.Read( buf, len ) != len )
					return( false );

				m_sourceCode.UngetWriteBuf( len );
			}

			SetFilename( name, true );
			Modify( false );
			UpdateAllViews();

			return( true );
		}
	}
}
