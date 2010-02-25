//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgsTestSuite.h"

#include "pgscript/expressions/pgsAssign.h"
#include "pgscript/expressions/pgsAssignToRecord.h"
#include "pgscript/expressions/pgsDifferent.h"
#include "pgscript/expressions/pgsIdent.h"
#include "pgscript/expressions/pgsLower.h"
#include "pgscript/expressions/pgsRemoveLine.h"
#include "pgscript/objects/pgsNumber.h"
#include "pgscript/objects/pgsString.h"
#include "pgscript/statements/pgsDeclareRecordStmt.h"
#include "pgscript/statements/pgsExpressionStmt.h"
#include "pgscript/statements/pgsStmtList.h"

void pgsTestSuite::test_expression_record(void)
{
	pgsVarMap vars;

	pgsStmtList * SL1 = 0;

	wxArrayString columns;
	SL1 = pnew pgsStmtList(pgsTestClass::get_cout());

	{
		columns.Add(wxT("a"));
		columns.Add(wxT("b"));
		columns.Add(wxT("c"));
		columns.Add(wxT("d"));
	}

	// r := { a, b, c, d }
	{
		pgsStmt * S = 0;
		S = pnew pgsDeclareRecordStmt(wxT("r"), columns);
		SL1->insert_back(S);
	}

	// r[2][a] := 5
	{
		pgsStmt * S = 0;
		S = pnew pgsExpressionStmt(pnew pgsAssignToRecord(wxT("r"),
				pnew pgsNumber(wxT("2")), pnew pgsString(wxT("a")),
				pnew pgsNumber(wxT("5"))));
		SL1->insert_back(S);
	}

	// r[1][b] := "abc"
	{
		pgsStmt * S = 0;
		S = pnew pgsExpressionStmt(pnew pgsAssignToRecord(wxT("r"),
				pnew pgsNumber(wxT("1")), pnew pgsString(wxT("b")),
				pnew pgsString(wxT("abc"))));
		SL1->insert_back(S);
	}

	// r[0][0] := 1
	{
		pgsStmt * S = 0;
		S = pnew pgsExpressionStmt(pnew pgsAssignToRecord(wxT("r"),
				pnew pgsNumber(wxT("0")), pnew pgsNumber(wxT("0")),
				pnew pgsNumber(wxT("1"))));
		SL1->insert_back(S);
	}

	// w := r
	{
		pgsStmt * S = 0;
		S = pnew pgsExpressionStmt(pnew pgsAssign(wxT("w"),
				pnew pgsIdent(wxT("r"))));
		SL1->insert_back(S);
	}

	// p = (r != w)
	{
		pgsStmt * S = 0;
		S = pnew pgsExpressionStmt(pnew pgsAssign(wxT("p"),
				pnew pgsDifferent(pnew pgsIdent(wxT("r")),
				pnew pgsIdent(wxT("w")))));
		SL1->insert_back(S);
	}

	// r.remove_line(1)
	{
		pgsStmt * S = 0;
		S = pnew pgsExpressionStmt(pnew pgsRemoveLine(wxT("r"),
				pnew pgsNumber(wxT("1"))));
		TS_ASSERT(pgsRemoveLine(wxT("r"), pnew pgsNumber(wxT("1")))
				.value() == wxT("RMLINE(r[1])"));
		SL1->insert_back(S);
	}

	// q = (r != w)
	{
		pgsStmt * S = 0;
		S = pnew pgsExpressionStmt(pnew pgsAssign(wxT("q"),
				pnew pgsDifferent(pnew pgsIdent(wxT("r")),
				pnew pgsIdent(wxT("w")))));
		SL1->insert_back(S);
	}

	// o = (r < w)
	{
		pgsStmt * S = 0;
		S = pnew pgsExpressionStmt(pnew pgsAssign(wxT("o"),
				pnew pgsLower(pnew pgsIdent(wxT("r")),
				pnew pgsIdent(wxT("w")))));
		SL1->insert_back(S);
	}

	SL1->eval(vars);

	// Test symbol table at the end of the execution
	TS_ASSERT(vars[wxT("p")]->value() == wxT("0"));
	TS_ASSERT(vars[wxT("q")]->value() == wxT("1"));
	TS_ASSERT(vars[wxT("o")]->value() == wxT("1"));

	pdelete(SL1);
}
