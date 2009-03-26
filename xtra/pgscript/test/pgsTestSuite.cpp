//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2009, The pgAdmin Development Team
// This software is released under the BSD Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgsTestSuite.h"

#include "pgscript/exceptions/pgsException.h"

pgsTestSuite::pgsTestSuite()
{
	
}

void pgsTestSuite::test_execute_tests(void)
{
	try
	{
		test_expression_cast();
		test_expression_execute();
		test_expression_ident();
		test_expression_operation();
		test_expression_record();
		test_generator_date();
		test_generator_date_time();
		test_generator_dictionary();
		test_generator_int();
		test_generator_real();
		test_generator_regex();
		test_generator_reference();
		test_generator_string();
		test_generator_time();
		test_object_generator();
		test_object_record();
		test_object_variable();
		test_operator_generator();
		test_operator_number();
		test_operator_record();
		test_operator_string();
		test_statement_stmt();
		test_utility_context();
		test_utility_mapm();
		test_utility_utilities();
		test_expression_ident();
	}
	catch (const pgsException & e)
	{
		wxLogError(e.message());
	}
}

pgsTestSuite & pgsTestSuite::instance()
{
	static pgsTestSuite x;
	return x;
}

wxStringOutputStream pgsTestClass::output;
wxTextOutputStream pgsTestClass::cout(output);

wxTextOutputStream & pgsTestClass::get_cout()
{
	return cout;
}

int main(int argc, char * argv[])
{
	// Initilize application
	wxInitializer initializer;
	if (!initializer.IsOk())
	{
		wxLogError(wxT("Cannot initialize the application"));
		return EXIT_FAILURE;
	}
	
	// Logging options
	sysLogger::logLevel = LOG_SQL;
	
	wxLogMessage(wxT("Testing classes"));
	pgsTestSuite::instance().test_execute_tests();
	wxLogMessage(wxT("Class test done"));
	
#if defined(PGSDEBUG)
	pgsAlloc::instance().dump();
#endif
	
	return EXIT_SUCCESS;
}
