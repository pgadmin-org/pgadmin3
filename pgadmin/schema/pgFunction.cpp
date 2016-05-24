//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
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
#include "frm/frmMain.h"
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

wxString pgFunction::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on function");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing function");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop function \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop function \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop function cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop function?");
			break;
		case PROPERTIESREPORT:
			message = _("Function properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Function properties");
			break;
		case DDLREPORT:
			message = _("Function DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Function DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("Function dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Function dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Function dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Function dependents");
			break;
	}

	return message;
}

wxString pgTriggerFunction::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on trigger function");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing trigger function");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop trigger function \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop trigger function \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop trigger function cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop trigger function?");
			break;
		case PROPERTIESREPORT:
			message = _("Trigger function properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Trigger function properties");
			break;
		case DDLREPORT:
			message = _("Trigger function DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Trigger function DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("Trigger function dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Trigger function dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Trigger function dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Trigger function dependents");
			break;
	}

	return message;
}

wxString pgProcedure::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on procedure");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing procedure");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop procedure \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop procedure \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop procedure cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop procedure?");
			break;
		case PROPERTIESREPORT:
			message = _("Procedure properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Procedure properties");
			break;
		case DDLREPORT:
			message = _("Procedure DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Procedure DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("Procedure dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Procedure dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Procedure dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Procedure dependents");
			break;
	}

	return message;
}

void pgFunction::ShowStatistics(frmMain *form, ctlListView *statistics)
{
	if (GetConnection()->BackendMinimumVersion(8, 4))
	{
		wxString sql = wxT("SELECT calls AS ") + qtIdent(_("Number of calls")) +
		               wxT(", total_time AS ") + qtIdent(_("Total Time")) +
		               wxT(", self_time AS ") + qtIdent(_("Self Time")) +
		               wxT(" FROM pg_stat_user_functions") +
		               wxT(" WHERE funcid = ") + NumToStr(GetOid());
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

bool pgFunction::ResetStats()
{
	wxString sql = wxT("SELECT pg_stat_reset_single_function_counters(")
	               + NumToStr(this->GetOid())
	               + wxT(")");
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
		wxString qtName = GetQuotedFullIdentifier()  + wxT("(") + GetArgListWithNames(true) + wxT(")");
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

			if (GetConnection()->BackendMinimumVersion(9, 2) && GetIsLeakProof())
				sql += wxT(" LEAKPROOF");
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

		if (GetConnection()->BackendMinimumVersion(9, 1))
			sql += GetSeqLabelsSql();
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
		properties->AppendYesNoItem(_("Returns a set?"), GetReturnAsSet());
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
		if (GetConnection()->BackendMinimumVersion(9, 2))
			properties->AppendYesNoItem(_("Leak proof?"), GetIsLeakProof());
		properties->AppendYesNoItem(_("Security of definer?"), GetSecureDefiner());
		properties->AppendYesNoItem(_("Strict?"), GetIsStrict());
		if (GetConnection()->BackendMinimumVersion(8, 4))
			properties->AppendYesNoItem(_("Window?"), GetIsWindow());

		size_t i;
		for (i = 0 ; i < configList.GetCount() ; i++)
		{
			wxString item = configList.Item(i);
			properties->AppendItem(item.BeforeFirst('='), item.AfterFirst('='));
		}

		properties->AppendItem(_("ACL"), GetAcl());
		properties->AppendYesNoItem(_("System function?"), GetSystemObject());
		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));

		if (!GetLabels().IsEmpty())
		{
			wxArrayString seclabels = GetProviderLabelArray();
			if (seclabels.GetCount() > 0)
			{
				for (unsigned int index = 0 ; index < seclabels.GetCount() - 1 ; index += 2)
				{
					properties->AppendItem(seclabels.Item(index), seclabels.Item(index + 1));
				}
			}
		}
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

wxString pgFunction::GetArgListWithNames(bool multiline)
{
	wxString args;
	unsigned int nArgs = 0;

	for (unsigned int i = 0; i < argTypesArray.Count(); i++)
	{
		/*
		* All Table arguments lies at the end of the list
		* Do not include them as the part of the argument list
		*/
		if (argModesArray.Item(i) == wxT("TABLE"))
			break;

		nArgs++;
		if (args.Length() > 0)
		{
			args += (multiline) ? wxT(",\n    ") : wxT(", ");
		}

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
		if (GetConnection()->HasFeature(FEATURE_FUNCTION_DEFAULTS)
		        && !argDefsArray.IsEmpty())
		{
			if ((argModesArray.Item(i).IsEmpty() ||
			        argModesArray.Item(i) == wxT("IN") ||
			        // 'edbspl' does not support default value with
			        // INOUT parameter
			        (argModesArray.Item(i) == wxT("INOUT") &&
			         GetLanguage() != wxT("edbspl")) ||
			        argModesArray.Item(i) == wxT("VARIADIC")) &&
			        !argDefsArray.Item(i).IsEmpty() &&
			        i < argDefsArray.Count())
			{
				arg += wxT(" DEFAULT ") + argDefsArray.Item(i);
			}
		}

		args += arg;
	}

	if (multiline && nArgs > 1)
	{
		args = wxT("\n    ") + args;
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
		// OUT parameters are not considered part of the signature, except for EDB-SPL,
		// although this is not true for EDB AS90 onwards..
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
			if (GetLanguage() == wxT("edbspl") && argModesArray.Item(i) != wxT("TABLE") &&
			        !this->GetConnection()->EdbMinimumVersion(9, 0))
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
	wxString argNamesCol, argDefsCol, proConfigCol, proType, seclab;
	if (obj->GetConnection()->BackendMinimumVersion(8, 0))
		argNamesCol = wxT("proargnames, ");
	if (obj->GetConnection()->HasFeature(FEATURE_FUNCTION_DEFAULTS) && !obj->GetConnection()->BackendMinimumVersion(8, 4))
		argDefsCol = wxT("proargdefvals, COALESCE(substring(array_dims(proargdefvals), E'1:(.*)\\]')::integer, 0) AS pronargdefaults, ");
	if (obj->GetConnection()->BackendMinimumVersion(8, 4))
		argDefsCol = wxT("pg_get_expr(proargdefaults, 'pg_catalog.pg_class'::regclass) AS proargdefaultvals, pronargdefaults, ");
	if (obj->GetConnection()->BackendMinimumVersion(8, 3))
		proConfigCol = wxT("proconfig, ");
	if (obj->GetConnection()->EdbMinimumVersion(8, 1))
		proType = wxT("protype, ");
	if (obj->GetConnection()->BackendMinimumVersion(9, 1))
	{
		seclab = wxT(",\n")
		         wxT("(SELECT array_agg(label) FROM pg_seclabels sl1 WHERE sl1.objoid=pr.oid) AS labels,\n")
		         wxT("(SELECT array_agg(provider) FROM pg_seclabels sl2 WHERE sl2.objoid=pr.oid) AS providers");
	}

	pgSet *functions;
	if (obj->GetConnection()->GetIsGreenplum())
	{
		// the Open Source version of Greenplum already has the pg_get_function_result() function,
		// however the 4.3 stable release does not have this function
		functions = obj->GetDatabase()->ExecuteSet(
		                       wxT("SELECT pr.oid, pr.xmin, pr.*, format_type(TYP.oid, NULL) AS typname, typns.nspname AS typnsp, lanname, ") +
		                       argNamesCol  + argDefsCol + proConfigCol + proType +
		                       wxT("       pg_get_userbyid(proowner) as funcowner, description") + seclab + wxT("\n")
		                       wxT("  FROM pg_proc pr\n")
		                       wxT("  JOIN pg_type typ ON typ.oid=prorettype\n")
		                       wxT("  JOIN pg_namespace typns ON typns.oid=typ.typnamespace\n")
		                       wxT("  JOIN pg_language lng ON lng.oid=prolang\n")
		                       wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=pr.oid AND des.classoid='pg_proc'::regclass)\n")
		                       + restriction +
		                       wxT(" ORDER BY proname"));
	}
	else
	{
		// new code for !Greenplum
		functions = obj->GetDatabase()->ExecuteSet(
		                       wxT("SELECT pr.oid, pr.xmin, pr.*, pg_get_function_result(pr.oid) AS typname, typns.nspname AS typnsp, lanname, ") +
		                       argNamesCol  + argDefsCol + proConfigCol + proType +
		                       wxT("       pg_get_userbyid(proowner) as funcowner, description") + seclab + wxT("\n")
		                       wxT("  FROM pg_proc pr\n")
		                       wxT("  JOIN pg_type typ ON typ.oid=prorettype\n")
		                       wxT("  JOIN pg_namespace typns ON typns.oid=typ.typnamespace\n")
		                       wxT("  JOIN pg_language lng ON lng.oid=prolang\n")
		                       wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=pr.oid AND des.classoid='pg_proc'::regclass)\n")
		                       + restriction +
		                       wxT(" ORDER BY proname"));
	}

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
			else if (typname == wxT("\"trigger\"") || typname == wxT("trigger") || typname == wxT("event_trigger") || typname == wxT("\"event_trigger\""))
				function = new pgTriggerFunction(schema, functions->GetVal(wxT("proname")));
			else
				function = new pgFunction(schema, functions->GetVal(wxT("proname")));

			// Tokenize the arguments
			wxStringTokenizer argTypesTkz(wxEmptyString), argModesTkz(wxEmptyString);
			wxString tmp;

			wxArrayString argNamesArray;
			wxArrayString argDefValArray;

			// We always have types
			argTypesTkz.SetString(functions->GetVal(wxT("proargtypes")));

			// We only have names in 8.0+
			if (obj->GetConnection()->BackendMinimumVersion(8, 0))
			{
				tmp = functions->GetVal(wxT("proargnames"));
				if (!tmp.IsEmpty())
					getArrayFromCommaSeparatedList(tmp.Mid(1, tmp.Length() - 2), argNamesArray);
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
					getArrayFromCommaSeparatedList(tmp.Mid(1, tmp.Length() - 2), argDefValArray);

				function->iSetArgDefValCount(functions->GetLong(wxT("pronargdefaults")));
			}

			if (obj->GetConnection()->BackendMinimumVersion(8, 4))
			{
				tmp = functions->GetVal(wxT("proargdefaultvals"));
				getArrayFromCommaSeparatedList(tmp, argDefValArray);

				function->iSetArgDefValCount(functions->GetLong(wxT("pronargdefaults")));

				// Check if it is a window function
				function->iSetIsWindow(functions->GetBool(wxT("proiswindow")));
			}
			else
				function->iSetIsWindow(false);

			// Now iterate the arguments and build the arrays
			wxString type, name, mode;
			size_t nArgsIN = 0;
			size_t nArgNames = 0;

			while (argTypesTkz.HasMoreTokens())
			{
				if (nArgNames < argNamesArray.GetCount())
				{
					name = argNamesArray[nArgNames++];
				}
				else
					name = wxEmptyString;
				if (!name.IsEmpty())
				{
					// Now add the name, stripping the quotes and \" if
					// necessary.
					if (name[0] == '"')
						name = name.Mid(1, name.Length() - 2);
					name.Replace(wxT("\\\""), wxT("\""));

					// In EDBAS 90, if an SPL-function has both an OUT-parameter
					// and a return value (which is not possible on PostgreSQL otherwise),
					// the return value is transformed into an extra OUT-parameter
					// named "_retval_"
					if (obj->GetConnection()->EdbMinimumVersion(9, 0))
					{
						if (name == wxT("_retval_"))
						{
							type = argTypesTkz.GetNextToken();
							// this will be the return type for this object
							function->iSetReturnType(typeCache[type]);

							// consume uniformly, mode will definitely be "OUT"
							mode = argModesTkz.GetNextToken();

							continue;
						}
					}
					function->iAddArgName(name);
				}
				else
					function->iAddArgName(wxEmptyString);

				// Add the arg type. This is a type oid, so
				// look it up in the hashmap
				type = argTypesTkz.GetNextToken();
				function->iAddArgType(typeCache[type]);

				// Now the mode
				mode = argModesTkz.GetNextToken();
				if (!mode.IsEmpty())
				{
					if (mode == wxT('o') || mode == wxT("2"))
						mode = wxT("OUT");
					else if (mode == wxT("b") || mode == wxT("3"))
					{
						if (isProcedure)
							mode = wxT("IN OUT");
						else
						{
							mode = wxT("INOUT");
							// 'edbspl' does not support default values for the
							// INOUT parameters.
							if (lanname != wxT("edbspl"))
							{
								nArgsIN++;
							}
						}
					}
					else if (mode == wxT("v"))
					{
						mode = wxT("VARIADIC");
						nArgsIN++;
					}
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
			}

			function->iSetArgCount(functions->GetLong(wxT("pronargs")));

			wxString strReturnTableArgs;
			// Process default values
			size_t currINindex = 0;
			for (size_t index = 0; index < function->GetArgModesArray().Count(); index++)
			{
				wxString def = wxEmptyString;
				if(function->GetArgModesArray()[index].IsEmpty() ||
				        function->GetArgModesArray()[index] == wxT("IN") ||
				        (function->GetArgModesArray()[index] == wxT("INOUT") &&
				         lanname != wxT("edbspl")) ||
				        function->GetArgModesArray()[index] == wxT("VARIADIC"))
				{
					if (!argDefValArray.IsEmpty() && nArgsIN <= argDefValArray.GetCount())
					{
						def = argDefValArray[currINindex++];

						if (!def.IsEmpty() && def != wxT("-"))
						{
							// Only EDB 8.3 does not support get the default value
							// using pg_get_expr directly
							if (function->GetConnection()->HasFeature(FEATURE_FUNCTION_DEFAULTS) &&
							        !function->GetConnection()->BackendMinimumVersion(8, 4))
							{
								// Check the cache first - if we don't have a value, get it and cache for next time
								wxString val = exprCache[def];

								if (val == wxEmptyString)
								{
									val = obj->GetDatabase()->ExecuteScalar(
									          wxT("SELECT pg_get_expr('") + def.Mid(1, def.Length() - 2) + wxT("', 'pg_catalog.pg_class'::regclass)"));
									exprCache[def] = val;
								}
								def = val;
							}
						}
						else
						{
							def = wxEmptyString;
						}
					}
					nArgsIN--;
				}
				else if(function->GetConnection()->BackendMinimumVersion(8, 4) &&
				        function->GetArgModesArray()[index] == wxT("TABLE"))
				{
					if (strReturnTableArgs.Length() > 0)
						strReturnTableArgs += wxT(", ");
					wxString strName = function->GetArgNamesArray()[index];
					if (!strName.IsEmpty())
						strReturnTableArgs += qtIdent(strName) + wxT(" ");
					strReturnTableArgs += function->GetArgTypesArray()[index];
				}
				function->iAddArgDef(def);
			}

			function->iSetOid(functions->GetOid(wxT("oid")));
			function->iSetXid(functions->GetOid(wxT("xmin")));

			if (browser)
				function->UpdateSchema(browser, functions->GetOid(wxT("pronamespace")));

			function->iSetOwner(functions->GetVal(wxT("funcowner")));
			function->iSetAcl(functions->GetVal(wxT("proacl")));

			// set the return type only if not already set..
			if (function->GetReturnType().IsEmpty())
			{
				wxString strType = functions->GetVal(wxT("typname"));
				if (strType.Lower() == wxT("record") && !strReturnTableArgs.IsEmpty())
				{
					strType = wxT("TABLE(") + strReturnTableArgs + wxT(")");
				}
				function->iSetReturnType(strType);
			}
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

			if (obj->GetConnection()->BackendMinimumVersion(9, 1))
			{
				function->iSetProviders(functions->GetVal(wxT("providers")));
				function->iSetLabels(functions->GetVal(wxT("labels")));
			}

			if (obj->GetConnection()->BackendMinimumVersion(9, 2))
			{
				function->iSetIsLeakProof(functions->GetBool(wxT("proleakproof")));
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
	if (trigger->GetMetaType() == PGM_TRIGGER || trigger->GetMetaType() == PGM_EVENTTRIGGER)
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
	                           + wxT("::oid\n   AND typname NOT IN ('trigger', 'event_trigger') \n");

	if (collection->GetConnection()->EdbMinimumVersion(8, 1))
		funcRestriction += wxT("   AND NOT (lanname = 'edbspl' AND protype = '1')\n");
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
	                           + wxT("::oid\n");
	if(collection->GetConnection()->BackendMinimumVersion(9, 3))
	{
		funcRestriction += wxT("AND (typname IN ('trigger', 'event_trigger') \nAND lanname NOT IN ('edbspl', 'sql', 'internal'))");
	}
	else
	{
		funcRestriction += wxT("AND (typname = 'trigger'\n AND lanname != 'edbspl')");
	}

	// Get the Functions
	return AppendFunctions(collection, collection->GetSchema(), browser, funcRestriction);
}


pgObject *pgProcedureFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restr)
{
	wxString funcRestriction = wxT(
	                               " WHERE proisagg = FALSE AND pronamespace = ") + NumToStr(collection->GetSchema()->GetOid())
	                           + wxT("::oid AND lanname = 'edbspl'\n");

	if (collection->GetConnection()->EdbMinimumVersion(8, 1))
		funcRestriction += wxT("   AND protype = '1'\n");
	else
		funcRestriction += wxT("   AND typname = 'void'\n");

	// Get the Functions
	return AppendFunctions(collection, collection->GetSchema(), browser, funcRestriction);
}


#include "images/function.pngc"
#include "images/functions.pngc"

pgFunctionFactory::pgFunctionFactory(const wxChar *tn, const wxChar *ns, const wxChar *nls, wxImage *img)
	: pgSchemaObjFactory(tn, ns, nls, img)
{
	metaType = PGM_FUNCTION;
}

pgFunctionFactory functionFactory(__("Function"), __("New Function..."), __("Create a new Function."), function_png_img);
static pgaCollectionFactory cf(&functionFactory, __("Functions"), functions_png_img);


#include "images/triggerfunction.pngc"
#include "images/triggerfunctions.pngc"

pgTriggerFunctionFactory::pgTriggerFunctionFactory()
	: pgFunctionFactory(__("Trigger Function"), __("New Trigger Function..."), __("Create a new Trigger Function."), triggerfunction_png_img)
{
}

pgTriggerFunctionFactory triggerFunctionFactory;
static pgaCollectionFactory cft(&triggerFunctionFactory, __("Trigger Functions"), triggerfunctions_png_img);

#include "images/procedure.pngc"
#include "images/procedures.pngc"

pgProcedureFactory::pgProcedureFactory()
	: pgFunctionFactory(__("Procedure"), __("New Procedure"), __("Create a new Procedure."), procedure_png_img)
{
}

pgProcedureFactory procedureFactory;
static pgaCollectionFactory cfp(&procedureFactory, __("Procedures"), procedures_png_img);

pgFunctionCollection::pgFunctionCollection(pgaFactory *factory, pgSchema *sch)
	: pgSchemaObjCollection(factory, sch)
{
}

wxString pgFunctionCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on functions");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing functions");
			break;
		case GRANTWIZARDTITLE:
			message = _("Privileges for functions");
			break;
		case OBJECTSLISTREPORT:
			message = _("Functions list report");
			break;
	}

	return message;
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


resetFunctionStatsFactory::resetFunctionStatsFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("&Reset function statistics"),  _("Reset statistics of the selected function."));
}


wxWindow *resetFunctionStatsFactory::StartDialog(frmMain *form, pgObject *obj)
{
	if (wxMessageBox(_("Are you sure you wish to reset statistics of this function?"), _("Reset function statistics"), wxYES_NO) != wxYES)
		return 0;

	((pgFunction *)obj)->ResetStats();
	((pgFunction *)obj)->ShowStatistics(form, form->GetStatistics());

	return 0;
}


bool resetFunctionStatsFactory::CheckEnable(pgObject *obj)
{
	return obj && obj->IsCreatedBy(functionFactory) && ((pgFunction *)obj)->GetConnection()->BackendMinimumVersion(9, 0);
}

wxString pgTriggerFunctionCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on trigger functions");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing trigger functions");
			break;
		case GRANTWIZARDTITLE:
			message = _("Privileges for trigger functions");
			break;
		case OBJECTSLISTREPORT:
			message = _("Trigger functions list report");
			break;
	}

	return message;
}

wxString pgProcedureCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on procedures");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing procedures");
			break;
		case GRANTWIZARDTITLE:
			message = _("Privileges for procedures");
			break;
		case OBJECTSLISTREPORT:
			message = _("Procedures list report");
			break;
	}

	return message;
}


