//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsPrintStmt.cpp,v 1.4 2008/08/13 23:04:37 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/statements/pgsPrintStmt.h"

#include "pgscript/utilities/pgsThread.h"
#include "pgscript/utilities/pgsUtilities.h"

pgsPrintStmt::pgsPrintStmt(const pgsExpression * var, pgsOutputStream & cout,
		pgsThread * app) :
	pgsStmt(app), m_var(var), m_cout(cout)
{

}

pgsPrintStmt::~pgsPrintStmt()
{
	pdelete(m_var);
}

void pgsPrintStmt::eval(pgsVarMap & vars) const
{
	if (m_app != 0)
	{
		m_app->LockOutput();
	}
	
	m_cout << wxT("[OUTPUT] ") << wx_static_cast(const wxString,
			m_var->eval(vars)->value()) << wxT("\n");
	
	if (m_app != 0)
	{
		m_app->UnlockOutput();
	}
}
