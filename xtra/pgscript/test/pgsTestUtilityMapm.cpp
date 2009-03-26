//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2009, The pgAdmin Development Team
// This software is released under the BSD Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgsTestSuite.h"

#include "pgscript/utilities/pgsMapm.h"

void pgsTestSuite::test_utility_mapm(void)
{
	// Basic stuff about MAPM
	{
		TS_ASSERT(-MAPM(123e-4) == MAPM("-0.000123e3") / 10);
		TS_ASSERT(MAPM(1e5) == MAPM("-100000").abs());
		TS_ASSERT(MAPM("12345678909876541").neg()
				== MAPM("-.12345678909876541e17"));
		TS_ASSERT(MAPM("123.4") * 10 == MAPM("+1234"));
	}

	// String output
	{
		TS_ASSERT(pgsMapm::pgs_mapm_str(".0012345e-1") == wxT("1.2345E-4"));
		TS_ASSERT(pgsMapm::pgs_mapm_str("-.0012345e+8") == wxT("-123450"));
		TS_ASSERT(pgsMapm::pgs_mapm_str("123.e-30000") == wxT("1.23E-29998"));
	}

	// Round function
	{
		TS_ASSERT(pgsMapm::pgs_mapm_round(".0012345e-1") == MAPM("0"));
		TS_ASSERT(pgsMapm::pgs_mapm_round("-.0012345e+8") == MAPM("-123450"));
		TS_ASSERT(pgsMapm::pgs_mapm_round("123.e-30000") == MAPM("0"));
		TS_ASSERT(pgsMapm::pgs_mapm_round("123.45678E2") == MAPM("12345"));
	}
}
