//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsTestExpressionExecute.cpp,v 1.2 2008/08/10 17:45:36 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgsTestSuite.h"

#include "pgscript/expressions/pgsExecute.h"
#include "pgscript/objects/pgsNumber.h"
#include "pgscript/objects/pgsRecord.h"
#include "pgscript/objects/pgsString.h"

void pgsTestSuite::test_expression_execute(void)
{
#if 0
	pgsVarMap vars;
	
	// Fill in the symbol table
	{
		vars[wxT("@a")] = pnew pgsString(wxT("I'm the Game"));
		vars[wxT("@b")] = pnew pgsString(wxT("You can't \"defeat me\""));
		vars[wxT("@c")] = pnew pgsNumber(wxT("10"));
		pgsRecord * rec = 0;
		rec = pnew pgsRecord(2);
		rec->insert(0, 0, pnew pgsString(wxT("I'm the Game")));
		rec->insert(0, 1, pnew pgsNumber(wxT("10")));
		rec->insert(1, 0, pnew pgsString(wxT("You can't \"defeat me\"")));
		rec->insert(1, 1, pnew pgsNumber(wxT("10")));
		vars[wxT("@d")] = rec;
	}
	
	// Test execution of a statement
	{
		pgsExecute * S1 = 0;
		
		wxString str_out, query, expected, result;
		wxStringOutputStream output(&str_out);
		wxTextOutputStream out(output);
		
		query = wxString(wxT("INSERT ('@a', '@b', '@c', '@d', '@e', ")
				wxT("'\\@c', '\\\\\\c', '\\\\\\@c', '\\\\@c', 'my''string', ")
				wxT("'@c\\@', 'c', 'c@', '@ c', 'a@cb', 'a @c b', 10)"));
		expected = wxString(wxT("INSERT ('I''m the Game', 'You can''t \"defeat me\"', ")
				wxT("'10', '(\"I''m the Game\",10)\n(\"You can''t \\\"defeat me\\\"\",10)', '@e', ")
				wxT("'@c', '\\\\c', '\\@c', '\\@c', 'my''string', ")
				wxT("'10@', 'c', 'c@', '@ c', 'a@cb', 'a 10 b', 10)"));
		S1 = pnew pgsExecute(query, &out);
		for (int i = 0; i < 10; i++)
		{
			S1->eval(vars);
			result = str_out.Strip(wxString::both);
			result.Replace(wxT("\r"), wxT(""));
			TS_ASSERT(result == expected);
			str_out.Clear();
		}
		
		// Test copy constructor
		pgsExecute copy(*S1);
		for (int i = 0; i < 5; i++)
		{
			copy.eval(vars);
			result = str_out.Strip(wxString::both);
			result.Replace(wxT("\r"), wxT(""));
			TS_ASSERT(result == expected);
			str_out.Clear();
		}
		
		// Test assignment
		copy = *S1;
		for (int i = 0; i < 5; i++)
		{
			copy.eval(vars);
			result = str_out.Strip(wxString::both);
			result.Replace(wxT("\r"), wxT(""));
			TS_ASSERT(result == expected);
			str_out.Clear();
		}
		
		pdelete(S1);
	}
#endif
}
