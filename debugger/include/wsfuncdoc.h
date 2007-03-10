//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: wsfuncdoc.h 5827 2007-01-04 16:35:14 hiroshi $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsfuncdoc.h - debugger 
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// 	class wsFuncDoc
//
// 	This class implements the "function document" in the document/view 
//	architecture.  A wsFuncDoc object represents an editable document that
//	happens to be the source code for a function/procedure.  We create a 
//  new wsFuncDoc object whenever you open a function for editing.
//
//	Each wsFuncDoc object corresponds to a single function/procedure.  For 
//  each function/procedure, we keep track of the function signature (name+
//	argument types), the function source code, result type, schema, owner,
//  and other function-related attributes.
//
//	A wsFuncDoc object knows how to retreive it's definition from the server
//  (see readFunction()) and knows how to save itself back to the server (see
//   OnSaveDocument()).

#ifndef WSFUNCDOCH
#define WSFUNCDOCH

#include "wspgconn.h"
#include "wsdbresult.h"

#include <wx/docview.h>
#include <wx/cmdproc.h>

class wsFuncDoc : public wxDocument
{
	DECLARE_DYNAMIC_CLASS( wsFuncDoc )

public:

	// You should only create wsFuncDoc objects by calling OpenDoc() (notice that this is a static function)
	static wsFuncDoc * OpenDoc( const wxString & sourceCode, const wxString & title, const wxString & funcOid, int startingPos );
	~wsFuncDoc() { }

	wxString  		 & getSourceCode();									// Returns a reference to the source code (string)
	void  			 * getDocHandle( void * defaultHandle );			// Sets/returns handle to the STC handle for this document

private:
	wsFuncDoc();														// Hide the default constructor
	wsFuncDoc( const wsFuncDoc & src );									// Prohibit the copy constructor (unimplemented)
	wsFuncDoc & operator=( const wsFuncDoc & src );						// Prohibit copying (unimplemented)

	wsFuncDoc( const wxString & sourceCode, const wxString & title, const wxString & funcOID );

	virtual bool 	   	OnSaveDocument( const wxString & fileName );	// Called when the user chooses to Save this document
	virtual bool	    OnOpenDocument( const wxString & name );		// Called when the user chooses to Open a document
	virtual bool 	    IsModified( void ) const;						// Returns true if user has modified this document
	virtual void 	   	Modify( bool mod );								// Mark the document as modified (or unmodified if mod == false)
	        void		readFunction( const wxString & funcOID );		// Read the function definition from the server
			void 	   	OnResultReady( wxCommandEvent & event );		// Called when a query completes
			wxString   	makeSignature();								// Returns quoted function signature (schema.name(arg1 type1, arg2 type2, ...))
			wsPgConn * 	getPgConn();									// Returns pointer to the database connection handle
			bool		saveToServer( const wxString & fileName, const wxString & command );
			bool		saveToFile( const wxString & fileName, const wxString & command );

	wxString	m_sourceCode;			// Contains the (original unmodified) source code for this function
	void      * m_docHandle;			// STC handle for this document (coordinates multiple views)
	wxString	m_oid;					// OID of this function
	wsPgConn  * m_conn;					// Connection to the PG server 
	PGresult  * m_result;				// PostgreSQL esult set 

	wxString	m_rawArgNames;			// Unquoted argument names {name1,name2,...}
	wxString	m_rawArgTypes;			// Unquoted argument types type1,type2,...
	wxString	m_rawSchema;			// Unquoted schema name
	wxString	m_rawName;				// Unquoted function name
	wxString	m_rawResultType;		// Unquoted result type
	wxString	m_rawOwner;				// Unquoted owner name
	wxString	m_rawLanguage;			// Unquoted implementation language
	wxString	m_comment;				// Quoted comment
	wxString	m_isStrict;				// STRICT qualifier (or blank)
	wxString	m_securityDefiner;		// SECURITY DEFINER qualifier (or blank)

	DECLARE_EVENT_TABLE()
};

#endif
