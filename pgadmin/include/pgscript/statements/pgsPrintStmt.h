//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsPrintStmt.h,v 1.2 2008/08/10 17:45:36 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSPRINTSTMT_H_
#define PGSPRINTSTMT_H_

#include "pgscript/pgScript.h"
#include "pgscript/statements/pgsStmt.h"

class pgsPrintStmt : public pgsStmt
{

private:

	const pgsExpression * m_var;
	
	pgsOutputStream & m_cout;

public:

	pgsPrintStmt(const pgsExpression * var, pgsOutputStream & cout,
			pgsThread * app = 0);

	virtual ~pgsPrintStmt();

	virtual void eval(pgsVarMap & vars) const;

private:

	pgsPrintStmt(const pgsPrintStmt & that);

	pgsPrintStmt & operator=(const pgsPrintStmt & that);

};

#endif /*PGSPRINTSTMT_H_*/
