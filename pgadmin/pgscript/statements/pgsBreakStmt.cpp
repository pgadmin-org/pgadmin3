//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsBreakStmt.cpp,v 1.2 2008/08/10 17:45:37 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
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
