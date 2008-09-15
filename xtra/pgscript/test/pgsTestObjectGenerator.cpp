//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsTestObjectGenerator.cpp,v 1.3 2008/08/10 17:45:36 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgsTestSuite.h"

#include <wx/file.h>
#include "pgscript/exceptions/pgsParameterException.h"
#include "pgscript/expressions/pgsGenDateTime.h"
#include "pgscript/expressions/pgsGenDate.h"
#include "pgscript/expressions/pgsGenDictionary.h"
#include "pgscript/expressions/pgsGenInt.h"
#include "pgscript/expressions/pgsGenReal.h"
#include "pgscript/expressions/pgsGenReference.h"
#include "pgscript/expressions/pgsGenRegex.h"
#include "pgscript/expressions/pgsGenString.h"
#include "pgscript/expressions/pgsGenTime.h"
#include "pgscript/generators/pgsStringGen.h"
#include "pgscript/objects/pgsNumber.h"
#include "pgscript/objects/pgsString.h"

void pgsTestSuite::test_object_generator(void)
{
	const int nb_iterations = 20;
	pgsVarMap vars;
	
	///////////////////////
	// INTEGER GENERATOR //
	///////////////////////

	// With an integer generator integer(1, 2, false, wxDateTime::GetTimeNow())
	{
		// [1] Create the generator
		pgsExpression * min = pnew pgsNumber(wxT("1"));
		pgsExpression * max = pnew pgsNumber(wxT("2"));
		pgsExpression * sequence = pnew pgsNumber(wxT("0"));
		pgsExpression * seed = pnew pgsNumber(wxString()
				<< wxDateTime::GetTimeNow());
		pgsGenInt * gen = 0;
		gen = pnew pgsGenInt(min, max, sequence, seed);
		
		// [2] Check type properties
		
		// [3] Check result
		wxString result;
		pgsOperand rnd = gen->eval(vars);
		for (int i = 0; i < nb_iterations; i++)
		{
			result = rnd->eval(vars)->value();
			TS_ASSERT(result == wxT("1") || result == wxT("2"));
		}
		
		// [4] Test copy constructor
		pgsGenInt * copy = pnew pgsGenInt(*gen);
		TS_ASSERT(copy->value() = gen->value());
		TS_ASSERT(copy->value() = gen->value());
		pdelete(copy);
		
		// [5] Test assignment operator
		pgsGenInt * assignment = pnew pgsGenInt(min->clone(), max->clone(),
				sequence->clone(), seed->clone());
		*gen = *assignment;
		TS_ASSERT(assignment->value() = gen->value());
		TS_ASSERT(assignment->value() = gen->value());
		pdelete(assignment);
		
		// [6] Delete the generator
		pdelete(gen);
	}
	
	// With an integer generator integer(-nb_iterations, nb_iterations, true, wxDateTime::GetTimeNow())
	{
		// [1] Create the generator
		pgsExpression * min = pnew pgsNumber(wxString() << -nb_iterations);
		pgsExpression * max = pnew pgsNumber(wxString() << nb_iterations);
		pgsExpression * sequence = pnew pgsNumber(wxT("1"));
		pgsExpression * seed = pnew pgsNumber(wxString()
				<< wxDateTime::GetTimeNow());
		pgsGenInt * gen = 0;
		gen = pnew pgsGenInt(min, max, sequence, seed);
		
		// [2] Check type properties
		
		// [3] Check result is unique
		wxString result;
		wxArrayString sav;
		pgsOperand rnd = gen->eval(vars);
		for (int i = 0; i < nb_iterations * 2 + 1; i++)
		{
			result = rnd->eval(vars)->value();
			TS_ASSERT(sav.Index(result) == wxNOT_FOUND);
			sav.push_back(result);
		}
		
		// [4] Test copy constructor
		pgsGenInt * copy = pnew pgsGenInt(*gen);
		TS_ASSERT(copy->value() = gen->value());
		TS_ASSERT(copy->value() = gen->value());
		pdelete(copy);
		
		// [5] Test assignment operator
		pgsGenInt * assignment = pnew pgsGenInt(min->clone(), max->clone(),
				sequence->clone(), seed->clone());
		*gen = *assignment;
		TS_ASSERT(assignment->value() = gen->value());
		TS_ASSERT(assignment->value() = gen->value());
		pdelete(assignment);
		
		// [6] Delete the generator
		pdelete(gen);
	}
	
	// With an invalid integer generator integer(1, 2.0, false, wxDateTime::GetTimeNow())
	{
		// [1] Create the generator
		pgsExpression * min = pnew pgsNumber(wxT("1"));
		pgsExpression * max = pnew pgsNumber(wxT("2.0"), pgsReal);
		pgsExpression * sequence = pnew pgsNumber(wxT("0"));
		pgsExpression * seed = pnew pgsNumber(wxString()
				<< wxDateTime::GetTimeNow());
		pgsGenInt * gen = 0;
		gen = pnew pgsGenInt(min, max, sequence, seed);
		
		// [2] Check type properties
		
		// [3] Check that the result is invalid
		try
		{
			gen->eval(vars);
			TS_ASSERT(false);
		}
		catch (const pgsParameterException &)
		{
			// Parameters are not correct: exception expected
		}
		
		// [4] Delete the generator
		pdelete(gen);
	}
	
	////////////////////
	// REAL GENERATOR //
	////////////////////

	// With a real generator real(80, 118.123456789, 6, false, wxDateTime::GetTimeNow())
	{
		// [1] Create the generator
		pgsExpression * min = pnew pgsNumber(wxT("80"));
		pgsExpression * max = pnew pgsNumber(wxT("118.123456789"), pgsReal);
		pgsExpression * precision = pnew pgsNumber(wxT("6"));
		pgsExpression * sequence = pnew pgsNumber(wxT("0"));
		pgsExpression * seed = pnew pgsNumber(wxString()
				<< wxDateTime::GetTimeNow());
		pgsGenReal * gen = 0;
		gen = pnew pgsGenReal(min, max, precision, sequence, seed);
		
		// [2] Check type properties
		
		// [3] Check result
		wxString result;
		pgsOperand rnd = gen->eval(vars);
		for (int i = 0; i < nb_iterations; i++)
		{
			result = rnd->eval(vars)->value();
			TS_ASSERT(MAPM(result.mb_str()) >= "80");
			TS_ASSERT(MAPM(result.mb_str()) <= "118.123456");
		}
		
		// [4] Test copy constructor
		pgsGenReal * copy = pnew pgsGenReal(*gen);
		TS_ASSERT(copy->value() = gen->value());
		TS_ASSERT(copy->value() = gen->value());
		pdelete(copy);
		
		// [5] Test assignment operator
		pgsGenReal * assignment = pnew pgsGenReal(min->clone(), max->clone(),
				precision->clone(), sequence->clone(), seed->clone());
		*gen = *assignment;
		TS_ASSERT(assignment->value() = gen->value());
		TS_ASSERT(assignment->value() = gen->value());
		pdelete(assignment);
		
		// [6] Delete the generator
		pdelete(gen);
	}

	// With a real generator (invalid precision) real(80, 90, 6.3, false, wxDateTime::GetTimeNow())
	{
		// [1] Create the generator
		pgsExpression * min = pnew pgsNumber(wxT("80"));
		pgsExpression * max = pnew pgsNumber(wxT("90"));
		pgsExpression * precision = pnew pgsNumber(wxT("6.3"), pgsReal);
		pgsExpression * sequence = pnew pgsNumber(wxT("0"));
		pgsExpression * seed = pnew pgsNumber(wxString()
				<< wxDateTime::GetTimeNow());
		pgsGenReal * gen = 0;
		gen = pnew pgsGenReal(min, max, precision, sequence, seed);
		
		// [2] Check type properties
		
		// [3] Check that the result is invalid
		try
		{
			gen->eval(vars);
			TS_ASSERT(false);
		}
		catch (const pgsParameterException &)
		{
			// Parameters are not correct: exception expected
		}
		
		// [4] Delete the generator
		pdelete(gen);
	}

	// With a real generator (invalid sequence identifier) real(80, 90, 5, 5.0, wxDateTime::GetTimeNow())
	{
		// [1] Create the generator
		pgsExpression * min = pnew pgsNumber(wxT("80"));
		pgsExpression * max = pnew pgsNumber(wxT("90"));
		pgsExpression * precision = pnew pgsNumber(wxT("5"));
		pgsExpression * sequence = pnew pgsNumber(wxT("5.0"), pgsReal);
		pgsExpression * seed = pnew pgsNumber(wxString()
				<< wxDateTime::GetTimeNow());
		pgsGenReal * gen = 0;
		gen = pnew pgsGenReal(min, max, precision, sequence, seed);
		
		// [2] Check type properties
		
		// [3] Check that the result is invalid
		try
		{
			gen->eval(vars);
			TS_ASSERT(false);
		}
		catch (const pgsParameterException &)
		{
			// Parameters are not correct: exception expected
		}
		
		// [4] Delete the generator
		pdelete(gen);
	}
	
	//////////////////////
	// STRING GENERATOR //
	//////////////////////

	// With a string generator string(5, 40, 5, 123456789L)
	{
		// [1] Create the generator
		pgsExpression * min = pnew pgsNumber(wxT("5"));
		pgsExpression * max = pnew pgsNumber(wxT("40"));
		pgsExpression * nb_words = pnew pgsNumber(wxT("5"));
		pgsExpression * seed = pnew pgsNumber(wxT("123456789"));
		pgsGenString * gen = pnew pgsGenString(min, max, nb_words, seed);
		
		// [2] Check type properties
		
		// [3] Check result
		pgsStringGen comparator(5, 40, 5, 123456789L);
		wxString result;
		pgsOperand rnd = gen->eval(vars);
		for (int i = 0; i < nb_iterations; i++)
		{
			result = rnd->eval(vars)->value();
			TS_ASSERT(result == comparator.random());
		}
		
		// [4] Test copy constructor
		pgsGenString * copy = pnew pgsGenString(*gen);
		TS_ASSERT(copy->value() = gen->value());
		TS_ASSERT(copy->value() = gen->value());
		pdelete(copy);
		
		// [5] Test assignment operator
		pgsGenString * assignment = pnew pgsGenString(min->clone(),
				max->clone(), nb_words->clone(), seed->clone());
		*gen = *assignment;
		TS_ASSERT(assignment->value() = gen->value());
		TS_ASSERT(assignment->value() = gen->value());
		pdelete(assignment);
		
		// [6] Delete the generator
		pdelete(gen);
	}

	// With a string generator (invalid number of words) string(5, 40, 5.5, 123456789L)
	{
		// [1] Create the generator
		pgsExpression * min = pnew pgsNumber(wxT("5"));
		pgsExpression * max = pnew pgsNumber(wxT("40"));
		pgsExpression * nb_words = pnew pgsNumber(wxT("5.5"),
				pgsReal);
		pgsExpression * seed = pnew pgsNumber(wxT("123456789"));
		pgsGenString * gen = 0;
		gen = pnew pgsGenString(min, max, nb_words, seed);
		
		// [2] Check type properties
		
		// [2] Check that the result is invalid
		try
		{
			gen->eval(vars);
			TS_ASSERT(false);
		}
		catch (const pgsParameterException &)
		{
			// Parameters are not correct: exception expected
		}
		
		// [4] Delete the generator
		pdelete(gen);
	}
	
	/////////////////////////
	// DATE_TIME GENERATOR //
	/////////////////////////
	
	// With a date_time generator date_time(2008-01-01 00:00:00,
	//                                      2008-01-01 05:00:00, false, wxDateTime::GetTimeNow())
	{
		// [1] Create the generator
		pgsExpression * min = pnew pgsString(wxT("2008-01-01 00:00:00"));
		pgsExpression * max = pnew pgsString(wxT("2008-01-01 05:00:00"));
		pgsExpression * sequence = pnew pgsNumber(wxT("0"));
		pgsExpression * seed = pnew pgsNumber(wxString()
				<< wxDateTime::GetTimeNow());
		pgsGenDateTime * gen = 0;
		gen = pnew pgsGenDateTime(min, max, sequence, seed);
		
		// [2] Check type properties
		
		// [3] Check result
		wxString result;
		pgsOperand rnd = gen->eval(vars);
		for (int i = 0; i < nb_iterations; i++)
		{
			result = rnd->eval(vars)->value();
			TS_ASSERT(result.StartsWith(wxT("2008-01-01 0")));
		}
		
		// [4] Test copy constructor
		pgsGenDateTime * copy = pnew pgsGenDateTime(*gen);
		TS_ASSERT(copy->value() = gen->value());
		TS_ASSERT(copy->value() = gen->value());
		pdelete(copy);
		
		// [5] Test assignment operator
		pgsGenDateTime * assignment = pnew pgsGenDateTime(min->clone(),
				max->clone(), sequence->clone(), seed->clone());
		*gen = *assignment;
		TS_ASSERT(assignment->value() = gen->value());
		TS_ASSERT(assignment->value() = gen->value());
		pdelete(assignment);
		
		// [6] Delete the generator
		pdelete(gen);
	}
	
	// With an invalid date_time generator date_time(20080101,
	//                                      20080101, false, wxDateTime::GetTimeNow())
	// DateTime parameters are numbers
	{
		// [1] Create the generator
		pgsExpression * min = pnew pgsNumber(wxT("20080101"));
		pgsExpression * max = pnew pgsString(wxT("20080101"));
		pgsExpression * sequence = pnew pgsNumber(wxT("0"));
		pgsExpression * seed = pnew pgsNumber(wxString()
				<< wxDateTime::GetTimeNow());
		pgsGenDateTime * gen = 0;
		gen = pnew pgsGenDateTime(min, max, sequence, seed);
		
		// [2] Check type properties
		
		// [3] Check that the result is invalid
		try
		{
			gen->eval(vars);
			TS_ASSERT(false);
		}
		catch (const pgsParameterException &)
		{
			// Parameters are not correct: exception expected
		}
		
		// [4] Delete the generator
		pdelete(gen);
	}
	
	// With an invalid date_time generator date_time(20080101,
	//                                      20080101, false, wxDateTime::GetTimeNow())
	// DateTime parameters are numbers
	{
		// [1] Create the generator
		pgsExpression * min = pnew pgsString(wxT("20080101"));
		pgsExpression * max = pnew pgsNumber(wxT("20080101"));
		pgsExpression * sequence = pnew pgsNumber(wxT("0"));
		pgsExpression * seed = pnew pgsNumber(wxString()
				<< wxDateTime::GetTimeNow());
		pgsGenDateTime * gen = 0;
		gen = pnew pgsGenDateTime(min, max, sequence, seed);
		
		// [2] Check type properties
		
		// [3] Check that the result is invalid
		try
		{
			gen->eval(vars);
			TS_ASSERT(false);
		}
		catch (const pgsParameterException &)
		{
			// Parameters are not correct: exception expected
		}
		
		// [4] Delete the generator
		pdelete(gen);
	}
	
	// With an invalid date_time generator date_time(2008-01-01 00:00:00,
	//                                      2008-2008-2008 05:00:00, false, wxDateTime::GetTimeNow())
	{
		// [1] Create the generator
		pgsExpression * min = pnew pgsString(wxT("2008-01-01 00:00:00"));
		pgsExpression * max = pnew pgsString(wxT("2008-2008-2008 05:00:00"));
		pgsExpression * sequence = pnew pgsNumber(wxT("0"));
		pgsExpression * seed = pnew pgsNumber(wxString()
				<< wxDateTime::GetTimeNow());
		pgsGenDateTime * gen = 0;
		gen = pnew pgsGenDateTime(min, max, sequence, seed);
		
		// [2] Check type properties
		
		// [3] Check that the result is invalid
		try
		{
			gen->eval(vars);
			TS_ASSERT(false);
		}
		catch (const pgsParameterException &)
		{
			// Parameters are not correct: exception expected
		}
		
		// [4] Delete the generator
		pdelete(gen);
	}
	
	////////////////////
	// DATE GENERATOR //
	////////////////////
	
	// With a date generator date(2008-01-01, 2008-01-02, false, wxDateTime::GetTimeNow())
	{
		// [1] Create the generator
		pgsExpression * min = pnew pgsString(wxT("2008-01-01"));
		pgsExpression * max = pnew pgsString(wxT("2008-01-02"));
		pgsExpression * sequence = pnew pgsNumber(wxT("0"));
		pgsExpression * seed = pnew pgsNumber(wxString()
				<< wxDateTime::GetTimeNow());
		pgsGenDate * gen = 0;
		gen = pnew pgsGenDate(min, max, sequence, seed);
		
		// [2] Check type properties
		
		// [3] Check result
		wxString result;
		pgsOperand rnd = gen->eval(vars);
		for (int i = 0; i < nb_iterations; i++)
		{
			result = rnd->eval(vars)->value();
			TS_ASSERT(result == wxT("2008-01-01") || result == wxT("2008-01-02"));
		}
		
		// [4] Test copy constructor
		pgsGenDate * copy = pnew pgsGenDate(*gen);
		TS_ASSERT(copy->value() = gen->value());
		TS_ASSERT(copy->value() = gen->value());
		pdelete(copy);
		
		// [5] Test assignment operator
		pgsGenDate * assignment = pnew pgsGenDate(min->clone(), max->clone(),
				sequence->clone(), seed->clone());
		*gen = *assignment;
		TS_ASSERT(assignment->value() = gen->value());
		TS_ASSERT(assignment->value() = gen->value());
		pdelete(assignment);
		
		// [6] Delete the generator
		pdelete(gen);
	}
	
	// With an invalid date generator date(20080101, 20080101, false, wxDateTime::GetTimeNow())
	// Date parameters are numbers
	{
		// [1] Create the generator
		pgsExpression * min = pnew pgsNumber(wxT("20080101"));
		pgsExpression * max = pnew pgsString(wxT("20080101"));
		pgsExpression * sequence = pnew pgsNumber(wxT("0"));
		pgsExpression * seed = pnew pgsNumber(wxString()
				<< wxDateTime::GetTimeNow());
		pgsGenDate * gen = 0;
		gen = pnew pgsGenDate(min, max, sequence, seed);
		
		// [2] Check type properties
		
		// [3] Check that the result is invalid
		try
		{
			gen->eval(vars);
			TS_ASSERT(false);
		}
		catch (const pgsParameterException &)
		{
			// Parameters are not correct: exception expected
		}
		
		// [4] Delete the generator
		pdelete(gen);
	}
	
	// With an invalid date generator date(20080101, 20080101, false, wxDateTime::GetTimeNow())
	// Date parameters are numbers
	{
		// [1] Create the generator
		pgsExpression * min = pnew pgsString(wxT("20080101"));
		pgsExpression * max = pnew pgsNumber(wxT("20080101"));
		pgsExpression * sequence = pnew pgsNumber(wxT("0"));
		pgsExpression * seed = pnew pgsNumber(wxString()
				<< wxDateTime::GetTimeNow());
		pgsGenDate * gen = 0;
		gen = pnew pgsGenDate(min, max, sequence, seed);
		
		// [2] Check type properties
		
		// [3] Check that the result is invalid
		try
		{
			gen->eval(vars);
			TS_ASSERT(false);
		}
		catch (const pgsParameterException &)
		{
			// Parameters are not correct: exception expected
		}
		
		// [4] Delete the generator
		pdelete(gen);
	}
	
	// With an invalid date generator date(2008-01-01, 2008-2008-2008, false, wxDateTime::GetTimeNow())
	{
		// [1] Create the generator
		pgsExpression * min = pnew pgsString(wxT("2008-01-01"));
		pgsExpression * max = pnew pgsString(wxT("2008-2008-2008"));
		pgsExpression * sequence = pnew pgsNumber(wxT("0"));
		pgsExpression * seed = pnew pgsNumber(wxString()
				<< wxDateTime::GetTimeNow());
		pgsGenDate * gen = 0;
		gen = pnew pgsGenDate(min, max, sequence, seed);
		
		// [2] Check type properties
		
		// [3] Check that the result is invalid
		try
		{
			gen->eval(vars);
			TS_ASSERT(false);
		}
		catch (const pgsParameterException &)
		{
			// Parameters are not correct: exception expected
		}
		
		// [4] Delete the generator
		pdelete(gen);
	}
	
	////////////////////
	// TIME GENERATOR //
	////////////////////
	
	// With a time generator time(05:30:00, 05:30:10, false, wxDateTime::GetTimeNow())
	{
		// [1] Create the generator
		pgsExpression * min = pnew pgsString(wxT("05:30:00"));
		pgsExpression * max = pnew pgsString(wxT("05:30:10"));
		pgsExpression * sequence = pnew pgsNumber(wxT("0"));
		pgsExpression * seed = pnew pgsNumber(wxString()
				<< wxDateTime::GetTimeNow());
		pgsGenTime * gen = 0;
		gen = pnew pgsGenTime(min, max, sequence, seed);
		
		// [2] Check type properties
		
		// [3] Check result
		wxString result;
		pgsOperand rnd = gen->eval(vars);
		for (int i = 0; i < nb_iterations; i++)
		{
			result = rnd->eval(vars)->value();
			TS_ASSERT(result.StartsWith(wxT("05:30:")));
		}
		
		// [4] Test copy constructor
		pgsGenTime * copy = pnew pgsGenTime(*gen);
		TS_ASSERT(copy->value() = gen->value());
		TS_ASSERT(copy->value() = gen->value());
		pdelete(copy);
		
		// [5] Test assignment operator
		pgsGenTime * assignment = pnew pgsGenTime(min->clone(), max->clone(),
				sequence->clone(), seed->clone());
		*gen = *assignment;
		TS_ASSERT(assignment->value() = gen->value());
		TS_ASSERT(assignment->value() = gen->value());
		pdelete(assignment);
		
		// [6] Delete the generator
		pdelete(gen);
	}
	
	// With an invalid time generator time(120000, 120000, false, wxDateTime::GetTimeNow())
	// DateTime parameters are numbers
	{
		// [1] Create the generator
		pgsExpression * min = pnew pgsNumber(wxT("120000"));
		pgsExpression * max = pnew pgsString(wxT("120000"));
		pgsExpression * sequence = pnew pgsNumber(wxT("0"));
		pgsExpression * seed = pnew pgsNumber(wxString()
				<< wxDateTime::GetTimeNow());
		pgsGenTime * gen = 0;
		gen = pnew pgsGenTime(min, max, sequence, seed);
		
		// [2] Check type properties
		
		// [3] Check that the result is invalid
		try
		{
			gen->eval(vars);
			TS_ASSERT(false);
		}
		catch (const pgsParameterException &)
		{
			// Parameters are not correct: exception expected
		}
		
		// [4] Delete the generator
		pdelete(gen);
	}
	
	// With an invalid time generator time(120000, 120000, false, wxDateTime::GetTimeNow())
	// DateTime parameters are numbers
	{
		// [1] Create the generator
		pgsExpression * min = pnew pgsString(wxT("120000"));
		pgsExpression * max = pnew pgsNumber(wxT("120000"));
		pgsExpression * sequence = pnew pgsNumber(wxT("0"));
		pgsExpression * seed = pnew pgsNumber(wxString()
				<< wxDateTime::GetTimeNow());
		pgsGenTime * gen = 0;
		gen = pnew pgsGenTime(min, max, sequence, seed);
		
		// [2] Check type properties
		
		// [3] Check that the result is invalid
		try
		{
			gen->eval(vars);
			TS_ASSERT(false);
		}
		catch (const pgsParameterException &)
		{
			// Parameters are not correct: exception expected
		}
		
		// [4] Delete the generator
		pdelete(gen);
	}
	
	// With an invalid time generator time(80:80:80, 85:85:85, false, wxDateTime::GetTimeNow())
	{
		// [1] Create the generator
		pgsExpression * min = pnew pgsString(wxT("80:80:80"));
		pgsExpression * max = pnew pgsString(wxT("85:85:85"));
		pgsExpression * sequence = pnew pgsNumber(wxT("0"));
		pgsExpression * seed = pnew pgsNumber(wxString()
				<< wxDateTime::GetTimeNow());
		pgsGenTime * gen = 0;
		gen = pnew pgsGenTime(min, max, sequence, seed);
		
		// [2] Check type properties
		
		// [3] Check that the result is invalid
		try
		{
			gen->eval(vars);
			TS_ASSERT(false);
		}
		catch (const pgsParameterException &)
		{
			// Parameters are not correct: exception expected
		}
		
		// [4] Delete the generator
		pdelete(gen);
	}
	
	/////////////////////////
	// REFERENCE GENERATOR //
	/////////////////////////
	
	// Nothing can work because there is no valid connection provided to
	// the pgsGenReference
	{
		// [1] Create the generator
		pgsExpression * table = 0;
		table = pnew pgsString(wxT("table"));
		pgsExpression * column = 0;
		column = pnew pgsString(wxT("column"));
		pgsExpression * sequence = 0;
		sequence = pnew pgsNumber(wxT("0"));
		pgsExpression * seed = 0;
		seed = pnew pgsNumber(wxString() << wxDateTime::GetTimeNow());
		pgsGenReference * gen = 0;
		gen = pnew pgsGenReference(table, column, sequence, seed);
		
		// [2] Check type properties
		
		// [3] Check result
		try
		{
			gen->eval(vars);
			TS_ASSERT(false);
		}
		catch (const pgsParameterException &)
		{
			
		}
		
		// [4] Test copy constructor
		pgsGenReference * copy = 0;
		copy = pnew pgsGenReference(*gen);
		TS_ASSERT(copy->value() = gen->value());
		TS_ASSERT(copy->value() = gen->value());
		try
		{
			copy->eval(vars);
			TS_ASSERT(false);
		}
		catch (const pgsParameterException &)
		{
			
		}
		pdelete(copy);
		
		// [5] Test assignment operator
		pgsGenReference * assignment = 0;
		assignment = pnew pgsGenReference(table->clone(),
				column->clone(), sequence->clone(), seed->clone());
		*gen = *assignment;
		TS_ASSERT(assignment->value() = gen->value());
		TS_ASSERT(assignment->value() = gen->value());
		try
		{
			assignment->eval(vars);
			TS_ASSERT(false);
		}
		catch (const pgsParameterException &)
		{
			
		}
		pdelete(assignment);
		
		// [6] Delete the generator
		pdelete(gen);
	}
	
	// Use an empty string for the table
	{
		// [1] Create the generator
		pgsExpression * table = pnew pgsString(wxT(""));
		pgsExpression * column = pnew pgsString(wxT("column"));
		pgsExpression * sequence = pnew pgsNumber(wxT("0"));
		pgsExpression * seed = pnew pgsNumber(wxString()
				<< wxDateTime::GetTimeNow());
		pgsGenReference * gen = 0;
		gen = pnew pgsGenReference(table, column, sequence, seed);
		
		// [2] Check type properties
		
		// [3] Check result
		try
		{
			gen->eval(vars);
			TS_ASSERT(false);
		}
		catch (const pgsParameterException &)
		{
			
		}
		
		// ...
		
		// [6] Delete the generator
		pdelete(gen);
	}
	
	// Use an empty string for the column
	{
		// [1] Create the generator
		pgsExpression * table = pnew pgsString(wxT("table"));
		pgsExpression * column = pnew pgsString(wxT(""));
		pgsExpression * sequence = pnew pgsNumber(wxT("0"));
		pgsExpression * seed = pnew pgsNumber(wxString()
				<< wxDateTime::GetTimeNow());
		pgsGenReference * gen = 0;
		gen = pnew pgsGenReference(table, column, sequence, seed);
		
		// [2] Check type properties
		
		// [3] Check result
		try
		{
			gen->eval(vars);
			TS_ASSERT(false);
		}
		catch (const pgsParameterException &)
		{
			
		}
		
		// ...
		
		// [6] Delete the generator
		pdelete(gen);
	}
	
	// Use a number for the table
	{
		// [1] Create the generator
		pgsExpression * table = pnew pgsNumber(wxT("123"));
		pgsExpression * column = pnew pgsString(wxT("column"));
		pgsExpression * sequence = pnew pgsNumber(wxT("0"));
		pgsExpression * seed = pnew pgsNumber(wxString()
				<< wxDateTime::GetTimeNow());
		pgsGenReference * gen = 0;
		gen = pnew pgsGenReference(table, column, sequence, seed);
		
		// [2] Check type properties
		
		// [3] Check result
		try
		{
			gen->eval(vars);
			TS_ASSERT(false);
		}
		catch (const pgsParameterException &)
		{
			
		}
		
		// ...
		
		// [6] Delete the generator
		pdelete(gen);
	}
	
	// Use a number for the column
	{
		// [1] Create the generator
		pgsExpression * table = pnew pgsString(wxT("table"));
		pgsExpression * column = pnew pgsNumber(wxT("123"));
		pgsExpression * sequence = pnew pgsNumber(wxT("0"));
		pgsExpression * seed = pnew pgsNumber(wxString()
				<< wxDateTime::GetTimeNow());
		pgsGenReference * gen = 0;
		gen = pnew pgsGenReference(table, column, sequence, seed);
		
		// [2] Check type properties
		
		// [3] Check result
		try
		{
			gen->eval(vars);
			TS_ASSERT(false);
		}
		catch (const pgsParameterException &)
		{
			
		}
		
		// ...
		
		// [6] Delete the generator
		pdelete(gen);
	}
	
	/////////////////////
	// REGEX GENERATOR //
	/////////////////////
	
	// With a regex generator regex(abc{2}[a-z]{1,50}, wxDateTime::GetTimeNow())
	{
		// [1] Create the generator
		pgsExpression * regex = pnew pgsString(wxT("abc{2}[a-z]{1,50}"));
		pgsExpression * seed = pnew pgsNumber(wxString()
				<< wxDateTime::GetTimeNow());
		pgsGenRegex * gen = 0;
		gen = pnew pgsGenRegex(regex, seed);
		
		// [2] Check type properties
		
		// [3] Check result
		wxString result;
		pgsOperand rnd = gen->eval(vars);
		for (int i = 0; i < nb_iterations; i++)
		{
			result = rnd->eval(vars)->value();
			TS_ASSERT(result.StartsWith(wxT("abcc")));
		}
		
		// [4] Test copy constructor
		pgsGenRegex * copy = pnew pgsGenRegex(*gen);
		TS_ASSERT(copy->value() = gen->value());
		TS_ASSERT(copy->value() = gen->value());
		pdelete(copy);
		
		// [5] Test assignment operator
		pgsGenRegex * assignment = pnew pgsGenRegex(regex->clone(),
				seed->clone());
		*gen = *assignment;
		TS_ASSERT(assignment->value() = gen->value());
		TS_ASSERT(assignment->value() = gen->value());
		pdelete(assignment);
		
		// [6] Delete the generator
		pdelete(gen);
	}
	
	// With an invalid regex generator regex(123, wxDateTime::GetTimeNow())
	// The regex is a number
	{
		// [1] Create the generator
		pgsExpression * regex = pnew pgsNumber(wxT("123"));
		pgsExpression * seed = pnew pgsNumber(wxString()
				<< wxDateTime::GetTimeNow());
		pgsGenRegex * gen = 0;
		gen = pnew pgsGenRegex(regex, seed);
		
		// [2] Check type properties
		
		// [3] Check that the result is invalid
		try
		{
			gen->eval(vars);
			TS_ASSERT(false);
		}
		catch (const pgsParameterException &)
		{
			// Parameters are not correct: exception expected
		}
		
		// [4] Delete the generator
		pdelete(gen);
	}
	
	// With an invalid regex generator regex([ab{], wxDateTime::GetTimeNow())
	// The regex is a number
	{
		// [1] Create the generator
		pgsExpression * regex = pnew pgsString(wxT("[ab{]"));
		pgsExpression * seed = pnew pgsNumber(wxString()
				<< wxDateTime::GetTimeNow());
		pgsGenRegex * gen = 0;
		gen = pnew pgsGenRegex(regex, seed);
		
		// [2] Check type properties
		
		// [3] Check that the result is invalid
		try
		{
			gen->eval(vars);
			TS_ASSERT(false);
		}
		catch (const pgsParameterException &)
		{
			// Parameters are not correct: exception expected
		}
		
		// [4] Delete the generator
		pdelete(gen);
	}
	
	//////////////////////////
	// DICTIONARY GENERATOR //
	//////////////////////////
	
	const wxString dictionary = wxT("cities.txt"); // More than 2 lines
	wxCSConv conv(wxT("utf-8")); // Encoding used for creating the dictionary
	
	// Content of the dictionary
	wxString example =	wxT("DixieWorld\n")
						wxT("SnoRite\n")
						wxT("SunGlo\n")
						wxT("Ozark Park\n")
						wxT("Like\n")
						wxT("Healthy Heights\n")
						wxT("Insure\n")
						wxT("First National State\n")
						wxT("AmeriBeach\n")
						wxT("Peaches N' More\n")
						wxT("Isle B Seaing U\n")
						wxT("Perpetua\n")
						wxT("Chi+\n")
						wxT("Town and Country\n")
						wxT("Sunny Farms\n")
						wxT("Midstate\n")
						wxT("Equus\n")
						wxT("Hot N' Humid\n")
						wxT("@lantia\n")
						wxT("Heritage Home\n")
						wxT("Bay Way\n")
						wxT("Aqua-rama\n")
						wxT("Lakes-a-plenty\n")
						wxT("Magnolia\n")
						wxT("Riverview\n")
						wxT("Skyview\n")
						wxT("Korn-O-copia\n")
						wxT("WinALot\n")
						wxT("Hard Rock State\n")
						wxT("Consolidated Peoples\n")
						wxT("I Can't Believe It's Not Mexico\n")
						wxT("DollarDaze\n")
						wxT("Phatlantic\n")
						wxT("Northland\n")
						wxT("South Lake Shores\n")
						wxT("Indiana\n")
						wxT("Ocean View\n")
						wxT("Gettysburg Estates\n")
						wxT("MicroState\n")
						wxT("Charlie\n")
						wxT("South Northland\n")
						wxT("Opry-Tune-Itee\n")
						wxT("Superultramax\n")
						wxT("Salterra\n")
						wxT("Ethan Allendale\n")
						wxT("Washington\n")
						wxT("Kountry Korner\n")
						wxT("Friendly Folks\n")
						wxT("DairyKween\n")
						wxT("Fin and Fur\n");
	
	// Create the dictionary
	wxFile dict_file;
	dict_file.Create(dictionary, true);
	TS_ASSERT(dict_file.IsOpened());
	dict_file.Write(example, conv);
	dict_file.Close();
	
	// With a dictionary generator file(dictionary, false, wxDateTime::GetTimeNow(), utf-8)
	{
		// [1] Create the generator
		pgsExpression * file_path = pnew pgsString(dictionary);
		pgsExpression * sequence = pnew pgsNumber(wxT("0"));
		pgsExpression * seed = pnew pgsNumber(wxString()
				<< wxDateTime::GetTimeNow());
		pgsExpression * encoding = pnew pgsString(wxT("utf-8"));
		pgsGenDictionary * gen = 0;
		gen = pnew pgsGenDictionary(file_path, sequence, seed, encoding);
		
		// [2] Check type properties
		
		// [3] Check result
		wxString result;
		pgsOperand rnd = gen->eval(vars);
		for (int i = 0; i < nb_iterations; i++)
		{
			result = rnd->eval(vars)->value();
			TS_ASSERT(result.Length() >= 4); // The shortest city name in the dictionary
		}
		
		// [4] Test copy constructor
		pgsGenDictionary * copy = pnew pgsGenDictionary(*gen);
		TS_ASSERT(copy->value() = gen->value());
		TS_ASSERT(copy->value() = gen->value());
		pdelete(copy);
		
		// [5] Test assignment operator
		pgsGenDictionary * assignment = pnew pgsGenDictionary(file_path->clone(),
				sequence->clone(), seed->clone(), encoding->clone());
		*gen = *assignment;
		TS_ASSERT(assignment->value() = gen->value());
		TS_ASSERT(assignment->value() = gen->value());
		pdelete(assignment);
		
		// [6] Delete the generator
		pdelete(gen);
	}
	
	// With an invalid dictionary generator file(dictionary, false, wxDateTime::GetTimeNow(), utf-8)
	// Sequence is a string
	{
		// [1] Create the generator
		pgsExpression * file_path = pnew pgsString(dictionary);
		pgsExpression * sequence = pnew pgsString(wxT("0"));
		pgsExpression * seed = pnew pgsNumber(wxString()
				<< wxDateTime::GetTimeNow());
		pgsExpression * encoding = pnew pgsString(wxT("utf-8"));
		pgsGenDictionary * gen = 0;
		gen = pnew pgsGenDictionary(file_path, sequence, seed, encoding);
		
		// [2] Check type properties
		
		// [3] Check that the result is invalid
		try
		{
			gen->eval(vars);
			TS_ASSERT(false);
		}
		catch (const pgsParameterException &)
		{
			// Parameters are not correct: exception expected
		}
		
		// [4] Delete the generator
		pdelete(gen);
	}
	
	// With an invalid dictionary generator file(dictionary, false, wxDateTime::GetTimeNow(), utf-8)
	// Seed is a string
	{
		// [1] Create the generator
		pgsExpression * file_path = pnew pgsString(dictionary);
		pgsExpression * sequence = pnew pgsNumber(wxT("0"));
		pgsExpression * seed = pnew pgsString(wxString()
				<< wxDateTime::GetTimeNow());
		pgsExpression * encoding = pnew pgsString(wxT("utf-8"));
		pgsGenDictionary * gen = 0;
		gen = pnew pgsGenDictionary(file_path, sequence, seed, encoding);
		
		// [2] Check type properties
		
		// [3] Check that the result is invalid
		try
		{
			gen->eval(vars);
			TS_ASSERT(false);
		}
		catch (const pgsParameterException &)
		{
			// Parameters are not correct: exception expected
		}
		
		// [4] Delete the generator
		pdelete(gen);
	}
	
	// With an invalid dictionary generator file(123, false, wxDateTime::GetTimeNow(), utf-8)
	// Seed is a string
	{
		// [1] Create the generator
		pgsExpression * file_path = pnew pgsNumber(wxT("123"));
		pgsExpression * sequence = pnew pgsNumber(wxT("0"));
		pgsExpression * seed = pnew pgsNumber(wxString()
				<< wxDateTime::GetTimeNow());
		pgsExpression * encoding = pnew pgsString(wxT("utf-8"));
		pgsGenDictionary * gen = 0;
		gen = pnew pgsGenDictionary(file_path, sequence, seed, encoding);
		
		// [2] Check type properties
		
		// [3] Check that the result is invalid
		try
		{
			gen->eval(vars);
			TS_ASSERT(false);
		}
		catch (const pgsParameterException &)
		{
			// Parameters are not correct: exception expected
		}
		
		// [4] Delete the generator
		pdelete(gen);
	}
	
	// Remove the dictionary
	::wxRemoveFile(dictionary);
}
