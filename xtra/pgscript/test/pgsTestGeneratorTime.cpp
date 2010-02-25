//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgsTestSuite.h"

#include "pgscript/generators/pgsTimeGen.h"

void pgsTestSuite::test_generator_time(void)
{
	const int nb_iterations = 500;

	// Generate times between 01:00:00 and 01:00:01
	{
		wxDateTime min, max;
		min.ParseTime(wxT("01:00:00"));
		max.ParseTime(wxT("01:00:01"));
		pgsTimeGen gen(min, max, false);
		TS_ASSERT(!gen.is_sequence());
		wxString result;
		for (int i = 0; i < nb_iterations; i++)
		{
			result = gen.random(); // Output is ISO formatted
			TS_ASSERT(result == wxT("01:00:00") || result == wxT("01:00:01"));
		}
	}

	// Generate *unique* times
	{
		wxDateTime min, max;
		min.ParseTime(wxT("05:00:00"));
		max.ParseTime(wxT("05:30:00"));
		pgsTimeGen gen(min, max, true);
		TS_ASSERT(gen.is_sequence());
		wxString result;
		wxArrayString sav;
		for (int i = 0; i < 30 * 60 + 1; i++)
		{
			result = gen.random(); // Output is ISO formatted
			TS_ASSERT(sav.Index(result) == wxNOT_FOUND);
			sav.push_back(result);
		}
		TS_ASSERT(sav.size() == 30 * 60 + 1);
		TS_ASSERT(gen.random() == sav.Item(0));
	}
	
	// Generate times but min > max
	{
		wxDateTime min, max;
		min.ParseTime(wxT("12:00:00"));
		max.ParseTime(wxT("11:00:00"));
		pgsTimeGen gen(min, max, false);
		TS_ASSERT(!gen.is_sequence());
		for (int i = 0; i < nb_iterations; i++)
		{
			gen.random();
		}
	}

	// Test that two generators with same seed generate same values
	{
		wxDateTime min, max;
		min.ParseTime(wxT("12:00:00"));
		max.ParseTime(wxT("14:00:00"));
		pgsTimeGen gen(min, max, false, 123456789L);
		pgsTimeGen comparator(min, max, false, 123456789L);
		TS_ASSERT(!gen.is_sequence() && !comparator.is_sequence());
		for (int i = 0; i < nb_iterations; i++)
		{
			TS_ASSERT(gen.random() == comparator.random());
		}
	}

	// Same thing as previous with min > max and seed == 0
	// min and max must be swapped
	// seed must be set to one otherwise we would have zeros only
	{
		wxDateTime min, max;
		max.ParseTime(wxT("14:00:00"));
		min.ParseTime(wxT("12:00:00"));
		pgsTimeGen gen(min, max, true, 0);
		pgsTimeGen comparator(min, max, true, 0);
		TS_ASSERT(gen.is_sequence() && comparator.is_sequence());
		wxString result;
		wxArrayString sav;
		for (int i = 0; i < 120 * 60 + 1; i++)
		{
			result = gen.random();
			TS_ASSERT(result == comparator.random());
			TS_ASSERT(sav.Index(result) == wxNOT_FOUND);
			sav.push_back(result);
		}
	}

	// Test copy constructor
	{
		// Create a generator and generate values
		wxDateTime min, max;
		min.ParseTime(wxT("12:00:00"));
		max.ParseTime(wxT("15:00:00"));
		pgsTimeGen gen(min, max, false);
		TS_ASSERT(!gen.is_sequence());
		wxString result, res_cmp;
		for (int i = 0; i < nb_iterations / 2; i++)
		{
			result = gen.random();
		}
		
		// Copy this generator to a new one
		// Both generators must generate the same values
		pgsTimeGen comparator(gen);
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
		wxDateTime min1, max1;
		min1.ParseTime(wxT("12:00:00"));
		max1.ParseTime(wxT("15:00:00"));
		pgsTimeGen gen(min1, max1, false);
		wxDateTime min2, max2;
		min2.ParseTime(wxT("02:00:00"));
		max2.ParseTime(wxT("22:00:00"));
		pgsTimeGen comparator(min2, max2, true);
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
