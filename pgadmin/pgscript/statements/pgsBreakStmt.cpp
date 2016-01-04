//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/statements/pgsBreakStmt.h"

#include "pgscript/exceptions/pgsBreakException.h"

pgsBreakStmt::pgsBreakStmt(pgsThread *app) :
	pgsStmt(app)
{

}

pgsBreakStmt::~pgsBreakStmt()
{

}

void pgsBreakStmt::eval(pgsVarMap &vars) const
{
	throw pgsBreakException();
}
