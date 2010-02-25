//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2009, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSTESTSUITE_H_
#define PGSTESTSUITE_H_

#include "pgscript/pgScript.h"
#include <wx/file.h>
#include <wx/datetime.h>
#include <wx/sstream.h>
#include <wx/txtstrm.h>

// Choose an assert macro
#include <cassert>
#define  TS_ASSERT assert

class pgsTestClass
{

private:

	static wxStringOutputStream output;
	static wxTextOutputStream cout;

public:

	static wxTextOutputStream & get_cout();

};

class pgsTestSuite
{

private:

	void test_expression_cast(void);
	void test_expression_execute(void);
	void test_expression_ident(void);
	void test_expression_operation(void);
	void test_expression_record(void);
	void test_generator_date(void);
	void test_generator_date_time(void);
	void test_generator_dictionary(void);
	void test_generator_int(void);
	void test_generator_real(void);
	void test_generator_regex(void);
	void test_generator_reference(void);
	void test_generator_string(void);
	void test_generator_time(void);
	void test_object_generator(void);
	void test_object_record(void);
	void test_object_variable(void);
	void test_operator_generator(void);
	void test_operator_number(void);
	void test_operator_record(void);
	void test_operator_string(void);
	void test_statement_stmt(void);
	void test_utility_context(void);
	void test_utility_mapm(void);
	void test_utility_utilities(void);
	
	pgsTestSuite();

public:

	void test_execute_tests(void);
	
	static pgsTestSuite & instance();

};

#endif /*PGSTESTSUITE_H_*/
