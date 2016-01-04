//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/statements/pgsIfStmt.h"

pgsIfStmt::pgsIfStmt(const pgsExpression *cond, const pgsStmt *stmt_list_if,
                     const pgsStmt *stmt_list_else, pgsThread *app) :
	pgsStmt(app), m_cond(cond), m_stmt_list_if(stmt_list_if),
	m_stmt_list_else(stmt_list_else)
{

}

pgsIfStmt::~pgsIfStmt()
{
	pdelete(m_cond);
	pdelete(m_stmt_list_if);
	pdelete(m_stmt_list_else);
}

void pgsIfStmt::eval(pgsVarMap &vars) const
{
	if (m_cond->eval(vars)->pgs_is_true())
	{
		m_stmt_list_if->eval(vars);
	}
	else
	{
		m_stmt_list_else->eval(vars);
	}
}
