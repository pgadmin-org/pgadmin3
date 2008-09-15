//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsTestUtilityUtilities.cpp,v 1.2 2008/08/10 17:45:36 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgsTestSuite.h"

#include "pgscript/utilities/pgsUtilities.h"

void pgsTestSuite::test_utility_utilities(void)
{
	// Uniform line returns
	{
		wxString s = wxT("  My text \r\n goes to a \r new line and another ")
			wxT("\n one ");

		TS_ASSERT(s.Contains(wxT("\r\n")));
		TS_ASSERT(s.Contains(wxT("\r")));

		s = pgsUtilities::uniform_line_returns(s);
		TS_ASSERT(!s.Contains(wxT("\r\n")));
		TS_ASSERT(!s.Contains(wxT("\r")));
		TS_ASSERT(s.Freq(wxT('\n')) == 3);
	}

	// Escape and unescape quotes
	{
		wxString s = wxT("78''0\n'8");
		wxString r = pgsUtilities::unescape_quotes(s);
		TS_ASSERT(r == wxT("78'0\n'8"));
		TS_ASSERT(pgsUtilities::escape_quotes(r) == wxT("78''0\n''8"));

		s = wxT("' 78''0\\' ''");
		r = pgsUtilities::unescape_quotes(s);
		TS_ASSERT(r == wxT("' 78'0' '"));
		TS_ASSERT(pgsUtilities::escape_quotes(r) == wxT("'' 78''0'' ''"));

		s = wxT("' 78'''\\0 ''");
		r = pgsUtilities::unescape_quotes(s);
		TS_ASSERT(r == wxT("' 78''\\0 '"));
		TS_ASSERT(pgsUtilities::escape_quotes(r) == wxT("'' 78''''\\\\0 ''"));
		
		s = wxT("''name''''");
		r = pgsUtilities::unescape_quotes(s);
		TS_ASSERT(r == wxT("'name''"));
		TS_ASSERT(pgsUtilities::escape_quotes(r) == wxT("''name''''"));
	}
}
