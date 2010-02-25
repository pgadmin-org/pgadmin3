//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgsTestSuite.h"

#include "pgscript/exceptions/pgsParameterException.h"
#include "pgscript/expressions/pgsAssign.h"
#include "pgscript/expressions/pgsAssignToRecord.h"
#include "pgscript/expressions/pgsColumns.h"
#include "pgscript/expressions/pgsIdent.h"
#include "pgscript/expressions/pgsIdentRecord.h"
#include "pgscript/expressions/pgsLines.h"
#include "pgscript/expressions/pgsTrim.h"
#include "pgscript/objects/pgsNumber.h"
#include "pgscript/objects/pgsRecord.h"
#include "pgscript/objects/pgsString.h"

void pgsTestSuite::test_expression_ident(void)
{	
	const int init_inc_val = 10;
	wxString s_init_inc_val = wxString() << init_inc_val;
	
	pgsVarMap vars;
	
	// Fill in test data
	{
		// a := { a }; a[1].a = "a"
		pgsRecord * R1 = pnew pgsRecord(1);
		R1->insert(1, 0, pnew pgsString(wxT("a")));
		R1->set_column_name(0, wxT("a"));
		vars[wxT("a")] = R1;
		
		// b := { b }; b[8].b = "b"
		pgsRecord * R2 = pnew pgsRecord(1);
		R2->insert(8, 0, pnew pgsString(wxT("b")));
		R2->set_column_name(0, wxT("b"));
		vars[wxT("b")] = R2;
		
		// r := { c }; r[0].c = init_inc_val
		pgsRecord * R3 = pnew pgsRecord(1);
		R3->insert(0, 0, pnew pgsNumber(wxString()
				<< init_inc_val));
		R3->set_column_name(0, wxT("c"));
		vars[wxT("r")] = R3;
		
		vars[wxT("c")] = pnew pgsString(wxT("c"));
		vars[wxT("d")] = pnew pgsNumber(wxT("1"));
		vars[wxT("i")] = pnew pgsNumber(s_init_inc_val);
	}
	
	// Test pgsIdent
	{
		// [1] Test eval
		TS_ASSERT(pgsIdent(wxT("a")).eval(vars)->value() == vars[wxT("a")]->value());
		TS_ASSERT(pgsIdent(wxT("b")).eval(vars)->value() == vars[wxT("b")]->value());
		TS_ASSERT(pgsIdent(wxT("c")).eval(vars)->value() == vars[wxT("c")]->value());
		TS_ASSERT(pgsIdent(wxT("d")).eval(vars)->value() == vars[wxT("d")]->value());
		TS_ASSERT(pgsIdent(wxT("e")).eval(vars)->value() == wxT(""));
		TS_ASSERT(pgsIdent(wxT(" ")).eval(vars)->value() == wxT(""));
		TS_ASSERT(pgsIdent(wxT("A")).eval(vars)->value() == wxT(""));
		TS_ASSERT(pgsIdent(wxT("B")).eval(vars)->value() == wxT(""));
		TS_ASSERT(pgsIdent(wxT("C")).eval(vars)->value() == wxT(""));
		TS_ASSERT(pgsIdent(wxT("D")).eval(vars)->value() == wxT(""));
		TS_ASSERT(pgsIdent(pgsIdent::m_now.Lower()).eval(vars)->value() == wxT(""));
		wxString now = wxString() << wxDateTime::GetTimeNow();
		TS_ASSERT(pgsIdent(pgsIdent::m_now).eval(vars)->value() == now);
		
		// [2] Test value
		TS_ASSERT(pgsIdent(wxT("a")).value() == wxT("a"));
		TS_ASSERT(pgsIdent(wxT("b")).value() == wxT("b"));
		TS_ASSERT(pgsIdent(wxT("c")).value() == wxT("c"));
		TS_ASSERT(pgsIdent(wxT("d")).value() == wxT("d"));
		TS_ASSERT(pgsIdent(wxT("e")).value() == wxT("e"));
		TS_ASSERT(pgsIdent(wxT(" ")).value() == wxT(" "));
		TS_ASSERT(pgsIdent(wxT("A")).value() == wxT("A"));
		TS_ASSERT(pgsIdent(wxT("B")).value() == wxT("B"));
		TS_ASSERT(pgsIdent(wxT("C")).value() == wxT("C"));
		TS_ASSERT(pgsIdent(wxT("D")).value() == wxT("D"));
		TS_ASSERT(pgsIdent(pgsIdent::m_now).value() == pgsIdent::m_now);
		
		// [3] Test copy constructor
		pgsIdent ident(wxT("d"));
		TS_ASSERT(ident.eval(vars)->value() == wxT("1"));
		pgsIdent copy(ident);
		TS_ASSERT(copy.eval(vars)->value() == wxT("1"));
		TS_ASSERT(copy.value() == wxT("d"));
		
		// [4] Test assignment
		pgsIdent assign(wxT("a")); // a is a record
		assign = ident;
		TS_ASSERT(assign.eval(vars)->value() == wxT("1"));
		TS_ASSERT(assign.value() == wxT("d"));
	}
	
	// Test pgsIdentRecord with [int][str]
	{
		// [1] Test eval
		TS_ASSERT(pgsIdentRecord(wxT("a"), pnew pgsNumber(wxT("0")), pnew
				pgsNumber(wxT("0"))).eval(vars)->value() == wxT("")); // Wrong line
		TS_ASSERT(pgsIdentRecord(wxT("a"), pnew pgsNumber(wxT("1")), pnew
				pgsNumber(wxT("0"))).eval(vars)->value() == wxT("a")); // OK
		TS_ASSERT(pgsIdentRecord(wxT("a"), pnew pgsNumber(wxT("2")), pnew
				pgsNumber(wxT("0"))).eval(vars)->value() == wxT("")); // Wrong line
		TS_ASSERT(pgsIdentRecord(wxT("a"), pnew pgsNumber(wxT("0")), pnew
				pgsNumber(wxT("1"))).eval(vars)->value() == wxT("")); // Wrong column
		TS_ASSERT(pgsIdentRecord(wxT("a"), pnew pgsNumber(wxT("1")), pnew
				pgsNumber(wxT("1"))).eval(vars)->value() == wxT("")); // Wrong line & column
		TS_ASSERT(pgsIdentRecord(wxT("a"), pnew pgsRecord(1),        pnew
				pgsNumber(wxT("0"))).eval(vars)->value() == wxT("")); // Wrong line (record)
		TS_ASSERT(pgsIdentRecord(wxT("a"), pnew pgsNumber(wxT("1")), pnew
				pgsRecord(0)).eval(vars)->value() == wxT("")); // Wrong column (record)
		TS_ASSERT(pgsIdentRecord(wxT("a"), pnew pgsString(wxT("1")), pnew
				pgsNumber(wxT("0"))).eval(vars)->value() == wxT("")); // Wrong line (string)
		TS_ASSERT(pgsIdentRecord(wxT("a"), pnew pgsNumber(wxT("1")), pnew
				pgsString(wxT("0"))).eval(vars)->value() == wxT("")); // Wrong column (string)
		TS_ASSERT(pgsIdentRecord(wxT("a"), pnew pgsNumber(wxT("1"), pgsReal), pnew
				pgsNumber(wxT("0"))).eval(vars)->value() == wxT("")); // Wrong line (real)
		TS_ASSERT(pgsIdentRecord(wxT("a"), pnew pgsNumber(wxT("1")), pnew
				pgsNumber(wxT("0"), pgsReal)).eval(vars)->value() == wxT("")); // Wrong column (real)
		TS_ASSERT(pgsIdentRecord(wxT("c"), pnew pgsNumber(wxT("0")), pnew
				pgsNumber(wxT("0"))).eval(vars)->value() == wxT("")); // Not a record
		TS_ASSERT(pgsIdentRecord(pgsIdent::m_now, pnew pgsNumber(wxT("0")), pnew
				pgsNumber(wxT("0"))).eval(vars)->value() == wxT("")); // Does not exist
		TS_ASSERT(pgsIdentRecord(wxT("e"), pnew pgsNumber(wxT("0")), pnew
				pgsNumber(wxT("0"))).eval(vars)->value() == wxT("")); // Does not exist
		
		// [2] Test value
		TS_ASSERT(pgsIdentRecord(wxT("a"), pnew pgsNumber(wxT("0")), pnew
				pgsNumber(wxT("0"))).value() == wxT("a[0][0]"));
		TS_ASSERT(pgsIdentRecord(wxT("a"), pnew pgsNumber(wxT("1")), pnew
				pgsNumber(wxT("0"))).value() == wxT("a[1][0]"));
		TS_ASSERT(pgsIdentRecord(wxT("a"), pnew pgsNumber(wxT("2")), pnew
				pgsNumber(wxT("0"))).value() == wxT("a[2][0]"));
		TS_ASSERT(pgsIdentRecord(wxT("a"), pnew pgsNumber(wxT("0")), pnew
				pgsNumber(wxT("1"))).value() == wxT("a[0][1]"));
		TS_ASSERT(pgsIdentRecord(wxT("a"), pnew pgsNumber(wxT("1")), pnew
				pgsNumber(wxT("1"))).value() == wxT("a[1][1]"));
		TS_ASSERT(pgsIdentRecord(wxT("a"), pnew pgsString(wxT("1")), pnew
				pgsNumber(wxT("0"))).value() == wxT("a[1][0]"));
		TS_ASSERT(pgsIdentRecord(wxT("a"), pnew pgsNumber(wxT("1")), pnew
				pgsString(wxT("0"))).value() == wxT("a[1][0]"));
		TS_ASSERT(pgsIdentRecord(wxT("c"), pnew pgsNumber(wxT("0")), pnew
				pgsNumber(wxT("0"))).value() == wxT("c[0][0]"));
		TS_ASSERT(pgsIdentRecord(wxT("e"), pnew pgsNumber(wxT("0")), pnew
				pgsNumber(wxT("0"))).value() == wxT("e[0][0]"));
		
		// [3] Test copy constructor
		pgsIdentRecord ident(wxT("a"), pnew pgsNumber(wxT("1")),
				pnew pgsNumber(wxT("0")));
		TS_ASSERT(ident.eval(vars)->value() == wxT("a"));
		pgsIdentRecord copy(ident);
		TS_ASSERT(copy.eval(vars)->value() == wxT("a"));
		TS_ASSERT(copy.value() == wxT("a[1][0]"));
		
		// [4] Test assignment
		pgsIdentRecord assign(wxT("a"), pnew pgsNumber(wxT("0")),
				pnew pgsNumber(wxT("0"))); // Wrong location
		assign = ident;
		TS_ASSERT(assign.eval(vars)->value() == wxT("a"));
		TS_ASSERT(copy.value() == wxT("a[1][0]"));
	}
	
	// Test pgsIdentRecord with [int]
	{
		// [1] Test eval
		TS_ASSERT(pgsIdentRecord(wxT("a"), pnew pgsNumber(wxT("0")))
				.eval(vars)->value() == wxT("(\"\")")); // Wrong line
		TS_ASSERT(pgsIdentRecord(wxT("a"), pnew pgsNumber(wxT("1")))
				.eval(vars)->value() == wxT("(\"a\")")); // OK
		TS_ASSERT(pgsIdentRecord(wxT("a"), pnew pgsNumber(wxT("2")))
				.eval(vars)->value() == wxT("")); // Wrong line
		TS_ASSERT(pgsIdentRecord(wxT("b"), pnew pgsNumber(wxT("7")))
				.eval(vars)->value() == wxT("(\"\")")); // Wrong line
		TS_ASSERT(pgsIdentRecord(wxT("b"), pnew pgsNumber(wxT("8")))
				.eval(vars)->value() == wxT("(\"b\")")); // OK
		TS_ASSERT(pgsIdentRecord(wxT("b"), pnew pgsNumber(wxT("9")))
				.eval(vars)->value() == wxT("")); // Wrong line
		TS_ASSERT(pgsIdentRecord(wxT("a"), pnew pgsRecord(1))
				.eval(vars)->value() == wxT("")); // Wrong line (record)
		TS_ASSERT(pgsIdentRecord(wxT("a"), pnew pgsString(wxT("1")))
				.eval(vars)->value() == wxT("")); // Wrong line (string)
		TS_ASSERT(pgsIdentRecord(wxT("a"), pnew pgsNumber(wxT("1"), pgsReal))
				.eval(vars)->value() == wxT("")); // Wrong line (real)
		TS_ASSERT(pgsIdentRecord(wxT("c"), pnew pgsNumber(wxT("0")))
				.eval(vars)->value() == wxT("")); // Not a record
		TS_ASSERT(pgsIdentRecord(wxT("e"), pnew pgsString(wxT("0")))
				.eval(vars)->value() == wxT("")); // Does not exist
		
		// [2] Test value
		TS_ASSERT(pgsIdentRecord(wxT("a"), pnew pgsNumber(wxT("0")))
				.value() == wxT("a[0]"));
		TS_ASSERT(pgsIdentRecord(wxT("a"), pnew pgsNumber(wxT("1")))
				.value() == wxT("a[1]"));
		TS_ASSERT(pgsIdentRecord(wxT("a"), pnew pgsNumber(wxT("2")))
				.value() == wxT("a[2]"));
		TS_ASSERT(pgsIdentRecord(wxT("b"), pnew pgsNumber(wxT("7")))
				.value() == wxT("b[7]"));
		TS_ASSERT(pgsIdentRecord(wxT("b"), pnew pgsNumber(wxT("8")))
				.value() == wxT("b[8]"));
		TS_ASSERT(pgsIdentRecord(wxT("b"), pnew pgsNumber(wxT("9")))
				.value() == wxT("b[9]"));
		TS_ASSERT(pgsIdentRecord(wxT("a"), pnew pgsString(wxT("1")))
				.value() == wxT("a[1]"));
		TS_ASSERT(pgsIdentRecord(wxT("a"), pnew pgsNumber(wxT("1"), pgsReal))
				.value() == wxT("a[1]"));
		TS_ASSERT(pgsIdentRecord(wxT("c"), pnew pgsNumber(wxT("0")))
				.value() == wxT("c[0]"));
		TS_ASSERT(pgsIdentRecord(wxT("e"), pnew pgsString(wxT("0")))
				.value() == wxT("e[0]"));
		
		// [3] Test copy constructor
		pgsIdentRecord ident(wxT("a"), pnew pgsNumber(wxT("1")));
		ident.eval(vars);
		pgsIdentRecord copy(ident);
		TS_ASSERT(copy.eval(vars)->value() == wxT("(\"a\")"));
		TS_ASSERT(copy.value() == wxT("a[1]"));
		
		// [4] Test assignment
		pgsIdentRecord assign(wxT("a"), pnew pgsNumber(wxT("0")));
		assign = ident;
		TS_ASSERT(assign.eval(vars)->value() == wxT("(\"a\")"));
		TS_ASSERT(copy.value() == wxT("a[1]"));
	}
	
	// Test pgsLines
	{
		// [1] Test eval
		TS_ASSERT(pgsLines(wxT("a")).eval(vars)->value() == wxT("2"));
		TS_ASSERT(pgsLines(wxT("b")).eval(vars)->value() == wxT("9"));
		TS_ASSERT(pgsLines(wxT("c")).eval(vars)->value() == wxT("1")); // Not a record
		TS_ASSERT(pgsLines(wxT("d")).eval(vars)->value() == wxT("1")); // Not a record
		TS_ASSERT(pgsLines(wxT("e")).eval(vars)->value() == wxT("0")); // Does not exist
		TS_ASSERT(pgsLines(wxT(" ")).eval(vars)->value() == wxT("0")); // Does not exist
		
		// [2] Test value
		TS_ASSERT(pgsLines(wxT("a")).value() == wxT("LINES(a)"));
		TS_ASSERT(pgsLines(wxT("b")).value() == wxT("LINES(b)"));
		TS_ASSERT(pgsLines(wxT("c")).value() == wxT("LINES(c)"));
		TS_ASSERT(pgsLines(wxT("d")).value() == wxT("LINES(d)"));
		TS_ASSERT(pgsLines(wxT("e")).value() == wxT("LINES(e)"));
		TS_ASSERT(pgsLines(wxT(" ")).value() == wxT("LINES( )"));
		
		// [3] Test copy constructor
		pgsLines ident(wxT("a"));
		ident.eval(vars);
		pgsLines copy(ident);
		TS_ASSERT(copy.eval(vars)->value() == wxT("2"));
		TS_ASSERT(copy.value() == wxT("LINES(a)"));
		
		// [4] Test assignment
		pgsLines assign(wxT("b"));
		assign = ident;
		TS_ASSERT(assign.eval(vars)->value() == wxT("2"));
		TS_ASSERT(assign.value() == wxT("LINES(a)"));
	}
	
	// Test pgsColumns
	{
		// [1] Test eval
		TS_ASSERT(pgsColumns(wxT("a")).eval(vars)->value() == wxT("1"));
		TS_ASSERT(pgsColumns(wxT("b")).eval(vars)->value() == wxT("1"));
		TS_ASSERT(pgsColumns(wxT("c")).eval(vars)->value() == wxT("1")); // Not a record
		TS_ASSERT(pgsColumns(wxT("d")).eval(vars)->value() == wxT("1")); // Not a record
		TS_ASSERT(pgsColumns(wxT("e")).eval(vars)->value() == wxT("0")); // Does not exist
		TS_ASSERT(pgsColumns(wxT(" ")).eval(vars)->value() == wxT("0")); // Does not exist
		
		// [2] Test value
		TS_ASSERT(pgsColumns(wxT("a")).value() == wxT("COLUMNS(a)"));
		TS_ASSERT(pgsColumns(wxT("b")).value() == wxT("COLUMNS(b)"));
		TS_ASSERT(pgsColumns(wxT("c")).value() == wxT("COLUMNS(c)"));
		TS_ASSERT(pgsColumns(wxT("d")).value() == wxT("COLUMNS(d)"));
		TS_ASSERT(pgsColumns(wxT("e")).value() == wxT("COLUMNS(e)"));
		TS_ASSERT(pgsColumns(wxT(" ")).value() == wxT("COLUMNS( )"));
		
		// [3] Test copy constructor
		pgsColumns ident(wxT("a"));
		ident.eval(vars);
		pgsColumns copy(ident);
		TS_ASSERT(copy.eval(vars)->value() == wxT("1"));
		TS_ASSERT(copy.value() == wxT("COLUMNS(a)"));
		
		// [4] Test assignment
		pgsColumns assign(wxT("e"));
		assign = ident;
		TS_ASSERT(assign.eval(vars)->value() == wxT("1"));
		TS_ASSERT(assign.value() == wxT("COLUMNS(a)"));
	}
	
	// Test pgsTrim
	{
		// [1] Test eval
		TS_ASSERT(pgsTrim(pnew pgsString(wxT("  abc "))).eval(vars)->value() == wxT("abc"));
		TS_ASSERT(pgsTrim(pnew pgsString(wxT("abc "))).eval(vars)->value() == wxT("abc"));
		TS_ASSERT(pgsTrim(pnew pgsString(wxT("  abc\n "))).eval(vars)->value() == wxT("abc"));
		TS_ASSERT(pgsTrim(pnew pgsNumber(wxT("10"))).eval(vars)->value() == wxT("10"));
		TS_ASSERT(pgsTrim(pnew pgsIdent(wxT("c"))).eval(vars)->value() == wxT("c"));
		
		// [2] Test value
		TS_ASSERT(pgsTrim(pnew pgsString(wxT("  abc "))).value() == wxT("TRIM(  abc )"));
		TS_ASSERT(pgsTrim(pnew pgsString(wxT("abc "))).value() == wxT("TRIM(abc )"));
		TS_ASSERT(pgsTrim(pnew pgsNumber(wxT("10"))).value() == wxT("TRIM(10)"));
		TS_ASSERT(pgsTrim(pnew pgsIdent(wxT("c"))).value() == wxT("TRIM(c)"));
		
		// [3] Test copy constructor
		pgsTrim trim(pnew pgsString(wxT("  a  ")));
		trim.eval(vars);
		pgsTrim copy(trim);
		TS_ASSERT(copy.eval(vars)->value() == wxT("a"));
		TS_ASSERT(copy.value() == wxT("TRIM(  a  )"));
		
		// [4] Test assignment
		pgsTrim assign(pnew pgsString(wxT(" b ")));
		assign = trim;
		TS_ASSERT(assign.eval(vars)->value() == wxT("a"));
		TS_ASSERT(assign.value() == wxT("TRIM(  a  )"));
	}
	
	// Test pgsAssign
	{
		// [1] Test eval & value
		{
			pgsAssign assign(wxT("c"), pnew pgsString(wxT("a")));
			assign.eval(vars); // c was 'c'
			TS_ASSERT(vars[wxT("c")]->value() == wxT("a"));
			TS_ASSERT(assign.value() == wxT("SET c = a"));
		}
		{
			pgsAssign assign(wxT("h"), pnew pgsNumber(wxT("4")));
			assign.eval(vars); // h did not exist
			TS_ASSERT(vars[wxT("h")]->value() == wxT("4"));
			TS_ASSERT(assign.value() == wxT("SET h = 4"));
		}
		
		// [2] Test copy constructor
		pgsAssign ident(wxT("g"), pnew pgsNumber(wxT("5")));
		pgsAssign copy(ident);
		TS_ASSERT(ident.eval(vars)->value() == copy.eval(vars)->value()
				&& copy.eval(vars)->value() == wxT("5"));
		TS_ASSERT(copy.value() == wxT("SET g = 5"));
		
		// [3] Test assignment
		pgsAssign assign(wxT("h"), pnew pgsNumber(wxT("4")));
		assign = copy;
		TS_ASSERT(assign.eval(vars)->value() == copy.eval(vars)->value()
				&& assign.eval(vars)->value() == wxT("5"));
		TS_ASSERT(assign.value() == wxT("SET g = 5"));
	}
	
	// Test pgsAssignToRecord
	{
		// [1] Test eval & value
		{
			pgsAssignToRecord assign(wxT("a"), pnew pgsNumber(wxT("1")),
					pnew pgsString(wxT("a")), pnew pgsString(wxT("x")));
			TS_ASSERT(assign.eval(vars)->value() == wxT("x"));
			TS_ASSERT(assign.value() == wxT("SET a[1][a] = x"));
		}
		{
			pgsAssignToRecord assign(wxT("a"), pnew pgsNumber(wxT("1")),
					pnew pgsNumber(wxT("0")), pnew pgsString(wxT("z")));
			TS_ASSERT(assign.eval(vars)->value() == wxT("z"));
			TS_ASSERT(assign.value() == wxT("SET a[1][0] = z"));
		}
		{
			pgsAssignToRecord assign(wxT("a"), pnew pgsNumber(wxT("5")),
					pnew pgsNumber(wxT("0")), pnew pgsString(wxT("v")));
			TS_ASSERT(assign.eval(vars)->value() == wxT("v"));
			TS_ASSERT(assign.value() == wxT("SET a[5][0] = v"));
		}
		try
		{
			pgsAssignToRecord(wxT("c"), pnew pgsNumber(wxT("1")), pnew
					pgsNumber(wxT("0")), pnew pgsString(wxT("x"))).eval(vars);
			TS_ASSERT(false); // Not a record
		}
		catch (const pgsParameterException &)
		{
			
		}
		try
		{
			pgsAssignToRecord(wxT("a"), pnew pgsNumber(wxT("1")), pnew
					pgsNumber(wxT("1")), pnew pgsString(wxT("x"))).eval(vars);
			TS_ASSERT(false); // Wrong column
		}
		catch (const pgsParameterException &)
		{
			
		}
		try
		{
			pgsAssignToRecord(wxT("a"), pnew pgsNumber(wxT("1")), pnew
					pgsRecord(1), pnew pgsString(wxT("x"))).eval(vars);
			TS_ASSERT(false); // Wrong column (record)
		}
		catch (const pgsParameterException &)
		{
			
		}
		try
		{
			pgsAssignToRecord(wxT("a"), pnew pgsNumber(wxT("1")), pnew
					pgsString(wxT("b")), pnew pgsString(wxT("x"))).eval(vars);
			TS_ASSERT(false); // Wrong column
		}
		catch (const pgsParameterException &)
		{
			
		}
		try
		{
			pgsAssignToRecord(wxT("a"), pnew pgsNumber(wxT("1")), pnew
					pgsNumber(wxT("0"), pgsReal), pnew pgsString(wxT("x"))).eval(vars);
			TS_ASSERT(false); // Wrong column (real)
		}
		catch (const pgsParameterException &)
		{
			
		}
		try
		{
			pgsAssignToRecord(wxT("a"), pnew pgsRecord(1), pnew
					pgsNumber(wxT("0")), pnew pgsString(wxT("x"))).eval(vars);
			TS_ASSERT(false); // Wrong line (record)
		}
		catch (const pgsParameterException &)
		{
			
		}
		try
		{
			pgsAssignToRecord(wxT("a"), pnew pgsString(wxT("1")), pnew
					pgsNumber(wxT("0")), pnew pgsString(wxT("x"))).eval(vars);
			TS_ASSERT(false); // Wrong line (string)
		}
		catch (const pgsParameterException &)
		{
			
		}
		try
		{
			pgsAssignToRecord(wxT("a"), pnew pgsNumber(wxT("1"), pgsReal), pnew
					pgsNumber(wxT("0")), pnew pgsString(wxT("x"))).eval(vars);
			TS_ASSERT(false); // Wrong line (real)
		}
		catch (const pgsParameterException &)
		{
			
		}
		
		// [2] Test copy constructor
		pgsAssignToRecord ident(wxT("b"), pnew pgsNumber(wxT("8")), pnew
				pgsNumber(wxT("0")), pnew pgsString(wxT("x")));
		pgsAssignToRecord copy(ident);
		TS_ASSERT(ident.eval(vars)->value() == copy.eval(vars)->value()
				&& copy.eval(vars)->value() == wxT("x"));
		TS_ASSERT(copy.value() == wxT("SET b[8][0] = x"));
		
		// [3] Test assignment
		pgsAssignToRecord assign(wxT("b"), pnew pgsNumber(wxT("1")), pnew
				pgsNumber(wxT("0")), pnew pgsString(wxT("v")));
		assign = copy;
		TS_ASSERT(assign.eval(vars)->value() == copy.eval(vars)->value()
				&& assign.eval(vars)->value() == wxT("x"));
		TS_ASSERT(assign.value() == wxT("SET b[8][0] = x"));
	}
}
