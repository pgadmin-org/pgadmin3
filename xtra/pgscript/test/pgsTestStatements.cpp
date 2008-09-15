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
#include "pgscript/expressions/pgsGenInt.h"
#include "pgscript/expressions/pgsGenRegex.h"
#include "pgscript/expressions/pgsGenString.h"
#include "pgscript/expressions/pgsGreater.h"
#include "pgscript/expressions/pgsIdent.h"
#include "pgscript/expressions/pgsLower.h"
#include "pgscript/expressions/pgsMinus.h"
#include "pgscript/expressions/pgsPlus.h"
#include "pgscript/objects/pgsNumber.h"
#include "pgscript/objects/pgsString.h"
#include "pgscript/statements/pgsBreakStmt.h"
#include "pgscript/statements/pgsContinueStmt.h"
#include "pgscript/statements/pgsExpressionStmt.h"
#include "pgscript/statements/pgsIfStmt.h"
#include "pgscript/statements/pgsStmtList.h"
#include "pgscript/statements/pgsWhileStmt.h"

void pgsTestSuite::test_statement_stmt(void)
{
	// Symbol table
	pgsVarMap vars;

	// Test Assignment Statement
	{
		pgsStmt * S1 = 0, * S2 = 0, * S3 = 0;

		// x := 0
		S1 = pnew pgsExpressionStmt(pnew pgsAssign(wxT("x"),
				pnew pgsNumber(wxT("0"), pgsInt)));
		S1->eval(vars);
		
		// y := 2
		S2 = pnew pgsExpressionStmt(pnew pgsAssign(wxT("y"),
				pnew pgsNumber(wxT("2"), pgsInt)));
		S2->eval(vars);

		// Check that assignments went fine
		TS_ASSERT(vars[wxT("x")]->value() == wxT("0"));
		TS_ASSERT(vars[wxT("y")]->value() == wxT("2"));

		// x := 1
		S3 = pnew pgsExpressionStmt(pnew pgsAssign(wxT("x"),
				pnew pgsNumber(wxT("1"), pgsInt)));
		S3->eval(vars);

		TS_ASSERT(vars[wxT("x")]->value() == wxT("1"));
		TS_ASSERT(vars[wxT("y")]->value() == wxT("2"));

		pdelete(S1);
		pdelete(S2);
		pdelete(S3);
	}

	// Test If Statement (if 0 then z := 0 else z := 1 fi)
	{
		pgsStmt * S1 = 0;
		pgsStmtList * SL1 = 0, * SL2 = 0;

		// z := 0
		SL1 = pnew pgsStmtList(pgsTestClass::get_cout());
		SL1->insert_front(pnew pgsExpressionStmt(pnew pgsAssign(wxT("z"),
				pnew pgsNumber(wxT("0"), pgsInt))));
		
		// z := 1
		SL2 = pnew pgsStmtList(pgsTestClass::get_cout());
		SL2->insert_front(pnew pgsExpressionStmt(pnew pgsAssign(wxT("z"),
				pnew pgsNumber(wxT("1"), pgsInt))));

		// if 0 then z := 0 else z := 1 fi
		S1 = pnew pgsIfStmt(pnew pgsNumber(wxT("0"), pgsInt), SL1, SL2);
		S1->eval(vars);

		TS_ASSERT(vars[wxT("x")]->value() == wxT("1"));
		TS_ASSERT(vars[wxT("y")]->value() == wxT("2"));
		TS_ASSERT(vars[wxT("z")]->value() == wxT("1"));

		pdelete(S1);
	}

	// Test If Statement (if 1 then z := 0 else z := 1 fi)
	{
		pgsStmt * S1 = 0;
		pgsStmtList * SL1 = 0, * SL2 = 0;

		// z := 0
		SL1 = pnew pgsStmtList(pgsTestClass::get_cout());
		SL1->insert_front(pnew pgsExpressionStmt(pnew pgsAssign(wxT("z"),
				pnew pgsNumber(wxT("0"), pgsInt))));
		
		// z := 1
		SL2 = pnew pgsStmtList(pgsTestClass::get_cout());
		SL2->insert_front(pnew pgsExpressionStmt(pnew pgsAssign(wxT("z"),
				pnew pgsNumber(wxT("1"), pgsInt))));

		// if 1 then z := 0 else z := 1 fi
		S1 = pnew pgsIfStmt(pnew pgsNumber(wxT("1"), pgsInt), SL1, SL2);
		S1->eval(vars);

		TS_ASSERT(vars[wxT("x")]->value() == wxT("1"));
		TS_ASSERT(vars[wxT("y")]->value() == wxT("2"));
		TS_ASSERT(vars[wxT("z")]->value() == wxT("0"));

		pdelete(S1);
	}
	
	// Test While Statement (while i do s := s + i; i := i-1 done)
	{
		pgsStmt * S1 = 0, * S2 = 0, * S3 = 0;
		pgsStmtList * SL1 = 0;

		// i := 3
		pgsExpressionStmt(pnew pgsAssign(wxT("i"), pnew pgsNumber(wxT("3"),
				pgsInt))).eval(vars);
		
		// s := 0
		pgsExpressionStmt(pnew pgsAssign(wxT("s"), pnew pgsNumber(wxT("0"),
				pgsInt))).eval(vars);

		TS_ASSERT(vars[wxT("x")]->value() == wxT("1"));
		TS_ASSERT(vars[wxT("y")]->value() == wxT("2"));
		TS_ASSERT(vars[wxT("z")]->value() == wxT("0"));
		TS_ASSERT(vars[wxT("i")]->value() == wxT("3"));
		TS_ASSERT(vars[wxT("s")]->value() == wxT("0"));
		TS_ASSERT(vars.size() == 5);
		
		// i == 3 ?
		TS_ASSERT(pgsIdent(wxT("i")).eval(vars)->value() == wxT("3"));
		// s + i == 3 ?
		TS_ASSERT(pgsPlus(pnew pgsIdent(wxT("s")),
				pnew pgsIdent(wxT("i"))).eval(vars)->value() == wxT("3"));
		
		// s := s + i
		S1 = pnew pgsExpressionStmt(pnew pgsAssign(wxT("s"),
				pnew pgsPlus(pnew pgsIdent(wxT("s")),
						pnew pgsIdent(wxT("i")))));
		// i := i-1
		S2 = pnew pgsExpressionStmt(pnew pgsAssign(wxT("i"),
				pnew pgsMinus(pnew pgsIdent(wxT("i")),
						pnew pgsNumber(wxT("1"), pgsInt))));
		SL1 = pnew pgsStmtList(pgsTestClass::get_cout());
		SL1->insert_front(S2);
		SL1->insert_front(S1);

		// while i do s := s + i; i := i-1 done
		S3 = pnew pgsWhileStmt(pnew pgsIdent(wxT("i")), SL1);
		S3->eval(vars);
		pdelete(S3);

		TS_ASSERT(vars[wxT("x")]->value() == wxT("1"));
		TS_ASSERT(vars[wxT("y")]->value() == wxT("2"));
		TS_ASSERT(vars[wxT("z")]->value() == wxT("0"));
		TS_ASSERT(vars[wxT("i")]->value() == wxT("0"));
		TS_ASSERT(vars[wxT("s")]->value() == wxT("6"));
		TS_ASSERT(vars.size() == 5);
	}
	
	// Test While Statement
	// while i < 10 do
	//   i := i + i;
	//   if i > 5 then
	//     break;
	//   else
	//     continue;
	//     s := s + 1;
	//   fi
	// done
	{
		pgsStmt * S1 = 0, * S2 = 0, * S3 = 0, * S4 = 0;
		pgsStmtList * SL1 = 0, * SL2 = 0;

		// i := 0
		pgsExpressionStmt(pnew pgsAssign(wxT("i"), pnew pgsNumber(wxT("0"),
				pgsInt))).eval(vars);
		
		// s := 5
		pgsExpressionStmt(pnew pgsAssign(wxT("s"), pnew pgsNumber(wxT("5"),
				pgsInt))).eval(vars);

		TS_ASSERT(vars[wxT("i")]->value() == wxT("0"));
		TS_ASSERT(vars[wxT("s")]->value() == wxT("5"));
		
		// i := i + 1
		S1 = pnew pgsExpressionStmt(pnew pgsAssign(wxT("i"),
				pnew pgsPlus(pnew pgsIdent(wxT("i")),
						pnew pgsNumber(wxT("1"), pgsInt))));
		
		// s := s + 1
		S2 = pnew pgsExpressionStmt(pnew pgsAssign(wxT("s"),
				pnew pgsPlus(pnew pgsIdent(wxT("s")),
						pnew pgsNumber(wxT("1"), pgsInt))));

		// s := s + 1; continue
		SL2 = pnew pgsStmtList(pgsTestClass::get_cout());
		SL2->insert_back(pnew pgsContinueStmt());
		SL2->insert_back(S2);
		
		// if i > 5 ... fi
		S3 = pnew pgsIfStmt(pnew pgsGreater(pnew pgsIdent(wxT("i")),
				pnew pgsNumber(wxT("5"))), pnew pgsBreakStmt(), SL2);

		// while ... done
		SL1 = pnew pgsStmtList(pgsTestClass::get_cout());
		SL1->insert_back(S1);
		SL1->insert_back(S3);
		S4 = pnew pgsWhileStmt(pnew pgsLower(pnew pgsIdent(wxT("i")),
				pnew pgsNumber(wxT("10"))), SL1);
		S4->eval(vars);
		pdelete(S4);

		TS_ASSERT(vars[wxT("i")]->value() == wxT("6"));
		TS_ASSERT(vars[wxT("s")]->value() == wxT("5"));
	}

	// Test Assign Statement with a generator
	{
		pgsExpression * N0 = 0, * N1 = 0, * N2 = 0;
		pgsStmt * S1 = 0, * S2 = 0, * S3 = 0;

		// m := -10
		pgsExpressionStmt(pnew pgsAssign(wxT("m"),
				pnew pgsNumber(wxT("-10")))).eval(vars);

		// integer(m, 10, true, now)
		N0 = pnew pgsGenInt(pnew pgsIdent(wxT("m")), pnew pgsNumber(wxT("10")),
				pnew pgsNumber(wxT("1")), pnew pgsNumber(wxString()
						<< wxDateTime::GetTimeNow()));
		// string(1, 10, 2, now)
		N1 = pnew pgsGenString(pnew pgsNumber(wxT("1")),
				pnew pgsNumber(wxT("10")), pnew pgsNumber(wxT("2")),
						pnew pgsNumber(wxString() << wxDateTime::GetTimeNow()));
		// regex(a{2}[a-zA-Z]{10}c{3}, now)
		N2 = pnew pgsGenRegex(pnew pgsString(wxT("a{2}[a-zA-Z]{10}c{3}")),
						pnew pgsNumber(wxString() << wxDateTime::GetTimeNow()));

		// x := integer(m, 10, true, now)
		S1 = pnew pgsExpressionStmt(pnew pgsAssign(wxT("x"), N0));
		S1->eval(vars);
		// y := string(1, 10, 2, now)
		S2 = pnew pgsExpressionStmt(pnew pgsAssign(wxT("y"), N1));
		S2->eval(vars);
		// z := regex(a{2}[a-zA-Z]{10}c{3}, now)
		S3 = pnew pgsExpressionStmt(pnew pgsAssign(wxT("z"), N2));
		S3->eval(vars);

		wxString result1, result2, result3;
		wxArrayString sav;
		for (int i = 0; i < 21; i++)
		{
			// Check that x result is unique (we wanted to generate a sequence)
			result1 = pgsIdent(wxT("x")).eval(vars)->value();
			TS_ASSERT(sav.Index(result1) == wxNOT_FOUND);
			sav.Add(result1);
			
			// Check that y result is at least 3 characters (two words of 1
			// character with a space in the middle) and less than 21
			result2 = pgsIdent(wxT("y")).eval(vars)->value();
			TS_ASSERT(result2.Length() >= 3 && result2.Length() <= 21);
			
			// Check the regex-based generators
			result3 = pgsIdent(wxT("z")).eval(vars)->value();
			TS_ASSERT(result3.Length() == 15);
			TS_ASSERT(result3.StartsWith(wxT("aa")) && result3.EndsWith(wxT("ccc")));
		}

		pdelete(S1); // Deletes N0
		pdelete(S2); // Deletes N1
		pdelete(S3); // Deletes N2
	}

	
	// Test an unknown identifier
	{
		pgsExpression * E1;
		
		E1 = pnew pgsIdent(wxT("unknown"));
		TS_ASSERT(E1->eval(vars)->value() == wxT(""));
		
		pdelete(E1);
	}
}
