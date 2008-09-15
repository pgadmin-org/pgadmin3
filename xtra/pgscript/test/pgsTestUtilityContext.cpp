//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgsTestSuite.h"

#include "pgscript/expressions/pgsAssign.h"
#include "pgscript/objects/pgsNumber.h"
#include "pgscript/objects/pgsRecord.h"
#include "pgscript/objects/pgsString.h"
#include "pgscript/statements/pgsExpressionStmt.h"
#include "pgscript/utilities/pgsContext.h"

void pgsTestSuite::test_utility_context(void)
{
	// Test the various utilities provided by pgsContext
	
	pgsContext context(pgsTestClass::get_cout());
	
	// Test stacks
	{
		// Checks stack sizes
		{
			TS_ASSERT(context.size_vars() == 0);
			TS_ASSERT(context.size_stmts() == 0);
		}

		// Generate a "zero"
		{
			pgsVariable * zero = context.zero();
			TS_ASSERT(zero->value() == wxT("0"));
			TS_ASSERT(context.size_vars() == 1);
		}

		// Generate a "one"
		{
			pgsVariable * one = context.one();
			TS_ASSERT(one->value() == wxT("1"));
			TS_ASSERT(context.size_vars() == 2);
		}

		// Add a variable on the variable stack and delete it
		{
			pgsRecord * rec = pnew pgsRecord(5);
			context.push_var(rec);
			TS_ASSERT(context.size_vars() == 3);
			context.pop_var();
			TS_ASSERT(context.size_vars() == 2);
			pdelete(rec);
		}

		// Add a statement on the statement stack
		{
			pgsStmt * stmt = pnew pgsExpressionStmt(pnew pgsAssign(wxT("a"),
					context.zero()));
			context.push_stmt(stmt);
			context.pop_var(); // Zero is in the statement now
			TS_ASSERT(context.size_stmts() == 1);
		}

		// Delete everything
		{
			context.clear_stacks();
			TS_ASSERT(context.size_vars() == 0);
			TS_ASSERT(context.size_stmts() == 0);
		}
	}
	
	// Add columns and delete them
	{
		context.add_column(wxString(wxT("a")));
		context.add_column(wxString(wxT("b")));
		TS_ASSERT(context.columns().GetCount() == 2);
		context.clear_columns();
		TS_ASSERT(context.columns().GetCount() == 0);
	}
}
