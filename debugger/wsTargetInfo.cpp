//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wstargetinfo.cpp - debugger 
//
//////////////////////////////////////////////////////////////////////////

#include "debugger.h"
#include "wsTargetInfo.h"
#include "wsResultset.h"
#include "wsPgconn.h"
#include <wx/tokenzr.h>
#include <wx/arrimpl.cpp>
#include <stdexcept>

WX_DEFINE_OBJARRAY( wsArgInfoArray );

#define COL_TARGET_NAME		"targetname"
#define COL_SCHEMA_NAME		"nspname"
#define COL_LANGUAGE_NAME	"lanname"
#define COL_ARG_NAMES		"argnames"
#define COL_ARG_MODES		"argmodes"
#define COL_ARG_TYPES		"argtypes"
#define COL_IS_FUNCTION		"isfunc"
#define COL_TARGET_OID		"target"
#define COL_PACKAGE_OID		"pkg"
#define COL_FQ_NAME			"fqname"		/* Fully qualified name	*/

////////////////////////////////////////////////////////////////////////////////
// wsTargetInfo constructor
//
//	This class implements a container that holds information necessary to invoke
//  a debugger target (a function or procedure).
//
//  When the constructor is called, it sends a query to the server to retreive:
//		the OID of the target,
//		the name of the target,
//		the name of the schema in which the target is defined
//		the name of the language in which the target is defined
//		the number of arguments expected by the target
//		the argument names
//		the argument types
//		the argument modes (IN, OUT, or INOUT)
//		the target type (function or procedure)
//
//	This class offers a number of (inline) member functions that you can call
//  to extract the above information after it's been queried from the server.

wsTargetInfo::wsTargetInfo( const wxString & target,  wsPgConn * conn, char targetType )
{
	wxString query = 
		wxT("select")
		wxT("  t.target, t.pkg, t.targetname, t.argnames, t.argmodes, t.isfunc, t.fqname,")
		wxT("  pg_catalog.oidvectortypes( t.argtypes ) as argtypes,")
		wxT("  l.lanname, n.nspname")
		wxT(" from")
		wxT("  pldbg_get_target_info( '%s', '%c' ) t , pg_namespace n, pg_language l")
		wxT(" where")
		wxT("  n.oid = t.schema and ")
		wxT("  l.oid = t.targetlang" );

	wsResultSet * result = new wsResultSet( conn->waitForCommand( wxString::Format( query, target.c_str(), targetType )));

	if( result->getCommandStatus() != PGRES_TUPLES_OK )
		throw( std::runtime_error( result->getRawErrorMessage()));

	m_name     	 = result->getString( wxString( COL_TARGET_NAME, wxConvUTF8 ));
	m_schema   	 = result->getString( wxString( COL_SCHEMA_NAME, wxConvUTF8 ));
	m_language 	 = result->getString( wxString( COL_LANGUAGE_NAME, wxConvUTF8 ));
	m_argNames 	 = result->getString( wxString( COL_ARG_NAMES, wxConvUTF8 ));
	m_argModes 	 = result->getString( wxString( COL_ARG_MODES, wxConvUTF8 ));
	m_argTypes 	 = result->getString( wxString( COL_ARG_TYPES, wxConvUTF8 ));
	m_isFunction = result->getBool( wxString( COL_IS_FUNCTION, wxConvUTF8 ));
	m_oid      	 = result->getLong( wxString( COL_TARGET_OID, wxConvUTF8 ));
	m_pkgOid	 = result->getLong( wxString( COL_PACKAGE_OID, wxConvUTF8 ));
	m_fqName	 = result->getString( wxString( COL_FQ_NAME, wxConvUTF8 ));

	// Parse out the argument types, names, and modes
  
	// By creating a tokenizer with wxTOKEN_STRTOK and a delimiter string
	// that contains ",{}", we can parse out PostgreSQL array strings like:
	//	 {int, varchar, numeric}

	wxStringTokenizer names( m_argNames, wxT( ",{}" ), wxTOKEN_STRTOK );
	wxStringTokenizer types( m_argTypes, wxT( ",{}" ), wxTOKEN_STRTOK );
	wxStringTokenizer modes( m_argModes, wxT( ",{}" ), wxTOKEN_STRTOK );

	// Create one wsArgInfo for each target argument

	m_argInCount = m_argOutCount = m_argInOutCount = 0;
	int	argCount = 0;

	while( types.HasMoreTokens())
	{
		argCount++;

		wxString	argName = names.GetNextToken();

		if( argName.IsEmpty())
			argName.Printf( wxT( "$%d" ), argCount );

		wsArgInfo	argInfo( argName, types.GetNextToken(), modes.GetNextToken());

		if( argInfo.getMode() == wxT( "i" ))
			m_argInCount++;
		else if( argInfo.getMode() == wxT( "o" ))
			m_argOutCount++;
		else if( argInfo.getMode() == wxT( "b" ))
			m_argInOutCount++;

		m_argInfo.Add( argInfo );
	}
}

////////////////////////////////////////////////////////////////////////////////
// operator[]
//
//	This operator function makes it easy to index into the m_argInfo[] array
//	using concise syntax.

wsArgInfo & wsTargetInfo::operator[]( int index )
{
	return( m_argInfo[index] );
}

////////////////////////////////////////////////////////////////////////////////
// wsArgInfo constructor
//
//	A wsArgInfo object contains information about a function (or procedure) 
//  argument.  Inside of each wsArgInfo object, we store the name of the argument,
//	the argument type, and the argument mode (IN (i), OUT (o), or INOUT (b)).
//
//	Once the user has had a chance to enter values for each of the IN and INOUT
//	arguments, we store those values inside of the corresponding wsArgInfo objects

wsArgInfo::wsArgInfo( const wxString & argName, const wxString & argType, const wxString & argMode )
	: m_name( argName.Strip( wxString::both )),
	  m_type( argType.Strip( wxString::both )),
	  m_mode( argMode == wxT( "" ) ? wxT( "i" ) : argMode.Strip( wxString::both )),
	  m_value()
{
}

////////////////////////////////////////////////////////////////////////////////
// quoteValue()
//
//	This function will extract the argument value from the given wsArgInfo object
//  and will wrap that value in single quotes (unless the value has already been
//	quoted).  If the argument value (entered by the user) is blank, we return NULL
//	instead of a quoted string.
//
//	NOTE: we quote all value regardless of type - it's perfectly valid to quote a 
//  	  numeric (or boolean) value in PostgreSQL

const wxString wsArgInfo::quoteValue()
{
	if( m_value.CmpNoCase( wxT( "NULL" )) == 0 )
		return( m_value );
	else if( m_value[0] == '\'' )
		return( m_value );
	else
		return( wxString( wxT( "'" ) + m_value + wxT( "'" )));
}
