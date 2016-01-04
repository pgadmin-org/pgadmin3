//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dbgTargetInfo.h - debugger
//
//////////////////////////////////////////////////////////////////////////
#ifndef DBGTARGETINFO_H
#define DBGTARGETINFO_H

#include <wx/wx.h>
#include "db/pgConn.h"
#include "db/pgQueryThread.h"


////////////////////////////////////////////////////////////////////////////////
// class dbgArgInfo
//
//	A dbgArgInfo object contains information about a function (or procedure)
//  argument.  Inside of each wsArgInfo object, we store the name of the argument,
//	the argument type, and the argument mode (IN (i), OUT (o), or INOUT (b)).
//
//	Once the user has had a chance to enter values for each of the IN and INOUT
//	arguments, we store those values inside of the corresponding wsArgInfo objects
class dbgArgInfo
{
public:
	dbgArgInfo(const wxString &_name, const wxString &_type, Oid _typeOid,
	           short _mode = pgParam::PG_PARAM_IN);

	Oid             GetType()
	{
		return m_typeOid;
	}
	const wxString &GetTypeName()
	{
		return m_type;
	}
	wxString       &GetName()
	{
		return m_name;
	}
	short           GetMode()
	{
		return m_mode;
	}
	bool            IsArray()
	{
		return !(m_baseType.IsEmpty());
	}
	const wxString &GetBaseType()
	{
		return m_baseType;
	}
	bool           &Null()
	{
		return m_null;
	}
	wxString       &Value()
	{
		return m_val;
	}

	void            SetDefault(const wxString &val)
	{
		m_hasDefault = true;
		m_defValue = val;
	}
	wxString       &Default()
	{
		return m_defValue;
	}
	bool            HasDefault()
	{
		return m_hasDefault;
	}
	bool           &UseDefault()
	{
		return m_useDefault;
	}

	pgParam *GetParam(wxMBConv *_conv = NULL);

private:
	dbgArgInfo(const dbgArgInfo &_arg)
	{
		wxASSERT(0);
	}

	dbgArgInfo &operator= (const dbgArgInfo &)
	{
		wxASSERT(0);
		return *this;
	}

	wxString   m_name;       /* Name of the argument */
	wxString   m_type;       /* Type of the argument */
	wxString   m_baseType;   /* Base type of an array type */
	wxString   m_defValue;   /* Default Value */
	Oid        m_typeOid;    /* OID of Type */
	short      m_mode;       /* IN, IN OUT, OUT, or VARAIDIC */
	bool       m_hasDefault; /* Has the default value? */
	bool       m_useDefault; /* Use the default value? */
	bool       m_null;       /* Is Value NULL */
	wxString   m_val;        /* Value */
};

WX_DEFINE_ARRAY_PTR(dbgArgInfo *, pgDbgArgs);

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
//
class dbgTargetInfo
{
public:
	dbgTargetInfo(Oid _target, pgConn *_conn);

	const wxString &GetQualifiedName()
	{
		return m_fqName;
	}

	const wxString &GetLanguage()
	{
		return m_language;
	}

	const wxString &GetPackageName()
	{
		return m_package;
	}
	const wxString &GetSchemaName()
	{
		return m_schema;
	}

	const wxString &GetName()
	{
		return m_name;
	}

	const wxString &GetReturnType()
	{
		return m_returnType;
	}

	long GetOid()
	{
		return m_oid;
	}

	long GetPkgOid()
	{
		return m_pkgOid;
	}

	long GetSchemaOid()
	{
		return m_schemaOid;
	}

	long GetPkgInitOid()
	{
		return m_pkgInitOid;
	}

	bool GetIsFunction()
	{
		return m_isFunction;
	}

	bool GetReturnsSet()
	{
		return m_returnsSet;
	}

	bool RequireUserInput()
	{
		return ((m_pkgOid != 0 && m_pkgInitOid != 0) || m_inputParamCnt != 0);
	}

	bool &DebugPackageConstructor()
	{
		return m_debugPkgCon;
	}

	dbgArgInfo *operator[](int index);

	pgDbgArgs *GetArgs()
	{
		return m_args;
	}

	bool HasVariadic()
	{
		return m_hasVariadic;
	}

	bool AddForExecution(pgQueryThread *_thread);

private:
	wxString   m_name;          // Target name (function or procedure)
	wxString   m_schema;        // Schema in which target resides
	wxString   m_package;       // Package in which target resides
	wxString   m_language;      // Language in which target is defined

	wxString   m_returnType;    // Return type
	wxString   m_funcSignature; // Function Signature

	wxString   m_fqName;        // Function qualified name

	bool       m_isFunction;    // true->target is a function, false->target is a procedure
	bool       m_returnsSet;    // Returns a set?
	bool       m_hasVariadic;   // Has the variadic argument

	bool       m_debugPkgCon;    // Debug Package Constructor

	long       m_oid;           // Target function/procedure OID
	long       m_pkgOid;        // Package in which target defined (if non-zero)
	long       m_pkgInitOid;    // OID of the package initializer function.
	long       m_schemaOid;     // OID of the schema

	size_t     m_inputParamCnt; // IN/IN OUT parameter count

	pgDbgArgs *m_args;          // Function arguments
};

#endif
