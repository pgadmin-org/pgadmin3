//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2009, The pgAdmin Development Team
// This software is released under the BSD Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgsTestSuite.h"

#include "pgscript/generators/pgsIntegerGen.h"

void pgsTestSuite::test_generator_int(void)
{
	// Iterate a certain number of times to be sure the generator
	// is correct in any circumstances
	const int nb_iterations = 500;

	// Generate a number between 1 and 2
	// Test the expected average: 1.5 +/- 5%
	{
		pgsIntegerGen gen(1, 2, false);
		TS_ASSERT(!gen.is_sequence());
		long result;
		MAPM sum = 0, avg;
		for (int i = 0; i < nb_iterations; i++)
		{
			result = gen.random_long();
			TS_ASSERT(result == 1 || result == 2);
			sum += result;
		}
		avg = sum / nb_iterations;
		TS_ASSERT(avg > 1.45 && avg < 1.55);
	}

	// Generate negative and positive *unique* numbers
	// Expected average is of course 0
	{
		pgsIntegerGen gen("-2000", "2000", true);
		TS_ASSERT(gen.is_sequence());
		long result;
		MAPM sum = 0;
		pgsVectorMapm sav;
		// Iterate only 4001 in order to generate each value once
		for (int i = 0; i < 4001; i++)
		{
			result = gen.random_long();
			TS_ASSERT(result >= -2000 && result <= 2000);
			sum += result;
			// Check that the result does not already exist
			TS_ASSERT(sav.Index(result) == wxNOT_FOUND);
			sav.push_back(result);
			TS_ASSERT(sav.GetCount() == wx_static_cast(size_t, i + 1));
		}
		TS_ASSERT(sav.size() == 4001 && sum == 0);
	}

	// Test that two generators with same seed generate same values
	// Generate a number between 0 and 1000
	// Test the expected average: 5000 +/- 10%
	{
		pgsIntegerGen gen(0, 10000, false, 123456789L);
		pgsIntegerGen comparator(0, 10000, false, 123456789L);
		TS_ASSERT(!gen.is_sequence() && !comparator.is_sequence());
		long result, res_cmp;
		MAPM sum = 0, avg;
		for (int i = 0; i < nb_iterations; i++)
		{
			result = gen.random_long();
			res_cmp = comparator.random_long();
			TS_ASSERT(result >= 0 && result <= 10000);
			TS_ASSERT(result == res_cmp);
			sum += result;
		}
		avg = sum / nb_iterations;
		TS_ASSERT(avg > 4500 && avg < 5500);
	}

	// Same thing as previous with min > max and seed == 0
	// min and max must be swapped
	// seed must be set to one otherwise we would have zeros only
	{
		pgsIntegerGen gen(10000, 0, true, 0);
		pgsIntegerGen comparator(10000, 0, true, 0);
		TS_ASSERT(gen.is_sequence() && comparator.is_sequence());
		long result, res_cmp;
		MAPM sum = 0, avg;
		for (int i = 0; i < nb_iterations; i++)
		{
			result = gen.random_long();
			res_cmp = comparator.random_long();
			TS_ASSERT(result >= 0 && result <= 10000);
			TS_ASSERT(result == res_cmp);
			sum += result;
		}
		avg = sum / nb_iterations;
		TS_ASSERT(avg > 4500 && avg < 5500);
	}

	// Generate big numbers and tests the string output
	// Expected average is included in the bounds
	{
		pgsIntegerGen gen("12345678987654321", "123456789876543210", false);
		TS_ASSERT(!gen.is_sequence());
		MAPM sum = 0, avg, result;
		for (int i = 0; i < nb_iterations; i++)
		{
			result = MAPM(gen.random().mb_str());
			TS_ASSERT(result >= "12345678987654321");
			TS_ASSERT(result <= "123456789876543210");
			sum += result;
		}
		avg = sum / nb_iterations;
		TS_ASSERT(avg > "12345678987654321" && avg < "123456789876543210");
	}

	// Test copy constructor
	// Generate a number between -80000 and -5
	{
		// Create a generator and generate values
		pgsIntegerGen gen(-80000, -5, false);
		long result, res_cmp;
		for (int i = 0; i < nb_iterations / 2; i++)
		{
			result = gen.random_long();
			TS_ASSERT(result >= -80000 && result <= -5);
		}
		
		// Copy this generator to a new one
		// Both generators must generate the same values
		pgsIntegerGen comparator(gen);
		TS_ASSERT(!gen.is_sequence() && !comparator.is_sequence());
		for (int i = nb_iterations / 2; i < nb_iterations; i++)
		{
			result = gen.random_long();
			res_cmp = comparator.random_long();
			TS_ASSERT(result >= -80000 && result <= -5);
			TS_ASSERT(result == res_cmp);
		}
	}

	// Test assignment operator
	// Generate a number between -80000 and -5
	{
		// Create two different generators and generate values
		pgsIntegerGen gen(-80000, -5, false);
		pgsIntegerGen comparator(1, 2, true);
		long result, res_cmp;
		for (int i = 0; i < nb_iterations / 2; i++)
		{
			result = gen.random_long();
			TS_ASSERT(result >= -80000 && result <= -5);
			res_cmp = comparator.random_long();
			TS_ASSERT(res_cmp >= 1 && res_cmp <= 2);
		}
		
		// Copy one of the generators to the other one
		// Both generators must generate the same values
		comparator = gen;
		TS_ASSERT(!gen.is_sequence() && !comparator.is_sequence());
		for (int i = nb_iterations / 2; i < nb_iterations; i++)
		{
			result = gen.random_long();
			res_cmp = comparator.random_long();
			TS_ASSERT(result >= -80000 && result <= -5);
			TS_ASSERT(result == res_cmp);
		}
	}
}
