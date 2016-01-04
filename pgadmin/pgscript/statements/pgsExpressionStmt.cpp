//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/statements/pgsExpressionStmt.h"

pgsExpressionStmt::pgsExpressionStmt(const pgsExpression *var, pgsThread *app) :
	pgsStmt(app), m_var(var)
{

}

pgsExpressionStmt::~pgsExpressionStmt()
{
	pdelete(m_var)
}

void pgsExpressionStmt::eval(pgsVarMap &vars) const
{
	m_var->eval(vars);
}
