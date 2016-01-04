//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dbgTargetInfo.cpp - debugger
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"
#include "utils/pgDefs.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "debugger/dbgTargetInfo.h"
#include "debugger/dbgConst.h"
#include "utils/misc.h"
#include "utils/pgfeatures.h"

#include <stdexcept>

////////////////////////////////////////////////////////////////////////////////
// dbgTargetInfo constructor
//
//    This class implements a container that holds information necessary to invoke
//  a debugger target (a function or procedure).
//
//  When the constructor is called, it sends a query to the server to retreive:
//    	the OID of the target,
//    	the name of the target,
//    	the name of the schema in which the target is defined
//    	the name of the language in which the target is defined
//    	the number of arguments expected by the target
//    	the argument names
//    	the argument types
//    	the argument modes (IN, OUT, or INOUT)
//    	the target type (function or procedure)
//
//    This class offers a number of (inline) member functions that you can call
//  to extract the above information after it's been queried from the server.

dbgTargetInfo::dbgTargetInfo(Oid _target, pgConn *_conn)
	: m_args(NULL), m_inputParamCnt(0), m_hasVariadic(false)
{
	wxMBConv *conv = _conn->GetConv();
	wxString targetQuery =
	    wxT("SELECT\n")
	    wxT("	p.proname AS name, l.lanname, p.proretset, p.prorettype, y.typname AS rettype,\n")
	    wxT("	CASE WHEN proallargtypes IS NOT NULL THEN\n")
	    wxT("			pg_catalog.array_to_string(ARRAY(\n")
	    wxT("				SELECT\n")
	    wxT("					pg_catalog.format_type(p.proallargtypes[s.i], NULL)\n")
	    wxT("				FROM\n")
	    wxT("					pg_catalog.generate_series(0, pg_catalog.array_upper(\n")
	    wxT("						p.proallargtypes, 1)) AS s(i)), ',')\n")
	    wxT("		ELSE\n")
	    wxT("			pg_catalog.array_to_string(ARRAY(\n")
	    wxT("				SELECT\n")
	    wxT("					pg_catalog.format_type(p.proargtypes[s.i], NULL)\n")
	    wxT("				FROM\n")
	    wxT("					pg_catalog.generate_series(0, pg_catalog.array_upper(\n")
	    wxT("						p.proargtypes, 1)) AS s(i)), ',')\n")
	    wxT("	END AS proargtypenames,\n")
	    wxT("	CASE WHEN proallargtypes IS NOT NULL THEN\n")
	    wxT("			pg_catalog.array_to_string(ARRAY(\n")
	    wxT("				SELECT proallargtypes[s.i] FROM\n")
	    wxT("					pg_catalog.generate_series(0, pg_catalog.array_upper(proallargtypes, 1)) s(i)), ',')\n")
	    wxT("		ELSE\n")
	    wxT("			pg_catalog.array_to_string(ARRAY(\n")
	    wxT("				SELECT proargtypes[s.i] FROM\n")
	    wxT("					pg_catalog.generate_series(0, pg_catalog.array_upper(proargtypes, 1)) s(i)), ',')\n")
	    wxT("	END AS proargtypes,\n")
	    wxT("	pg_catalog.array_to_string(p.proargnames, ',') AS proargnames,\n")
	    wxT("	pg_catalog.array_to_string(proargmodes, ',') AS proargmodes,\n");

	if (_conn->GetIsEdb())
	{
		targetQuery +=
		    wxT("	CASE WHEN n.nspparent <> 0 THEN n.oid ELSE 0 END AS pkg,\n")
		    wxT("	CASE WHEN n.nspparent <> 0 THEN n.nspname ELSE '' END AS pkgname,\n")
		    wxT("	CASE WHEN n.nspparent <> 0 THEN (SELECT oid FROM pg_proc WHERE pronamespace=n.oid AND proname='cons') ELSE 0 END AS pkgconsoid,\n")
		    wxT("	CASE WHEN n.nspparent <> 0 THEN g.oid ELSE n.oid END AS schema,\n")
		    wxT("	CASE WHEN n.nspparent <> 0 THEN g.nspname ELSE n.nspname END AS schemaname,\n")
		    wxT("	NOT (l.lanname = 'edbspl' AND protype = '1') AS isfunc,\n");
	}
	else
	{
		targetQuery +=
		    wxT("	0 AS pkg,\n")
		    wxT("	'' AS pkgname,\n")
		    wxT("	0 AS pkgconsoid,\n")
		    wxT("	n.oid     AS schema,\n")
		    wxT("	n.nspname AS schemaname,\n")
		    wxT("	true AS isfunc,\n");
	}
	if (_conn->BackendMinimumVersion(8, 4))
	{
		targetQuery += wxT("	pg_catalog.pg_get_function_identity_arguments(p.oid) AS signature,");
	}
	else if (_conn->BackendMinimumVersion(8, 1))
	{
		targetQuery +=
		    wxT("	CASE\n")
		    wxT("		WHEN proallargtypes IS NOT NULL THEN pg_catalog.array_to_string(ARRAY(\n")
		    wxT("			SELECT\n")
		    wxT("				CASE\n")
		    wxT("					WHEN p.proargmodes[s.i] = 'i' THEN ''\n")
		    wxT("					WHEN p.proargmodes[s.i] = 'o' THEN 'OUT '\n")
		    wxT("					WHEN p.proargmodes[s.i] = 'b' THEN 'INOUT '\n")
		    wxT("					WHEN p.proargmodes[s.i] = 'v' THEN 'VARIADIC '\n")
		    wxT("				END ||\n")
		    wxT("				CASE WHEN COALESCE(p.proargnames[s.i], '') = '' THEN ''\n")
		    wxT("					ELSE p.proargnames[s.i] || ' '\n")
		    wxT("				END ||\n")
		    wxT("				pg_catalog.format_type(p.proallargtypes[s.i], NULL)\n")
		    wxT("			FROM\n")
		    wxT("				pg_catalog.generate_series(1, pg_catalog.array_upper(p.proallargtypes, 1)) AS s(i)\n")
		    wxT("			WHERE p.proargmodes[s.i] != 't'\n")
		    wxT("			), ', ')\n")
		    wxT("		ELSE\n")
		    wxT("			pg_catalog.array_to_string(ARRAY(\n")
		    wxT("				SELECT\n")
		    wxT("					CASE\n")
		    wxT("						WHEN COALESCE(p.proargnames[s.i+1], '') = '' THEN ''\n")
		    wxT("						ELSE p.proargnames[s.i+1] || ' '\n")
		    wxT("					END ||\n")
		    wxT("					pg_catalog.format_type(p.proargtypes[s.i], NULL)\n")
		    wxT("				FROM\n")
		    wxT("					pg_catalog.generate_series(1, pg_catalog.array_upper(p.proargtypes, 1)) AS s(i)\n")
		    wxT("				), ', ')\n")
		    wxT("	END AS signature,\n");
	}
	else
	{
		targetQuery += wxT("	'' AS signature,");
	}

	if (_conn->HasFeature(FEATURE_FUNCTION_DEFAULTS))
	{
		// EnterpriseDB 8.3R2
		if(!_conn->BackendMinimumVersion(8, 4))
		{
			targetQuery +=
			    wxT("	pg_catalog.array_to_string(ARRAY(\n")
			    wxT("	SELECT\n")
			    wxT("		CASE WHEN p.proargdefvals[x.j] != '-' THEN\n")
			    wxT("			pg_catalog.pg_get_expr(p.proargdefvals[x.j], 'pg_catalog.pg_class'::regclass, true)\n")
			    wxT("		ELSE '-' END\n")
			    wxT("	FROM\n")
			    wxT("		pg_catalog.generate_series(1, pg_catalog.array_upper(p.proargdefvals, 1)) AS x(j)\n")
			    wxT("	), ',') AS proargdefaults,\n")
			    wxT("	CASE WHEN p.proargdefvals IS NULL THEN '0'\n")
			    wxT("		ELSE pg_catalog.array_upper(p.proargdefvals, 1)::text END AS pronargdefaults\n");
		}
		else
		{
			targetQuery +=
			    wxT("	pg_catalog.pg_get_expr(p.proargdefaults, 'pg_catalog.pg_class'::regclass, false) AS proargdefaults,\n")
			    wxT("	p.pronargdefaults\n");
		}
	}
	else
	{
		targetQuery +=
		    wxT("	'' AS proargdefaults, 0 AS pronargdefaults\n");
	}
	targetQuery +=
	    wxT("FROM\n")
	    wxT("	pg_catalog.pg_proc p\n")
	    wxT("	LEFT JOIN pg_catalog.pg_namespace n ON p.pronamespace = n.oid\n")
	    wxT("	LEFT JOIN pg_catalog.pg_language l ON p.prolang = l.oid\n")
	    wxT("	LEFT JOIN pg_catalog.pg_type y ON p.prorettype = y.oid\n");
	if(_conn->GetIsEdb())
	{
		targetQuery +=
		    wxT("	LEFT JOIN pg_catalog.pg_namespace g ON n.nspparent = g.oid\n");
	}
	targetQuery +=
	    wxString::Format(wxT("WHERE p.oid = %ld"), (long)_target);

	pgSet *set = _conn->ExecuteSet(targetQuery);

	if (conv == NULL)
	{
		conv = &wxConvLibc;
	}

	if (!set || _conn->GetLastResultStatus() != PGRES_TUPLES_OK)
	{
		if (set)
			delete set;
		wxLogError(_("Could not fetch information about the debugger target.\n") +
		           _conn->GetLastError());

		throw (std::runtime_error(
		           (const char *)(_conn->GetLastError().c_str())));
	}

	if (set->NumRows() == 0)
	{
		delete set;

		wxLogError(_("Can't find the debugging target"));
		throw (std::runtime_error("Can't find target!"));
	}

	m_oid           = _target;
	m_name          = set->GetVal(wxT("name"));
	m_schema        = set->GetVal(wxT("schemaname"));
	m_package       = set->GetVal(wxT("pkgname"));
	m_language      = set->GetVal(wxT("lanname"));
	m_returnType    = set->GetVal(wxT("rettype"));
	m_funcSignature = set->GetVal(wxT("signature"));
	m_isFunction    = set->GetBool(wxT("isfunc"));
	m_returnsSet    = set->GetBool(wxT("proretset"));
	m_pkgOid        = set->GetOid(wxT("pkg"));
	m_pkgInitOid    = set->GetOid(wxT("pkgconsoid"));
	m_schemaOid     = set->GetOid(wxT("schema"));
	m_fqName        = qtIdent(m_schema) + wxT(".") +
	                  (m_pkgOid == 0 ? wxT("") : (qtIdent(m_package) + wxT("."))) + qtIdent(m_name);

	wxArrayString argModes, argNames, argTypes, argTypeOids, argDefVals,
	              argBaseTypes;

	// Fetch Argument Modes (if available)
	if (!set->IsNull(set->ColNumber(wxT("proargmodes"))))
	{
		wxString tmp;
		tmp = set->GetVal(wxT("proargmodes"));

		if (!tmp.IsEmpty())
			getArrayFromCommaSeparatedList(tmp, argModes);
	}
	// Fetch Argument Names (if available)
	if (!set->IsNull(set->ColNumber(wxT("proargnames"))))
	{
		wxString tmp;
		tmp = set->GetVal(wxT("proargnames"));

		if (!tmp.IsEmpty())
			getArrayFromCommaSeparatedList(tmp, argNames);
	}
	// Fetch Argument Type-Names (if available)
	if (!set->IsNull(set->ColNumber(wxT("proargtypenames"))))
	{
		wxString tmp;
		tmp = set->GetVal(wxT("proargtypenames"));

		if (!tmp.IsEmpty())
			getArrayFromCommaSeparatedList(tmp, argTypes);
	}
	// Fetch Argument Type-Names (if available)
	if (!set->IsNull(set->ColNumber(wxT("proargtypes"))))
	{
		wxString tmp;
		tmp = set->GetVal(wxT("proargtypes"));
		if (!tmp.IsEmpty())
			getArrayFromCommaSeparatedList(tmp, argTypeOids);
	}

	size_t nArgDefs = (size_t)set->GetLong(wxT("pronargdefaults"));
	// Fetch Argument Default Values (if available)
	if (!set->IsNull(set->ColNumber(wxT("proargdefaults"))) && nArgDefs != 0)
	{
		wxString tmp;
		tmp = set->GetVal(wxT("proargdefaults"));

		if (!tmp.IsEmpty())
			getArrayFromCommaSeparatedList(tmp, argDefVals);
	}

	wxString argName, argDefVal;
	short    argMode;
	Oid      argTypeOid;
	size_t   argCnt = argTypes.Count();

	// This function/procedure does not take any arguments
	if (argCnt == 0)
	{
		return;
	}

	size_t idx = 0;
	m_args = new pgDbgArgs();

	for (; idx < argCnt; idx++)
	{
		argTypeOid = (Oid)strtoul(argTypeOids[idx].mb_str(wxConvUTF8), 0, 10);
		argDefVal  = wxEmptyString;

		argName = wxEmptyString;
		if (idx < argNames.Count())
			argName = argNames[idx];

		if (argName.IsEmpty())
			argName.Printf( wxT( "dbgParam%d" ), (idx + 1));

		if (idx < argModes.Count())
		{
			wxString tmp = argModes[idx];
			switch ((char)(tmp.c_str())[0])
			{
				case 'i':
					argMode = pgParam::PG_PARAM_IN;
					m_inputParamCnt++;
					break;
				case 'b':
					m_inputParamCnt++;
					argMode = pgParam::PG_PARAM_INOUT;
					break;
				case 'o':
					argMode = pgParam::PG_PARAM_OUT;
					break;
				case 'v':
					m_inputParamCnt++;
					argMode = pgParam::PG_PARAM_VARIADIC;
					m_hasVariadic = true;
					break;
				case 't':
					continue;
				default:
					m_inputParamCnt++;
					argMode = pgParam::PG_PARAM_IN;
					break;
			}
		}
		else
		{
			m_inputParamCnt++;
			argMode = pgParam::PG_PARAM_IN;
		}

		// In EDBAS 90, if an SPL-function has both an OUT-parameter
		// and a return value (which is not possible on PostgreSQL otherwise),
		// the return value is transformed into an extra OUT-parameter
		// named "_retval_"
		if (argName == wxT("_retval_") && _conn->EdbMinimumVersion(9, 0))
		{
			// this will be the return type for this object
			m_returnType = argTypes[idx];

			continue;
		}

		m_args->Add(new dbgArgInfo(argName, argTypes[idx], argTypeOid, argMode));
	}

	if (m_args->GetCount() == 0)
	{
		delete m_args;
		m_args = NULL;

		return;
	}

	if (nArgDefs != 0)
	{
		argCnt = m_args->GetCount();

		// Set the default as the value for the argument
		for (idx = argCnt - 1;; idx--)
		{
			dbgArgInfo *arg = (dbgArgInfo *)((*m_args)[idx]);

			if (arg->GetMode() == pgParam::PG_PARAM_INOUT ||
			        arg->GetMode() == pgParam::PG_PARAM_IN)
			{
				nArgDefs--;

				if (argDefVals[nArgDefs] != wxT("-"))
				{
					arg->SetDefault(argDefVals[nArgDefs]);
				}

				if (nArgDefs == 0)
				{
					break;
				}
			}
			if (idx == 0)
				break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// operator[]
//
//    This operator function makes it easy to index into the m_args array
//    using concise syntax.
dbgArgInfo *dbgTargetInfo::operator[](int index)
{
	if (m_args == NULL)
		return (dbgArgInfo *)NULL;

	if (index < 0 || index >= (int)m_args->GetCount())
		return (dbgArgInfo *)NULL;

	return (dbgArgInfo *)((*m_args)[index]);
}

dbgArgInfo::dbgArgInfo(const wxString &_name, const wxString &_type, Oid _typeOid,
                       short _mode)
	: m_name(_name), m_type(_type), m_typeOid(_typeOid), m_mode(_mode),
	  m_hasDefault(false), m_useDefault(false), m_null(false)
{
	if (!_type.EndsWith(wxT("[]"), &m_baseType))
	{
		m_baseType = wxEmptyString;
	}
}


pgParam *dbgArgInfo::GetParam(wxMBConv *_conv)
{
	return new pgParam(m_typeOid,
	                   (m_null ? (wxString *)NULL : &m_val),
	                   _conv, m_mode);
}

bool dbgTargetInfo::AddForExecution(pgQueryThread *_thread)
{
	wxASSERT(_thread != NULL);

	if (_thread == NULL)
		return false;

	pgConn *conn = _thread->GetConn();

	pgParamsArray *params   = NULL;
	wxString       strQuery;
	bool           useCallable = false;

	// Basically - we can call the function/target three ways:
	// 1. If it is a edbspl procedure, we can use callable statement
	// 2. If the database server is of type EnterpriseDB, and
	//    function/procedure is type 'edbspl', we should use the anonymous
	//    function block for:
	//    a. Version < 9.0
	//    b. Package function/procedure
	// 3. Otherwise, we should use the simple function call (except using EXEC
	//    for the procedure in 'edbspl' instead of using SELECT)
	if (_thread->SupportCallableStatement() &&
	        m_language == wxT("edbspl") &&
	        !m_isFunction)
	{
		useCallable = true;
		strQuery = wxT("CALL ") + m_fqName + wxT("(");

		if (m_args)
		{
			params = new pgParamsArray();
			wxMBConv *conv = conn->GetConv();

			for(int idx = 0; idx < (int)m_args->GetCount(); idx++)
			{
				params->Add(((*m_args)[idx])->GetParam(conv));

				if (idx != 0)
					strQuery += wxT(", ");
				strQuery += wxString::Format(wxT("$%d::"), idx + 1) +
				            ((*m_args)[idx])->GetTypeName();
			}
		}
		strQuery += wxT(")");
	}
	else if (conn->GetIsEdb() && !conn->BackendMinimumVersion(9, 3))
	{
		wxString strDeclare, strStatement, strResult;
		bool useAnonymousBlock = false;

		if (m_language == wxT("edbspl"))
		{
			useAnonymousBlock = true;
			if (!m_isFunction)
			{
				strStatement = wxT("\tEXEC ") + m_fqName;
			}
			else if (m_returnType == wxT("void") || m_returnsSet ||
			         !conn->BackendMinimumVersion(8, 4))
			{
				strStatement = wxT("\tPERFORM ") + m_fqName;
			}
			else
			{
				wxString resultVar = wxT("v_retVal");
				strStatement = wxT("\t") + resultVar + wxT(" := ") + m_fqName;
				strDeclare.Append(wxT("\t"))
				.Append(resultVar)
				.Append(wxT(" "))
				.Append(m_returnType)
				.Append(wxT(";\n"));
				strResult = wxT("\tDBMS_OUTPUT.PUT_LINE(E'\\n\\nResult:\\n--------\\n' || ") +
				            resultVar +
				            wxT("::text || E'\\n\\nNOTE: This is the result generated during the function execution by the debugger.\\n');\n");
			}
		}
		else
		{
			if (m_returnType == wxT("record"))
			{
				strStatement = wxT("\tSELECT ") + m_fqName;
			}
			else
			{
				strStatement = wxT("\tSELECT * FROM ") + m_fqName;
			}
		}

		if (m_args && m_args->GetCount() > 0)
		{
			strStatement.Append(wxT("("));

			for(int idx = 0, firstProcessed = false; idx < (int)m_args->GetCount(); idx++)
			{
				dbgArgInfo *arg = (*m_args)[idx];

				if (!conn->EdbMinimumVersion(8, 4) &&
				        arg->GetMode() == pgParam::PG_PARAM_OUT &&
				        (!m_isFunction || m_language == wxT("edbspl")))
				{
					if (firstProcessed)
						strStatement.Append(wxT(", "));
					firstProcessed = true;

					strStatement.Append(wxT("NULL::")).Append(arg->GetTypeName());
				}
				else if (conn->EdbMinimumVersion(8, 4) && useAnonymousBlock &&
				         (arg->GetMode() == pgParam::PG_PARAM_OUT ||
				          arg->GetMode() == pgParam::PG_PARAM_INOUT))
				{
					wxString strParam = wxString::Format(wxT("p_param%d"), idx);

					strDeclare.Append(wxT("\t"))
					.Append(strParam)
					.Append(wxT(" "))
					.Append(arg->GetTypeName());

					if (arg->GetMode() == pgParam::PG_PARAM_INOUT)
					{
						strDeclare.Append(wxT(" := "))
						.Append(arg->Null() ? wxT("NULL") : conn->qtDbString(arg->Value()))
						.Append(wxT("::"))
						.Append(arg->GetTypeName());
					}
					strDeclare.Append(wxT(";\n"));

					if (firstProcessed)
						strStatement.Append(wxT(", "));
					firstProcessed = true;

					strStatement.Append(strParam);
				}
				else if (arg->GetMode() != pgParam::PG_PARAM_OUT)
				{
					if (firstProcessed)
						strStatement.Append(wxT(", "));
					firstProcessed = true;

					if (arg->GetMode() == pgParam::PG_PARAM_VARIADIC)
						strStatement += wxT("VARIADIC ");

					strStatement
					.Append(arg->Null() ? wxT("NULL") : conn->qtDbString(arg->Value()))
					.Append(wxT("::"))
					.Append(arg->GetTypeName());
				}
			}
			strStatement.Append(wxT(")"));
			strQuery = strStatement;
		}
		else if (!m_isFunction && m_language == wxT("edbspl"))
		{
			strQuery = strStatement;
		}
		else
		{
			strQuery = strStatement.Append(wxT("()"));
		}
		if (useAnonymousBlock)
		{
			strQuery = wxT("DECLARE\n") +
			           strDeclare + wxT("BEGIN\n") + strStatement + wxT(";\n") + strResult + wxT("END;");
		}
	}
	else
	{
		if (!m_isFunction)
		{
			strQuery = wxT("EXEC ") + m_fqName + wxT("(");
		}
		else if (m_returnType == wxT("record"))
		{
			strQuery = wxT("SELECT ") + m_fqName + wxT("(");
		}
		else
		{
			strQuery = wxT("SELECT * FROM ") + m_fqName + wxT("(");
		}

		if (m_args)
		{
			params = new pgParamsArray();
			wxMBConv *conv = conn->GetConv();
			unsigned int noInParams = 0;

			for(int idx = 0; idx < (int)m_args->GetCount(); idx++)
			{
				dbgArgInfo *arg = (*m_args)[idx];

				if (arg->GetMode() != pgParam::PG_PARAM_OUT)
				{
					params->Add(arg->GetParam(conv));

					if (noInParams != 0)
						strQuery += wxT(", ");

					if (arg->GetMode() == pgParam::PG_PARAM_VARIADIC)
						strQuery += wxT("VARIADIC ");

					noInParams++;
					strQuery += wxString::Format(wxT("$%d::"), noInParams) +
					            ((*m_args)[idx])->GetTypeName();
				}
			}

			/*
			 * The function may not have IN/IN OUT/VARIADIC arguments, but only
			 * OUT one(s).
			 */
			if (params->GetCount() == 0)
			{
				delete params;
				params = NULL;
			}
		}
		strQuery += wxT(")");
	}

	_thread->AddQuery(strQuery, params, RESULT_ID_DIRECT_TARGET_COMPLETE, NULL, useCallable);

	return true;
}
