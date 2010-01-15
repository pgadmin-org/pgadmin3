//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the BSD Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/statements/pgsBreakStmt.h"

#include "pgscript/exceptions/pgsBreakException.h"

pgsBreakStmt::pgsBreakStmt(pgsThread * app) :
	pgsStmt(app)
{

}

pgsBreakStmt::~pgsBreakStmt()
{

}

void pgsBreakStmt::eval(pgsVarMap & vars) const
{
	throw pgsBreakException();
}
