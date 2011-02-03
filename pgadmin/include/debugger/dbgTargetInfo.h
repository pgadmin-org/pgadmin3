//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dbgTargetInfo.h - debugger
//
//////////////////////////////////////////////////////////////////////////
#ifndef DBGTARGETINFO_H
#define DBGTARGETINFO_H

////////////////////////////////////////////////////////////////////////////////
// class wsArgInfo
//
//	A wsArgInfo object contains information about a function (or procedure)
//  argument.  Inside of each wsArgInfo object, we store the name of the argument,
//	the argument type, and the argument mode (IN (i), OUT (o), or INOUT (b)).
//
//	Once the user has had a chance to enter values for each of the IN and INOUT
//	arguments, we store those values inside of the corresponding wsArgInfo objects

class wsArgInfo
{
public:
	wsArgInfo( const wxString &argName, const wxString &argType, const wxString &argMode, const wxString &argTypeOid );

	const wxString &getName()
	{
		return m_name;
	}
	const wxString &getType()
	{
		return m_type;
	}
	const wxString &getMode()
	{
		return m_mode;
	}
	const Oid getTypeOid()
	{
		return m_typeOid;
	}
	wxString &getValue()
	{
		return m_value;    // NOTE: non-const, caller may modifiy value
	}
	const wxString   quoteValue();
	void  setValue( const wxString &newValue )
	{
		m_value = newValue;
	}

private:
	wxString	m_name;
	wxString	m_type;
	wxString	m_mode;
	wxString	m_value;
	Oid         m_typeOid;
};

WX_DECLARE_OBJARRAY(wsArgInfo, wsArgInfoArray);

////////////////////////////////////////////////////////////////////////////////
// class dbgTargetInfo
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

class dbgPgConn;

class dbgTargetInfo
{
public:
	dbgTargetInfo( const wxString &target, dbgPgConn *conn, char targetType );

	int	getArgInCount()
	{
		return( m_argInCount );
	}
	int 	getArgOutCount()
	{
		return( m_argOutCount );
	}
	int 	getArgInOutCount()
	{
		return( m_argInOutCount );
	}
	int	getArgCount()
	{
		return( m_argInCount + m_argOutCount + m_argInOutCount );
	}

	const wxString &getLanguage()
	{
		return( m_language );
	}
	const wxString &getSchema()
	{
		return( m_schema );
	}
	const wxString &getName()
	{
		return( m_name );
	}
	const wxString &getFQName()
	{
		return( m_fqName );
	}
	const wxString &getReturnType()
	{
		return( m_returnType );
	}
	long	getOid()
	{
		return( m_oid );
	}
	long	getPkgOid()
	{
		return( m_pkgOid );
	}
	long	getPkgInitOid()
	{
		return( m_pkgInitOid );
	}
	bool	getIsFunction()
	{
		return( m_isFunction );
	}
	bool	getReturnsSet()
	{
		return( m_returnsSet );
	}

	wsArgInfo &operator[]( int index );

private:
	wxString	m_name;		 // Target name (function or procedure)
	wxString	m_schema;	 // Schema in which target resides
	wxString	m_language;	 // Language in which target is defined
	wxString	m_argNames;	 // Argument names
	wxString	m_argModes;	 // Argument modes
	wxString	m_argTypes;	 // Argument types
	wxString	m_argTypeOids; // Argument type OIDs
	wxString	m_fqName;	 // Fully-qualified name (schema.package.func or package.func)
	wxString    m_returnType;// Return type
	bool	m_isFunction;	 // true->target is a function, false->target is a procedure
	bool    m_returnsSet;    // Returns a set?
	int	m_argInCount;	 // Number of IN arguments
	int	m_argOutCount;	 // Number of OUT arguments
	int	m_argInOutCount; // Number of INOUT arguments
	long	m_oid;		 // Target function/procedure OID
	long	m_pkgOid;	 // Package in which target defined (if non-zero)
	long    m_pkgInitOid; // OID of the package initializer function.
	wsArgInfoArray	m_argInfo;

};

#endif
