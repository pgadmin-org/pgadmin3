//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsContinueStmt.cpp,v 1.2 2008/08/10 17:45:37 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/statements/pgsContinueStmt.h"

#include "pgscript/exceptions/pgsContinueException.h"

pgsContinueStmt::pgsContinueStmt(pgsThread * app) :
	pgsStmt(app)
{

}

pgsContinueStmt::~pgsContinueStmt()
{

}

void pgsContinueStmt::eval(pgsVarMap & vars) const
{
	throw pgsContinueException();
}
