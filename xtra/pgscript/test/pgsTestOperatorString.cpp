//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgsTestSuite.h"

#include "pgscript/exceptions/pgsException.h"
#include "pgscript/generators/pgsIntegerGen.h"
#include "pgscript/generators/pgsStringGen.h"
#include "pgscript/objects/pgsGenerator.h"
#include "pgscript/objects/pgsNumber.h"
#include "pgscript/objects/pgsRecord.h"
#include "pgscript/objects/pgsString.h"

void pgsTestSuite::test_operator_string(void)
{
	// From a string with a string
	{
		// [1] Create variables
		pgsVariable * var0 = 0;
		var0 = pnew pgsString(wxT("abc"));
		pgsVariable * var1 = 0;
		var1 = pnew pgsString(wxT("ABC"));
		
		// [2] Addition
		TS_ASSERT((*var0 + *var1)->value() == wxT("abcABC"));
		
		// [3] Subtraction
		try
		{
			(*var0 - *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [4] Multiplication
		try
		{
			(*var0 * *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [5] Division
		try
		{
			(*var0 / *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [6] Modulo
		try
		{
			(*var0 % *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [9] Equal
		TS_ASSERT((*var0 == *var1)->value() == wxT("0"));
		
		// [10] Different
		TS_ASSERT((*var0 != *var1)->value() == wxT("1"));
		
		// [11] Lower
		TS_ASSERT((*var0 < *var1)->value() == wxT("0"));
		
		// [12] Greater
		TS_ASSERT((*var0 > *var1)->value() == wxT("1"));
		
		// [13] Lower or equal
		TS_ASSERT((*var0 <= *var1)->value() == wxT("0"));
		
		// [14] Greater or equal
		TS_ASSERT((*var0 >= *var1)->value() == wxT("1"));
		
		// [15] Not
		TS_ASSERT((!(*var0))->value() == wxT(""));
		pgsOperand op(pnew pgsString(wxT("")));
		TS_ASSERT((!(*op))->value() == wxT("1"));
		
		// [16] Almost equal
		TS_ASSERT((*var0 &= *var1)->value() == wxT("1"));
		
		// [17] Is true?
		TS_ASSERT(var0->pgs_is_true() == true);
		TS_ASSERT((!(*var0))->pgs_is_true() == false);
		
		// [18] Delete variables
		pdelete(var0);
		pdelete(var1);
	}
	
	// From a string with a number
	{
		// [1] Create variables
		pgsVariable * var0 = 0;
		var0 = pnew pgsString(wxT("abc"));
		pgsVariable * var1 = 0;
		var1 = pnew pgsNumber(wxT("123"));
		
		// [2] Addition
		try
		{
			(*var0 + *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [3] Subtraction
		try
		{
			(*var0 - *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [4] Multiplication
		try
		{
			(*var0 * *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [5] Division
		try
		{
			(*var0 / *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [6] Modulo
		try
		{
			(*var0 % *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [9] Equal
		try
		{
			(*var0 == *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [10] Different
		try
		{
			(*var0 != *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [11] Lower
		try
		{
			(*var0 < *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [12] Greater
		try
		{
			(*var0 > *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [13] Lower or equal
		try
		{
			(*var0 <= *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [14] Greater or equal
		try
		{
			(*var0 >= *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [15] Not
		TS_ASSERT((!(*var0))->value() == wxT(""));
		
		// [16] Almost equal
		try
		{
			(*var0 &= *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [17] Is true?
		TS_ASSERT(var0->pgs_is_true() == true);
		TS_ASSERT((!(*var0))->pgs_is_true() == false);
		
		// [18] Delete variables
		pdelete(var0);
		pdelete(var1);
	}
	
	// From a string with a record
	{
		// [1] Create variables
		pgsVariable * var0 = 0;
		var0 = pnew pgsString(wxT("abc"));
		pgsRecord * rec = 0;
		rec = pnew pgsRecord(1);
		rec->insert(0, 0, pnew pgsString(wxT("ABC")));
		pgsVariable * var1 = 0;
		var1 = rec;
		
		// [2] Addition
		try
		{
			(*var0 + *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [3] Subtraction
		try
		{
			(*var0 - *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [4] Multiplication
		try
		{
			(*var0 * *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [5] Division
		try
		{
			(*var0 / *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [6] Modulo
		try
		{
			(*var0 % *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [9] Equal
		try
		{
			(*var0 == *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [10] Different
		try
		{
			(*var0 != *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [11] Lower
		try
		{
			(*var0 < *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [12] Greater
		try
		{
			(*var0 > *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [13] Lower or equal
		try
		{
			(*var0 <= *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [14] Greater or equal
		try
		{
			(*var0 >= *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [15] Not
		TS_ASSERT((!(*var0))->value() == wxT(""));
		
		// [16] Almost equal
		try
		{
			(*var0 &= *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [17] Is true?
		TS_ASSERT(var0->pgs_is_true() == true);
		TS_ASSERT((!(*var0))->pgs_is_true() == false);
		
		// [18] Delete variables
		pdelete(var0);
		pdelete(var1);
	}
	
	// From a string with a string generator
	{
		// [1] Create variables
		pgsVariable * var0 = 0;
		var0 = pnew pgsString(wxT("abc"));
		pgsStringGen * gen = 0;
		gen = pnew pgsStringGen(10, 20);
		pgsVariable * var1 = 0;
		var1 = pnew pgsGenerator(pgsVariable::pgsTString, gen);
		
		// [2] Addition
		TS_ASSERT((*var0 + *var1)->value().StartsWith(wxT("abc")));
		
		// [3] Subtraction
		try
		{
			(*var0 - *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [4] Multiplication
		try
		{
			(*var0 * *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [5] Division
		try
		{
			(*var0 / *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [6] Modulo
		try
		{
			(*var0 % *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [9] Equal
		TS_ASSERT((*var0 == *var1)->value() == wxT("0"));
		
		// [10] Different
		TS_ASSERT((*var0 != *var1)->value() == wxT("1"));
		
		// [11] Lower
		try
		{
			(*var0 < *var1);
		}
		catch (const pgsException &)
		{
			TS_ASSERT(false);
		}
		
		// [12] Greater
		try
		{
			(*var0 > *var1);
		}
		catch (const pgsException &)
		{
			TS_ASSERT(false);
		}
		
		// [13] Lower or equal
		try
		{
			(*var0 <= *var1);
		}
		catch (const pgsException &)
		{
			TS_ASSERT(false);
		}
		
		// [14] Greater or equal
		try
		{
			(*var0 >= *var1);
		}
		catch (const pgsException &)
		{
			TS_ASSERT(false);
		}
		
		// [15] Not
		TS_ASSERT((!(*var0))->value() == wxT(""));
		
		// [16] Almost equal
		TS_ASSERT((*var0 &= *var1)->value() == wxT("0"));
		
		// [17] Is true?
		TS_ASSERT(var0->pgs_is_true() == true);
		TS_ASSERT((!(*var0))->pgs_is_true() == false);
		
		// [18] Delete variables
		pdelete(var0);
		pdelete(var1);
	}
	
	// From a string with an integer generator
	{
		// [1] Create variables
		pgsVariable * var0 = 0;
		var0 = pnew pgsString(wxT("abc"));
		pgsIntegerGen * gen = 0;
		gen = pnew pgsIntegerGen(10, 10);
		pgsVariable * var1 = 0;
		var1 = pnew pgsGenerator(pgsVariable::pgsTInt, gen);
		
		// [2] Addition
		try
		{
			(*var0 + *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [3] Subtraction
		try
		{
			(*var0 - *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [4] Multiplication
		try
		{
			(*var0 * *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [5] Division
		try
		{
			(*var0 / *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [6] Modulo
		try
		{
			(*var0 % *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [9] Equal
		try
		{
			(*var0 == *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [10] Different
		try
		{
			(*var0 != *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [11] Lower
		try
		{
			(*var0 < *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [12] Greater
		try
		{
			(*var0 > *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [13] Lower or equal
		try
		{
			(*var0 <= *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [14] Greater or equal
		try
		{
			(*var0 >= *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [15] Not
		TS_ASSERT((!(*var0))->value() == wxT(""));
		
		// [16] Almost equal
		try
		{
			(*var0 &= *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		
		// [17] Is true?
		TS_ASSERT(var0->pgs_is_true() == true);
		TS_ASSERT((!(*var0))->pgs_is_true() == false);
		
		// [18] Delete variables
		pdelete(var0);
		pdelete(var1);
	}
}
