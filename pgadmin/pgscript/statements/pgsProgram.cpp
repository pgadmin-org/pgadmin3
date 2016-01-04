//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/statements/pgsProgram.h"

#include "pgscript/exceptions/pgsException.h"
#include "pgscript/statements/pgsStmtList.h"

pgsProgram::pgsProgram(pgsVarMap &vars) :
	m_vars(vars)
{

}

pgsProgram::~pgsProgram()
{

}

void pgsProgram::dump()
{
	dump(m_vars);
}

void pgsProgram::dump(const pgsVarMap &vars)
{
	pgsVarMap::const_iterator it;
	for (it = vars.begin(); it != vars.end(); it++)
	{
		wxLogMessage(wxString() << it->first << wxT(" -> ")
		             << it->second->value());
	}
}

void pgsProgram::eval(pgsStmtList *stmt_list)
{
	wxLogScript(wxT("Entering program"));

	try
	{
		stmt_list->eval(m_vars);
	}
	catch (const pgsException &)
	{

	}
	catch (const std::exception &)
	{

	}

	pgsStmtList::m_exception_thrown = false;

	wxLogScript(wxT("Leaving  program"));
}
