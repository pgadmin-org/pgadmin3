//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/statements/pgsPrintStmt.h"

#include "pgscript/exceptions/pgsException.h"
#include "pgscript/utilities/pgsThread.h"
#include "pgscript/utilities/pgsUtilities.h"

pgsPrintStmt::pgsPrintStmt(const pgsExpression *var, pgsOutputStream &cout,
                           pgsThread *app) :
	pgsStmt(app), m_var(var), m_cout(cout)
{

}

pgsPrintStmt::~pgsPrintStmt()
{
	pdelete(m_var);
}

void pgsPrintStmt::eval(pgsVarMap &vars) const
{
	if (m_app != 0)
	{
		m_app->LockOutput();
	}

	try
	{
		m_cout << PGSOUTPGSCRIPT << wx_static_cast(const wxString,
		        m_var->eval(vars)->value()) << wxT("\n");
	}
	catch (const pgsException &)
	{
		if (m_app != 0)
		{
			m_app->UnlockOutput();
		}

		throw;
	}

	if (m_app != 0)
	{
		m_app->UnlockOutput();
	}
}
