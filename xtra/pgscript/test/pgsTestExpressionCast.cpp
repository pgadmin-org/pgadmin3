//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2009, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgsTestSuite.h"

#include "pgscript/exceptions/pgsCastException.h"
#include "pgscript/objects/pgsVariable.h"

#include "pgscript/parser.tab.hh"
typedef pgscript::pgsParser::token token;

void pgsTestSuite::test_expression_cast(void)
{
	pgsCast * cast = 0;
	pgsVariable * expr = 0;
	pgsOperand res;
	pgsVarMap vars;

	// Convert an integer
	{
		// [1] Create source variable
		expr = pnew pgsNumber(wxT("123"), pgsInt);
		TS_ASSERT(expr->is_number() && expr->is_integer() && !expr->is_real());
		TS_ASSERT(!expr->is_string() && !expr->is_record());

		// [2] Convert it to the first format
		cast = pnew pgsCast(token::PGS_REAL, expr->clone());
		res = cast->eval(vars);
		TS_ASSERT(res->is_number() && !res->is_integer() && res->is_real());
		TS_ASSERT(!res->is_string() && !res->is_record());
		TS_ASSERT(res->value() == expr->value());
		pdelete(cast);

		// [3] Convert it to the second format
		cast = pnew pgsCast(token::PGS_STRING, expr->clone());
		res = cast->eval(vars);
		TS_ASSERT(!res->is_number() && !res->is_integer() && !res->is_real());
		TS_ASSERT(res->is_string() && !res->is_record());
		TS_ASSERT(res->value() == expr->value());
		pdelete(cast);

		// [4] Convert it to the third format
		cast = pnew pgsCast(token::PGS_RECORD, expr->clone());
		res = cast->eval(vars);
		TS_ASSERT(!res->is_number() && !res->is_integer() && !res->is_real());
		TS_ASSERT(!res->is_string() && res->is_record());
		pdelete(cast);

		// [5] Test copy constructor
		cast = pnew pgsCast(token::PGS_STRING, expr->clone());
		pgsCast copy(*cast);
		res = copy.eval(vars);
		TS_ASSERT(!res->is_number() && !res->is_integer() && !res->is_real());
		TS_ASSERT(res->is_string() && !res->is_record());
		TS_ASSERT(res->value() == expr->value());
		pdelete(cast);

		// [6] Test assignment
		cast = pnew pgsCast(token::PGS_RECORD, expr->clone());
		copy = *cast;
		res = copy.eval(vars);
		TS_ASSERT(!res->is_number() && !res->is_integer() && !res->is_real());
		TS_ASSERT(!res->is_string() && res->is_record());
		pdelete(cast);

		// [7] Delete source variable
		pdelete(expr);
	}

	// Convert a real
	{
		// [1] Create source variable
		expr = pnew pgsNumber(wxT("123.5"), pgsReal);
		TS_ASSERT(expr->is_number() && !expr->is_integer() && expr->is_real());
		TS_ASSERT(!expr->is_string() && !expr->is_record());

		// [2] Convert it to the first format
		cast = pnew pgsCast(token::PGS_INTEGER, expr->clone());
		res = cast->eval(vars);
		TS_ASSERT(res->is_number() && res->is_integer() && !res->is_real());
		TS_ASSERT(!res->is_string() && !res->is_record());
		TS_ASSERT(res->value() == wxT("123"));
		pdelete(cast);

		// [3] Convert it to the second format
		cast = pnew pgsCast(token::PGS_STRING, expr->clone());
		res = cast->eval(vars);
		TS_ASSERT(!res->is_number() && !res->is_integer() && !res->is_real());
		TS_ASSERT(res->is_string() && !res->is_record());
		TS_ASSERT(res->value() == expr->value());
		pdelete(cast);

		// [4] Convert it to the third format
		cast = pnew pgsCast(token::PGS_RECORD, expr->clone());
		res = cast->eval(vars);
		TS_ASSERT(!res->is_number() && !res->is_integer() && !res->is_real());
		TS_ASSERT(!res->is_string() && res->is_record());
		pdelete(cast);

		// [5] Test copy constructor
		cast = pnew pgsCast(token::PGS_RECORD, expr->clone());
		pgsCast copy(*cast);
		res = copy.eval(vars);
		TS_ASSERT(!res->is_number() && !res->is_integer() && !res->is_real());
		TS_ASSERT(!res->is_string() && res->is_record());
		pdelete(cast);

		// [6] Test assignment
		cast = pnew pgsCast(token::PGS_STRING, expr->clone());
		copy = *cast;
		res = copy.eval(vars);
		TS_ASSERT(!res->is_number() && !res->is_integer() && !res->is_real());
		TS_ASSERT(res->is_string() && !res->is_record());
		TS_ASSERT(res->value() == expr->value());
		pdelete(cast);

		// [7] Delete source variable
		pdelete(expr);
	}

	// Convert a string
	{
		// [1] Create source variable
		expr = pnew pgsString(wxT("azerty"));
		TS_ASSERT(!expr->is_number() && !expr->is_integer() && !expr->is_real());
		TS_ASSERT(expr->is_string() && !expr->is_record());

		// [2] Convert it to the first format
		cast = pnew pgsCast(token::PGS_INTEGER, expr->clone());
		try
		{
			res = cast->eval(vars);
			TS_ASSERT(false);
		}
		catch (const pgsCastException &)
		{
			pdelete(cast);
		}

		// [3] Convert it to the second format
		cast = pnew pgsCast(token::PGS_REAL, expr->clone());
		try
		{
			res = cast->eval(vars);
			TS_ASSERT(false);
		}
		catch (const pgsCastException &)
		{
			pdelete(cast);
		}

		// [4] Convert it to the third format
		cast = pnew pgsCast(token::PGS_RECORD, expr->clone());
		res = cast->eval(vars);
		TS_ASSERT(!res->is_number() && !res->is_integer() && !res->is_real());
		TS_ASSERT(!res->is_string() && res->is_record());
		pdelete(cast);

		// [5] Delete source variable
		pdelete(expr);
	}

	// Convert a record
	{
		// [1] Create source variable
		pgsRecord * rec = 0;
		rec = pnew pgsRecord(2);
		rec->insert(0, 0, pnew pgsString(wxT("abc")));
		rec->insert(0, 1, pnew pgsString(wxT("def")));
		expr = dynamic_cast<pgsVariable *>(rec);
		TS_ASSERT(!expr->is_number() && !expr->is_integer() && !expr->is_real());
		TS_ASSERT(!expr->is_string() && expr->is_record());

		// [2] Convert it to the first format
		cast = pnew pgsCast(token::PGS_INTEGER, expr->clone());
		try
		{
			res = cast->eval(vars);
			TS_ASSERT(false);
		}
		catch (const pgsCastException &)
		{
			pdelete(cast);
		}

		// [3] Convert it to the second format
		cast = pnew pgsCast(token::PGS_REAL, expr->clone());
		try
		{
			res = cast->eval(vars);
			TS_ASSERT(false);
		}
		catch (const pgsCastException &)
		{
			pdelete(cast);
		}

		// [4] Convert it to the third format
		cast = pnew pgsCast(token::PGS_STRING, expr->clone());
		res = cast->eval(vars);
		TS_ASSERT(!res->is_number() && !res->is_integer() && !res->is_real());
		TS_ASSERT(res->is_string() && !res->is_record());
		TS_ASSERT(res->value() == rec->value());
		pdelete(cast);

		// [5] Delete source variable
		pdelete(expr);
	}
}
