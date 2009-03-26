//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2009, The pgAdmin Development Team
// This software is released under the BSD Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgsTestSuite.h"

#include "pgscript/exceptions/pgsArithmeticException.h"
#include "pgscript/expressions/pgsPlus.h"
#include "pgscript/objects/pgsNumber.h"
#include "pgscript/objects/pgsString.h"

void pgsTestSuite::test_object_variable(void)
{
	// A check is done when a pgsNumber is assigned to verify whether it is
	// a valid number. Default is an integer otherwise pgsReal must be
	// specified as second parameter of the constructor
	// pgsString does not perform any check but does not authorize arithmetic
	// on it even if it stores a number
	
	// Test string type
	{
		pgsString a(wxT("test"));
		TS_ASSERT(a.is_string() && !a.is_number() && !a.is_record());
		pgsString b(wxT("123456."));
		TS_ASSERT(b.is_string() && !b.is_number() && !b.is_record());
		pgsString c(wxT("423432"));
		TS_ASSERT(c.is_string() && !c.is_number() && !c.is_record());
		pgsString d(wxT("+.644e5"));
		TS_ASSERT(d.is_string() && !d.is_number() && !d.is_record());
		pgsString e(wxT("0x0"));
		TS_ASSERT(e.is_string() && !e.is_number() && !e.is_record());
	}

	// Test number type
	{
		pgsNumber a(wxT("123456."), pgsReal);
		TS_ASSERT(a.is_real() && !a.is_string() && !a.is_record());
		pgsNumber b(wxT("423432"));
		TS_ASSERT(b.is_integer() && !b.is_string() && !b.is_record());
		pgsNumber c(wxT("+.644e5"), pgsReal);
		TS_ASSERT(c.is_real() && !c.is_string() && !c.is_record());
	}

	// Test integers
	{
		for (int i = 1; i <= 100; i++)
		{
			// [1] Generate a random integer as a string
			wxString str_rnd;
			for (int j = 0; j < i; j++)
			{
				char c = rand() % 9 + 48;
				str_rnd << c;
			}
			
			// [2] Allocate a number and test type properties
			pgsNumber exp(str_rnd);
			TS_ASSERT(exp.value() == str_rnd);
			TS_ASSERT(exp.is_number() && !exp.is_string() && !exp.is_record());
			TS_ASSERT(exp.is_integer() && !exp.is_real());
			
			// [3] Test copy constructor
			pgsNumber copy(exp);
			TS_ASSERT(copy.value() == exp.value() && copy.is_integer());
			
			// [4] Test assignment operator
			exp = pgsNumber(wxT("1") + str_rnd, pgsReal);
			TS_ASSERT(exp.is_number() && !exp.is_string() && !exp.is_record());
			TS_ASSERT(!exp.is_integer() && exp.is_real());
		}
	}

	// Test reals
	{
		for (int i = 2; i <= 16; i++)
		{
			// [1] Generate a random real as a string
			wxString str_rnd;
			for (int j = 0; j < i / 2; j++)
			{
				char c = rand() % 9 + 48;
				str_rnd << c;
			}
			str_rnd << wxT(".");
			for (int j = 0; j < i / 2; j++)
			{
				char c = rand() % 9 + 48;
				str_rnd << c;
			}
			
			// [2] Allocate a number and test type properties
			pgsNumber exp(str_rnd, pgsReal);
			TS_ASSERT(exp.is_number() && !exp.is_string() && !exp.is_record());
			TS_ASSERT(!exp.is_integer() && exp.is_real());
			
			// [3] Test copy constructor
			pgsNumber copy(exp);
			TS_ASSERT(copy.value() == exp.value() && copy.is_real());
			
			// [4] Test assignment operator
			exp = pgsNumber(wxT("1") + str_rnd, pgsReal);
			TS_ASSERT(exp.is_number() && !exp.is_string() && !exp.is_record());
			TS_ASSERT(!exp.is_integer() && exp.is_real());
		}
	}

	// Test real
	{
		pgsNumber exp(wxT("+1.5e-300000000000657788"), pgsReal);
		TS_ASSERT(exp.is_real() && !exp.is_integer() && !exp.is_string());
	}

	// Test real
	{
		pgsNumber exp(wxT("-1.e+0"), pgsReal);
		TS_ASSERT(exp.is_real() && !exp.is_integer() && !exp.is_string());
	}

	// Test real
	{
		pgsNumber exp(wxT("+.0e-1"), pgsReal);
		TS_ASSERT(exp.is_real() && !exp.is_integer() && !exp.is_string());
	}

	// Test real
	{
		pgsNumber exp(wxT("-0.0E5"), pgsReal);
		TS_ASSERT(exp.is_real() && !exp.is_integer() && !exp.is_string());
	}

	// Test real
	{
		pgsNumber exp(wxT("0."), pgsReal);
		TS_ASSERT(exp.is_real() && !exp.is_integer() && !exp.is_string());
	}

	// Test real
	{
		pgsNumber exp(wxT(".1234567890098765432"), pgsReal);
		TS_ASSERT(exp.is_real() && !exp.is_integer() && !exp.is_string());
	}

	// Test string
	{
		pgsString exp(wxT("."));
		TS_ASSERT(!exp.is_real() && !exp.is_integer() && exp.is_string());
	}

	// Test string
	{
		pgsString exp(wxT(""));
		TS_ASSERT(!exp.is_real() && !exp.is_integer() && exp.is_string());
	}

	// Test string
	{
		pgsString exp(wxT("e5"));
		TS_ASSERT(!exp.is_real() && !exp.is_integer() && exp.is_string());
	}

	// Test real
	{
		pgsNumber exp(wxT("0e0"), pgsReal);
		TS_ASSERT(exp.is_real() && !exp.is_integer() && !exp.is_string());
	}

	// Test real
	{
		pgsNumber exp(wxT("100000000000000000e1"), pgsReal);
		TS_ASSERT(exp.is_real() && !exp.is_integer() && !exp.is_string());
	}

	// Test string
	{
		pgsString exp(wxT("100000000000000000e"));
		TS_ASSERT(!exp.is_real() && !exp.is_integer() && exp.is_string());
	}

	// Test some operations
	{
		pgsVariable * a = pnew pgsNumber(wxT("123."), pgsReal);
		pgsVariable * b = pnew pgsNumber(wxT("2"), pgsInt);
		pgsVariable * c = pnew pgsString(wxT("0x1"));
		
		pgsVarMap vars;
		
		// 123. + 2 gives 125
		pgsPlus * d = 0;
		d = pnew pgsPlus(a, b); // Deletes a and b		
		pgsOperand v = d->eval(vars);
		TS_ASSERT(v->value() == wxT("125") && v->is_real());
		
		// (123. + 2) + 0x1 gives the concatenation of the strings
		pgsPlus * e = 0;
		e = pnew pgsPlus(d, c); // Deletes d and c
		try
		{
			e->eval(vars);
			TS_ASSERT(false);
		}
		catch (const pgsArithmeticException &)
		{
			
		}
		
		// Test copy
		pgsPlus f(*e); // f is automatically deleted
		pdelete(e); // Deletes e
	}
}
