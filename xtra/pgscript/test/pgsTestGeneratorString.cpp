//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsTestGeneratorString.cpp,v 1.2 2008/08/10 17:45:36 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgsTestSuite.h"

#include "pgscript/generators/pgsStringGen.h"

void pgsTestSuite::test_generator_string(void)
{
	const int nb_iterations = 100;

	// Generate empty strings because of inconsistent arguments
	{
		pgsStringGen gen1(0, 0, 10);
		pgsStringGen gen2(10, 20, 0);
		// pgsStringGen gen3(-10, -20, 10);
		// pgsStringGen gen4(10, 20, -10);
		TS_ASSERT(gen1.random() == wxT(""));
		TS_ASSERT(gen2.random() == wxT(""));
		// TS_ASSERT(gen3.random() == wxT(""));
		// TS_ASSERT(gen4.random() == wxT(""));
	}

	// Generate strings with 'a' only
	{
		pgsVectorChar chars;
		chars.Add(wxT('a'));
		chars.Add(wxT('a'));
		pgsStringGen gen(2, 2, 3, wxDateTime::GetTimeNow(), chars);
		for (int i = 0; i < nb_iterations; i++)
		{
			TS_ASSERT(gen.random() == wxT("aa aa aa"));
		}
	}

	// Test strings that are in fact numbers
	// Test the size of generated strings
	{
		pgsVectorChar chars;
		chars.Add(wxT('1'));
		chars.Add(wxT('2'));
		chars.Add(wxT('3'));
		chars.Add(wxT('4'));
		pgsStringGen gen(5, 5, 1, wxDateTime::GetTimeNow(), chars);
		wxString result;
		for (int i = 0; i < nb_iterations; i++)
		{
			result = gen.random();
			long aux_res;
			result.ToLong(&aux_res);
			TS_ASSERT(MAPM(result.mb_str()) == aux_res && result.Length() == 5);
		}
	}

	// Test the size of generated strings
	{
		pgsStringGen gen(10, 11, 3, 123);
		pgsStringGen comparator(10, 11, 3, 123);
		wxString result;
		for (int i = 0; i < nb_iterations; i++)
		{
			result = gen.random();
			TS_ASSERT(result == comparator.random());
			TS_ASSERT(result.Length() >= 32 && result.Length() <= 35);
		}
	}

	// Test copy constructor
	{
		// Create a generator and generate values
		pgsStringGen gen(10, 11, 3, 123456789L);
		wxString result, res_cmp;
		for (int i = 0; i < nb_iterations / 2; i++)
		{
			result = gen.random();
		}
		
		// Copy this generator to a new one
		// Both generators must generate the same values
		pgsStringGen comparator(gen);
		for (int i = nb_iterations / 2; i < nb_iterations; i++)
		{
			result = gen.random();
			res_cmp = comparator.random();
			TS_ASSERT(result == res_cmp);
		}
	}

	// Test assignment operator
	{
		// Create two different generators and generate values
		pgsStringGen gen(20, 30, 20);
		pgsStringGen comparator(0, 0, 10);
		wxString result, res_cmp;
		for (int i = 0; i < nb_iterations / 2; i++)
		{
			result = gen.random();
			res_cmp = comparator.random();
		}
		
		// Copy one of the generators to the other one
		// Both generators must generate the same values
		comparator = gen;
		for (int i = nb_iterations / 2; i < nb_iterations; i++)
		{
			result = gen.random();
			res_cmp = comparator.random();
			TS_ASSERT(result == res_cmp);
		}
	}
}
