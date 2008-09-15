//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgsTestSuite.h"

#include "pgscript/generators/pgsDateGen.h"

void pgsTestSuite::test_generator_date(void)
{
	const int nb_iterations = 500;

	// Generate dates between May 05, 2008 and May 06, 2008
	{
		wxDateTime min, max;
		min.ParseDate(wxT("May 05, 2008"));
		max.ParseDate(wxT("May 06, 2008"));
		pgsDateGen gen(min, max, false);
		TS_ASSERT(!gen.is_sequence());
		wxString result;
		for (int i = 0; i < nb_iterations; i++)
		{
			result = gen.random(); // Output is ISO formatted
			TS_ASSERT(result == wxT("2008-05-05")
					|| result == wxT("2008-05-06"));
		}
	}

	// Generate *unique* dates
	{
		wxDateTime min, max;
		min.ParseDate(wxT("May 05, 2008"));
		max.ParseDate(wxT("May 05, 2011"));
		pgsDateGen gen(min, max, true);
		TS_ASSERT(gen.is_sequence());
		wxString result;
		wxArrayString sav;
		for (int i = 0; i < 3 * 365 + 1; i++)
		{
			result = gen.random(); // Output is ISO formatted
			TS_ASSERT(sav.Index(result) == wxNOT_FOUND);
			sav.push_back(result);
		}
		TS_ASSERT(sav.size() == 3 * 365 + 1);
		TS_ASSERT(gen.random() == sav.Item(0));
	}
	
	// Generate dates but min > max
	{
		wxDateTime min, max;
		min.ParseDate(wxT("Jan 08, 5"));
		max.ParseDate(wxT("Jan 01, 1"));
		pgsDateGen gen(min, max, false);
		TS_ASSERT(!gen.is_sequence());
		for (int i = 0; i < nb_iterations; i++)
		{
			gen.random();
		}
	}

	// Test that two generators with same seed generate same values
	{
		wxDateTime min, max;
		min.ParseDate(wxT("2008-01-01"));
		max.ParseDate(wxT("2008-01-02"));
		pgsDateGen gen(min, max, false, 123456789L);
		pgsDateGen comparator(min, max, false, 123456789L);
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
		max.ParseDate(wxT("2009-01-01"));
		min.ParseDate(wxT("2008-01-01"));
		pgsDateGen gen(min, max, true, 0);
		pgsDateGen comparator(min, max, true, 0);
		TS_ASSERT(gen.is_sequence() && comparator.is_sequence());
		wxString result;
		wxArrayString sav;
		for (int i = 0; i < 366; i++)
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
		min.ParseDate(wxT("2008-01-01"));
		max.ParseDate(wxT("2015-01-01"));
		pgsDateGen gen(min, max, false);
		TS_ASSERT(!gen.is_sequence());
		wxString result, res_cmp;
		for (int i = 0; i < nb_iterations / 2; i++)
		{
			result = gen.random();
		}
		
		// Copy this generator to a new one
		// Both generators must generate the same values
		pgsDateGen comparator(gen);
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
		min1.ParseDate(wxT("2008-01-01"));
		max1.ParseDate(wxT("2015-01-01"));
		pgsDateGen gen(min1, max1, false);
		wxDateTime min2, max2;
		min2.ParseDate(wxT("1008-01-01"));
		max2.ParseDate(wxT("1015-01-01"));
		pgsDateGen comparator(min2, max2, true);
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
