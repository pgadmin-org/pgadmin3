//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2009, The pgAdmin Development Team
// This software is released under the BSD Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgsTestSuite.h"

#include "pgscript/generators/pgsRegexGen.h"
#include "pgscript/objects/pgsNumber.h"

void pgsTestSuite::test_generator_regex(void)
{
	const int nb_iterations = 20;

	// Generate empty strings
	{
		pgsRegexGen gen(wxT(""));
		TS_ASSERT(gen.is_valid());
		for (int i = 0; i < nb_iterations; i++)
		{
			TS_ASSERT(gen.random() == wxT(""));
		}
	}
	
	// Generate a space
	{
		pgsRegexGen gen(wxT(" "));
		TS_ASSERT(gen.is_valid());
		for (int i = 0; i < nb_iterations; i++)
		{
			TS_ASSERT(gen.random() == wxT(" "));
		}
	}
	
	// Generate a fixed string
	{
		pgsRegexGen gen(wxT("[1].[0]"));
		TS_ASSERT(gen.is_valid());
		for (int i = 0; i < nb_iterations; i++)
		{
			TS_ASSERT(gen.random() == wxT("1.0"));
		}
	}
	
	// Create an invalid regex (special character in a range)
	{
		pgsRegexGen gen(wxT("[abc{]"));
		TS_ASSERT(!gen.is_valid());
	}
	
	// Create an invalid regex (end of range without beginning)
	{
		pgsRegexGen gen(wxT("a]"));
		TS_ASSERT(!gen.is_valid());
	}
	
	// Create a valid regex with escaped characters
	{
		pgsRegexGen gen(wxT("\\]\\}\\-\\[\\{"));
		TS_ASSERT(gen.is_valid());
		for (int i = 0; i < nb_iterations; i++)
		{
			TS_ASSERT(gen.random() == wxT("]}-[{"));
		}
	}
	
	// Test a complex regex
	{
		// [1] Test class
		wxString regex = wxT("{14}\\\" [a-zBG0-3<>]{1,3}{3,4,5}  [ab\\{ab\\}c]")
						wxT(" @ [1234hg]{2} ' \\[{1<} @ [\\[\\-.]");
		pgsRegexGen gen(regex);
		TS_ASSERT(gen.is_valid());
		for (int i = 0; i < nb_iterations; i++)
		{
			TS_ASSERT(gen.random().Length() > 0);
		}
		
		// [2] Test copy constructor
		pgsRegexGen copy(gen);
		TS_ASSERT(copy.is_valid());
		for (int i = 0; i < nb_iterations; i++)
		{
			TS_ASSERT(gen.random() == copy.random());
		}
		
		// [3] Test assignment
		pgsRegexGen cmp(wxT("abc"));
		for (int i = 0; i < nb_iterations; i++)
		{
			TS_ASSERT(cmp.random() == wxT("abc"));
		}
		cmp = gen;
		for (int i = 0; i < nb_iterations; i++)
		{
			TS_ASSERT(gen.random() == cmp.random());
		}
	}
	
	// Test that two generators with same seed generate same values
	// Those values are integers
	{
		// [1] Test class
		wxString regex = wxT("[+\\-]{0,1}[0-9]{1,40}");
		pgsRegexGen gen(regex, 0);
		pgsRegexGen cmp(regex, 0);
		TS_ASSERT(gen.is_valid() && cmp.is_valid());
		wxString result;
		for (int i = 0; i < nb_iterations; i++)
		{
			result = gen.random();
			TS_ASSERT(result == cmp.random());
			// [2] Generate a number to see whether it is valid
			pgsNumber num(result, pgsInt);
		}
	}
}
