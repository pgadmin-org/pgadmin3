//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// 
// Copyright (C) 2002 - 2009, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgsTestSuite.h"

#include "pgscript/generators/pgsReferenceGen.h"

void pgsTestSuite::test_generator_reference(void)
{
	const int nb_iterations = 10;

	// Table & column do not exist so this will return empty string only
	{
		pgsReferenceGen gen(0, wxT("table"), wxT("column"), false);
		TS_ASSERT(!gen.is_sequence());
		wxString result;
		for (int i = 0; i < nb_iterations; i++)
		{
			result = gen.random();
			TS_ASSERT(result == wxT(""));
		}
	}

	// Unique values but everything is going to be empty
	{
		pgsReferenceGen gen(0, wxT("table"), wxT("column"), true);
		TS_ASSERT(gen.is_sequence());
		wxString result;
		for (int i = 0; i < nb_iterations; i++)
		{
			result = gen.random();
			TS_ASSERT(result == wxT(""));
		}
	}
	
	// Test copy constructor
	{
		// Create a generator and generate values
		pgsReferenceGen gen(0, wxT("table"), wxT("column"), false);
		TS_ASSERT(!gen.is_sequence());
		wxString result, res_cmp;
		for (int i = 0; i < nb_iterations / 2; i++)
		{
			result = gen.random();
		}
		
		// Copy this generator to a new one
		// Both generators must generate the same values
		pgsReferenceGen comparator(gen);
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
		pgsReferenceGen gen(0, wxT("table"), wxT("column"), false);
		pgsReferenceGen comparator(0, wxT("t"), wxT("c"), false);
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
