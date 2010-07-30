//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// 
// Copyright (C) 2002 - 2009, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgsTestSuite.h"

#include "pgscript/generators/pgsRealGen.h"

void pgsTestSuite::test_generator_real(void)
{
	const int nb_iterations = 500;

	// Generate *unique* numbers between 1 and 2 with 3 digits
	// Test the expected average: 1.5
	{
		pgsRealGen gen(1, 2, 3, true);
		TS_ASSERT(gen.is_sequence());
		MAPM sum = 0, avg, result;
		pgsVectorMapm sav;
		for (int i = 0; i < 1001; i++)
		{
			result = MAPM(gen.random().mb_str());
			TS_ASSERT(result >= 1 && result <= 2);
			TS_ASSERT(sav.Index(result) == wxNOT_FOUND);
			sum += result;
			sav.push_back(result);
		}
		avg = sum / (1001);
		TS_ASSERT(avg == MAPM("1500e-3"));
	}

	// Generate negative and positive *unique* integer numbers (precision == 0)
	// Expected average is of course 0
	{
		pgsRealGen gen("-50", "50", 0, true);
		TS_ASSERT(gen.is_sequence());
		MAPM sum = 0, result;
		pgsVectorMapm sav;
		for (int i = 0; i < 101; i++)
		{
			result = MAPM(gen.random().mb_str());
			TS_ASSERT(result >= -50 && result <= 50);
			sum += result;
			TS_ASSERT(sav.Index(result) == wxNOT_FOUND);
			sav.push_back(result);
		}
		TS_ASSERT(sav.size() == 101 && sum == 0);
	}

	// Test that two generators with same seed generate same values
	{
		pgsRealGen gen(0, 10000.567890, 6, false, 123456789L);
		pgsRealGen comparator(0, 10000.567890, 6, false, 123456789L);
		TS_ASSERT(!gen.is_sequence() && !comparator.is_sequence());
		wxString res_cmp;
		MAPM sum = 0, avg, result;
		for (int i = 0; i < nb_iterations; i++)
		{
			res_cmp = comparator.random();
			result = MAPM(gen.random().mb_str());
			TS_ASSERT(result >= 0 && result <= "1.0000567890e4");
			TS_ASSERT(result == MAPM(res_cmp.mb_str()));
			sum += result;
		}
		avg = sum / nb_iterations;
		TS_ASSERT(avg > 4500 && avg < 5500);
	}

	// Same thing as previous with min > max and seed == 0
	// min and max must be swapped
	// seed must be set to one otherwise we would have zeros only
	{
		pgsRealGen gen(10000.567890, 0, 6, true, 0);
		pgsRealGen comparator(10000.567890, 0, 6, true, 0);
		TS_ASSERT(gen.is_sequence() && comparator.is_sequence());
		wxString res_cmp;
		MAPM sum = 0, avg, result;
		for (int i = 0; i < nb_iterations; i++)
		{
			res_cmp = comparator.random();
			result = MAPM(gen.random().mb_str());
			TS_ASSERT(result >= 0 && result <= "1.0000567890e4");
			TS_ASSERT(result == MAPM(res_cmp.mb_str()));
			sum += result;
		}
		avg = sum / nb_iterations;
		TS_ASSERT(avg > 4500 && avg < 5500);
	}

	// Generate big numbers with a too low precision and test the string output
	// Last decimals should not be taken into account
	{
		pgsRealGen gen("123456789876.54321", "123456789876.5432134", 5, false);
		TS_ASSERT(!gen.is_sequence());
		MAPM result;
		for (int i = 0; i < nb_iterations; i++)
		{
			result = MAPM(gen.random().mb_str());
			TS_ASSERT(result == "123456789876.54321");
		}
	}

	// Generate numbers with exponents
	// Test the expected average: 1.5
	{
		pgsRealGen gen("0", "1e100", 0, false);
		TS_ASSERT(!gen.is_sequence());
		MAPM sum = 0, avg, result;
		for (int i = 0; i < nb_iterations; i++)
		{
			result = MAPM(gen.random().mb_str());
			TS_ASSERT(result >= "0" && result <= "1e100");
			sum += result;
		}
		avg = sum / nb_iterations;
		TS_ASSERT(avg >= "4.5e99" && avg <= "5.5e99");
	}

	// Test copy constructor
	{
		// Create a generator and generate values
		pgsRealGen gen(0, 10000.567890, 6, false);
		wxString result, res_cmp;
		for (int i = 0; i < nb_iterations / 2; i++)
		{
			result = gen.random();
		}
		
		// Copy this generator to a new one
		// Both generators must generate the same values
		pgsRealGen comparator(gen);
		TS_ASSERT(!gen.is_sequence() && !comparator.is_sequence());
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
		pgsRealGen gen(0, 10000.567890, 6, false);
		pgsRealGen comparator(1, 2, 3, true);
		wxString result, res_cmp;
		for (int i = 0; i < nb_iterations / 2; i++)
		{
			result = gen.random();
			res_cmp = comparator.random();
		}
		
		// Copy one of the generators to the other one
		// Both generators must generate the same values
		comparator = gen;
		TS_ASSERT(!gen.is_sequence() && !comparator.is_sequence());
		for (int i = nb_iterations / 2; i < nb_iterations; i++)
		{
			result = gen.random();
			res_cmp = comparator.random();
			TS_ASSERT(result == res_cmp);
		}
	}
}
