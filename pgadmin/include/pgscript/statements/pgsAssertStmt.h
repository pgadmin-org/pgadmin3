//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSASSERTSTMT_H_
#define PGSASSERTSTMT_H_

#include "pgscript/pgScript.h"
#include "pgscript/statements/pgsStmt.h"

class pgsAssertStmt : public pgsStmt
{

private:

	const pgsExpression *m_cond;

public:

	pgsAssertStmt(const pgsExpression *cond, pgsThread *app = 0);

	virtual ~pgsAssertStmt();

	virtual void eval(pgsVarMap &vars) const;

private:

	pgsAssertStmt(const pgsAssertStmt &that);

	pgsAssertStmt &operator=(const pgsAssertStmt &that);

};

#endif /*PGSASSERTSTMT_H_*/
