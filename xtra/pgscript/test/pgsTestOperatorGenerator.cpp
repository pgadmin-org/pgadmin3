//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// 
// Copyright (C) 2002 - 2009, The pgAdmin Development Team
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

void pgsTestSuite::test_operator_generator(void)
{
	// From an integer generator with a string and a number
	{
		// [1] Create variables
		pgsIntegerGen * gen = 0;
		gen = pnew pgsIntegerGen(100, 100);
		pgsVariable * var0 = 0;
		var0 = pnew pgsGenerator(pgsVariable::pgsTInt, gen);
		pgsVariable * var1 = 0;
		var1 = pnew pgsString(wxT("5"));
		pgsVariable * var2 = 0;
		var2 = pnew pgsNumber(wxT("5"));
		
		// [2] Addition
		try
		{
			(*var0 + *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		TS_ASSERT((*var0 + *var2)->value() == wxT("105"));
		
		// [3] Subtraction
		try
		{
			(*var0 - *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		TS_ASSERT((*var0 - *var2)->value() == wxT("95"));
		
		// [4] Multiplication
		try
		{
			(*var0 * *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		TS_ASSERT((*var0 * *var2)->value() == wxT("500"));
		
		// [5] Division
		try
		{
			(*var0 / *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		TS_ASSERT((*var0 / *var2)->value() == wxT("20"));
		
		// [6] Modulo
		try
		{
			(*var0 % *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		TS_ASSERT((*var0 % *var2)->value() == wxT("0"));
		
		// [9] Equal
		try
		{
			(*var0 == *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		TS_ASSERT((*var0 == *var2)->value() == wxT("0"));
		
		// [10] Different
		try
		{
			(*var0 != *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		TS_ASSERT((*var0 != *var2)->value() == wxT("1"));
		
		// [11] Lower
		try
		{
			(*var0 < *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		TS_ASSERT((*var0 < *var2)->value() == wxT("0"));
		
		// [12] Greater
		try
		{
			(*var0 > *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		TS_ASSERT((*var0 > *var2)->value() == wxT("1"));
		
		// [13] Lower or equal
		try
		{
			(*var0 <= *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		TS_ASSERT((*var0 <= *var2)->value() == wxT("0"));
		
		// [14] Greater or equal
		try
		{
			(*var0 >= *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		TS_ASSERT((*var0 >= *var2)->value() == wxT("1"));
		
		// [15] Not
		TS_ASSERT((!(*var0))->value() == wxT("0"));
		
		// [16] Almost equal
		try
		{
			(*var0 &= *var1);
			TS_ASSERT(false);
		}
		catch (const pgsException &)
		{
			
		}
		TS_ASSERT((*var0 &= *var2)->value() == wxT("0"));
		
		// [17] Is true?
		TS_ASSERT(var0->pgs_is_true() == true);
		TS_ASSERT((!(*var0))->pgs_is_true() == false);
		
		// [18] Delete variables
		pdelete(var0);
		pdelete(var1);
		pdelete(var2);
	}
	
	// From an string generator with a string
	{
		// [1] Create variables
		pgsStringGen * gen = 0;
		gen = pnew pgsStringGen(10, 20);
		pgsVariable * var0 = 0;
		var0 = pnew pgsGenerator(pgsVariable::pgsTString, gen);
		pgsVariable * var1 = 0;
		var1 = pnew pgsString(wxT("abc"));
		
		// [2] Addition
		TS_ASSERT((*var0 + *var1)->value().EndsWith(wxT("abc")));
		
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
}
