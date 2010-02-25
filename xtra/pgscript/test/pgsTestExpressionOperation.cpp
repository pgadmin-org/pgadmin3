//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgsTestSuite.h"

#include "pgscript/exceptions/pgsArithmeticException.h"
#include "pgscript/expressions/pgsAnd.h"
#include "pgscript/expressions/pgsDifferent.h"
#include "pgscript/expressions/pgsEqual.h"
#include "pgscript/expressions/pgsGreater.h"
#include "pgscript/expressions/pgsGreaterEqual.h"
#include "pgscript/expressions/pgsLower.h"
#include "pgscript/expressions/pgsLowerEqual.h"
#include "pgscript/expressions/pgsMinus.h"
#include "pgscript/expressions/pgsModulo.h"
#include "pgscript/expressions/pgsNegate.h"
#include "pgscript/expressions/pgsNot.h"
#include "pgscript/expressions/pgsOr.h"
#include "pgscript/expressions/pgsOver.h"
#include "pgscript/expressions/pgsParenthesis.h"
#include "pgscript/expressions/pgsPlus.h"
#include "pgscript/expressions/pgsTimes.h"
#include "pgscript/objects/pgsNumber.h"
#include "pgscript/objects/pgsString.h"

void pgsTestSuite::test_expression_operation(void)
{
	srand(wxDateTime::GetTimeNow());
	
	const int max = +16383;
	const int min = -16384;
	const int nb_iterations = 100;
	
	pgsVarMap vars;

	// Test each operation with integers
	{
		// Iterate a certain number of times in order to be sure it works everytime
		// The goal is to use small integers that can be computed with standard C
		// and then compare to what computes the pgScript operations
		// pgScript uses strings as a representation for any number
		for (int i = 1; i <= nb_iterations; i++)
		{
			// Generate random integers
			// pgScript operations manipulate strings: convert them to strings
			int j = rand() % (max - min) + min;
			int k = rand() % (max - min) + min;
			wxString sj;
			sj << j;
			wxString sk;
			sk << k;

			// pgsPlus (addition)
			{
				pgsPlus operation(pnew pgsNumber(sj), pnew pgsNumber(sk));
				// [1] Check the result
				int result = j + k;
				wxString res_str;
				res_str << result;
				TS_ASSERT(res_str == operation.eval(vars)->value());
				TS_ASSERT(MAPM(result) == MAPM(operation.eval(vars)->value().mb_str()));
				// [2] Check the copy constructor
				pgsPlus copy(operation);
				TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
				// [3] Check the assignment operator
				operation = copy;
				TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
				// [4] Check value()
				TS_ASSERT(operation.value() == (wxString() << j << wxT(" + ") << k));
			}

			// pgsPlus with pgsString
			{
				pgsPlus operation(pnew pgsString(sj), pnew pgsString(sk));
				// [1] Check the result: concatenation instead of sum
				wxString res_str;
				res_str << j << k;
				TS_ASSERT(res_str == operation.eval(vars)->value());
				// [2] Check the copy constructor
				pgsPlus copy(operation);
				TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
				// [3] Check the assignment operator
				operation = copy;
				TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
				// [4] Check value()
				TS_ASSERT(operation.value() == (wxString() << j << wxT(" + ") << k));
			}

			// pgsMinus
			{
				pgsMinus operation(pnew pgsNumber(sj), pnew pgsNumber(sk));
				// [1] Check the result
				int result = j - k;
				wxString res_str;
				res_str << result;
				TS_ASSERT(res_str == operation.eval(vars)->value());
				TS_ASSERT(MAPM(result) == MAPM(operation.eval(vars)->value().mb_str()));
				// [2] Check the copy constructor
				pgsMinus copy(operation);
				TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
				// [3] Check the assignment operator
				operation = copy;
				TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
				// [4] Check value()
				TS_ASSERT(operation.value() == (wxString() << j << wxT(" - ") << k));
			}

			// pgsTimes
			{
				pgsTimes operation(pnew pgsNumber(sj), pnew pgsNumber(sk));
				// [1] Check the result
				int result = j * k;
				wxString res_str;
				res_str << result;
				TS_ASSERT(res_str == operation.eval(vars)->value());
				TS_ASSERT(MAPM(result) == MAPM(operation.eval(vars)->value().mb_str()));
				// [2] Check the copy constructor
				pgsTimes copy(operation);
				TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
				// [3] Check the assignment operator
				operation = copy;
				TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
				// [4] Check value()
				TS_ASSERT(operation.value() == (wxString() << j << wxT(" * ") << k));
			}

			// pgsOver with divisor != 0
			if (k != 0)
			{
				pgsOver operation(pnew pgsNumber(sj), pnew pgsNumber(sk));
				// [1] Check the result
				int result = j / k;
				wxString res_str;
				res_str << result;
				TS_ASSERT(res_str == operation.eval(vars)->value());
				TS_ASSERT(MAPM(result) == MAPM(operation.eval(vars)->value().mb_str()));
				// [2] Check the copy constructor
				pgsOver copy(operation);
				TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
				// [3] Check the assignment operator
				operation = copy;
				TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
				// [4] Check value()
				TS_ASSERT(operation.value() == (wxString() << j << wxT(" / ") << k));
			}

			// pgsOver with divisor == 0
			{
				pgsOver operation(pnew pgsNumber(sj), pnew pgsNumber(wxT("0.e-10"),
						pgsReal));
				try
				{
					operation.eval(vars)->value();
					TS_ASSERT(false);
				}
				catch (const pgsArithmeticException &)
				{
					// Divsion with 0
				}
			}

			// pgsOver with divisor == 0
			{
				pgsOver operation(pnew pgsNumber(sj), pnew pgsNumber(wxT(".0"),
						pgsReal));
				try
				{
					operation.eval(vars)->value();
					TS_ASSERT(false);
				}
				catch (const pgsArithmeticException &)
				{
					// Divsion with 0
				}
			}

			// pgsModulo with divisor == 0
			{
				pgsModulo operation(pnew pgsNumber(sj), pnew pgsNumber(wxT("0"),
						pgsInt));
				try
				{
					operation.eval(vars)->value();
					TS_ASSERT(false);
				}
				catch (const pgsArithmeticException &)
				{
					// Divsion with 0
				}
			}

			// pgsModulo
			if (k != 0)
			{
				pgsModulo operation(pnew pgsNumber(sj), pnew pgsNumber(sk));
				// [1] Check the result
				int result = j % k;
				wxString res_str;
				res_str << result;
				TS_ASSERT(res_str == operation.eval(vars)->value());
				TS_ASSERT(MAPM(result) == MAPM(operation.eval(vars)->value().mb_str()));
				// [2] Check the copy constructor of the super class
				pgsOperation & copy(operation);
				TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
				// [4] Check value()
				TS_ASSERT(operation.value() == (wxString() << j << wxT(" % ") << k));
			}

			// pgsEqual
			{
				pgsEqual operation(pnew pgsNumber(sj), pnew pgsNumber(sk));
				// [1] Check the result
				int result = (j == k ? 1 : 0);
				wxString res_str;
				res_str << result;
				TS_ASSERT(res_str == operation.eval(vars)->value());
				TS_ASSERT(MAPM(result) == MAPM(operation.eval(vars)->value().mb_str()));
				// [2] Check the copy constructor of the super class
				pgsOperation & copy(operation);
				TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
				// [4] Check value()
				TS_ASSERT(operation.value() == (wxString() << j << wxT(" = ") << k));
			}

			// pgsDifferent
			{
				pgsDifferent operation(pnew pgsNumber(sj), pnew pgsNumber(sk));
				// [1] Check the result
				int result = (j != k ? 1 : 0);
				wxString res_str;
				res_str << result;
				TS_ASSERT(res_str == operation.eval(vars)->value());
				TS_ASSERT(MAPM(result) == MAPM(operation.eval(vars)->value().mb_str()));
				// [2] Check the copy constructor of the super class
				pgsOperation & copy(operation);
				TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
				// [4] Check value()
				TS_ASSERT(operation.value() == (wxString() << j << wxT(" <> ") << k));
			}

			// pgsGreater
			{
				pgsGreater operation(pnew pgsNumber(sj), pnew pgsNumber(sk));
				// [1] Check the result
				int result = (j > k ? 1 : 0);
				wxString res_str;
				res_str << result;
				TS_ASSERT(res_str == operation.eval(vars)->value());
				TS_ASSERT(MAPM(result) == MAPM(operation.eval(vars)->value().mb_str()));
				// [2] Check the copy constructor of the super class
				pgsOperation & copy(operation);
				TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
				// [4] Check value()
				TS_ASSERT(operation.value() == (wxString() << j << wxT(" > ") << k));
			}

			// pgsLower
			{
				pgsLower operation(pnew pgsNumber(sj), pnew pgsNumber(sk));
				// [1] Check the result
				int result = (j < k ? 1 : 0);
				wxString res_str;
				res_str << result;
				TS_ASSERT(res_str == operation.eval(vars)->value());
				TS_ASSERT(MAPM(result) == MAPM(operation.eval(vars)->value().mb_str()));
				// [2] Check the copy constructor of the super class
				pgsOperation & copy(operation);
				TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
				// [4] Check value()
				TS_ASSERT(operation.value() == (wxString() << j << wxT(" < ") << k));
			}

			// pgsGreaterEqual
			{
				pgsGreaterEqual operation(pnew pgsNumber(sj), pnew pgsNumber(sk));
				// [1] Check the result
				int result = (j >= k ? 1 : 0);
				wxString res_str;
				res_str << result;
				TS_ASSERT(res_str == operation.eval(vars)->value());
				TS_ASSERT(MAPM(result) == MAPM(operation.eval(vars)->value().mb_str()));
				// [2] Check the copy constructor of the super class
				pgsOperation & copy(operation);
				TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
				// [4] Check value()
				TS_ASSERT(operation.value() == (wxString() << j << wxT(" >= ") << k));
			}

			// pgsLowerEqual
			{
				pgsLowerEqual operation(pnew pgsNumber(sj), pnew pgsNumber(sk));
				// [1] Check the result
				int result = (j <= k ? 1 : 0);
				wxString res_str;
				res_str << result;
				TS_ASSERT(res_str == operation.eval(vars)->value());
				TS_ASSERT(MAPM(result) == MAPM(operation.eval(vars)->value().mb_str()));
				// [2] Check the copy constructor of the super class
				pgsOperation & copy(operation);
				TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
				// [4] Check value()
				TS_ASSERT(operation.value() == (wxString() << j << wxT(" <= ") << k));
			}
			
			// pgsNegate
			{
				pgsNegate operation(pnew pgsNumber(sj));
				// [1] Check the result
				int result = -j;
				wxString res_str;
				res_str << result;
				TS_ASSERT(res_str == operation.eval(vars)->value());
				TS_ASSERT(MAPM(result) == MAPM(operation.eval(vars)->value().mb_str()));
				// [2] Check the copy constructor
				pgsNegate copy(operation);
				TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
				// [3] Check the assignment operator
				operation = copy;
				TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
				// [4] Check value()
				TS_ASSERT(operation.value() == (wxString() << wxT("-") << j));
			}
			
			// pgsParenthesis
			{
				pgsParenthesis operation(pnew pgsNumber(sj));
				// [1] Check the result
				int result = j;
				wxString res_str;
				res_str << result;
				TS_ASSERT(res_str == operation.eval(vars)->value());
				TS_ASSERT(MAPM(result) == MAPM(operation.eval(vars)->value().mb_str()));
				// [2] Check the copy constructor
				pgsParenthesis copy(operation);
				TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
				// [3] Check the assignment operator
				operation = copy;
				TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
				// [4] Check value()
				TS_ASSERT(operation.value() == (wxString() << wxT("(") << j << wxT(")")));
			}
		}
	}

	// Test AND and OR operations
	{
		for (int i = 1; i <= nb_iterations; i++)
		{
			// Generate random 0 or 1
			// pgScript operations manipulate strings: convert them to strings
			int j = (rand() % (max - min) + min) > (max + min) / 2 ? 1 : 0;
			int k = (rand() % (max - min) + min) < (max + min) / 2 ? 1 : 0;
			wxString sj;
			sj << j;
			wxString sk;
			sk << k;

			// pgsAnd
			{
				pgsAnd operation(pnew pgsNumber(sj), pnew pgsNumber(sk));
				// [1] Check the result
				int result = (j && k);
				wxString res_str;
				res_str << result;
				TS_ASSERT(res_str == operation.eval(vars)->value());
				TS_ASSERT(MAPM(result) == MAPM(operation.eval(vars)->value().mb_str()));
				// [2] Check the copy constructor of the super class
				pgsOperation & copy(operation);
				TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
				// [4] Check value()
				TS_ASSERT(operation.value() == (wxString() << j << wxT(" AND ") << k));
			}
			
			// pgsOr
			{
				pgsOr operation(pnew pgsNumber(sj), pnew pgsNumber(sk));
				// [1] Check the result
				int result = (j || k);
				wxString res_str;
				res_str << result;
				TS_ASSERT(res_str == operation.eval(vars)->value());
				TS_ASSERT(MAPM(result) == MAPM(operation.eval(vars)->value().mb_str()));
				// [2] Check the copy constructor of the super class
				pgsOperation & copy(operation);
				TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
				// [4] Check value()
				TS_ASSERT(operation.value() == (wxString() << j << wxT(" OR ") << k));
			}
		}
	}
	
	// Test NOT operation
	{
		// With a non empty string
		{
			pgsNot operation(pnew pgsString(wxT("abc")));
			// [1] Check the result
			TS_ASSERT(operation.eval(vars)->value() == wxT(""));
			// [2] Check the copy constructor
			pgsNot copy(operation);
			TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
			// [3] Check the assignment operator
			operation = copy;
			TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
			// [4] Check value()
			TS_ASSERT(operation.value() == wxT("NOT abc"));
		}
		
		// With an empty string
		{
			pgsNot operation(pnew pgsString(wxT("")));
			// [1] Check the result
			TS_ASSERT(operation.eval(vars)->value() != wxT(""));
			// [2] Check the copy constructor of the super class
			pgsOperation & copy(operation);
			TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
			// [4] Check value()
			TS_ASSERT(operation.value() == wxT("NOT "));
		}
		
		// With a number (not 0)
		{
			pgsNot operation(pnew pgsNumber(wxT("10.5"), pgsReal));
			// [1] Check the result
			TS_ASSERT(operation.eval(vars)->value() == wxT("0"));
			// [2] Check the copy constructor of the super class
			pgsOperation & copy(operation);
			TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
		}
		
		// With a number (0)
		{
			pgsNot operation(pnew pgsNumber(wxT("0"), pgsReal));
			// [1] Check the result
			TS_ASSERT(operation.eval(vars)->value() != wxT("0"));
			// [2] Check the copy constructor of the super class
			pgsOperation & copy(operation);
			TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
		}
	}

	// Test each operation with an integer and a string
	{
		wxString sj(wxT("1234567890987654321"));
		wxString sk(wxT("12x3456789098765432"));

		// pgsPlus
		{
			pgsPlus operation(pnew pgsNumber(sj), pnew pgsString(sk));
			try
			{
				operation.eval(vars)->value();
				TS_ASSERT(false);
			}
			catch (const pgsArithmeticException &)
			{
				// Impossible operation if one of the operands is a string
			}
		}

		// pgsMinus
		{
			pgsMinus operation(pnew pgsNumber(sj), pnew pgsString(sk));
			try
			{
				operation.eval(vars)->value();
				TS_ASSERT(false);
			}
			catch (const pgsArithmeticException &)
			{
				// Impossible operation if one of the operands is a string
			}
		}

		// pgsTimes
		{
			pgsTimes operation(pnew pgsNumber(sj), pnew pgsString(sk));
			try
			{
				operation.eval(vars)->value();
				TS_ASSERT(false);
			}
			catch (const pgsArithmeticException &)
			{
				// Impossible operation if one of the operands is a string
			}
		}

		// pgsOver
		{
			pgsOver operation(pnew pgsNumber(sj), pnew pgsString(sk));
			try
			{
				operation.eval(vars)->value();
				TS_ASSERT(false);
			}
			catch (const pgsArithmeticException &)
			{
				// Impossible operation if one of the operands is a string
			}
		}

		// pgsModulo
		{
			pgsModulo operation(pnew pgsNumber(sj), pnew pgsString(sk));
			try
			{
				operation.eval(vars)->value();
				TS_ASSERT(false);
			}
			catch (const pgsArithmeticException &)
			{
				// Impossible operation if one of the operands is a string
			}
		}

		// pgsEqual
		{
			pgsEqual operation(pnew pgsNumber(sj), pnew pgsString(sk));
			try
			{
				operation.eval(vars)->value();
				TS_ASSERT(false);
			}
			catch (const pgsArithmeticException &)
			{
				// Impossible operation if one of the operands is a string
			}
		}

		// pgsDifferent
		{
			pgsDifferent operation(pnew pgsNumber(sj), pnew pgsString(sk));
			try
			{
				operation.eval(vars)->value();
				TS_ASSERT(false);
			}
			catch (const pgsArithmeticException &)
			{
				// Impossible operation if one of the operands is a string
			}
		}

		// pgsGreater
		{
			pgsGreater operation(pnew pgsNumber(sj), pnew pgsString(sk));
			try
			{
				operation.eval(vars)->value();
				TS_ASSERT(false);
			}
			catch (const pgsArithmeticException &)
			{
				// Impossible operation if one of the operands is a string
			}
		}

		// pgsLower
		{
			pgsLower operation(pnew pgsNumber(sj), pnew pgsString(sk));
			try
			{
				operation.eval(vars)->value();
				TS_ASSERT(false);
			}
			catch (const pgsArithmeticException &)
			{
				// Impossible operation if one of the operands is a string
			}
		}

		// pgsGreaterEqual
		{
			pgsGreaterEqual operation(pnew pgsNumber(sj), pnew pgsString(sk));
			try
			{
				operation.eval(vars)->value();
				TS_ASSERT(false);
			}
			catch (const pgsArithmeticException &)
			{
				// Impossible operation if one of the operands is a string
			}
		}

		// pgsLowerEqual
		{
			pgsLowerEqual operation(pnew pgsNumber(sj), pnew pgsString(sk));
			try
			{
				operation.eval(vars)->value();
				TS_ASSERT(false);
			}
			catch (const pgsArithmeticException &)
			{
				// Impossible operation if one of the operands is a string
			}
		}

		// pgsAnd
		{
			pgsAnd operation(pnew pgsNumber(sj), pnew pgsString(sk));
			// [1] Check the result: concatenation
			TS_ASSERT(operation.eval(vars)->value() == wxT("1"));
			// [2] Check the copy constructor
			pgsAnd copy(operation);
			TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
			// [3] Check the assignment operator
			operation = copy;
			TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
		}

		// pgsOr
		{
			pgsOr operation(pnew pgsNumber(sj), pnew pgsString(sk));
			// [1] Check the result: concatenation
			TS_ASSERT(operation.eval(vars)->value() == wxT("1"));
			// [2] Check the copy constructor
			pgsOr copy(operation);
			TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
			// [3] Check the assignment operator
			operation = copy;
			TS_ASSERT(operation.eval(vars)->value() == copy.eval(vars)->value());
		}
	}
	
	// Test pgsEqual
	{
		// Equal with case_sensitive = true
		{
			pgsEqual operation(pnew pgsString(wxT("a")),
					pnew pgsString(wxT("a")));
			TS_ASSERT(operation.eval(vars)->value() == wxT("1"));
		}
		
		// Not equal with case_sensitive = true
		{
			pgsEqual operation(pnew pgsString(wxT("a")),
					pnew pgsString(wxT("A")));
			TS_ASSERT(operation.eval(vars)->value() == wxT("0"));
		}
		
		// Equal with case_sensitive = false
		{
			pgsEqual operation(pnew pgsString(wxT("a")),
					pnew pgsString(wxT("A")), false);
			TS_ASSERT(operation.eval(vars)->value() == wxT("1"));
		}
		
		// Not equal with case_sensitive = true
		{
			pgsEqual operation(pnew pgsString(wxT("abcdefg!:;")),
					pnew pgsString(wxT("abcDefg!:;")));
			TS_ASSERT(operation.eval(vars)->value() == wxT("0"));
		}
		
		// Equal with case_sensitive = false
		{
			pgsEqual operation(pnew pgsString(wxT("abcdefg!:;")),
					pnew pgsString(wxT("abcDefg!:;")), false);
			TS_ASSERT(operation.eval(vars)->value() == wxT("1"));
		}
	}
}
