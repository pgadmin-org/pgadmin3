//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
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

void pgsTestSuite::test_operator_number(void)
{
	// From a number with a string
	{
		// [1] Create variables
		pgsVariable * var0 = 0;
		var0 = pnew pgsNumber(wxT("100"));
		pgsVariable * var1 = 0;
		var1 = pnew pgsString(wxT("5"));
		
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
		TS_ASSERT((!(*var0))->value() == wxT("0"));
		pgsOperand op(pnew pgsNumber(wxT("0")));
		TS_ASSERT((!(*op))->value() == wxT("1"));
		
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
	
	// From a number (integer) with a real number
	{
		// [1] Create variables
		pgsVariable * var0 = 0;
		var0 = pnew pgsNumber(wxT("100"), pgsInt);
		pgsVariable * var1 = 0;
		var1 = pnew pgsNumber(wxT("5"), pgsReal);
		
		// [2] Addition
		TS_ASSERT((*var0 + *var1)->value() == wxT("105"));
		
		// [3] Subtraction
		TS_ASSERT((*var0 - *var1)->value() == wxT("95"));
		
		// [4] Multiplication
		TS_ASSERT((*var0 * *var1)->value() == wxT("500"));
		
		// [5] Division
		TS_ASSERT((*var0 / *var1)->value() == wxT("20"));
		
		// [6] Modulo
		TS_ASSERT((*var0 % *var1)->value() == wxT("0"));
		
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
		TS_ASSERT((!(*var0))->value() == wxT("0"));
		
		// [16] Almost equal
		TS_ASSERT((*var0 &= *var1)->value() == wxT("0"));
		
		// [17] Is true?
		TS_ASSERT(var0->pgs_is_true() == true);
		TS_ASSERT((!(*var0))->pgs_is_true() == false);
		
		// [18] Delete variables
		pdelete(var0);
		pdelete(var1);
	}
	
	// From a number with a record
	{
		// [1] Create variables
		pgsVariable * var0 = 0;
		var0 = pnew pgsNumber(wxT("100"));
		pgsRecord * rec = 0;
		rec = pnew pgsRecord(1);
		rec->insert(0, 0, pnew pgsString(wxT("5")));
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
		
		// [17] Is true?
		TS_ASSERT(var0->pgs_is_true() == true);
		TS_ASSERT((!(*var0))->pgs_is_true() == false);
		
		// [18] Delete variables
		pdelete(var0);
		pdelete(var1);
	}
	
	// From a number with a record
	{
		// [1] Create variables
		pgsVariable * var0 = 0;
		var0 = pnew pgsNumber(wxT("100"));
		pgsRecord * rec = 0;
		rec = pnew pgsRecord(1);
		rec->insert(0, 0, pnew pgsNumber(wxT("5")));
		rec->insert(1, 0, pnew pgsNumber(wxT("1")));
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
		
		// [18] Delete variables
		pdelete(var0);
		pdelete(var1);
	}
	
	// From a number with a string generator
	{
		// [1] Create variables
		pgsVariable * var0 = 0;
		var0 = pnew pgsNumber(wxT("100"));
		pgsStringGen * gen = 0;
		gen = pnew pgsStringGen(10, 20);
		pgsVariable * var1 = 0;
		var1 = pnew pgsGenerator(pgsVariable::pgsTString, gen);
		
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
		
		// [18] Delete variables
		pdelete(var0);
		pdelete(var1);
	}
	
	// From a number with an integer generator
	{
		// [1] Create variables
		pgsVariable * var0 = 0;
		var0 = pnew pgsNumber(wxT("100"));
		pgsIntegerGen * gen = 0;
		gen = pnew pgsIntegerGen(5, 5);
		pgsVariable * var1 = 0;
		var1 = pnew pgsGenerator(pgsVariable::pgsTInt, gen);
		
		// [2] Addition
		TS_ASSERT((*var0 + *var1)->value() == wxT("105"));
		
		// [3] Subtraction
		TS_ASSERT((*var0 - *var1)->value() == wxT("95"));
		
		// [4] Multiplication
		TS_ASSERT((*var0 * *var1)->value() == wxT("500"));
		
		// [5] Division
		TS_ASSERT((*var0 / *var1)->value() == wxT("20"));
		
		// [6] Modulo
		TS_ASSERT((*var0 % *var1)->value() == wxT("0"));
		
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
		TS_ASSERT((!(*var0))->value() == wxT("0"));
		
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
