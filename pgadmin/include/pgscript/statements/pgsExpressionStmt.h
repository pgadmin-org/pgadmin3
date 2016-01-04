//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSEXPRESSIONSTMT_H_
#define PGSEXPRESSIONSTMT_H_

#include "pgscript/pgScript.h"
#include "pgscript/statements/pgsStmt.h"

class pgsExpressionStmt : public pgsStmt
{

private:

	const pgsExpression *m_var;

public:

	pgsExpressionStmt(const pgsExpression *var, pgsThread *app = 0);

	virtual ~pgsExpressionStmt();

	virtual void eval(pgsVarMap &vars) const;

private:

	pgsExpressionStmt(const pgsExpressionStmt &that);

	pgsExpressionStmt &operator=(const pgsExpressionStmt &that);

};

#endif /*PGSEXPRESSIONSTMT_H_*/
