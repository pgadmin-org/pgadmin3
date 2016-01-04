//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/statements/pgsWhileStmt.h"

#include "pgscript/exceptions/pgsBreakException.h"
#include "pgscript/exceptions/pgsContinueException.h"
#include "pgscript/exceptions/pgsInterruptException.h"
#include "pgscript/utilities/pgsThread.h"

pgsWhileStmt::pgsWhileStmt(const pgsExpression *cond, const pgsStmt *stmt_list,
                           pgsThread *app) :
	pgsStmt(app), m_cond(cond), m_stmt_list(stmt_list)
{

}

pgsWhileStmt::~pgsWhileStmt()
{
	pdelete(m_cond);
	pdelete(m_stmt_list);
}

void pgsWhileStmt::eval(pgsVarMap &vars) const
{
loop:
	if (m_cond->eval(vars)->pgs_is_true())
	{
		try
		{
			m_stmt_list->eval(vars);
		}
		catch (const pgsBreakException &)
		{
			goto end;
		}
		catch (const pgsContinueException &)
		{

		}
		if (m_app != 0 && m_app->TestDestroy())
			throw pgsInterruptException();

		goto loop;
	}
end:
	;
}
