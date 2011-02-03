//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgFunction.cpp - function class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "frm/menu.h"
#include "utils/pgfeatures.h"
#include "utils/misc.h"
#include "schema/pgFunction.h"
#include "frm/frmReport.h"
#include "frm/frmHint.h"

pgFunction::pgFunction(pgSchema *newSchema, const wxString &newName)
	: pgSchemaObject(newSchema, functionFactory, newName)
{
}


pgFunction::pgFunction(pgSchema *newSchema, pgaFactory &factory, const wxString &newName)
	: pgSchemaObject(newSchema, factory, newName)
{
}

pgTriggerFunction::pgTriggerFunction(pgSchema *newSchema, const wxString &newName)
	: pgFunction(newSchema, triggerFunctionFactory, newName)
{
}

pgProcedure::pgProcedure(pgSchema *newSchema, const wxString &newName)
	: pgFunction(newSchema, procedureFactory, newName)
{
}

void pgFunction::ShowStatistics(frmMain *form, ctlListView *statistics)
{
	if (GetConnection()->BackendMinimumVersion(8, 4))
	{
		wxString sql = wxT("SELECT calls AS ") + qtIdent(_("Number of calls")) +
		               wxT(", total_time AS ") + qtIdent(_("Total Time")) +
		               wxT(", self_time AS ") + qtIdent(_("Self Time")) +
		               wxT(" FROM pg_stat_user_functions") +
		               wxT(" WHERE schemaname = ") + qtDbString(GetSchema()->GetName()) +
		               wxT(" AND funcname = ") + qtDbString(GetName());
		DisplayStatistics(statistics, sql);
	}
}

bool pgFunction::IsUpToDate()
{
	wxString sql = wxT("SELECT xmin FROM pg_proc WHERE oid = ") + this->GetOidStr();
	if (!this->GetDatabase()->GetConnection() || this->GetDatabase()->ExecuteScalar(sql) != NumToStr(GetXid()))
		return false;
	else
		return true;
}

bool pgFunction::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("DROP FUNCTION ")  + this->GetSchema()->GetQuotedIdentifier() + wxT(".") + this->GetQuotedIdentifier() + wxT("(") + GetArgSigList() + wxT(")");
	if (cascaded)
		sql += wxT(" CASCADE");
	return GetDatabase()->ExecuteVoid(sql);
}

wxString pgFunction::GetFullName()
{
	return GetName() + wxT("(") + GetArgSigList() + wxT(")");
}

wxString pgProcedure::GetFullName()
{
	if (GetArgSigList().IsEmpty())
		return GetName();
	else
		return GetName() + wxT("(") + GetArgSigList() + wxT(")");
}


wxString pgFunction::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		wxString qtName = GetQuotedFullIdentifier()  + wxT("(") + GetArgListWithNames() + wxT(")");
		wxString qtSig = GetQuotedFullIdentifier()  + wxT("(") + GetArgSigList() + wxT(")");

		sql = wxT("-- Function: ") + qtSig + wxT("\n\n")
		      + wxT("-- DROP FUNCTION ") + qtSig + wxT(";")
		      + wxT("\n\nCREATE OR REPLACE FUNCTION ") + qtName;

		// Use Oracle style syntax for edb-spl functions
		if (GetLanguage() == wxT("edbspl") && GetProcType() == 2)
		{
			sql += wxT("\nRETURN ");
			sql += GetReturnType();

			sql += wxT(" AS");
			if (GetSource().StartsWith(wxT("\n")))
				sql += GetSource();
			else
				sql += wxT("\n") + GetSource();
		}
		else
		{
			sql += wxT("\n  RETURNS ");
			if (GetReturnAsSet() && !GetReturnType().StartsWith(wxT("TABLE")))
				sql += wxT("SETOF ");
			sql += GetReturnType();

			sql += wxT(" AS\n");

			if (GetLanguage().IsSameAs(wxT("C"), false))
			{
				sql += qtDbString(GetBin()) + wxT(", ") + qtDbString(GetSource());
			}
			else
			{
				if (GetConnection()->BackendMinimumVersion(7, 5))
					sql += qtDbStringDollar(GetSource());
				else
					sql += qtDbString(GetSource());
			}
			sql += wxT("\n  LANGUAGE ") + GetLanguage() + wxT(" ");
			if (GetConnection()->BackendMinimumVersion(8, 4) && GetIsWindow())
				sql += wxT("WINDOW ");
			sql += GetVolatility();

			if (GetIsStrict())
				sql += wxT(" STRICT");
			if (GetSecureDefiner())
				sql += wxT(" SECURITY DEFINER");

			// PostgreSQL 8.3+ cost/row estimations
			if (GetConnection()->BackendMinimumVersion(8, 3))
			{
				sql += wxT("\n  COST ") + NumToStr(GetCost());

				if (GetReturnAsSet())
					sql += wxT("\n  ROWS ") + NumToStr(GetRows());
			}
		}

		if (!sql.Strip(wxString::both).EndsWith(wxT(";")))
			sql += wxT(";");

		size_t i;
		for (i = 0 ; i < configList.GetCount() ; i++)
		{
			if (configList.Item(i).BeforeFirst('=') != wxT("search_path") &&
			        configList.Item(i).BeforeFirst('=') != wxT("temp_tablespaces"))
				sql += wxT("\nALTER FUNCTION ") + qtSig
				       + wxT(" SET ") + configList.Item(i).BeforeFirst('=') + wxT("='") + configList.Item(i).AfterFirst('=') + wxT("';\n");
			else
				sql += wxT("\nALTER FUNCTION ") + qtSig
				       + wxT(" SET ") + configList.Item(i).BeforeFirst('=') + wxT("=") + configList.Item(i).AfterFirst('=') + wxT(";\n");
		}

		sql += wxT("\n")
		       +  GetOwnerSql(8, 0, wxT("FUNCTION ") + qtSig)
		       +  GetGrant(wxT("X"), wxT("FUNCTION ") + qtSig);

		if (!GetComment().IsNull())
		{
			sql += wxT("COMMENT ON FUNCTION ") + qtSig
			       + wxT(" IS ") + qtDbString(GetComment()) + wxT(";\n");
		}
	}

	return sql;
}


void pgFunction::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetOid());
		properties->AppendItem(_("Owner"), GetOwner());
		properties->AppendItem(_("Argument count"), GetArgCount());
		properties->AppendItem(_("Arguments"), GetArgListWithNames());
		properties->AppendItem(_("Signature arguments"), GetArgSigList());
		if (!GetIsProcedure())
			properties->AppendItem(_("Return type"), GetReturnType());
		properties->AppendItem(_("Language"), GetLanguage());
		properties->AppendItem(_("Returns a set?"), GetReturnAsSet());
		if (GetLanguage().IsSameAs(wxT("C"), false))
		{
			properties->AppendItem(_("Object file"), GetBin());
			properties->AppendItem(_("Link symbol"), GetSource());
		}
		else
			properties->AppendItem(_("Source"), firstLineOnly(GetSource()));

		if (GetConnection()->BackendMinimumVersion(8, 3))
		{
			properties->AppendItem(_("Estimated cost"), GetCost());
			if (GetReturnAsSet())
				properties->AppendItem(_("Estimated rows"), GetRows());
		}

		properties->AppendItem(_("Volatility"), GetVolatility());
		properties->AppendItem(_("Security of definer?"), GetSecureDefiner());
		properties->AppendItem(_("Strict?"), GetIsStrict());
		if (GetConnection()->BackendMinimumVersion(8, 4))
			properties->AppendItem(_("Window?"), GetIsWindow());

		size_t i;
		for (i = 0 ; i < configList.GetCount() ; i++)
		{
			wxString item = configList.Item(i);
			properties->AppendItem(item.BeforeFirst('='), item.AfterFirst('='));
		}

		properties->AppendItem(_("ACL"), GetAcl());
		properties->AppendItem(_("System function?"), GetSystemObject());
		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
	}
}

void pgFunction::ShowHint(frmMain *form, bool force)
{
	wxArrayString hints;
	hints.Add(HINT_OBJECT_EDITING);
	frmHint::ShowHint((wxWindow *)form, hints, GetFullIdentifier(), force);
}

wxString pgProcedure::GetSql(ctlTree *browser)
{
	if (!GetConnection()->EdbMinimumVersion(8, 0))
		return pgFunction::GetSql(browser);

	if (sql.IsNull())
	{
		wxString qtName, qtSig;

		if (GetArgListWithNames().IsEmpty())
		{
			qtName = GetQuotedFullIdentifier();
			qtSig = GetQuotedFullIdentifier();
		}
		else
		{
			qtName = GetQuotedFullIdentifier() + wxT("(") + GetArgListWithNames() + wxT(")");
			qtSig = GetQuotedFullIdentifier()  + wxT("(") + GetArgSigList() + wxT(")");
		}

		sql = wxT("-- Procedure: ") + qtSig + wxT("\n\n")
		      + wxT("-- DROP PROCEDURE ") + qtSig + wxT(";")
		      + wxT("\n\nCREATE OR REPLACE PROCEDURE ") + qtName;


		sql += wxT(" AS")
		       + GetSource()
		       + wxT("\n\n")
		       + GetGrant(wxT("X"), wxT("PROCEDURE ") + qtSig);

		if (!GetComment().IsNull())
		{
			sql += wxT("COMMENT ON PROCEDURE ") + GetQuotedFullIdentifier()
			       + wxT(" IS ") + qtDbString(GetComment()) + wxT(";\n");
		}
	}

	return sql;
}


bool pgProcedure::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	if (!GetConnection()->EdbMinimumVersion(8, 0))
		return pgFunction::DropObject(frame, browser, cascaded);

	wxString sql = wxT("DROP PROCEDURE ") + this->GetSchema()->GetQuotedIdentifier() + wxT(".") + this->GetQuotedIdentifier();
	return GetDatabase()->ExecuteVoid(sql);
}

wxString pgFunction::GetArgListWithNames()
{
	wxString args;

	for (unsigned int i = 0; i < argTypesArray.Count(); i++)
	{
		/*
		* All Table arguments lies at the end of the list
		* Do not include them as the part of the argument list
		*/
		if (argModesArray.Item(i) == wxT("TABLE"))
			break;

		if (args.Length() > 0)
			args += wxT(", ");

		wxString arg;

		if (GetIsProcedure())
		{
			if (!argModesArray.Item(i).IsEmpty())
			{
				arg += qtIdent(argNamesArray.Item(i));
			}
			else
			{
				if (!argNamesArray.Item(i).IsEmpty())
					arg += qtIdent(argNamesArray.Item(i));
				arg += wxT(" ") + argModesArray.Item(i);
			}

			if (!argModesArray.Item(i).IsEmpty())
			{
				if (arg.IsEmpty())
					arg += argModesArray.Item(i);
				else
					arg += wxT(" ") + argModesArray.Item(i);
			}
		}
		else
		{
			if (!argModesArray.Item(i).IsEmpty())
				arg += argModesArray.Item(i);

			if (!argNamesArray.Item(i).IsEmpty())
			{
				if (arg.IsEmpty())
					arg += qtIdent(argNamesArray.Item(i));
				else
					arg += wxT(" ") + qtIdent(argNamesArray.Item(i));
			}
		}

		if (!arg.IsEmpty())
			arg += wxT(" ") + argTypesArray.Item(i);
		else
			arg += argTypesArray.Item(i);

		// Parameter default value
		if (GetConnection()->HasFeature(FEATURE_FUNCTION_DEFAULTS) || GetConnection()->BackendMinimumVersion(8, 4))
		{
			if (!argModesArray.Item(i).IsSameAs(wxT("OUT"), false) && !argDefsArray.Item(i).IsEmpty())
				arg += wxT(" DEFAULT ") + argDefsArray.Item(i);
		}

		args += arg;
	}
	return args;
}

// Return the signature arguments list. If forScript = true, we format the list
// appropriately for use in a SELECT script.
wxString pgFunction::GetArgSigList(const bool forScript)
{
	wxString args;

	for (unsigned int i = 0; i < argTypesArray.Count(); i++)
	{
		// OUT parameters are not considered part of the signature, except for EDB-SPL
		if (argModesArray.Item(i) != wxT("OUT") && argModesArray.Item(i) != wxT("TABLE"))
		{
			if (args.Length() > 0)
			{
				if (forScript)
					args += wxT(",\n");
				else
					args += wxT(", ");
			}

			if (forScript)
				args += wxT("    <") + argTypesArray.Item(i) + wxT(">");
			else
				args += argTypesArray.Item(i);
		}
		else
		{
			if (GetLanguage() == wxT("edbspl") && argModesArray.Item(i) != wxT("TABLE"))
			{
				if (args.Length() > 0)
				{
					if (forScript)
						args += wxT(",\n");
					else
						args += wxT(", ");
				}

				if (forScript)
					args += wxT("    <") + argTypesArray.Item(i) + wxT(">");
				else
					args += argTypesArray.Item(i);
			}
		}
	}
	return args;
}

pgFunction *pgFunctionFactory::AppendFunctions(pgObject *obj, pgSchema *schema, ctlTree *browser, const wxString &restriction)
{
	// Caches
	cacheMap typeCache, exprCache;

	pgFunction *function = 0;
	wxString argNamesCol, argDefsCol, proConfigCol, proType;
	if (obj->GetConnection()->BackendMinimumVersion(8, 0))
		argNamesCol = wxT("proargnames, ");
	if (obj->GetConnection()->HasFeature(FEATURE_FUNCTION_DEFAULTS) && !obj->GetConnection()->BackendMinimumVersion(8, 4))
		argDefsCol = wxT("proargdefvals, ");
	if (obj->GetConnection()->BackendMinimumVersion(8, 4))
		argDefsCol = wxT("pg_get_expr(proargdefaults, 'pg_catalog.pg_class'::regclass) AS proargdefaultvals, pronargdefaults, ");
	if (obj->GetConnection()->BackendMinimumVersion(8, 3))
		proConfigCol = wxT("proconfig, ");
	if (obj->GetConnection()->EdbMinimumVersion(8, 1))
		proType = wxT("protype, ");

	pgSet *functions = obj->GetDatabase()->ExecuteSet(
	                       wxT("SELECT pr.oid, pr.xmin, pr.*, format_type(TYP.oid, NULL) AS typname, typns.nspname AS typnsp, lanname, ") +
	                       argNamesCol  + argDefsCol + proConfigCol + proType +
	                       wxT("       pg_get_userbyid(proowner) as funcowner, description\n")
	                       wxT("  FROM pg_proc pr\n")
	                       wxT("  JOIN pg_type typ ON typ.oid=prorettype\n")
	                       wxT("  JOIN pg_namespace typns ON typns.oid=typ.typnamespace\n")
	                       wxT("  JOIN pg_language lng ON lng.oid=prolang\n")
	                       wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=pr.oid\n")
	                       + restriction +
	                       wxT(" ORDER BY proname"));

	pgSet *types = obj->GetDatabase()->ExecuteSet(wxT(
	                   "SELECT oid, format_type(oid, NULL) AS typname FROM pg_type"));

	if (types)
	{
		while(!types->Eof())
		{
			typeCache[types->GetVal(wxT("oid"))] = types->GetVal(wxT("typname"));
			types->MoveNext();
		}
	}

	if (functions)
	{
		while (!functions->Eof())
		{
			bool isProcedure = false;
			bool hasDefValSupport = false;
			wxString lanname = functions->GetVal(wxT("lanname"));
			wxString typname = functions->GetVal(wxT("typname"));

			// Is this an EDB Stored Procedure?
			if (obj->GetConnection()->EdbMinimumVersion(8, 1))
			{
				wxString protype = functions->GetVal(wxT("protype"));
				if (protype == wxT("1"))
					isProcedure = true;
			}
			else if (obj->GetConnection()->EdbMinimumVersion(8, 0) &&
					 lanname == wxT("edbspl") && typname == wxT("void"))
				isProcedure = true;

			// Create the new object
			if (isProcedure)
				function = new pgProcedure(schema, functions->GetVal(wxT("proname")));
			else if (typname == wxT("\"trigger\"") || typname == wxT("trigger"))
				function = new pgTriggerFunction(schema, functions->GetVal(wxT("proname")));
			else
				function = new pgFunction(schema, functions->GetVal(wxT("proname")));

			// Tokenize the arguments
			wxStringTokenizer argTypesTkz(wxEmptyString), argModesTkz(wxEmptyString);
			queryTokenizer argNamesTkz(wxEmptyString, (wxChar)','), argDefsTkz(wxEmptyString, (wxChar)',');
			wxString tmp;

			// Support for Default Value in PG 8.4
			wxArrayString argDefValArray;

			// We always have types
			argTypesTkz.SetString(functions->GetVal(wxT("proargtypes")));

			// We only have names in 8.0+
			if (obj->GetConnection()->BackendMinimumVersion(8, 0))
			{
				tmp = functions->GetVal(wxT("proargnames"));
				if (!tmp.IsEmpty())
					argNamesTkz.SetString(tmp.Mid(1, tmp.Length() - 2), wxT(","));
			}

			// EDB 8.0 had modes in pg_proc.proargdirs
			if (!obj->GetConnection()->EdbMinimumVersion(8, 1) && isProcedure)
				argModesTkz.SetString(functions->GetVal(wxT("proargdirs")));

			if (obj->GetConnection()->EdbMinimumVersion(8, 1))
				function->iSetProcType(functions->GetLong(wxT("protype")));

			// EDB 8.1 and PostgreSQL 8.1 have modes in pg_proc.proargmodes
			if (obj->GetConnection()->BackendMinimumVersion(8, 1))
			{
				tmp = functions->GetVal(wxT("proallargtypes"));
				if (!tmp.IsEmpty())
					argTypesTkz.SetString(tmp.Mid(1, tmp.Length() - 2), wxT(","));

				tmp = functions->GetVal(wxT("proargmodes"));
				if (!tmp.IsEmpty())
					argModesTkz.SetString(tmp.Mid(1, tmp.Length() - 2), wxT(","));
			}

			// EDB 8.3: Function defaults
			if (obj->GetConnection()->HasFeature(FEATURE_FUNCTION_DEFAULTS) &&
			        !obj->GetConnection()->BackendMinimumVersion(8, 4))
			{
				tmp = functions->GetVal(wxT("proargdefvals"));
				if (!tmp.IsEmpty())
					argDefsTkz.SetString(tmp.Mid(1, tmp.Length() - 2), wxT(","));
			}

			if (obj->GetConnection()->BackendMinimumVersion(8, 4))
			{
				hasDefValSupport = true;
				tmp = functions->GetVal(wxT("proargdefaultvals"));
				getArrayFromCommaSeparatedList(tmp, argDefValArray);

				function->iSetArgDefValCount(functions->GetLong(wxT("pronargdefaults")));

				// Check if it is a window function
				function->iSetIsWindow(functions->GetBool(wxT("proiswindow")));
			}
			else
				function->iSetIsWindow(false);

			// Now iterate the arguments and build the arrays
			wxString type, name, mode, def;
			size_t nArgsIN = 0;

			while (argTypesTkz.HasMoreTokens())
			{
				// Add the arg type. This is a type oid, so
				// look it up in the hashmap
				type = argTypesTkz.GetNextToken();
				function->iAddArgType(typeCache[type]);

				// Now add the name, stripping the quotes and \" if
				// necessary.
				name = argNamesTkz.GetNextToken();
				if (!name.IsEmpty())
				{
					if (name[0] == '"')
						name = name.Mid(1, name.Length() - 2);
					name.Replace(wxT("\\\""), wxT("\""));
					function->iAddArgName(name);
				}
				else
					function->iAddArgName(wxEmptyString);

				// Now the mode
				mode = argModesTkz.GetNextToken();
				if (!mode.IsEmpty())
				{
					if (mode == wxT('o') || mode == wxT("2"))
						mode = wxT("OUT");
					else if (mode == wxT("b"))
						if (isProcedure)
							mode = wxT("IN OUT");
						else
						{
							mode = wxT("INOUT");
							nArgsIN++;
						}
					else if (mode == wxT("3"))
						mode = wxT("IN OUT");
					else if (mode == wxT("v"))
						mode = wxT("VARIADIC");
					else if (mode == wxT("t"))
						mode = wxT("TABLE");
					else
					{
						mode = wxT("IN");
						nArgsIN++;
					}

					function->iAddArgMode(mode);
				}
				else
				{
					function->iAddArgMode(wxEmptyString);
					nArgsIN++;
				}

				// Finally the defaults, as we got them.
				if (!hasDefValSupport)
					def = argDefsTkz.GetNextToken();

				if (hasDefValSupport)
				{
					// We will process this later
				}
				else if (!def.IsEmpty() && !def.IsSameAs(wxT("-")))
				{
					if (def[0] == '"')
						def = def.Mid(1, def.Length() - 2);

					// Check the cache first - if we don't have a value, get it and cache for next time
					wxString val = exprCache[def];
					if (val == wxEmptyString)
					{
						val = obj->GetDatabase()->ExecuteScalar(wxT("SELECT pg_get_expr('") + def + wxT("', 'pg_catalog.pg_class'::regclass)"));
						exprCache[def] = val;
					}
					function->iAddArgDef(val);
				}
				else
					function->iAddArgDef(wxEmptyString);
			}

			function->iSetArgCount(functions->GetLong(wxT("pronargs")));

			wxString strReturnTableArgs;
			// Process default values
			if (obj->GetConnection()->BackendMinimumVersion(8, 4))
			{
				size_t currINindex = 0;
				for (size_t index = 0; index < function->GetArgModesArray().Count(); index++)
				{
					if (function->GetArgModesArray()[index] == wxT("IN") ||
					        function->GetArgModesArray()[index] == wxT("INOUT") ||
					        function->GetArgModesArray()[index].IsEmpty())
					{
						nArgsIN--;
						if (function->GetArgDefValCount() != 0 &&
						        nArgsIN < (size_t)function->GetArgDefValCount())
						{
							if (argDefValArray[currINindex++] != wxT("-"))
								function->iAddArgDef(argDefValArray[currINindex - 1]);
							else
								function->iAddArgDef(wxT(""));
						}
						else
							function->iAddArgDef(wxEmptyString);
					}
					else if(function->GetArgModesArray()[index] == wxT("TABLE"))
					{
						if (strReturnTableArgs.Length() > 0)
							strReturnTableArgs += wxT(", ");
						wxString strName = function->GetArgNamesArray()[index];
						if (!strName.IsEmpty())
							strReturnTableArgs += qtIdent(strName) + wxT(" ");
						strReturnTableArgs += function->GetArgTypesArray()[index];
					}
					else
						function->iAddArgDef(wxEmptyString);
				}
			}

			function->iSetOid(functions->GetOid(wxT("oid")));
			function->iSetXid(functions->GetOid(wxT("xmin")));

			if (browser)
				function->UpdateSchema(browser, functions->GetOid(wxT("pronamespace")));

			function->iSetOwner(functions->GetVal(wxT("funcowner")));
			function->iSetAcl(functions->GetVal(wxT("proacl")));
			wxString strType = functions->GetVal(wxT("typname"));
			if (strType.Lower() == wxT("record") && !strReturnTableArgs.IsEmpty())
			{
				strType = wxT("TABLE(") + strReturnTableArgs + wxT(")");
			}
			function->iSetReturnType(strType);
			function->iSetComment(functions->GetVal(wxT("description")));

			function->iSetLanguage(lanname);
			function->iSetSecureDefiner(functions->GetBool(wxT("prosecdef")));
			function->iSetReturnAsSet(functions->GetBool(wxT("proretset")));
			function->iSetIsStrict(functions->GetBool(wxT("proisstrict")));
			function->iSetSource(functions->GetVal(wxT("prosrc")));
			function->iSetBin(functions->GetVal(wxT("probin")));

			wxString vol = functions->GetVal(wxT("provolatile"));
			function->iSetVolatility(
			    vol.IsSameAs(wxT("i")) ? wxT("IMMUTABLE") :
			    vol.IsSameAs(wxT("s")) ? wxT("STABLE") :
			    vol.IsSameAs(wxT("v")) ? wxT("VOLATILE") : wxT("unknown"));

			// PostgreSQL 8.3 cost/row estimations
			if (obj->GetConnection()->BackendMinimumVersion(8, 3))
			{
				function->iSetCost(functions->GetLong(wxT("procost")));
				function->iSetRows(functions->GetLong(wxT("prorows")));
				wxString cfg = functions->GetVal(wxT("proconfig"));
				if (!cfg.IsEmpty())
					FillArray(function->GetConfigList(), cfg.Mid(1, cfg.Length() - 2));
			}

			if (browser)
			{
				browser->AppendObject(obj, function);
				functions->MoveNext();
			}
			else
				break;
		}

		delete functions;
		delete types;
	}
	return function;
}



pgObject *pgFunction::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *function = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		function = functionFactory.AppendFunctions(coll, GetSchema(), 0, wxT(" WHERE pr.oid=") + GetOidStr() + wxT("\n"));

	// We might be linked to trigger....
	pgObject *trigger = browser->GetParentObject(item);
	if (trigger->GetMetaType() == PGM_TRIGGER)
		function = functionFactory.AppendFunctions(trigger, GetSchema(), 0, wxT(" WHERE pr.oid=") + GetOidStr() + wxT("\n"));

	return function;
}

// Generate the SELECT Script SQL
wxString pgFunction::GetSelectSql(ctlTree *browser)
{
	wxString args = GetArgSigList(true);

	wxString sql = wxT("SELECT ") + GetQuotedFullIdentifier();

	if (args.Length())
		sql += wxT("(\n") + args + wxT("\n);\n");
	else
		sql += wxT("();\n");

	return sql;
}

// Generate the EXEC Script SQL
wxString pgProcedure::GetExecSql(ctlTree *browser)
{
	wxString args = GetArgSigList(true);

	wxString sql = wxT("EXEC ") + GetQuotedFullIdentifier();

	if (args.Length())
		sql += wxT("(\n") + args + wxT("\n);\n");
	else
		sql += wxT(";\n");

	return sql;
}

pgObject *pgFunctionFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restr)
{
	wxString funcRestriction = wxT(
	                               " WHERE proisagg = FALSE AND pronamespace = ") + NumToStr(collection->GetSchema()->GetOid())
	                           + wxT("::oid\n   AND typname <> 'trigger'\n");

	if (collection->GetConnection()->EdbMinimumVersion(8, 1))
		funcRestriction += wxT("   AND NOT (lanname = 'edbspl' AND protype = 1)\n");
	else if (collection->GetConnection()->EdbMinimumVersion(8, 0))
		funcRestriction += wxT("   AND NOT (lanname = 'edbspl' AND typname = 'void')\n");

	// Get the Functions
	return AppendFunctions(collection, collection->GetSchema(), browser, funcRestriction);
}


pgCollection *pgFunctionFactory::CreateCollection(pgObject *obj)
{
	return new pgFunctionCollection(GetCollectionFactory(), (pgSchema *)obj);
}

pgObject *pgTriggerFunctionFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restr)
{
	wxString funcRestriction = wxT(
	                               " WHERE proisagg = FALSE AND pronamespace = ") + NumToStr(collection->GetSchema()->GetOid())
	                           + wxT("::oid\n   AND typname = 'trigger'\n")
	                           + wxT("   AND lanname != 'edbspl'\n");

	// Get the Functions
	return AppendFunctions(collection, collection->GetSchema(), browser, funcRestriction);
}


pgObject *pgProcedureFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restr)
{
	wxString funcRestriction = wxT(
	                               " WHERE proisagg = FALSE AND pronamespace = ") + NumToStr(collection->GetSchema()->GetOid())
	                           + wxT("::oid AND lanname = 'edbspl'\n");

	if (collection->GetConnection()->EdbMinimumVersion(8, 1))
		funcRestriction += wxT("   AND protype = 1\n");
	else
		funcRestriction += wxT("   AND typname = 'void'\n");

	// Get the Functions
	return AppendFunctions(collection, collection->GetSchema(), browser, funcRestriction);
}


#include "images/function.xpm"
#include "images/functions.xpm"

pgFunctionFactory::pgFunctionFactory(const wxChar *tn, const wxChar *ns, const wxChar *nls, const char **img)
	: pgSchemaObjFactory(tn, ns, nls, img)
{
	metaType = PGM_FUNCTION;
}

pgFunctionFactory functionFactory(__("Function"), __("New Function..."), __("Create a new Function."), function_xpm);
static pgaCollectionFactory cf(&functionFactory, __("Functions"), functions_xpm);


#include "images/triggerfunction.xpm"
#include "images/triggerfunctions.xpm"

pgTriggerFunctionFactory::pgTriggerFunctionFactory()
	: pgFunctionFactory(__("Trigger Function"), __("New Trigger Function..."), __("Create a new Trigger Function."), triggerfunction_xpm)
{
}

pgTriggerFunctionFactory triggerFunctionFactory;
static pgaCollectionFactory cft(&triggerFunctionFactory, __("Trigger Functions"), triggerfunctions_xpm);

#include "images/procedure.xpm"
#include "images/procedures.xpm"

pgProcedureFactory::pgProcedureFactory()
	: pgFunctionFactory(__("Procedure"), __("New Procedure"), __("Create a new Procedure."), procedure_xpm)
{
}

pgProcedureFactory procedureFactory;
static pgaCollectionFactory cfp(&procedureFactory, __("Procedures"), procedures_xpm);

pgFunctionCollection::pgFunctionCollection(pgaFactory *factory, pgSchema *sch)
	: pgSchemaObjCollection(factory, sch)
{
}


void pgFunctionCollection::ShowStatistics(frmMain *form, ctlListView *statistics)
{
	if (GetConnection()->BackendMinimumVersion(8, 4))
	{
		wxLogInfo(wxT("Displaying statistics for functions on ") + GetSchema()->GetName());

		wxString sql = wxT("SELECT funcname, calls, total_time, self_time")
		               wxT(" FROM pg_stat_user_functions")
		               wxT(" WHERE schemaname = ") + qtDbString(GetSchema()->GetName())
		               + wxT(" ORDER BY funcname");

		// Add the statistics view columns
		statistics->ClearAll();
		statistics->AddColumn(_("Function"), 60);
		statistics->AddColumn(_("Calls"), 50);
		statistics->AddColumn(_("Total Time"), 60);
		statistics->AddColumn(_("Self Time"), 60);

		pgSet *stats = GetDatabase()->ExecuteSet(sql);
		if (stats)
		{
			long pos = 0;
			while (!stats->Eof())
			{
				statistics->InsertItem(pos, stats->GetVal(wxT("funcname")), PGICON_STATISTICS);
				statistics->SetItem(pos, 1, stats->GetVal(wxT("calls")));
				statistics->SetItem(pos, 2, stats->GetVal(wxT("total_time")));
				statistics->SetItem(pos, 3, stats->GetVal(wxT("self_time")));
				stats->MoveNext();
				pos++;
			}

			delete stats;
		}
	}
}
